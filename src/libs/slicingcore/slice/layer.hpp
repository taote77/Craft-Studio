#ifndef SLICINGCORE_LAYER_HPP
#define SLICINGCORE_LAYER_HPP

#include <slicingcore/slicingcore_global.h>
#include <slicingcore/geometry/coord.hpp>
#include <slicingcore/geometry/ex_polygon.hpp>

#include <vector>
#include <cstdint>

namespace slicing {

// ============================================================================
// Layer — one slice layer at a given Z height
//
// A Layer holds the 2D cross-section (ExPolygons) of the mesh at a specific
// Z height, along with optional per-layer metadata.
// ============================================================================

class SLICINGCORE_API Layer
{
public:
    Layer() = default;
    Layer(int id, coord_t z);

    int id() const;
    coord_t z() const;
    double zMm() const;

    const ExPolygons& slices() const;
    ExPolygons& slices();

    void addSlice(const ExPolygon& expoly);
    void setSlices(ExPolygons slices);

    /// Total number of polygon vertices across all slices
    size_t vertexCount() const;

    /// 2D bounding box of all slices in this layer
    BoundingBox2D boundingBox() const;

    /// Whether this layer has any geometry
    bool empty() const;

private:
    int _id = 0;
    coord_t _z = 0;
    ExPolygons _slices;
};

using Layers = std::vector<Layer>;

// ============================================================================
// LayerRegion — a contiguous region within a layer
//
// Multiple objects on the build plate may produce separate LayerRegions
// at the same Z height. Each region holds its own perimeters and infill.
// ============================================================================

enum class SLICINGCORE_API PrintRegionType
{
    Model,          // Solid model region
    Support,        // Support material
    Modifier        // Modifier volume (density override, etc.)
};

class SLICINGCORE_API LayerRegion
{
public:
    LayerRegion() = default;
    explicit LayerRegion(PrintRegionType type);

    PrintRegionType type() const;

    const ExPolygons& perimeters() const;
    ExPolygons& perimeters();

    const ExPolygons& infillArea() const;
    ExPolygons& infillArea();

    /// Total area in square microns
    double area() const;

private:
    PrintRegionType _type = PrintRegionType::Model;
    ExPolygons _perimeters;
    ExPolygons _infillArea;
};

using LayerRegions = std::vector<LayerRegion>;

} // namespace slicing

#endif // SLICINGCORE_LAYER_HPP
