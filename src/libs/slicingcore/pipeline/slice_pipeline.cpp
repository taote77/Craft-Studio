#include <slicingcore/pipeline/slice_pipeline.hpp>
#include <slicingcore/mesh/model_object.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>
#include <slicingcore/slice/mesh_slicer.hpp>

#include <clipper2/clipper.h>

#include <QDebug>
#include <algorithm>
#include <limits>
#include <sstream>

namespace slicing {

// ============================================================================
// SlicePipeline
// ============================================================================

SlicePipeline::SlicePipeline(QObject* parent)
    : QObject(parent)
{
    // Initialize custom stage slots (all nullptr = use default)
    _customStages.resize(10, nullptr); // enough for all State values
}

SlicePipeline::~SlicePipeline() = default;

// ============================================================================
// Execution
// ============================================================================

void SlicePipeline::execute(const std::vector<ModelObject*>& objects,
                             coord_t layerHeight,
                             State startFrom)
{
    _cancelRequested.store(false);
    _objects = objects;
    _layerHeight = layerHeight;
    if (startFrom == Importing)
        _layers.clear();
    _errorMessage.clear();

    if (_objects.empty())
    {
        _errorMessage = "No objects to slice";
        setState(Failed);
        emit finished(false);
        return;
    }

    if (_layerHeight <= 0)
    {
        _errorMessage = "Invalid layer height";
        setState(Failed);
        emit finished(false);
        return;
    }

    // Compute total slicing range from all object bounding boxes
    _minZ = std::numeric_limits<coord_t>::max();
    _maxZ = std::numeric_limits<coord_t>::min();
    for (auto* obj : _objects)
    {
        BoundingBox3D bb;
        for (const auto& vol : obj->volumes())
        {
            auto vbb = vol.mesh().boundingBox();
            bb.extend(vbb.min);
            bb.extend(vbb.max);
        }
        if (bb.min.z < _minZ) _minZ = bb.min.z;
        if (bb.max.z > _maxZ) _maxZ = bb.max.z;
    }

    if (_minZ >= _maxZ)
    {
        _errorMessage = "Invalid object geometry (zero height)";
        setState(Failed);
        emit finished(false);
        return;
    }

    log(QString("Slicing range: %1 to %2 microns (%3 layers at %4 um)")
            .arg(_minZ).arg(_maxZ)
            .arg((_maxZ - _minZ) / _layerHeight)
            .arg(_layerHeight));

    // Run the pipeline stages sequentially
    struct StageInfo {
        State state;
        const char* name;
        bool (SlicePipeline::*fn)();
    };

    StageInfo stages[] = {
        { Importing,             "Import",      &SlicePipeline::stageImport      },
        { Slicing,               "Slice",       &SlicePipeline::stageSlice       },
        { GeneratingPerimeters,  "Perimeters",  &SlicePipeline::stagePerimeters  },
        { GeneratingInfill,      "Infill",      &SlicePipeline::stageInfill      },
        { GeneratingSupport,     "Support",     &SlicePipeline::stageSupport     },
        { GeneratingGCode,       "GCode",       &SlicePipeline::stageGCode       },
    };

    bool success = true;
    bool skipping = true;
    for (const auto& stage : stages)
    {
        if (checkCancelled()) { success = false; break; }

        // Skip stages before startFrom (incremental re-slice)
        if (skipping && stage.state < startFrom)
        {
            log(QString("  (skip %1 — unchanged)").arg(stage.name));
            continue;
        }
        skipping = false;

        setState(stage.state);

        StageFunc custom = _customStages[stage.state];
        bool stageOk = custom ? custom(this) : (this->*stage.fn)();

        if (!stageOk)
        {
            log(QString("Stage '%1' failed: %2").arg(stage.name).arg(_errorMessage));
            success = false;
            break;
        }
    }

    setProgress(100);
    setState(success ? Completed : (_cancelRequested.load() ? Cancelled : Failed));
    emit finished(success);
}

void SlicePipeline::cancel()
{
    _cancelRequested.store(true);
    log("Cancellation requested");
}

// ============================================================================
// Results
// ============================================================================

SlicePipeline::State SlicePipeline::state() const
{
    return _state;
}

const Layers& SlicePipeline::layers() const
{
    return _layers;
}

int SlicePipeline::totalLayerCount() const
{
    return static_cast<int>(_layers.size());
}

coord_t SlicePipeline::minZ() const
{
    return _minZ;
}

coord_t SlicePipeline::maxZ() const
{
    return _maxZ;
}

QString SlicePipeline::errorMessage() const
{
    return _errorMessage;
}

QString SlicePipeline::gcodeText() const
{
    return _gcodeText;
}

void SlicePipeline::setCustomStage(State stage, StageFunc func)
{
    if (stage >= 0 && stage < static_cast<int>(_customStages.size()))
        _customStages[stage] = std::move(func);
}

// ============================================================================
// Stage Implementations (stubs for now)
// ============================================================================

bool SlicePipeline::stageImport()
{
    log(QString("Importing %1 object(s)...").arg(_objects.size()));

    // Verify all meshes are valid
    for (auto* obj : _objects)
    {
        for (auto& vol : obj->volumes())
        {
            if (vol.mesh().triangleCount() == 0)
            {
                _errorMessage = QString("Empty mesh in object '%1'")
                    .arg(QString::fromStdString(obj->name()));
                return false;
            }
        }
    }

    setProgress(100);
    return true;
}

bool SlicePipeline::stageSlice()
{
    coord_t totalLayers = (_maxZ - _minZ) / _layerHeight;
    log(QString("Slicing %1 layers at %2 um...")
            .arg(totalLayers).arg(_layerHeight));

    MeshSlicer slicer;

    for (auto* obj : _objects)
    {
        if (checkCancelled()) return false;

        for (auto& vol : obj->volumes())
        {
            if (vol.isModifier()) continue;

            const auto& mesh = vol.mesh();
            if (mesh.triangleCount() == 0) continue;

            log(QString("  Slicing '%1' (%2 tris)...")
                    .arg(QString::fromStdString(vol.name()))
                    .arg(mesh.triangleCount()));

            Layers meshLayers = slicer.slice(mesh, _minZ, _maxZ, _layerHeight,
                [this](float progress) {
                    setProgress(static_cast<int>(progress * 100));
                });

            // Merge meshLayers into global _layers
            if (_layers.empty())
            {
                _layers = std::move(meshLayers);
            }
            else
            {
                // Append slices from this mesh to existing layers
                for (size_t i = 0; i < meshLayers.size() && i < _layers.size(); ++i)
                {
                    for (auto& expoly : meshLayers[i].slices())
                        _layers[i].addSlice(expoly);
                }
            }
        }
    }

    // Count total vertices across all layers
    size_t totalVerts = 0;
    for (const auto& layer : _layers)
        totalVerts += layer.vertexCount();

    log(QString("Generated %1 layers, %2 vertices total")
            .arg(_layers.size()).arg(totalVerts));
    return true;
}

// Helper: convert ExPolygon contours to Clipper2 Paths64
static Clipper2Lib::Paths64 expolyToPaths(const ExPolygons& slices)
{
    Clipper2Lib::Paths64 paths;
    for (const auto& expoly : slices)
    {
        Clipper2Lib::Path64 path;
        for (const auto& pt : expoly.contour)
            path.push_back(Clipper2Lib::Point64(pt.x, pt.y));
        if (!path.empty())
            paths.push_back(path);
    }
    return paths;
}

// Helper: convert Clipper2 Paths64 back to Polygon
static Polygon pathToPolygon(const Clipper2Lib::Path64& path)
{
    Polygon poly;
    for (const auto& pt : path)
        poly.emplace_back(pt.x, pt.y);
    return poly;
}

bool SlicePipeline::stagePerimeters()
{
    log(QString("Generating perimeters for %1 layers...").arg(_layers.size()));

    // Offset each slice contour inward by 400um (typical 0.4mm nozzle)
    coord_t offsetAmount = -400; // negative = inward offset

    for (auto& layer : _layers)
    {
        if (checkCancelled()) return false;
        auto paths = expolyToPaths(layer.slices());
        if (paths.empty()) continue;

        // Use Clipper2 to offset contours inward
        auto offsetPaths = Clipper2Lib::InflatePaths(paths, offsetAmount,
            Clipper2Lib::JoinType::Miter, Clipper2Lib::EndType::Polygon);

        // Store offset polygons
        ExPolygons perimeters;
        for (const auto& p : offsetPaths)
        {
            if (p.size() >= 3)
                perimeters.emplace_back(pathToPolygon(p));
        }
        layer.setSlices(std::move(perimeters));
    }

    setProgress(100);
    return true;
}

bool SlicePipeline::stageInfill()
{
    log(QString("Generating infill for %1 layers...").arg(_layers.size()));

    // Simple grid infill: generate a grid pattern and clip to each layer's boundary
    coord_t spacing = scale(2.0); // 2mm grid spacing

    for (auto& layer : _layers)
    {
        if (checkCancelled()) return false;
        if (layer.empty()) continue;

        auto boundaryPaths = expolyToPaths(layer.slices());

        // Generate grid lines covering the layer bounding box
        BoundingBox2D bb = layer.boundingBox();
        Clipper2Lib::Paths64 gridLines;

        // Horizontal lines
        for (coord_t y = bb.min.y; y <= bb.max.y; y += spacing)
            gridLines.push_back({ {bb.min.x - spacing, y}, {bb.max.x + spacing, y} });

        // Vertical lines
        for (coord_t x = bb.min.x; x <= bb.max.x; x += spacing)
            gridLines.push_back({ {x, bb.min.y - spacing}, {x, bb.max.y + spacing} });

        // Clip grid lines to the layer boundary
        auto clipped = Clipper2Lib::Intersect(gridLines, boundaryPaths,
            Clipper2Lib::FillRule::NonZero);

        // Add clipped infill lines as additional slices
        if (!clipped.empty())
        {
            // Store infill lines alongside existing slices
            // (in production, infill would be separate from perimeters)
        }
    }

    setProgress(100);
    return true;
}

bool SlicePipeline::stageSupport()
{
    log("Generating support structures...");

    if (_layers.size() < 2)
    {
        setProgress(100);
        return true;
    }

    coord_t pillarSpacing = scale(3.0);  // 3mm between support pillars
    coord_t pillarSize    = scale(0.8);  // 0.8mm pillar width
    size_t overhangCount = 0;

    // Bottom-up overhang detection: compare each layer to the layer below
    for (size_t i = 1; i < _layers.size(); ++i)
    {
        if (checkCancelled()) return false;

        if (_layers[i].empty() || _layers[i-1].empty()) continue;

        auto currentPaths = expolyToPaths(_layers[i].slices());
        auto belowPaths   = expolyToPaths(_layers[i-1].slices());

        if (currentPaths.empty() || belowPaths.empty()) continue;

        // Overhang = current layer area NOT supported by layer below
        auto overhangPaths = Clipper2Lib::Difference(currentPaths, belowPaths,
            Clipper2Lib::FillRule::NonZero);

        if (overhangPaths.empty()) continue;
        overhangCount++;

        // Generate support pillars in overhang regions
        for (const auto& overhang : overhangPaths)
        {
            // Compute bounding box manually
            Clipper2Lib::Rect64 bb = {
                std::numeric_limits<int64_t>::max(),
                std::numeric_limits<int64_t>::max(),
                std::numeric_limits<int64_t>::min(),
                std::numeric_limits<int64_t>::min()
            };
            for (const auto& pt : overhang)
            {
                if (pt.x < bb.left)  bb.left  = pt.x;
                if (pt.y < bb.bottom) bb.bottom = pt.y;
                if (pt.x > bb.right)  bb.right = pt.x;
                if (pt.y > bb.top)    bb.top   = pt.y;
            }

            // Generate grid pillars within the overhang bounding box
            for (coord_t px = bb.left + pillarSize; px < bb.right; px += pillarSpacing)
            {
                for (coord_t py = bb.bottom + pillarSize; py < bb.top; py += pillarSpacing)
                {
                    // Create a small square pillar
                    Clipper2Lib::Path64 pillar = {
                        {px, py},
                        {px + pillarSize, py},
                        {px + pillarSize, py + pillarSize},
                        {px, py + pillarSize}
                    };

                    // Check if pillar is inside the overhang area
                    Clipper2Lib::Paths64 pillarPath = { pillar };
                    auto clipped = Clipper2Lib::Intersect(pillarPath, { overhang },
                        Clipper2Lib::FillRule::NonZero);

                    if (!clipped.empty())
                    {
                        // Add support pillar to THIS layer and all layers below
                        for (size_t j = 0; j <= i; ++j)
                        {
                            if (!_layers[j].empty() || j == 0)
                            {
                                _layers[j].addSlice(ExPolygon(pathToPolygon(pillar)));
                            }
                        }
                    }
                }
            }
        }

        setProgress(static_cast<int>((i + 1) * 100 / _layers.size()));
    }

    log(QString("Support: %1 overhang layers").arg(overhangCount));
    return true;
}

bool SlicePipeline::stageGCode()
{
    log("Generating G-code...");

    std::ostringstream gcode;
    gcode << "; CraftsStudio G-code\n";
    gcode << "; Generated " << _layers.size() << " layers\n";
    gcode << "G21 ; mm units\n";
    gcode << "G90 ; absolute positioning\n";
    gcode << "M104 S200 ; set extruder temp\n";
    gcode << "M140 S60  ; set bed temp\n";
    gcode << "G28 ; home all axes\n";
    gcode << "G1 Z5 F3000 ; move to safe Z\n\n";

    // Generate per-layer G-code from slice contours
    for (const auto& layer : _layers)
    {
        if (checkCancelled()) return false;

        double z = layer.zMm();
        gcode << "; LAYER:" << layer.id() << " Z=" << z << "\n";
        gcode << "G1 Z" << z << " F600\n";

        for (const auto& expoly : layer.slices())
        {
            // Emit moves along each contour
            if (expoly.contour.empty()) continue;

            const auto& first = expoly.contour.front();
            double fx = unscale(first.x);
            double fy = unscale(first.y);
            gcode << "G0 X" << fx << " Y" << fy << " F3000 ; travel\n";
            gcode << "G1 ; extrude perimeter\n";

            for (size_t i = 1; i < expoly.contour.size(); ++i)
            {
                double x = unscale(expoly.contour[i].x);
                double y = unscale(expoly.contour[i].y);
                gcode << "G1 X" << x << " Y" << y << " E0.05 F1200\n";
            }
            // Close the loop
            gcode << "G1 X" << fx << " Y" << fy << " E0.05 F1200\n";
        }
    }

    gcode << "\n; End G-code\n";
    gcode << "G1 Z" << _layers.back().zMm() + 5.0 << " F3000 ; lift\n";
    gcode << "M104 S0 ; turn off extruder\n";
    gcode << "M140 S0 ; turn off bed\n";
    gcode << "M84 ; disable motors\n";

    _gcodeText = QString::fromStdString(gcode.str());
    log(QString("Generated G-code: %1 bytes").arg(_gcodeText.size()));

    setProgress(100);
    return true;
}

// ============================================================================
// Internal Helpers
// ============================================================================

bool SlicePipeline::checkCancelled()
{
    if (_cancelRequested.load())
    {
        _errorMessage = "Cancelled by user";
        setState(Cancelled);
        return true;
    }
    return false;
}

void SlicePipeline::setState(State state)
{
    _state = state;
    emit stageChanged(static_cast<int>(state));
}

void SlicePipeline::setProgress(int percent)
{
    emit progress(percent);
}

void SlicePipeline::log(const QString& message)
{
    emit logMessage(message);
}

} // namespace slicing
