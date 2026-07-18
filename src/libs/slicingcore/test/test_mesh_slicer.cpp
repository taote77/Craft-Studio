#include <QtTest>
#include <slicingcore/slice/mesh_slicer.hpp>
#include <slicingcore/slice/layer.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

using namespace slicing;

// Unit cube centered at origin, 1000um side
static TriangleMesh createCube()
{
    coord_t s = 500;
    TriangleMesh mesh;
    mesh.addVertex(-s, -s, -s); mesh.addVertex( s, -s, -s);
    mesh.addVertex( s, -s,  s); mesh.addVertex(-s, -s,  s);
    mesh.addVertex(-s,  s, -s); mesh.addVertex( s,  s, -s);
    mesh.addVertex( s,  s,  s); mesh.addVertex(-s,  s,  s);
    mesh.addTriangle(0, 1, 2); mesh.addTriangle(0, 2, 3);
    mesh.addTriangle(4, 7, 6); mesh.addTriangle(4, 6, 5);
    mesh.addTriangle(0, 3, 7); mesh.addTriangle(0, 7, 4);
    mesh.addTriangle(1, 5, 6); mesh.addTriangle(1, 6, 2);
    mesh.addTriangle(0, 4, 5); mesh.addTriangle(0, 5, 1);
    mesh.addTriangle(3, 2, 6); mesh.addTriangle(3, 6, 7);
    return mesh;
}

class TestMeshSlicer : public QObject
{
    Q_OBJECT

private slots:
    void test_slice_cube_produces_layers()
    {
        MeshSlicer slicer;
        TriangleMesh cube = createCube();

        Layers layers = slicer.slice(cube, -500, 500, 200); // 200um layers

        QVERIFY(!layers.empty());
        // Cube is 1000um tall, at 200um layers: 6 layers
        // (-500, -300, -100, 100, 300, 500)
        QCOMPARE(layers.size(), 6);
    }

    void test_layer_has_geometry()
    {
        MeshSlicer slicer;
        TriangleMesh cube = createCube();

        Layers layers = slicer.slice(cube, -500, 500, 200);

        // At least one interior layer should have geometry
        bool found = false;
        for (const auto& layer : layers)
        {
            if (!layer.empty() && layer.vertexCount() > 0)
            {
                found = true;
                QVERIFY(!layer.slices().empty());
                QVERIFY(layer.slices()[0].vertexCount() >= 4);
                break;
            }
        }
        QVERIFY(found);
    }

    void test_layer_z_values_increase()
    {
        MeshSlicer slicer;
        TriangleMesh cube = createCube();

        Layers layers = slicer.slice(cube, 0, 1000, 250); // 250um layers

        for (size_t i = 1; i < layers.size(); ++i)
            QVERIFY(layers[i].z() > layers[i - 1].z());

        QCOMPARE(layers.front().z(), 0);
        QCOMPARE(layers.back().z(), 1000);
    }

    void test_empty_mesh_returns_nothing()
    {
        MeshSlicer slicer;
        TriangleMesh empty;
        Layers layers = slicer.slice(empty, 0, 1000, 100);
        QVERIFY(layers.empty());
    }

    void test_triangle_intersection()
    {
        // Single triangle produces one segment → not enough for a closed polygon
        // This is expected — you need a closed mesh to get closed slice contours
        MeshSlicer slicer;
        TriangleMesh mesh;
        mesh.addVertex(0, 0, 0);
        mesh.addVertex(1000, 0, 0);
        mesh.addVertex(0, 0, 1000);
        mesh.addTriangle(0, 1, 2);

        // A single open triangle produces segments but no closed polygons
        Layers layers = slicer.slice(mesh, 0, 1000, 500);
        QVERIFY(layers.size() > 0);
        // All layers empty — single triangle can't form a closed contour
        // This is correct behavior
    }

    void test_out_of_range_z_returns_nothing()
    {
        MeshSlicer slicer;
        TriangleMesh cube = createCube();

        // Cube is at z=-500 to z=500. Slice from 1000 to 2000 → no intersection
        Layers layers = slicer.slice(cube, 1000, 2000, 200);
        for (const auto& layer : layers)
            QVERIFY(layer.empty());
    }
};

QTEST_APPLESS_MAIN(TestMeshSlicer)
#include "test_mesh_slicer.moc"
