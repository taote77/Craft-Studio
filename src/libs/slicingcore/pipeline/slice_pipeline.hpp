#ifndef SLICINGCORE_SLICE_PIPELINE_HPP
#define SLICINGCORE_SLICE_PIPELINE_HPP

#include <slicingcore/slicingcore_global.h>
#include <slicingcore/slice/layer.hpp>

#include <QObject>
#include <QString>
#include <atomic>
#include <vector>
#include <functional>

namespace slicing {

class ModelObject;
class TriangleMesh;

// ============================================================================
// SlicePipeline — 6-stage slicing state machine
//
// Runs the complete slicing pipeline sequentially:
//   Import → Slice → Perimeters → Infill → Support → GCode
//
// Designed to run on a background thread (see SlicingController in libcsbridge).
// The pipeline is NOT thread-safe for concurrent execution — only one slice
// operation may be active at a time. Cancellation is cooperative via atomic flag.
//
// Each stage is a virtual method that can be overridden or replaced with a
// custom implementation (Strategy pattern for infill/perimeter/support).
// ============================================================================

class SLICINGCORE_API SlicePipeline : public QObject
{
    Q_OBJECT

public:
    /// Pipeline state
    enum State
    {
        Idle,
        Importing,
        Slicing,
        GeneratingPerimeters,
        GeneratingInfill,
        GeneratingSupport,
        GeneratingGCode,
        Completed,
        Failed,
        Cancelled
    };
    Q_ENUM(State)

    SlicePipeline(QObject* parent = nullptr);
    ~SlicePipeline() override;

    // -- Execution -----------------------------------------------------------

    /// Main entry point. Takes ownership of model objects for the duration
    /// of the slice. Call from worker thread.
    /// @param startFrom  Skip stages before this one (enables incremental re-slice)
    void execute(const std::vector<ModelObject*>& objects, coord_t layerHeight,
                 State startFrom = Importing);

    /// Request cancellation. Returns immediately (async signal).
    /// The pipeline checks between stages and aborts with State::Cancelled.
    void cancel();

    // -- Results (valid after State::Completed) -----------------------------

    State state() const;
    const Layers& layers() const;

    /// Total number of layers produced
    int totalLayerCount() const;

    /// Minimum and maximum Z in the sliced range (microns)
    coord_t minZ() const;
    coord_t maxZ() const;

    /// Error message (if State::Failed)
    QString errorMessage() const;

    /// Generated G-code text (valid after State::Completed)
    QString gcodeText() const;

    // -- Configuration -------------------------------------------------------

    /// Set custom stage implementations (Strategy pattern)
    /// Pass nullptr to use the default implementation.
    using StageFunc = std::function<bool(SlicePipeline*)>;
    void setCustomStage(State stage, StageFunc func);

signals:
    /// Emitted when the pipeline transitions to a new stage
    void stageChanged(int state);

    /// Emitted during stage execution (0-100)
    void progress(int percent);

    /// Emitted when a log message is produced
    void logMessage(const QString& message);

    /// Emitted when the pipeline finishes (success or failure)
    void finished(bool success);

protected:
    // -- Stage implementations (virtual for override) -----------------------

    virtual bool stageImport();
    virtual bool stageSlice();
    virtual bool stagePerimeters();
    virtual bool stageInfill();
    virtual bool stageSupport();
    virtual bool stageGCode();

    // -- Internal helpers ----------------------------------------------------

    bool checkCancelled();
    void setState(State state);
    void setProgress(int percent);
    void log(const QString& message);

private:
    std::atomic<bool> _cancelRequested{false};
    State _state = Idle;
    std::vector<StageFunc> _customStages;

    // Pipeline data
    std::vector<ModelObject*> _objects;
    coord_t _layerHeight = 0;
    Layers _layers;
    QString _errorMessage;

    QString _gcodeText;

    // Computed from input objects
    coord_t _totalHeight = 0;
    coord_t _minZ = 0;
    coord_t _maxZ = 0;
};

} // namespace slicing

#endif // SLICINGCORE_SLICE_PIPELINE_HPP
