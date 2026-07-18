#ifndef SLICE_PREVIEW_BRIDGE_H
#define SLICE_PREVIEW_BRIDGE_H

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <QColor>

// ============================================================================
// Bridges slice layer data from C++ (SlicePipeline) to QML (QtQuick3D View3D)
// ============================================================================

class SlicePreviewBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentLayer READ currentLayer WRITE setCurrentLayer NOTIFY currentLayerChanged)
    Q_PROPERTY(int totalLayers READ totalLayers NOTIFY totalLayersChanged)
    Q_PROPERTY(QVector<QVector3D> layerPoints READ layerPoints NOTIFY layerPointsChanged)
    Q_PROPERTY(QColor layerColor READ layerColor NOTIFY layerColorChanged)
    Q_PROPERTY(float layerZ READ layerZ NOTIFY layerZChanged)

public:
    explicit SlicePreviewBridge(QObject* parent = nullptr);

    int currentLayer() const { return _currentLayer; }
    int totalLayers() const { return _totalLayers; }
    QVector<QVector3D> layerPoints() const { return _layerPoints; }
    QColor layerColor() const { return _layerColor; }
    float layerZ() const { return _layerZ; }

    void setCurrentLayer(int layer);

    /// Load slice data from pipeline results
    void loadSliceData(const QVector<QVector<QVector3D>>& allLayerPoints,
                       const QVector<float>& layerZs);

public slots:
    void nextLayer();
    void prevLayer();

signals:
    void currentLayerChanged();
    void totalLayersChanged();
    void layerPointsChanged();
    void layerColorChanged();
    void layerZChanged();

private:
    int _currentLayer = 0;
    int _totalLayers = 0;
    QVector<QVector3D> _layerPoints;
    QColor _layerColor{ 0, 255, 0 };
    float _layerZ = 0;

    QVector<QVector<QVector3D>> _allLayerPoints;
    QVector<float> _layerZs;
};

#endif
