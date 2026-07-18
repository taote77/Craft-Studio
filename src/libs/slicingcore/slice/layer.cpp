#include <slicingcore/slice/layer.hpp>

namespace slicing {

// ============================================================================
// Layer
// ============================================================================

Layer::Layer(int id, coord_t z)
    : _id(id)
    , _z(z)
{
}

int Layer::id() const
{
    return _id;
}

coord_t Layer::z() const
{
    return _z;
}

double Layer::zMm() const
{
    return unscale(_z);
}

const ExPolygons& Layer::slices() const
{
    return _slices;
}

ExPolygons& Layer::slices()
{
    return _slices;
}

void Layer::addSlice(const ExPolygon& expoly)
{
    _slices.push_back(expoly);
}

void Layer::setSlices(ExPolygons slices)
{
    _slices = std::move(slices);
}

size_t Layer::vertexCount() const
{
    size_t n = 0;
    for (const auto& s : _slices)
        n += s.vertexCount();
    return n;
}

BoundingBox2D Layer::boundingBox() const
{
    BoundingBox2D bb;
    for (const auto& s : _slices)
    {
        auto sbb = s.boundingBox();
        bb.extend(sbb.min);
        bb.extend(sbb.max);
    }
    return bb;
}

bool Layer::empty() const
{
    return _slices.empty();
}

// ============================================================================
// LayerRegion
// ============================================================================

LayerRegion::LayerRegion(PrintRegionType type)
    : _type(type)
{
}

PrintRegionType LayerRegion::type() const
{
    return _type;
}

const ExPolygons& LayerRegion::perimeters() const
{
    return _perimeters;
}

ExPolygons& LayerRegion::perimeters()
{
    return _perimeters;
}

const ExPolygons& LayerRegion::infillArea() const
{
    return _infillArea;
}

ExPolygons& LayerRegion::infillArea()
{
    return _infillArea;
}

double LayerRegion::area() const
{
    // Very rough estimate — uses contour bounding box area as approximation
    // Full implementation would compute exact polygon area via shoelace formula
    double totalArea = 0.0;
    for (const auto& p : _perimeters)
    {
        BoundingBox2D bb = p.boundingBox();
        totalArea += static_cast<double>(bb.width()) * bb.height();
    }
    return totalArea;
}

} // namespace slicing
