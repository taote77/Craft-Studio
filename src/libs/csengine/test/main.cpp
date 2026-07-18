#include <QtTest>
#include <csengine/scene/scene_document.hpp>
#include <csengine/command/command.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

using namespace csengine;
using namespace slicing;

// ============================================================================
// Helper: create a small test cube mesh
// ============================================================================
static TriangleMesh createTestCube()
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

// ============================================================================
// TestSceneDocument
// ============================================================================
class TestSceneDocument : public QObject
{
    Q_OBJECT

private slots:
    void test_create_document()
    {
        SceneDocument doc;
        QCOMPARE(doc.objects().size(), 0);
        QVERIFY(!doc.hasSelection());
    }

    void test_add_model()
    {
        SceneDocument doc;
        TriangleMesh mesh = createTestCube();
        auto* obj = doc.addModel(mesh, "Cube");

        QVERIFY(obj != nullptr);
        QCOMPARE(obj->name(), "Cube");
        QCOMPARE(doc.objects().size(), 1);
        QCOMPARE(doc.objects()[0], obj);
    }

    void test_add_model_signal()
    {
        SceneDocument doc;
        QSignalSpy spy(&doc, &SceneDocument::objectsAdded);

        doc.addModel(createTestCube(), "Test");

        QCOMPARE(spy.count(), 1);
        auto args = spy.takeFirst();
        QCOMPARE(args.at(0).value<QList<ModelObject*>>().size(), 1);
    }

    void test_remove_model()
    {
        SceneDocument doc;
        auto* obj = doc.addModel(createTestCube(), "Cube");
        QCOMPARE(doc.objects().size(), 1);

        QSignalSpy spy(&doc, &SceneDocument::objectsRemoved);
        doc.removeModels({ obj });
        QCOMPARE(spy.count(), 1);
        QCOMPARE(doc.objects().size(), 0);
    }

    void test_clear_all()
    {
        SceneDocument doc;
        doc.addModel(createTestCube(), "A");
        doc.addModel(createTestCube(), "B");
        doc.addModel(createTestCube(), "C");

        doc.clearAll();
        QCOMPARE(doc.objects().size(), 0);
        QVERIFY(!doc.hasSelection());
    }

    void test_selection()
    {
        SceneDocument doc;
        auto* a = doc.addModel(createTestCube(), "A");
        auto* b = doc.addModel(createTestCube(), "B");

        // Single select
        doc.setSelected(a);
        QCOMPARE(doc.selection().size(), 1);
        QCOMPARE(doc.selection()[0], a);
        QVERIFY(doc.hasSelection());

        // Additive select
        doc.setSelected(b, true);
        QCOMPARE(doc.selection().size(), 2);

        // Clear
        doc.clearSelection();
        QCOMPARE(doc.selection().size(), 0);
        QVERIFY(!doc.hasSelection());

        // Select all
        doc.selectAll();
        QCOMPARE(doc.selection().size(), 2);
    }

    void test_find_object()
    {
        SceneDocument doc;
        auto* obj = doc.addModel(createTestCube(), "X");
        uint64_t id = obj->id();

        QCOMPARE(doc.findObject(id), obj);
        QVERIFY(doc.findObject(99999) == nullptr);
    }

    void test_translate()
    {
        SceneDocument doc;
        auto* obj = doc.addModel(createTestCube(), "Cube");

        doc.translate({ obj }, 10.0, 20.0, 0.0);
        QCOMPARE(obj->placement().posX, 10.0);
        QCOMPARE(obj->placement().posY, 20.0);
    }

    void test_rotate()
    {
        SceneDocument doc;
        auto* obj = doc.addModel(createTestCube(), "Cube");

        doc.rotate({ obj }, 0.0, 0.0, 45.0);
        QCOMPARE(obj->placement().rotZ, 45.0);
    }

