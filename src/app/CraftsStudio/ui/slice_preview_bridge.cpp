#include "slice_preview_bridge.h"

SlicePreviewBridge::SlicePreviewBridge(QObject* parent)
    : QObject(parent)
{
}

void SlicePreviewBridge::setCurrentLayer(int layer)
{
    if (layer < 0 || layer >= _totalLayers || layer == _currentLayer) return;
    _currentLayer = layer;
    _layerPoints = _allLayerPoints.value(layer);
    _layerZ = _layerZs.value(layer);
    emit currentLayerChanged();
    emit layerPointsChanged();
    emit layerZChanged();
}

void SlicePreviewBridge::loadSliceData(
    const QVector<QVector<QVector3D>>& allLayerPoints,
    const QVector<float>& layerZs)
{
    _allLayerPoints = allLayerPoints;
    _layerZs = layerZs;
    _totalLayers = allLayerPoints.size();
    _currentLayer = 0;
    _layerPoints = _allLayerPoints.value(0);
    _layerZ = _layerZs.value(0);
    emit totalLayersChanged();
    emit currentLayerChanged();
    emit layerPointsChanged();
    emit layerZChanged();
}

void SlicePreviewBridge::nextLayer()
{
    if (_currentLayer + 1 < _totalLayers)
        setCurrentLayer(_currentLayer + 1);
}

void SlicePreviewBridge::prevLayer()
{
    if (_currentLayer > 0)
        setCurrentLayer(_currentLayer - 1);
}
