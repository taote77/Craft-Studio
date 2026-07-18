#ifndef SLICINGCORE_EX_POLYGON_HPP
#define SLICINGCORE_EX_POLYGON_HPP

#include <slicingcore/geometry/coord.hpp>
#include <vector>

namespace slicing {

// ============================================================================
// ExPolygon — the fundamental 2D slice primitive
//
// An ExPolygon is a polygon with holes: one outer contour (counter-clockwise)
// and zero or more inner holes (clockwise). This is the standard representation
// for 2D slice cross-sections in all major slicers (PrusaSlicer, Cura).
// ============================================================================

using Polygon  = Points;   // single closed polygon (contour or hole)
using Polygons = std::vector<Polygon>;

struct ExPolygon
{
    Polygon contour;        // outer boundary, CCW
    Polygons holes;         // inner boundaries, CW

    ExPolygon() = default;

    ExPolygon(const Polygon& c)
        : contour(c)
    {}

    ExPolygon(const Polygon& c, const Polygons& h)
        : contour(c), holes(h)
    {}

    /// Total number of vertices across all contours
    size_t vertexCount() const
    {
        size_t n = contour.size();
        for (const auto& hole : holes)
            n += hole.size();
        return n;
    }

    /// Check if this ExPolygon is empty (no contour vertices)
    bool empty() const
    {
        return contour.empty();
    }

    /// Compute the 2D bounding box
    BoundingBox2D boundingBox() const
    {
        BoundingBox2D bb;
        for (const auto& p : contour)
            bb.extend(p);
        for (const auto& hole : holes)
            for (const auto& p : hole)
                bb.extend(p);
        return bb;
    }
};

using ExPolygons = std::vector<ExPolygon>;

} // namespace slicing

#endif // SLICINGCORE_EX_POLYGON_HPP
