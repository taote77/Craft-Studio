#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

// 只包含必要的 Qt 头文件
class QWidget;
class QMainWindow;
class QColor;
class QString;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QRectF;
class QPainter;
class QStyleOptionGraphicsItem;

// 避免使用宏定义，直接包含 Qt 头文件
#include <QMainWindow>
#include <QGraphicsItem>
#include <QColor>
#include <QString>

// Clipper2 头文件
#include <clipper2/clipper.h>

// 自定义图形项用于显示多边形
class PolygonItem : public QGraphicsItem
{
public:
  PolygonItem(const Clipper2Lib::Path64& path, QColor color, QGraphicsItem* parent = nullptr);

  QRectF boundingRect() const override;
  void paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
  Clipper2Lib::Path64 _path;
  QColor _color;
};

class MainWindow : public QMainWindow
{
public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow() = default;

  void demoUnion();
  void demoIntersection();
  void demoDifference();
  void demoXor();
  void demoOffset();
  void showInfo(const QString& info);

private:
  void setupUI();
  void createDemoPolygons();
  void clearScene();
  void addPolygonToScene(const Clipper2Lib::Path64& path, QColor color);

  QWidget* _centralWidget;
  QVBoxLayout* _mainLayout;
  QHBoxLayout* _buttonLayout;
  QGraphicsView* _graphicsView;
  QGraphicsScene* _scene;
  QLabel* _infoLabel;

  Clipper2Lib::Path64 _subject;
  Clipper2Lib::Path64 _clip;
};

#endif // MAINWINDOW_H