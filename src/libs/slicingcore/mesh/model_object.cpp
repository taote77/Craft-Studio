#include <slicingcore/mesh/model_object.hpp>
#include <cmath>

namespace slicing {

uint64_t ModelObject::_nextId = 1;

// ============================================================================
// ModelVolume
// ============================================================================

ModelVolume::ModelVolume() = default;

ModelVolume::ModelVolume(TriangleMesh mesh, const std::string& name)
    : _mesh(std::move(mesh))
    , _name(name)
{
}

const TriangleMesh& ModelVolume::mesh() const
{
    return _mesh;
}

TriangleMesh& ModelVolume::mesh()
{
    return _mesh;
}

const std::string& ModelVolume::name() const
{
    return _name;
}

void ModelVolume::setName(const std::string& name)
{
    _name = name;
}

bool ModelVolume::isModifier() const
{
    return _isModifier;
}

void ModelVolume::setModifier(bool modifier)
{
    _isModifier = modifier;
}

const Transform3D& ModelVolume::transform() const
{
    return _transform;
}

void ModelVolume::setTransform(const Transform3D& t)
{
    _transform = t;
}

BoundingBox3D ModelVolume::worldBoundingBox() const
{
    auto bb = _mesh.boundingBox();

    // Apply volume-local transform to bounding box corners
    // For simplicity, we transform the center and extend by scaled half-size
    // TODO: proper oriented bounding box after rotation
    Point3D center = bb.center();
    Point3D halfSize = bb.size() / 2;

    // Apply scale to half size
    coord_t hsx = static_cast<coord_t>(halfSize.x * std::abs(_transform.scaleX));
    coord_t hsy = static_cast<coord_t>(halfSize.y * std::abs(_transform.scaleY));
    coord_t hsz = static_cast<coord_t>(halfSize.z * std::abs(_transform.scaleZ));

    // Apply translation to center (convert mm to microns)
    coord_t cx = center.x + scale(_transform.posX);
    coord_t cy = center.y + scale(_transform.posY);
    coord_t cz = center.z + scale(_transform.posZ);

    // Rotation is ignored for now (conservative AABB)
    return BoundingBox3D(
        Point3D(cx - hsx, cy - hsy, cz - hsz),
        Point3D(cx + hsx, cy + hsy, cz + hsz)
    );
}

double ModelVolume::volumeMm3() const
{
    double v = _mesh.volumeMm3();
    // Apply scale (volume scales by the product of scale factors)
    v *= std::abs(_transform.scaleX * _transform.scaleY * _transform.scaleZ);
    return v;
}

// ============================================================================
// ModelObject
// ============================================================================

ModelObject::ModelObject()
    : _id(_nextId++)
{
}

ModelObject::ModelObject(const std::string& name)
    : _id(_nextId++)
    , _name(name)
{
}

uint64_t ModelObject::id() const
{
    return _id;
}

const std::string& ModelObject::name() const
{
    return _name;
}

void ModelObject::setName(const std::string& name)
{
    _name = name;
}

const std::vector<ModelVolume>& ModelObject::volumes() const
{
    return _volumes;
}

std::vector<ModelVolume>& ModelObject::volumes()
{
    return _volumes;
}

ModelVolume* ModelObject::addVolume(TriangleMesh mesh, const std::string& name)
{
    _volumes.emplace_back(std::move(mesh), name);
    return &_volumes.back();
}

void ModelObject::removeVolume(size_t index)
{
    if (index < _volumes.size())
        _volumes.erase(_volumes.begin() + static_cast<ptrdiff_t>(index));
}

size_t ModelObject::volumeCount() const
{
    return _volumes.size();
}

const Transform3D& ModelObject::placement() const
{
    return _placement;
}

void ModelObject::setPlacement(const Transform3D& t)
{
    _placement = t;
}

void ModelObject::setPosition(double x, double y, double z)
{
    _placement.posX = x;
    _placement.posY = y;
    _placement.posZ = z;
}

void ModelObject::setRotation(double rx, double ry, double rz)
{
    _placement.rotX = rx;
    _placement.rotY = ry;
    _placement.rotZ = rz;
}

void ModelObject::setScale(double sx, double sy, double sz)
{
    _placement.scaleX = sx;
    _placement.scaleY = sy;
    _placement.scaleZ = sz;
}

void ModelObject::setUniformScale(double s)
{
    _placement.scaleX = s;
    _placement.scaleY = s;
    _placement.scaleZ = s;
}

double ModelObject::totalVolumeMm3() const
{
    double total = 0.0;
    for (const auto& vol : _volumes)
    {
        if (!vol.isModifier())
            total += vol.volumeMm3();
    }
    return total;
}

void ModelObject::worldBounds(double& minX, double& minY, double& minZ,
                               double& maxX, double& maxY, double& maxZ) const
{
    minX = minY = minZ = std::numeric_limits<double>::max();
    maxX = maxY = maxZ = std::numeric_limits<double>::lowest();

    for (const auto& vol : _volumes)
    {
        auto bb = vol.worldBoundingBox();

        // Apply object-level placement offset (mm → microns for internal coords)
        coord_t offsetX = scale(_placement.posX);
        coord_t offsetY = scale(_placement.posY);
        coord_t offsetZ = scale(_placement.posZ);

        double vminX = unscalef(bb.min.x + offsetX);
        double vminY = unscalef(bb.min.y + offsetY);
        double vminZ = unscalef(bb.min.z + offsetZ);
        double vmaxX = unscalef(bb.max.x + offsetX);
        double vmaxY = unscalef(bb.max.y + offsetY);
        double vmaxZ = unscalef(bb.max.z + offsetZ);

        if (vminX < minX) minX = vminX;
        if (vminY < minY) minY = vminY;
        if (vminZ < minZ) minZ = vminZ;
        if (vmaxX > maxX) maxX = vmaxX;
        if (vmaxY > maxY) maxY = vmaxY;
        if (vmaxZ > maxZ) maxZ = vmaxZ;
    }
}

bool ModelObject::hasSupport() const
{
    return _hasSupport;
}

void ModelObject::setHasSupport(bool has)
{
    _hasSupport = has;
}

} // namespace slicing
