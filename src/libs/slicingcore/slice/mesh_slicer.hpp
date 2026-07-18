#ifndef SLICINGCORE_MESH_SLICER_HPP
#define SLICINGCORE_MESH_SLICER_HPP

#include <slicingcore/slicingcore_global.h>
#include <slicingcore/geometry/coord.hpp>
#include <slicingcore/geometry/ex_polygon.hpp>
#include <slicingcore/slice/layer.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

#include <vector>
#include <functional>

namespace slicing {

class TriangleMesh;

// ============================================================================
// MeshSlicer — produces 2D ExPolygon layers from a 3D TriangleMesh
//
// Algorithm: For each Z height, find all triangles intersecting the horizontal
// plane. Compute the line segment for each intersection. Stitch segments into
// closed polygon contours. Classify contours as outer (counter-clockwise) or
// holes (clockwise).
// ============================================================================

class SLICINGCORE_API MeshSlicer
{
public:
    MeshSlicer() = default;

    /// Slice a mesh at uniform Z intervals.
    /// @param mesh   The triangle mesh to slice
    /// @param minZ   Starting Z height (microns)
    /// @param maxZ   Ending Z height (microns)
    /// @param layerHeight  Layer thickness (microns)
    /// @param progressCallback  Optional progress reporter (0.0 - 1.0)
    /// @return Vector of Layer objects with 2D ExPolygon cross-sections
    Layers slice(const TriangleMesh& mesh,
                 coord_t minZ, coord_t maxZ, coord_t layerHeight,
                 std::function<void(float)> progressCallback = nullptr) const;

private:
    /// Compute the intersection segment of a triangle with a horizontal plane at Z
    /// Returns true if the triangle intersects the plane, fills p1,p2
    bool intersectTriangle(const Vertex& a, const Vertex& b, const Vertex& c,
                           coord_t z, Point& p1, Point& p2) const;

    /// Stitch unordered line segments into closed polygon contours
    static Polygons stitchSegments(const std::vector<std::pair<Point, Point>>& segments);

    /// Determine if a polygon is CCW (outer contour) or CW (hole)
    static bool isCCW(const Polygon& poly);
};

} // namespace slicing

#endif