    void test_transform_not_mutually_exclusive()
    {
        // Key regression test: the current codebase bug where setPosition clears rotation
        SceneDocument doc;
        auto* obj = doc.addModel(createTestCube(), "Cube");

        doc.translate({ obj }, 10.0, 20.0, 0.0);
        doc.rotate({ obj }, 0.0, 0.0, 45.0);

        // Position should be preserved after rotation
        QCOMPARE(obj->placement().posX, 10.0);
        QCOMPARE(obj->placement().posY, 20.0);
        QCOMPARE(obj->placement().rotZ, 45.0);
    }
};

// ============================================================================
// TestCommandStack
// ============================================================================
class TestCommandStack : public QObject
{
    Q_OBJECT

private slots:
    void test_empty_stack()
    {
        CommandStack stack;
        QVERIFY(!stack.canUndo());
        QVERIFY(!stack.canRedo());
        QVERIFY(stack.undoText().isEmpty());
    }

    void test_undo_translate()
    {
        SceneDocument doc;
        auto* obj = doc.addModel(createTestCube(), "Cube");

        double origX = obj->placement().posX;
        doc.translate({ obj }, 100.0, 0.0, 0.0);

        auto* stack = doc.commandStack();
        QVERIFY(stack->canUndo());
        QCOMPARE(obj->placement().posX, origX + 100.0);

        stack->undo();
        QCOMPARE(obj->placement().posX, origX); // back to original

        QVERIFY(stack->canRedo());
        stack->redo();
        QCOMPARE(obj->placement().posX, origX + 100.0);
    }

    void test_undo_add_model()
    {
        SceneDocument doc;
        QCOMPARE(doc.objects().size(), 0);

        doc.addModel(createTestCube(), "Test");
        QCOMPARE(doc.objects().size(), 1);

        doc.commandStack()->undo();
        QCOMPARE(doc.objects().size(), 0);

        doc.commandStack()->redo();
        QCOMPARE(doc.objects().size(), 1);
    }

    void test_undo_remove_model()
    {
        SceneDocument doc;
        auto* obj = doc.addModel(createTestCube(), "Test");
        QString originalName = QString::fromStdString(obj->name());

        doc.removeModels({ obj });
        QCOMPARE(doc.objects().size(), 0);

        doc.commandStack()->undo();
        QCOMPARE(doc.objects().size(), 1);
        // Restored object has the same name (new ID is expected since it's recreated)
        QCOMPARE(QString::fromStdString(doc.objects()[0]->name()), originalName);
    }

    void test_max_commands()
    {
        SceneDocument doc;
        auto* stack = doc.commandStack();
        stack->setMaxCommands(3);

        auto* obj = doc.addModel(createTestCube(), "Base");

        // Push 5 translations
        for (int i = 0; i < 5; ++i)
            doc.translate({ obj }, 1.0, 0.0, 0.0);

        // Should have at most 3 undos (oldest 2 trimmed)
        int count = 0;
        while (stack->canUndo())
        {
            stack->undo();
            count++;
        }
        QVERIFY(count <= 3);
    }

    void test_command_state_signal()
    {
        SceneDocument doc;
        QSignalSpy spy(doc.commandStack(), &CommandStack::stateChanged);

        doc.addModel(createTestCube(), "Test");
        QCOMPARE(spy.count(), 1); // execute

        doc.commandStack()->undo();
        QCOMPARE(spy.count(), 2); // undo

        doc.commandStack()->redo();
        QCOMPARE(spy.count(), 3); // redo
    }
};

// ============================================================================
// Main
// ============================================================================
int main(int argc, char* argv[])
{
    int result = 0;

    auto runTest = [&](QObject* test, const char* name) {
        int r = QTest::qExec(test, argc, argv);
        if (r != 0)
            qWarning("%s tests FAILED", name);
        result |= r;
        delete test;
    };

    runTest(new TestSceneDocument, "TestSceneDocument");
    runTest(new TestCommandStack, "TestCommandStack");

    return result;
}

#include "main.moc"
