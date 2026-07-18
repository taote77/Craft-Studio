#ifndef CSENGINE_VTK_CONVERTER_HPP
#define CSENGINE_VTK_CONVERTER_HPP

#include <csengine/csengine_global.h>
#include <slicingcore/mesh/triangle_mesh.hpp>

class vtkPolyData;

namespace csengine {

/// Convert VTK vtkPolyData → slicing::TriangleMesh
CSENGINE_API slicing::TriangleMesh triangleMeshFromVTK(vtkPolyData* polyData);

/// Convert slicing::TriangleMesh → vtkPolyData (caller takes ownership: Register() called)
CSENGINE_API vtkPolyData* meshToVTK(const slicing::TriangleMesh& mesh);

/// Repair a mesh using VTK filters: clean, fill holes, fix normals
/// Returns the repaired mesh. Original is unchanged if repair fails.
CSENGINE_API slicing::TriangleMesh repairMesh(const slicing::TriangleMesh& mesh);

} // namespace csengine

#endif
