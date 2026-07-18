#include <QtTest>
#include <slicingcore/geometry/coord.hpp>
#include <slicingcore/geometry/ex_polygon.hpp>

using namespace slicing;

class TestCoord : public QObject
{
    Q_OBJECT

private slots:
    void test_scale_unscale()
    {
        // 1 mm = 1000 microns
        QCOMPARE(scale(1.0), 1000);
        QCOMPARE(scale(0.5), 500);
        QCOMPARE(scale(0.001), 1);
        QCOMPARE(scale(100.0), 100000);

        // Round-trip
        QCOMPARE(unscale(scale(3.14159)), 3.142); // rounding to 0.001
    }

    void test_point_arithmetic()
    {
        Point a(10, 20);
        Point b(5, 8);

        Point sum = a + b;
        QCOMPARE(sum.x, 15);
        QCOMPARE(sum.y, 28);

        Point diff = a - b;
        QCOMPARE(diff.x, 5);
        QCOMPARE(diff.y, 12);

        Point scaled = a * 3;
        QCOMPARE(scaled.x, 30);
        QCOMPARE(scaled.y, 60);

        Point div = a / 2;
        QCOMPARE(div.x, 5);
        QCOMPARE(div.y, 10);
    }

    void test_point_3d()
    {
        Point3D a(1, 2, 3);
        Point3D b(4, 6, 8);

        Point3D sum = a + b;
        QCOMPARE(sum.x, 5);
        QCOMPARE(sum.y, 8);
        QCOMPARE(sum.z, 11);

        // Distance: (3,4,5) → sqrt(9+16+25) = sqrt(50) ≈ 7.071
        double d = a.dist(b);
        QVERIFY(qAbs(d - 7.071) < 0.01);
    }

    void test_bounding_box_2d()
    {
        BoundingBox2D bb;
        QVERIFY(!bb.valid());

        bb.extend(Point(0, 0));
        bb.extend(Point(100, 200));
        bb.extend(Point(50, 300));

        QVERIFY(bb.valid());
        QCOMPARE(bb.width(), 100);
        QCOMPARE(bb.height(), 300);

        Point center = bb.center();
        QCOMPARE(center.x, 50);
        QCOMPARE(center.y, 150);

        QVERIFY(bb.contains(Point(50, 150)));
        QVERIFY(!bb.contains(Point(-1, 0)));
    }

    void test_bounding_box_3d()
    {
        BoundingBox3D bb;
        bb.extend(Point3D(0, 0, 0));
        bb.extend(Point3D(1000, 2000, 3000));

        QCOMPARE(bb.width(), 1000);
        QCOMPARE(bb.height(), 2000);
        QCOMPARE(bb.depth(), 3000);

        Point3D c = bb.center();
        QCOMPARE(c.x, 500);
        QCOMPARE(c.y, 1000);
        QCOMPARE(c.z, 1500);
    }

    void test_ex_polygon()
    {
        Polygon contour = { Point(0,0), Point(100,0), Point(100,100), Point(0,100) };
        ExPolygon expoly(contour);

        QVERIFY(!expoly.empty());
        QCOMPARE(expoly.vertexCount(), 4);

        // Add a hole
        Polygon hole = { Point(25,25), Point(25,75), Point(75,75), Point(75,25) };
        expoly.holes.push_back(hole);
        QCOMPARE(expoly.vertexCount(), 8);

        BoundingBox2D bb = expoly.boundingBox();
        QCOMPARE(bb.min.x, 0);
        QCOMPARE(bb.min.y, 0);
        QCOMPARE(bb.max.x, 100);
        QCOMPARE(bb.max.y, 100);
    }
};

// Test class compiled into the main test executable
// main() is in main.cpp
QTEST_APPLESS_MAIN(TestCoord)
#include "test_coord.moc"
