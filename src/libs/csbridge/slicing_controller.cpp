#include <csbridge/slicing_controller.hpp>
#include <slicingcore/mesh/model_object.hpp>

#include <QDebug>

namespace csbridge {

// ============================================================================
// SliceWorker
// ============================================================================

SliceWorker::SliceWorker(const std::vector<slicing::ModelObject*>& objects,
                         slicing::coord_t layerHeight,
                         slicing::SlicePipeline::State startFrom,
                         QObject* parent)
    : QObject(parent)
    , _objects(objects)
    , _layerHeight(layerHeight)
    , _startFrom(startFrom)
{
    connect(&_pipeline, &slicing::SlicePipeline::stageChanged,
            this, &SliceWorker::stageChanged);
    connect(&_pipeline, &slicing::SlicePipeline::progress,
            this, &SliceWorker::progressChanged);
    connect(&_pipeline, &slicing::SlicePipeline::logMessage,
            this, &SliceWorker::logMessage);
    connect(&_pipeline, &slicing::SlicePipeline::finished,
            this, &SliceWorker::finished);
}

void SliceWorker::doWork()
{
    _pipeline.execute(_objects, _layerHeight, _startFrom);
}

void SliceWorker::cancel()
{
    _pipeline.cancel();
}

slicing::SlicePipeline* SliceWorker::pipeline() const
{
    return const_cast<slicing::SlicePipeline*>(&_pipeline);
}

// ============================================================================
// SlicingController
// ============================================================================

SlicingController::SlicingController(QObject* parent)
    : QObject(parent)
{
}

SlicingController::~SlicingController()
{
    cancelSlice();
    _workerThread.quit();
    _workerThread.wait(5000); // 5 second timeout
}

void SlicingController::startSlice(
    const std::vector<slicing::ModelObject*>& objects,
    slicing::coord_t layerHeight,
    slicing::SlicePipeline::State startFrom)
{
    if (_slicing.load())
    {
        qWarning() << "SlicingController: slice already in progress";
        return;
    }

    _slicing.store(true);

    // Clean up previous worker
    if (_worker)
    {
        _worker->deleteLater();
        _worker = nullptr;
    }

    _lastPipeline = nullptr;

    // Create worker and move to thread
    _worker = new SliceWorker(objects, layerHeight, startFrom);
    _worker->moveToThread(&_workerThread);

    // Wire signals
    connect(&_workerThread, &QThread::started, _worker, &SliceWorker::doWork);
    connect(_worker, &SliceWorker::finished, this, &SlicingController::onWorkerFinished);
    connect(_worker, &SliceWorker::stageChanged,
            this, &SlicingController::onWorkerStageChanged);
    connect(_worker, &SliceWorker::progressChanged,
            this, &SlicingController::progressChanged);
    connect(_worker, &SliceWorker::logMessage,
            this, &SlicingController::logMessage);

    // Clean up worker when thread finishes
    connect(&_workerThread, &QThread::finished, _worker, &QObject::deleteLater);

    emit sliceStarted();
    _workerThread.start();
}

void SlicingController::cancelSlice()
{
    if (_worker)
        _worker->cancel();
}

bool SlicingController::isSlicing() const
{
    return _slicing.load();
}

slicing::SlicePipeline* SlicingController::lastPipeline() const
{
    return _lastPipeline;
}

void SlicingController::onWorkerFinished(bool success)
{
    _slicing.store(false);

    // Capture the pipeline reference before the worker is deleted
    if (_worker)
    {
        _lastPipeline = _worker->pipeline();
    }

    _workerThread.quit();
    _workerThread.wait(1000);

    QString error;
    if (_lastPipeline && _lastPipeline->state() == slicing::SlicePipeline::Failed)
        error = _lastPipeline->errorMessage();

    emit sliceFinished(success, error);
}

void SlicingController::onWorkerStageChanged(int state)
{
    emit stageChanged(state);
}

} // namespace csbridge
