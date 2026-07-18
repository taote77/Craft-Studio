#include <slicingcore/mesh/triangle_mesh.hpp>

#include <cmath>
#include <algorithm>
#include <map>
#include <set>

namespace slicing {

// ============================================================================
// Construction / Destruction
// ============================================================================

TriangleMesh::TriangleMesh() = default;
TriangleMesh::~TriangleMesh() = default;

// ============================================================================
// Accessors
// ============================================================================

const std::vector<Vertex>& TriangleMesh::vertices() const
{
    return _vertices;
}

const std::vector<Triangle>& TriangleMesh::triangles() const
{
    return _triangles;
}

size_t TriangleMesh::vertexCount() const
{
    return _vertices.size();
}

size_t TriangleMesh::triangleCount() const
{
    return _triangles.size();
}

int TriangleMesh::addVertex(const Vertex& v)
{
    invalidateCache();
    _vertices.push_back(v);
    return static_cast<int>(_vertices.size()) - 1;
}

int TriangleMesh::addVertex(coord_t x, coord_t y, coord_t z)
{
    return addVertex(Vertex(x, y, z));
}

int TriangleMesh::addTriangle(int v0, int v1, int v2)
{
    invalidateCache();
    _triangles.emplace_back(v0, v1, v2);
    return static_cast<int>(_triangles.size()) - 1;
}

int TriangleMesh::addTriangle(const Triangle& t)
{
    return addTriangle(t.v0, t.v1, t.v2);
}

void TriangleMesh::clear()
{
    invalidateCache();
    _vertices.clear();
    _triangles.clear();
}

// ============================================================================
// Computed Properties
// ============================================================================

void TriangleMesh::invalidateCache()
{
    _bboxValid = false;
    _volumeValid = false;
    _watertightComputed = false;
}

BoundingBox3D TriangleMesh::boundingBox() const
{
    if (_bboxValid)
        return _cachedBBox;

    BoundingBox3D bb;
    for (const auto& v : _vertices)
        bb.extend({ v.x, v.y, v.z });

    _cachedBBox = bb;
    _bboxValid = true;
    return _cachedBBox;
}

double TriangleMesh::volume() const
{
    if (_volumeValid)
        return _cachedVolume;

    // Tetrahedral decomposition: sum signed volumes of tetrahedrons
    // formed by each triangle and the origin.
    // Volume = sum( (v0 × v1) · v2 ) / 6
    double totalVolume = 0.0;

    for (const auto& tri : _triangles)
    {
        const Vertex& a = _vertices[tri.v0];
        const Vertex& b = _vertices[tri.v1];
        const Vertex& c = _vertices[tri.v2];

        // Triple product: a · (b × c)
        double cross_x = static_cast<double>(b.y) * c.z - static_cast<double>(b.z) * c.y;
        double cross_y = static_cast<double>(b.z) * c.x - static_cast<double>(b.x) * c.z;
        double cross_z = static_cast<double>(b.x) * c.y - static_cast<double>(b.y) * c.x;

        double triple = a.x * cross_x + a.y * cross_y + a.z * cross_z;
        totalVolume += triple;
    }

    // The result is in cubic microns; divide by 6 for tetrahedral volume
    _cachedVolume = std::abs(totalVolume) / 6.0;
    _volumeValid = true;
    return _cachedVolume;
}

double TriangleMesh::volumeMm3() const
{
    // cubic microns → cubic millimeters: divide by 1e9
    return volume() / 1000000000.0;
}

bool TriangleMesh::isWatertight() const
{
    if (_watertightComputed)
        return _cachedWatertight;

    using EdgeKey = std::pair<int, int>;

    std::map<EdgeKey, int> edges;

    for (const auto& tri : _triangles)
    {
        auto addEdge = [&](int a, int b) {
            EdgeKey key = (a < b) ? EdgeKey(a, b) : EdgeKey(b, a);
            edges[key]++;
        };

        addEdge(tri.v0, tri.v1);
        addEdge(tri.v1, tri.v2);
        addEdge(tri.v2, tri.v0);
    }

    // Every edge must appear exactly 2 times (once per adjacent face)
    for (const auto& [edge, count] : edges)
    {
        if (count != 2)
        {
            _cachedWatertight = false;
            _watertightComputed = true;
            return false;
        }
    }

    _cachedWatertight = true;
    _watertightComputed = true;
    return true;
}

Point3D TriangleMesh::center() const
{
    auto bb = boundingBox();
    return bb.center();
}

Point3D TriangleMesh::size() const
{
    auto bb = boundingBox();
    return bb.size();
}

// ============================================================================
// Mutation
// ============================================================================

void TriangleMesh::translate(coord_t dx, coord_t dy, coord_t dz)
{
    invalidateCache();
    for (auto& v : _vertices)
    {
        v.x += dx;
        v.y += dy;
        v.z += dz;
    }
}

void TriangleMesh::scale(double factor)
{
    invalidateCache();
    for (auto& v : _vertices)
    {
        v.x = static_cast<coord_t>(std::round(v.x * factor));
        v.y = static_cast<coord_t>(std::round(v.y * factor));
        v.z = static_cast<coord_t>(std::round(v.z * factor));
    }
}

bool TriangleMesh::repair()
{
    // Stub — full implementation will use VTK filters
    // (vtkCleanPolyData, vtkFillHolesFilter, vtkPolyDataNormals)
    // For now, does basic degenerate triangle removal.
    invalidateCache();

    // Remove degenerate triangles (zero area)
    std::vector<Triangle> goodTriangles;
    for (const auto& tri : _triangles)
    {
        const auto& a = _vertices[tri.v0];
        const auto& b = _vertices[tri.v1];
        const auto& c = _vertices[tri.v2];

        // Check that all three vertices are distinct
        if (tri.v0 == tri.v1 || tri.v1 == tri.v2 || tri.v0 == tri.v2)
            continue;

        // Cross product magnitude as a coarse area check
        coord_t ab_x = b.x - a.x, ab_y = b.y - a.y, ab_z = b.z - a.z;
        coord_t ac_x = c.x - a.x, ac_y = c.y - a.y, ac_z = c.z - a.z;

        coord_t cross_x = ab_y * ac_z - ab_z * ac_y;
        coord_t cross_y = ab_z * ac_x - ab_x * ac_z;
        coord_t cross_z = ab_x * ac_y - ab_y * ac_x;

        coord_t areaSq = cross_x * cross_x + cross_y * cross_y + cross_z * cross_z;

        if (areaSq > 0)
            goodTriangles.push_back(tri);
    }

    _triangles = std::move(goodTriangles);

    return true;
}

void TriangleMesh::merge(const TriangleMesh& other)
{
    invalidateCache();

    int vertexOffset = static_cast<int>(_vertices.size());
    _vertices.insert(_vertices.end(), other._vertices.begin(), other._vertices.end());

    for (const auto& tri : other._triangles)
    {
        _triangles.emplace_back(
            tri.v0 + vertexOffset,
            tri.v1 + vertexOffset,
            tri.v2 + vertexOffset
        );
    }
}

} // namespace slicing
