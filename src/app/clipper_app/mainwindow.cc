#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPolygonF>

// PolygonItem 实现
PolygonItem::PolygonItem(const Clipper2Lib::Path64& path, QColor color, QGraphicsItem* parent)
    : QGraphicsItem(parent), _path(path), _color(color)
{
}

QRectF PolygonItem::boundingRect() const
{
    if (_path.empty()) return QRectF();
    
    double minX = _path[0].x;
    double maxX = _path[0].x;
    double minY = _path[0].y;
    double maxY = _path[0].y;
    
    for (const auto& point : _path) {
        if (point.x < minX) minX = point.x;
        if (point.x > maxX) maxX = point.x;
        if (point.y < minY) minY = point.y;
        if (point.y > maxY) maxY = point.y;
    }
    
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

void PolygonItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (_path.empty()) return;
    
    QPen pen(_color);
    pen.setWidth(2);
    painter->setPen(pen);
    
    QBrush brush(_color);
    brush.setStyle(Qt::BDiagPattern);
    painter->setBrush(brush);
    
    QPolygonF polygon;
    for (const auto& point : _path) {
        polygon << QPointF(point.x, point.y);
    }
    
    painter->drawPolygon(polygon);
}

// MainWindow 实现
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    createDemoPolygons();
    setWindowTitle("Clipper2 演示程序");
    resize(1200, 800);
}

void MainWindow::setupUI()
{
    _centralWidget = new QWidget(this);
    setCentralWidget(_centralWidget);
    
    _mainLayout = new QVBoxLayout(_centralWidget);
    
    // 创建按钮布局
    _buttonLayout = new QHBoxLayout();
    
    QPushButton* unionBtn = new QPushButton("并集操作", this);
    QPushButton* intersectBtn = new QPushButton("交集操作", this);
    QPushButton* diffBtn = new QPushButton("差集操作", this);
    QPushButton* xorBtn = new QPushButton("异或操作", this);
    QPushButton* offsetBtn = new QPushButton("偏移操作", this);
    
    connect(unionBtn, &QPushButton::clicked, this, &MainWindow::demoUnion);
    connect(intersectBtn, &QPushButton::clicked, this, &MainWindow::demoIntersection);
    connect(diffBtn, &QPushButton::clicked, this, &MainWindow::demoDifference);
    connect(xorBtn, &QPushButton::clicked, this, &MainWindow::demoXor);
    connect(offsetBtn, &QPushButton::clicked, this, &MainWindow::demoOffset);
    
    _buttonLayout->addWidget(unionBtn);
    _buttonLayout->addWidget(intersectBtn);
    _buttonLayout->addWidget(diffBtn);
    _buttonLayout->addWidget(xorBtn);
    _buttonLayout->addWidget(offsetBtn);
    
    _mainLayout->addLayout(_buttonLayout);
    
    // 创建图形视图
    _scene = new QGraphicsScene(this);
    _graphicsView = new QGraphicsView(_scene, this);
    _graphicsView->setRenderHint(QPainter::Antialiasing);
    _graphicsView->setSceneRect(-50, -50, 500, 500);
    
    _mainLayout->addWidget(_graphicsView);
    
    // 创建信息标签
    _infoLabel = new QLabel("点击按钮查看不同的 Clipper2 操作演示", this);
    _infoLabel->setAlignment(Qt::AlignCenter);
    _mainLayout->addWidget(_infoLabel);
}

void MainWindow::createDemoPolygons()
{
    // 创建主体多边形（矩形）
    _subject = {
        {100, 100}, {300, 100}, {300, 300}, {100, 300}
    };
    
    // 创建剪切多边形（圆形）
    _clip.clear();
    int centerX = 200;
    int centerY = 200;
    int radius = 100;
    int numPoints = 32;
    
    for (int i = 0; i < numPoints; ++i) {
        double angle = 2.0 * M_PI * i / numPoints;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        _clip.push_back({x, y});
    }
    
    // 显示初始多边形
    clearScene();
    addPolygonToScene(_subject, Qt::blue);
    addPolygonToScene(_clip, Qt::red);
}

