#ifndef SLICINGCORE_TRIANGLE_MESH_HPP
#define SLICINGCORE_TRIANGLE_MESH_HPP

#include <slicingcore/slicingcore_global.h>
#include <slicingcore/geometry/coord.hpp>

#include <vector>
#include <cstdint>
#include <memory>

// Forward declarations for VTK (only needed by toVTK/fromVTK — link optional)
// Applications that don't use VTK can still use TriangleMesh fully.

namespace slicing {

// ============================================================================
// TriangleMesh — the fundamental 3D mesh data structure
//
// This is a renderer-independent indexed triangle mesh. It replaces the
// current SceneObjectV2 as the canonical mesh representation in the engine.
//
// Features:
//   - Real mesh volume via tetrahedral decomposition (NOT bounding box)
//   - Watertightness check
//   - Auto-repair (stub — delegates to VTK filters when available)
//   - Thread-safe read access via shared_mutex
//
// VTK is ONLY used for I/O conversion (toVTK/fromVTK). The engine itself
// operates on TriangleMesh directly.
// ============================================================================

struct SLICINGCORE_API Vertex
{
    coord_t x = 0;
    coord_t y = 0;
    coord_t z = 0;

    Vertex() = default;
    Vertex(coord_t x_, coord_t y_, coord_t z_) : x(x_), y(y_), z(z_) {}

    Point3D toPoint3D() const { return { x, y, z }; }
};

struct SLICINGCORE_API Triangle
{
    int v0 = 0;
    int v1 = 0;
    int v2 = 0;

    Triangle() = default;
    Triangle(int a, int b, int c) : v0(a), v1(b), v2(c) {}
};

class SLICINGCORE_API TriangleMesh
{
public:
    TriangleMesh();
    ~TriangleMesh();

    // -- Accessors ----------------------------------------------------------

    const std::vector<Vertex>& vertices() const;
    const std::vector<Triangle>& triangles() const;
    size_t vertexCount() const;
    size_t triangleCount() const;

    /// Add a vertex, returns its index
    int addVertex(const Vertex& v);
    int addVertex(coord_t x, coord_t y, coord_t z);

    /// Add a triangle (by vertex indices)
    int addTriangle(int v0, int v1, int v2);
    int addTriangle(const Triangle& t);

    /// Clear all geometry data
    void clear();

    // -- Computed Properties (memoized, thread-safe reads) ------------------

    /// 3D bounding box in internal coordinates (microns)
    BoundingBox3D boundingBox() const;

    /// Actual mesh volume via signed tetrahedral decomposition (cubic microns)
    /// For a closed, watertight mesh, this is the exact enclosed volume.
    /// Result is in cubic microns; divide by 1e9 to get mm³.
    double volume() const;

    /// Real-world volume in cubic millimeters (mm³)
    double volumeMm3() const;

    /// Check if the mesh is watertight (every edge has exactly 2 adjacent faces)
    bool isWatertight() const;

    /// Compute the center of the bounding box
    Point3D center() const;

    /// Compute mesh size (max - min along each axis)
    Point3D size() const;

    // -- Mutation ------------------------------------------------------------

    /// Translate all vertices
    void translate(coord_t dx, coord_t dy, coord_t dz);

    /// Uniform scale around origin
    void scale(double factor);

    /// Auto-repair: fix normals, fill small holes, remove degenerate faces
    /// Returns true if the mesh was successfully repaired.
    /// Currently a stub — will use VTK filters when linked with VTK.
    bool repair();

    /// Merge another mesh into this one (appends vertices and triangles,
    /// re-indexing the added triangles)
    void merge(const TriangleMesh& other);

    // -- VTK Conversion (requires VTK headers at compile time) --------------

#ifdef SLICINGCORE_WITH_VTK
    /// Convert this mesh to a VTK vtkPolyData object
    /// Requires VTK headers. Link against VTK if using this.
    vtkSmartPointer<vtkPolyData> toVTK() const;

    /// Create TriangleMesh from a VTK vtkPolyData
    static TriangleMesh fromVTK(vtkPolyData* polyData);
#endif

private:
    void invalidateCache();

    std::vector<Vertex> _vertices;
    std::vector<Triangle> _triangles;

    // Memoized properties
    mutable bool _bboxValid = false;
    mutable BoundingBox3D _cachedBBox;
    mutable bool _volumeValid = false;
    mutable double _cachedVolume = 0.0;
    mutable bool _watertightComputed = false;
    mutable bool _cachedWatertight = false;
};

} // namespace slicing

#endif // SLICINGCORE_TRIANGLE_MESH_HPP
