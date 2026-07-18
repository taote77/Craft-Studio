#include <QtTest>
#include <slicingcore/pipeline/slice_pipeline.hpp>
#include <slicingcore/slice/layer.hpp>
#include <slicingcore/mesh/model_object.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

using namespace slicing;

// Helper: create a small test cube
static TriangleMesh createTestCube()
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

class TestSlicePipeline : public QObject
{
    Q_OBJECT

private slots:
    void test_initial_state()
    {
        SlicePipeline pipeline;
        QCOMPARE(pipeline.state(), SlicePipeline::Idle);
        QCOMPARE(pipeline.totalLayerCount(), 0);
    }

    void test_empty_objects_fails()
    {
        SlicePipeline pipeline;
        QSignalSpy spy(&pipeline, &SlicePipeline::finished);

        pipeline.execute({}, scale(0.2));

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toBool(), false);
        QCOMPARE(pipeline.state(), SlicePipeline::Failed);
        QVERIFY(!pipeline.errorMessage().isEmpty());
    }

    void test_invalid_layer_height_fails()
    {
        SlicePipeline pipeline;
        auto* obj = new ModelObject("Test");
        obj->addVolume(createTestCube(), "body");

        QSignalSpy spy(&pipeline, &SlicePipeline::finished);
        pipeline.execute({ obj }, 0); // zero layer height

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toBool(), false);

        delete obj;
    }

    void test_successful_pipeline_run()
    {
        SlicePipeline pipeline;
        auto* obj = new ModelObject("Test");
        obj->addVolume(createTestCube(), "body");

        QSignalSpy stageSpy(&pipeline, &SlicePipeline::stageChanged);
        QSignalSpy progressSpy(&pipeline, &SlicePipeline::progress);
        QSignalSpy finishedSpy(&pipeline, &SlicePipeline::finished);

        pipeline.execute({ obj }, scale(0.2)); // 0.2mm = 200 microns

        QCOMPARE(finishedSpy.count(), 1);
        QCOMPARE(finishedSpy.takeFirst().at(0).toBool(), true);
        QCOMPARE(pipeline.state(), SlicePipeline::Completed);

        // Should have visited all 6 stages + Completed
        QVERIFY(stageSpy.count() >= 7);

        // Should have layers (1mm cube / 0.2mm layer = 5 layers)
        QVERIFY(pipeline.totalLayerCount() > 0);

        delete obj;
    }

    void test_stage_order()
    {
        SlicePipeline pipeline;
        auto* obj = new ModelObject("Test");
        obj->addVolume(createTestCube(), "body");

        QSignalSpy stageSpy(&pipeline, &SlicePipeline::stageChanged);

        pipeline.execute({ obj }, scale(0.2));

        // Verify stages are in correct order
        QList<int> stages;
        for (int i = 0; i < stageSpy.count(); ++i)
            stages.append(stageSpy.at(i).at(0).toInt());

        QVERIFY(stages.contains(SlicePipeline::Importing));
        QVERIFY(stages.contains(SlicePipeline::Slicing));
        QVERIFY(stages.contains(SlicePipeline::GeneratingPerimeters));
        QVERIFY(stages.contains(SlicePipeline::GeneratingInfill));
        QVERIFY(stages.contains(SlicePipeline::GeneratingSupport));
        QVERIFY(stages.contains(SlicePipeline::GeneratingGCode));
        QVERIFY(stages.contains(SlicePipeline::Completed));

        // Check relative order: Import before Slice before Perimeters
        int importIdx = stages.indexOf(SlicePipeline::Importing);
        int sliceIdx = stages.indexOf(SlicePipeline::Slicing);
        int perimIdx = stages.indexOf(SlicePipeline::GeneratingPerimeters);
        QVERIFY(importIdx < sliceIdx);
        QVERIFY(sliceIdx < perimIdx);

        delete obj;
    }

    void test_layer_properties()
    {
        SlicePipeline pipeline;
        auto* obj = new ModelObject("Test");
        obj->addVolume(createTestCube(), "body");

        pipeline.execute({ obj }, scale(0.2));

        const auto& layers = pipeline.layers();
        QVERIFY(!layers.empty());

        // Layers should be sequential in Z
        for (size_t i = 1; i < layers.size(); ++i)
            QVERIFY(layers[i].z() > layers[i - 1].z());

        // Min/max should match
        QCOMPARE(layers.front().z(), pipeline.minZ());
        QVERIFY(pipeline.maxZ() >= layers.back().z());

        delete obj;
    }
};

QTEST_APPLESS_MAIN(TestSlicePipeline)
#include "test_slice_pipeline.moc"
