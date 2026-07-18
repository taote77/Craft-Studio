#ifndef CSBRIDGE_SLICING_CONTROLLER_HPP
#define CSBRIDGE_SLICING_CONTROLLER_HPP

#include <csbridge/csbridge_global.h>

#include <slicingcore/pipeline/slice_pipeline.hpp>

#include <QObject>
#include <QThread>
#include <QString>
#include <memory>
#include <atomic>

namespace slicing {
    class ModelObject;
    class SlicePipeline;
}

namespace csbridge {

// ============================================================================
// SliceWorker — runs SlicePipeline on a background thread
// ============================================================================

class SliceWorker : public QObject
{
    Q_OBJECT

public:
    SliceWorker(const std::vector<slicing::ModelObject*>& objects,
                slicing::coord_t layerHeight,
                slicing::SlicePipeline::State startFrom = slicing::SlicePipeline::Importing,
                QObject* parent = nullptr);

public slots:
    /// Main entry point — called on worker thread
    void doWork();

    /// Request cancellation
    void cancel();

    /// Access the pipeline results
    slicing::SlicePipeline* pipeline() const;

signals:
    void stageChanged(int state);
    void progressChanged(int percent);
    void logMessage(const QString& message);
    void finished(bool success);

private:
    std::vector<slicing::ModelObject*> _objects;
    slicing::coord_t _layerHeight;
    slicing::SlicePipeline::State _startFrom;
    slicing::SlicePipeline _pipeline;
};

// ============================================================================
// SlicingController — orchestrates slicing on a background QThread
//
// Usage:
//   SlicingController controller;
//   connect(&controller, &SlicingController::sliceFinished, ...);
//   controller.startSlice(objects, layerHeight);
//
// The controller owns a QThread and moves a SliceWorker onto it.
// Progress and completion signals are emitted on the main thread
// via queued connections.
// ============================================================================

class CSBRIDGE_API SlicingController : public QObject
{
    Q_OBJECT

public:
    explicit SlicingController(QObject* parent = nullptr);
    ~SlicingController() override;

    /// Start slicing on a background thread. Safe to call from main thread.
    /// @param startFrom  Skip stages before this (enables incremental re-slice)
    void startSlice(const std::vector<slicing::ModelObject*>& objects,
                    slicing::coord_t layerHeight,
                    slicing::SlicePipeline::State startFrom = slicing::SlicePipeline::Importing);

    /// Request cancellation. Non-blocking.
    void cancelSlice();

    /// Whether a slice operation is currently in progress
    bool isSlicing() const;

    /// Access the last completed pipeline (nullptr if never run)
    slicing::SlicePipeline* lastPipeline() const;

signals:
    void sliceStarted();
    void stageChanged(int state);
    void progressChanged(int percent);
    void logMessage(const QString& message);
    void sliceFinished(bool success, const QString& errorMessage);

private slots:
    void onWorkerFinished(bool success);
    void onWorkerStageChanged(int state);

private:
    QThread _workerThread;
    SliceWorker* _worker = nullptr;
    std::atomic<bool> _slicing{false};

    // Last completed pipeline result (owned by the worker, reset on new slice)
    slicing::SlicePipeline* _lastPipeline = nullptr;
};

} // namespace csbridge

#endif // CSBRIDGE_SLICING_CONTROLLER_HPP
