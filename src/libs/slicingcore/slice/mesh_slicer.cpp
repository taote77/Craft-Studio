#include <slicingcore/slice/mesh_slicer.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <unordered_map>

namespace slicing {

// ============================================================================
// MeshSlicer
// ============================================================================

Layers MeshSlicer::slice(const TriangleMesh& mesh,
                          coord_t minZ, coord_t maxZ, coord_t layerHeight,
                          std::function<void(float)> progressCallback) const
{
    Layers layers;

    if (mesh.triangleCount() == 0 || layerHeight <= 0 || minZ >= maxZ)
        return layers;

    int totalLayers = static_cast<int>((maxZ - minZ) / layerHeight) + 1;

    for (int layerIdx = 0; layerIdx < totalLayers; ++layerIdx)
    {
        coord_t z = minZ + layerIdx * layerHeight;

        // Collect all intersection segments for this Z height
        std::vector<std::pair<Point, Point>> segments;

        for (const auto& tri : mesh.triangles())
        {
            const Vertex& a = mesh.vertices()[tri.v0];
            const Vertex& b = mesh.vertices()[tri.v1];
            const Vertex& c = mesh.vertices()[tri.v2];

            // Quick reject: triangle entirely above or below Z plane
            if ((a.z < z && b.z < z && c.z < z) ||
                (a.z > z && b.z > z && c.z > z))
                continue;

            Point p1, p2;
            if (intersectTriangle(a, b, c, z, p1, p2))
                segments.emplace_back(p1, p2);
        }

        // Stitch segments into polygon contours
        Polygons polygons = stitchSegments(segments);

        // Simplify polygons: remove collinear intermediate vertices
        // (artifact of multiple triangles on same face)
        for (auto& poly : polygons)
        {
            if (poly.size() < 3) continue;
            Polygon simplified;
            simplified.push_back(poly.front());
            for (size_t i = 1; i + 1 < poly.size(); ++i)
            {
                // Check if poly[i] is collinear with prev and next
                const Point& prev = simplified.back();
                const Point& cur  = poly[i];
                const Point& next = poly[i + 1];
                // Cross product of (cur-prev) and (next-cur): 0 = collinear
                coord_t cross = (cur.x - prev.x) * (next.y - cur.y)
                              - (cur.y - prev.y) * (next.x - cur.x);
                if (cross != 0)
                    simplified.push_back(cur);
            }
            simplified.push_back(poly.back());
            poly = std::move(simplified);
        }

        // Build ExPolygons: all closed polygons are outer contours for now
        // (hole detection requires more context across layers)
        ExPolygons slices;
        for (auto& poly : polygons)
        {
            if (poly.size() >= 4) // square or more
                slices.emplace_back(std::move(poly));
        }

        Layer layer(layerIdx, z);
        layer.setSlices(std::move(slices));
        layers.push_back(std::move(layer));

        if (progressCallback && (layerIdx % 10 == 0 || layerIdx == totalLayers - 1))
            progressCallback(static_cast<float>(layerIdx + 1) / totalLayers);
    }

    return layers;
}

bool MeshSlicer::intersectTriangle(const Vertex& a, const Vertex& b, const Vertex& c,
                                    coord_t z, Point& p1, Point& p2) const
{
    // Determine which vertices are above/below the Z plane
    bool aAbove = a.z >= z;
    bool bAbove = b.z >= z;
    bool cAbove = c.z >= z;

    // All same side → no intersection
    if (aAbove == bAbove && bAbove == cAbove)
        return false;

    // We have exactly 2 edge intersections (one vertex on-plane counts as
    // an intersection on both edges meeting there — handled naturally).
    int count = 0;

    auto addIntersection = [&](const Vertex& v1, const Vertex& v2) {
        if (count >= 2) return;
        if (v1.z == v2.z) return; // edge is horizontal at Z

        double t = static_cast<double>(z - v1.z) / static_cast<double>(v2.z - v1.z);
        coord_t x = v1.x + static_cast<coord_t>(t * (v2.x - v1.x));
        coord_t y = v1.y + static_cast<coord_t>(t * (v2.y - v1.y));

        if (count == 0) p1 = Point(x, y);
        else            p2 = Point(x, y);
        count++;
    };

    // Check all 3 edges
    if (aAbove != bAbove) addIntersection(a, b);
    if (bAbove != cAbove) addIntersection(b, c);
    if (cAbove != aAbove) addIntersection(c, a);

    return count == 2;
}

// ============================================================================
// Segment stitching — connect unordered line segments into closed polygons
// ============================================================================

Polygons MeshSlicer::stitchSegments(
    const std::vector<std::pair<Point, Point>>& segments)
{
    Polygons result;
    if (segments.empty()) return result;

    // Build adjacency map: for each point, which segment indices connect to it
    using SegIdx = size_t;
    std::map<std::pair<coord_t, coord_t>, std::vector<SegIdx>> pointToSegs;

    auto pointKey = [](const Point& p) {
        return std::make_pair(p.x, p.y);
    };

    for (size_t i = 0; i < segments.size(); ++i)
    {
        pointToSegs[pointKey(segments[i].first)].push_back(i);
        pointToSegs[pointKey(segments[i].second)].push_back(i);
    }

    std::set<size_t> used;
    std::vector<size_t> unused;
    for (size_t i = 0; i < segments.size(); ++i)
        unused.push_back(i);

    // Greedy chain building
    while (!unused.empty())
    {
        // Start a new polygon with the first unused segment
        size_t startIdx = unused.back();
        unused.pop_back();
        if (used.count(startIdx)) continue;

        Polygon poly;
        Point current = segments[startIdx].first;
        Point next    = segments[startIdx].second;
        poly.push_back(current);
        poly.push_back(next);
        used.insert(startIdx);

        Point chainEnd = next;
        bool grown = true;
        while (grown)
        {
            grown = false;
            auto key = pointKey(chainEnd);
            auto it = pointToSegs.find(key);
            if (it != pointToSegs.end())
            {
                for (auto si : it->second)
                {
                    if (used.count(si)) continue;

                    // Find which endpoint of this segment connects
                    const auto& seg = segments[si];
                    Point other;
                    if (pointKey(seg.first) == key)
                        other = seg.second;
                    else if (pointKey(seg.second) == key)
                        other = seg.first;
                    else
                        continue;

                    poly.push_back(other);
                    chainEnd = other;
                    used.insert(si);
                    grown = true;

                    // Remove si from unused
                    auto uit = std::find(unused.begin(), unused.end(), si);
                    if (uit != unused.end()) unused.erase(uit);
                    break;
                }
            }
        }

        // Try to close the loop
        if (poly.size() >= 3)
        {
            if (poly.front().approxEqual(poly.back()))
                poly.pop_back(); // remove duplicate closing point
            result.push_back(std::move(poly));
        }

        // Clean up unused list
        unused.erase(std::remove_if(unused.begin(), unused.end(),
            [&used](size_t i) { return used.count(i); }),
            unused.end());
    }

    return result;
}

// ============================================================================
// Polygon winding order
// ============================================================================

bool MeshSlicer::isCCW(const Polygon& poly)
{
    if (poly.size() < 3) return true;

    // Compute signed area (shoelace formula)
    int64_t area = 0;
    for (size_t i = 0; i < poly.size(); ++i)
    {
        size_t j = (i + 1) % poly.size();
        area += static_cast<int64_t>(poly[i].x) * poly[j].y
              - static_cast<int64_t>(poly[j].x) * poly[i].y;
    }
    // Positive area → CCW (outer contour)
    return area > 0;
}

} // namespace slicing
