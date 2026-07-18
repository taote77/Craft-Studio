#include <QtTest>
#include <slicingcore/mesh/model_object.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

using namespace slicing;

class TestModelObject : public QObject
{
    Q_OBJECT

private:
    TriangleMesh createSmallCube()
    {
        coord_t s = 500;
        TriangleMesh mesh;
        mesh.addVertex(-s, -s, -s);
        mesh.addVertex( s, -s, -s);
        mesh.addVertex( s, -s,  s);
        mesh.addVertex(-s, -s,  s);
        mesh.addVertex(-s,  s, -s);
        mesh.addVertex( s,  s, -s);
        mesh.addVertex( s,  s,  s);
        mesh.addVertex(-s,  s,  s);
        mesh.addTriangle(0, 1, 2); mesh.addTriangle(0, 2, 3);
        mesh.addTriangle(4, 7, 6); mesh.addTriangle(4, 6, 5);
        mesh.addTriangle(0, 3, 7); mesh.addTriangle(0, 7, 4);
        mesh.addTriangle(1, 5, 6); mesh.addTriangle(1, 6, 2);
        mesh.addTriangle(0, 4, 5); mesh.addTriangle(0, 5, 1);
        mesh.addTriangle(3, 2, 6); mesh.addTriangle(3, 6, 7);
        return mesh;
    }

private slots:
    void test_create_object()
    {
        ModelObject obj("TestPart");
        QCOMPARE(obj.name(), "TestPart");
        QVERIFY(obj.id() > 0);
        QCOMPARE(obj.volumeCount(), 0);
    }

    void test_unique_ids()
    {
        ModelObject obj1;
        ModelObject obj2;
        QVERIFY(obj1.id() != obj2.id());
    }

    void test_add_volume()
    {
        ModelObject obj("TestPart");
        TriangleMesh mesh = createSmallCube();

        auto* vol = obj.addVolume(mesh, "body");
        QVERIFY(vol != nullptr);
        QCOMPARE(obj.volumeCount(), 1);
        QCOMPARE(vol->name(), "body");
        QVERIFY(qAbs(vol->volumeMm3() - 1.0) < 0.01);
    }

    void test_total_volume()
    {
        ModelObject obj("MultiPart");

        // Add two 1mm³ cubes
        obj.addVolume(createSmallCube(), "part1");
        obj.addVolume(createSmallCube(), "part2");

        double total = obj.totalVolumeMm3();
        QVERIFY(qAbs(total - 2.0) < 0.02);
    }

    void test_modifier_volume_excluded()
    {
        ModelObject obj("WithModifier");

        obj.addVolume(createSmallCube(), "body");
        auto* modVol = obj.addVolume(createSmallCube(), "modifier");
        modVol->setModifier(true);

        double total = obj.totalVolumeMm3();
        QVERIFY(qAbs(total - 1.0) < 0.01); // modifier excluded
    }

    void test_placement_transform()
    {
        ModelObject obj("Placed");

        obj.setPosition(10.0, 20.0, 0.0);
        QCOMPARE(obj.placement().posX, 10.0);
        QCOMPARE(obj.placement().posY, 20.0);

        obj.setRotation(0.0, 0.0, 45.0);
        QCOMPARE(obj.placement().rotZ, 45.0);

        // Key test: setPosition does NOT reset rotation (the current bug)
        obj.setPosition(30.0, 40.0, 0.0);
        QCOMPARE(obj.placement().posX, 30.0);
        QCOMPARE(obj.placement().rotZ, 45.0); // rotation preserved!
    }

    void test_transform_not_mutually_exclusive()
    {
        // This test verifies the fix for the current transform mutual-exclusion bug
        ModelObject obj("Test");

        obj.setPosition(10.0, 20.0, 30.0);
        obj.setRotation(0.0, 0.0, 45.0);
        obj.setScale(1.5, 1.5, 1.5);

        // All three should be set independently
        QCOMPARE(obj.placement().posX, 10.0);
        QCOMPARE(obj.placement().posY, 20.0);
        QCOMPARE(obj.placement().posZ, 30.0);
        QCOMPARE(obj.placement().rotZ, 45.0);
        QCOMPARE(obj.placement().scaleX, 1.5);
        QCOMPARE(obj.placement().scaleY, 1.5);
        QCOMPARE(obj.placement().scaleZ, 1.5);
    }

    void test_uniform_scale()
    {
        ModelObject obj("Test");
        obj.setUniformScale(2.0);
        QCOMPARE(obj.placement().scaleX, 2.0);
        QCOMPARE(obj.placement().scaleY, 2.0);
        QCOMPARE(obj.placement().scaleZ, 2.0);
    }

    void test_world_bounds()
    {
        ModelObject obj("Test");
        obj.addVolume(createSmallCube(), "cube");
        obj.setPosition(10.0, 0.0, 0.0); // shift 10mm in X

        double minX, minY, minZ, maxX, maxY, maxZ;
        obj.worldBounds(minX, minY, minZ, maxX, maxY, maxZ);

        // Cube is 1mm³, centered at origin, shifted 10mm in X
        QVERIFY(qAbs(minX - 9.5) < 0.05);  // 10 - 0.5
        QVERIFY(qAbs(maxX - 10.5) < 0.05); // 10 + 0.5
        QVERIFY(qAbs(minY + 0.5) < 0.05);  // -0.5
        QVERIFY(qAbs(maxY - 0.5) < 0.05);  //  0.5
    }

    void test_support_flag()
    {
        ModelObject obj("Test");
        QVERIFY(!obj.hasSupport());
        obj.setHasSupport(true);
        QVERIFY(obj.hasSupport());
    }

    void test_remove_volume()
    {
        ModelObject obj("Test");
        obj.addVolume(createSmallCube(), "v1");
        obj.addVolume(createSmallCube(), "v2");
        QCOMPARE(obj.volumeCount(), 2);

        obj.removeVolume(0);
        QCOMPARE(obj.volumeCount(), 1);
        QCOMPARE(obj.volumes()[0].name(), "v2");
    }
};

// Test class compiled into the main test executable
// main() is in main.cpp
QTEST_APPLESS_MAIN(TestModelObject)
#include "test_model_object.moc"