void MainWindow::clearScene()
{
    _scene->clear();
}

void MainWindow::addPolygonToScene(const Clipper2Lib::Path64& path, QColor color)
{
    if (path.empty()) return;
    
    PolygonItem* item = new PolygonItem(path, color);
    _scene->addItem(item);
}

void MainWindow::demoUnion()
{
    clearScene();
    
    // 执行并集操作
    Clipper2Lib::Paths64 solution;
    Clipper2Lib::Clipper64 c;
    
    c.AddSubject({_subject});
    c.AddClip({_clip});
    c.Execute(Clipper2Lib::ClipType::Union, Clipper2Lib::FillRule::EvenOdd, solution);
    
    // 显示结果
    addPolygonToScene(_subject, Qt::blue);
    addPolygonToScene(_clip, Qt::red);
    
    for (const auto& path : solution) {
        addPolygonToScene(path, Qt::green);
    }
    
    showInfo("并集操作：蓝色矩形与红色圆形的合并区域显示为绿色");
}

void MainWindow::demoIntersection()
{
    clearScene();
    
    // 执行交集操作
    Clipper2Lib::Paths64 solution;
    Clipper2Lib::Clipper64 c;
    
    c.AddSubject({_subject});
    c.AddClip({_clip});
    c.Execute(Clipper2Lib::ClipType::Intersection, Clipper2Lib::FillRule::EvenOdd, solution);
    
    // 显示结果
    addPolygonToScene(_subject, Qt::blue);
    addPolygonToScene(_clip, Qt::red);
    
    for (const auto& path : solution) {
        addPolygonToScene(path, Qt::green);
    }
    
    showInfo("交集操作：蓝色矩形与红色圆形的重叠区域显示为绿色");
}

void MainWindow::demoDifference()
{
    clearScene();
    
    // 执行差集操作
    Clipper2Lib::Paths64 solution;
    Clipper2Lib::Clipper64 c;
    
    c.AddSubject({_subject});
    c.AddClip({_clip});
    c.Execute(Clipper2Lib::ClipType::Difference, Clipper2Lib::FillRule::EvenOdd, solution);
    
    // 显示结果
    addPolygonToScene(_subject, Qt::blue);
    addPolygonToScene(_clip, Qt::red);
    
    for (const auto& path : solution) {
        addPolygonToScene(path, Qt::green);
    }
    
    showInfo("差集操作：蓝色矩形减去红色圆形后的区域显示为绿色");
}

void MainWindow::demoXor()
{
    clearScene();
    
    // 执行异或操作
    Clipper2Lib::Paths64 solution;
    Clipper2Lib::Clipper64 c;
    
    c.AddSubject({_subject});
    c.AddClip({_clip});
    c.Execute(Clipper2Lib::ClipType::Xor, Clipper2Lib::FillRule::EvenOdd, solution);
    
    // 显示结果
    addPolygonToScene(_subject, Qt::blue);
    addPolygonToScene(_clip, Qt::red);
    
    for (const auto& path : solution) {
        addPolygonToScene(path, Qt::green);
    }
    
    showInfo("异或操作：蓝色矩形与红色圆形不相交的区域显示为绿色");
}

void MainWindow::demoOffset()
{
    clearScene();
    
    // 执行偏移操作
    Clipper2Lib::Paths64 solution;
    solution = Clipper2Lib::InflatePaths({_subject}, 20, Clipper2Lib::JoinType::Round, Clipper2Lib::EndType::Polygon);
    
    // 显示原始多边形
    addPolygonToScene(_subject, Qt::blue);
    
    // 显示偏移结果
    for (const auto& path : solution) {
        addPolygonToScene(path, Qt::green);
    }
    
    showInfo("偏移操作：蓝色矩形的外扩20单位显示为绿色");
}

void MainWindow::showInfo(const QString& info)
{
    _infoLabel->setText(info);
    _infoLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 10px; border: 1px solid #ccc; }");
}