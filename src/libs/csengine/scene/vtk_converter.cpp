#include <csengine/scene/vtk_converter.hpp>

#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkCleanPolyData.h>
#include <vtkFillHolesFilter.h>
#include <vtkPoints.h>
#include <vtkPolyDataNormals.h>

namespace csengine {

slicing::TriangleMesh triangleMeshFromVTK(vtkPolyData* polyData)
{
    slicing::TriangleMesh mesh;

    if (!polyData) return mesh;

    vtkPoints* points = polyData->GetPoints();
    if (!points) return mesh;

    vtkIdType numPoints = points->GetNumberOfPoints();
    for (vtkIdType i = 0; i < numPoints; ++i)
    {
        double pt[3];
        points->GetPoint(i, pt);
        mesh.addVertex(
            slicing::scale(pt[0]),
            slicing::scale(pt[1]),
            slicing::scale(pt[2]));
    }

    vtkCellArray* polys = polyData->GetPolys();
    if (polys)
    {
        vtkIdType npts;
        const vtkIdType* indices;
        polys->InitTraversal();
        while (polys->GetNextCell(npts, indices))
        {
            if (npts == 3)
            {
                mesh.addTriangle(
                    static_cast<int>(indices[0]),
                    static_cast<int>(indices[1]),
                    static_cast<int>(indices[2]));
            }
            else if (npts > 3)
            {
                for (vtkIdType j = 1; j < npts - 1; ++j)
                {
                    mesh.addTriangle(
                        static_cast<int>(indices[0]),
                        static_cast<int>(indices[j]),
                        static_cast<int>(indices[j + 1]));
                }
            }
        }
    }

    return mesh;
}

vtkPolyData* meshToVTK(const slicing::TriangleMesh& mesh)
{
    vtkNew<vtkPolyData> polyData;

    vtkNew<vtkPoints> points;
    points->SetNumberOfPoints(static_cast<vtkIdType>(mesh.vertexCount()));
    for (size_t i = 0; i < mesh.vertexCount(); ++i)
    {
        const auto& v = mesh.vertices()[i];
        points->SetPoint(static_cast<vtkIdType>(i),
                         slicing::unscale(v.x),
                         slicing::unscale(v.y),
                         slicing::unscale(v.z));
    }
    polyData->SetPoints(points);

    vtkNew<vtkCellArray> polys;
    for (const auto& tri : mesh.triangles())
    {
        polys->InsertNextCell(3);
        polys->InsertCellPoint(tri.v0);
        polys->InsertCellPoint(tri.v1);
        polys->InsertCellPoint(tri.v2);
    }
    polyData->SetPolys(polys);

    polyData->Register(nullptr); // increment refcount for caller
    return polyData;
}

slicing::TriangleMesh repairMesh(const slicing::TriangleMesh& mesh)
{
    // Convert to VTK
    vtkSmartPointer<vtkPolyData> poly(meshToVTK(mesh));

    // Step 1: Clean — merge coincident points
    vtkNew<vtkCleanPolyData> cleaner;
    cleaner->SetInputData(poly);
    cleaner->SetTolerance(0.001); // 1 micron in mm
    cleaner->Update();

    // Step 2: Fill small holes
    vtkNew<vtkFillHolesFilter> holeFiller;
    holeFiller->SetInputConnection(cleaner->GetOutputPort());
    holeFiller->SetHoleSize(10.0); // fill holes up to 10mm
    holeFiller->Update();

    // Step 3: Fix normals (orient consistently outward)
    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(holeFiller->GetOutputPort());
    normals->ConsistencyOn();
    normals->SplittingOff();
    normals->AutoOrientNormalsOn();
    normals->Update();

    // Convert back
    return triangleMeshFromVTK(normals->GetOutput());
}

} // namespace csengine
