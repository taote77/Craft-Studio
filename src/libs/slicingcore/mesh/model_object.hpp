#ifndef SLICINGCORE_MODEL_OBJECT_HPP
#define SLICINGCORE_MODEL_OBJECT_HPP

#include <slicingcore/slicingcore_global.h>
#include <slicingcore/geometry/coord.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace slicing {

// ============================================================================
// Transform3D — placement of an object on the build plate
//
// Stored as data (position, rotation Euler angles, scale), NOT as a matrix.
// This avoids the mutual-exclusion bug in the current SceneObjectV2 where
// setPosition/resets rotation because they all modify the same vtkTransform.
// ============================================================================

struct SLICINGCORE_API Transform3D
{
    double posX = 0.0, posY = 0.0, posZ = 0.0;   // translation in mm
    double rotX = 0.0, rotY = 0.0, rotZ = 0.0;   // Euler angles in degrees
    double scaleX = 1.0, scaleY = 1.0, scaleZ = 1.0; // uniform or non-uniform scale

    Transform3D() = default;

    bool isIdentity() const
    {
        return posX == 0.0 && posY == 0.0 && posZ == 0.0
            && rotX == 0.0 && rotY == 0.0 && rotZ == 0.0
            && scaleX == 1.0 && scaleY == 1.0 && scaleZ == 1.0;
    }

    bool isUniformScale() const
    {
        return scaleX == scaleY && scaleY == scaleZ;
    }
};

// ============================================================================
// ModelVolume — a single volume within a ModelObject
//
// Each volume has its own mesh. Multi-volume objects (e.g. multi-material
// or assemblies) contain multiple ModelVolumes. Modifier volumes are special
// volumes that alter print settings in their region rather than being printed.
// ============================================================================

class SLICINGCORE_API ModelVolume
{
public:
    ModelVolume();
    explicit ModelVolume(TriangleMesh mesh, const std::string& name = "");

    const TriangleMesh& mesh() const;
    TriangleMesh& mesh();

    const std::string& name() const;
    void setName(const std::string& name);

    bool isModifier() const;
    void setModifier(bool modifier);

    const Transform3D& transform() const;
    void setTransform(const Transform3D& t);

    /// Bounding box in world space (after transform)
    BoundingBox3D worldBoundingBox() const;

    /// Volume in mm³
    double volumeMm3() const;

private:
    TriangleMesh _mesh;
    std::string _name;
    bool _isModifier = false;
    Transform3D _transform; // volume-local transform within the object
};

// ============================================================================
// ModelObject — a printable item on the build plate
//
// A ModelObject represents one object placed on the build plate. It can
// contain multiple volumes (parts). This replaces SceneObjectV2 as the
// canonical "thing on the build plate" representation.
// ============================================================================

class SLICINGCORE_API ModelObject
{
public:
    ModelObject();
    explicit ModelObject(const std::string& name);

    // -- Identity -----------------------------------------------------------

    uint64_t id() const;
    const std::string& name() const;
    void setName(const std::string& name);

    // -- Volumes ------------------------------------------------------------

    const std::vector<ModelVolume>& volumes() const;
    std::vector<ModelVolume>& volumes();

    ModelVolume* addVolume(TriangleMesh mesh, const std::string& name = "");
    void removeVolume(size_t index);

    size_t volumeCount() const;

    // -- Placement on Build Plate -------------------------------------------

    const Transform3D& placement() const;
    void setPlacement(const Transform3D& t);

    // Convenience setters (do NOT mutually exclude each other!)
    void setPosition(double x, double y, double z);
    void setRotation(double rx, double ry, double rz);
    void setScale(double sx, double sy, double sz);
    void setUniformScale(double s);

    // -- Computed Properties ------------------------------------------------

    /// Total volume of all non-modifier volumes (mm³)
    double totalVolumeMm3() const;

    /// Combined bounding box of all volumes in world space (mm)
    void worldBounds(double& minX, double& minY, double& minZ,
                     double& maxX, double& maxY, double& maxZ) const;

    /// Check if this object has any support structures
    bool hasSupport() const;
    void setHasSupport(bool has);

    // -- Import Metadata (set by FileImporter) ------------------------------
    void setMeshInfo(const std::string& info) { _meshInfo = info; }
    const std::string& meshInfo() const { return _meshInfo; }
    void setImportVolume(double v) { _importVolume = v; }
    double importVolumeMm3() const { return _importVolume; }
    void setImportWeight(double w) { _importWeight = w; }
    double importWeightG() const { return _importWeight; }

    // -- Print Settings (per-object overrides) ------------------------------

private:
    static uint64_t _nextId;

    uint64_t _id;
    std::string _name;
    std::vector<ModelVolume> _volumes;
    Transform3D _placement;
    bool _hasSupport = false;
    std::string _meshInfo;
    double _importVolume = 0.0;
    double _importWeight = 0.0;
};

} // namespace slicing

#endif // SLICINGCORE_MODEL_OBJECT_HPP
