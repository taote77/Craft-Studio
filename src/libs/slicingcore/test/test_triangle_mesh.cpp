#include <QtTest>
#include <slicingcore/mesh/triangle_mesh.hpp>

using namespace slicing;

class TestTriangleMesh : public QObject
{
    Q_OBJECT

private:
    /// Create a unit cube centered at origin, side length 1000 microns (1mm)
    ///   7----6
    ///  /|   /|
    /// 4----5 |
    /// | 3--|-2
    /// |/   |/
    /// 0----1
    TriangleMesh createUnitCube()
    {
        coord_t s = 500; // half-side = 500 microns
        TriangleMesh mesh;
        mesh.addVertex(-s, -s, -s); // 0
        mesh.addVertex( s, -s, -s); // 1
        mesh.addVertex( s, -s,  s); // 2
        mesh.addVertex(-s, -s,  s); // 3
        mesh.addVertex(-s,  s, -s); // 4
        mesh.addVertex( s,  s, -s); // 5
        mesh.addVertex( s,  s,  s); // 6
        mesh.addVertex(-s,  s,  s); // 7

        // Bottom face (CCW looking from outside, i.e. from below)
        mesh.addTriangle(0, 1, 2);
        mesh.addTriangle(0, 2, 3);

        // Top face
        mesh.addTriangle(4, 7, 6);
        mesh.addTriangle(4, 6, 5);

        // Front face (y = -s)
        mesh.addTriangle(0, 3, 7);
        mesh.addTriangle(0, 7, 4);

        // Back face (y = s)
        mesh.addTriangle(1, 5, 6);
        mesh.addTriangle(1, 6, 2);

        // Left face (x = -s)
        mesh.addTriangle(0, 4, 5);
        mesh.addTriangle(0, 5, 1);

        // Right face (x = s)
        mesh.addTriangle(3, 2, 6);
        mesh.addTriangle(3, 6, 7);

        return mesh;
    }

private slots:
    void test_empty_mesh()
    {
        TriangleMesh mesh;
        QCOMPARE(mesh.vertexCount(), 0);
        QCOMPARE(mesh.triangleCount(), 0);
        QCOMPARE(mesh.volume(), 0.0);
    }

    void test_add_vertex_triangle()
    {
        TriangleMesh mesh;
        int v0 = mesh.addVertex(0, 0, 0);
        int v1 = mesh.addVertex(10, 0, 0);
        int v2 = mesh.addVertex(0, 10, 0);

        QCOMPARE(v0, 0);
        QCOMPARE(v1, 1);
        QCOMPARE(v2, 2);
        QCOMPARE(mesh.vertexCount(), 3);

        mesh.addTriangle(v0, v1, v2);
        QCOMPARE(mesh.triangleCount(), 1);
    }

    void test_cube_volume()
    {
        // Unit cube: side = 1000 microns = 1 mm
        // Volume = 1 mm³
        TriangleMesh mesh = createUnitCube();

        double volumeMm3 = mesh.volumeMm3();
        QVERIFY(qAbs(volumeMm3 - 1.0) < 0.01); // within 1% of 1 mm³
    }

    void test_cube_watertight()
    {
        TriangleMesh mesh = createUnitCube();
        QVERIFY(mesh.isWatertight());
    }

    void test_non_watertight()
    {
        // Create a mesh with a missing face (not watertight)
        TriangleMesh mesh;
        mesh.addVertex(0, 0, 0);   // 0
        mesh.addVertex(1000, 0, 0); // 1
        mesh.addVertex(1000, 0, 1000); // 2
        mesh.addVertex(0, 0, 1000); // 3

        // Only bottom face, missing top and sides
        mesh.addTriangle(0, 2, 1);
        mesh.addTriangle(0, 3, 2);

        QVERIFY(!mesh.isWatertight());
    }

    void test_bounding_box()
    {
        TriangleMesh mesh = createUnitCube();
        BoundingBox3D bb = mesh.boundingBox();

        // Half-side = 500 microns
        QCOMPARE(bb.min.x, -500);
        QCOMPARE(bb.min.y, -500);
        QCOMPARE(bb.min.z, -500);
        QCOMPARE(bb.max.x,  500);
        QCOMPARE(bb.max.y,  500);
        QCOMPARE(bb.max.z,  500);
    }

    void test_translate()
    {
        TriangleMesh mesh = createUnitCube();
        mesh.translate(1000, 0, 0);

        BoundingBox3D bb = mesh.boundingBox();
        QCOMPARE(bb.min.x, 500);   // -500 + 1000
        QCOMPARE(bb.max.x, 1500);  //  500 + 1000
    }

    void test_scale()
    {
        TriangleMesh mesh = createUnitCube();
        mesh.scale(2.0);

        BoundingBox3D bb = mesh.boundingBox();
        QCOMPARE(bb.min.x, -1000);
        QCOMPARE(bb.max.x,  1000);

        // Volume should be 8x: 8 mm³
        double v = mesh.volumeMm3();
        QVERIFY(qAbs(v - 8.0) < 0.1);
    }

    void test_merge()
    {
        TriangleMesh mesh1 = createUnitCube();
        TriangleMesh mesh2 = createUnitCube();
        mesh2.translate(2000, 0, 0);

        mesh1.merge(mesh2);
        QCOMPARE(mesh1.vertexCount(), 16);
        QCOMPARE(mesh1.triangleCount(), 24);
    }

    void test_clear()
    {
        TriangleMesh mesh = createUnitCube();
        mesh.clear();
        QCOMPARE(mesh.vertexCount(), 0);
        QCOMPARE(mesh.triangleCount(), 0);
        QCOMPARE(mesh.volume(), 0.0);
    }

    void test_repair_removes_degenerate()
    {
        TriangleMesh mesh;
        mesh.addVertex(0, 0, 0);
        mesh.addVertex(10, 0, 0);
        mesh.addVertex(0, 10, 0);
        mesh.addVertex(5, 5, 0);

        // Good triangle
        mesh.addTriangle(0, 1, 2);
        // Degenerate (v1 == v2)
        mesh.addTriangle(0, 3, 3);

        mesh.repair();
        QCOMPARE(mesh.triangleCount(), 1);
    }
};

// Test class compiled into the main test executable
// main() is in main.cpp
QTEST_APPLESS_MAIN(TestTriangleMesh)
#include "test_triangle_mesh.moc"
