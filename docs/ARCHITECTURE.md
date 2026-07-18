# CraftsStudio 切片架构设计文档

> 状态: 顶层设计 | 日期: 2026-07-17

## 1. 产品定位

CraftsStudio 是 C++20/Qt6 桌面 3D 打印预处理切片软件。

**核心功能**：
- 残缺检测与修复（孔洞填充、非流形修复、水密转换）
- 套壳检测与操作（抽壳、Shell 拆分、布尔运算）
- 模型切片（Z 向层切、外墙/填充/支撑生成）
- G-code 导出

## 2. 三层架构

```
┌──────────────────────────────────────────────────────────────────┐
│                     CraftsStudio UI App                           │
│                                                                  │
│  ┌──────────────────────────────────┐  ┌───────────────────────┐ │
│  │ QWidget 面板层 (工具/设置/Tree)    │  │ QML 3D 视图层         │ │
│  │ MainWindow, ProjectTree,         │  │ PlaterView.qml        │ │
│  │ TransformPanel, PrintSettings,   │  │ SlicePreview.qml      │ │
│  │ SupportSettings                  │  │ GCodeView.qml         │ │
│  │ (QWidget, 保留现有代码风格)        │  │ (Qt3D, 新功能)        │ │
│  └──────────────────────────────────┘  └───────────────────────┘ │
│                                                                  │
│  渲染后端:                                                        │
│   ● VTK (Plater 3D 布局视图 — 渐进迁移)                           │
│   ● Qt3D (Preview / GCode 视图 — 新功能)                          │
│                                                                  │
│  Depends on: libcsbridge, Qt6::Widgets, Qt6::Quick,              │
│              Qt6::3D, VTK (legacy, 逐步移除)                      │
└────────────────────────────┬─────────────────────────────────────┘
                             │
┌────────────────────────────┴─────────────────────────────────────┐
│                   Bridge Layer (libcsbridge)                       │
│                                                                   │
│  SlicingController: 启动 SliceWorker 到后台线程                    │
│  SettingsWatcher: 监听配置变更，标记脏阶段，触发增量重切             │
│  Qt3DEntityMapper: ModelObject / Layer / ExtrusionPath →          │
│                    Qt3D Entity Tree (渲染数据桥接)                  │
│                                                                   │
│  Depends on: libslicingcore, Qt6::Core                            │
└────────────────────────────┬─────────────────────────────────────┘
                             │
┌────────────────────────────┴─────────────────────────────────────┐
│              Core Engine Library (libslicingcore)                  │
│                                                                   │
│  **NO** Qt::Widgets, **NO** VTK, **NO** Qt3D                       │
│  Headless-capable. 纯数据处理.                                     │
│                                                                   │
│  geometry/   — Point, ExPolygon, BoundingBox, coord (int64 微米)   │
│  mesh/       — TriangleMesh, ModelObject, ModelVolume, repair     │
│  slice/      — Layer, LayerRegion, 平面求交切片算法                │
│  infill/     — InfillStrategy (grid, honeycomb, gyroid...)        │
│  perimeter/  — PerimeterGenerator (classic offset / Arachne)      │
│  support/    — SupportGenerator (grid / tree / organic)           │
│  gcode/      — GCodeWriter, ExtrusionEntity, path ordering        │
│  config/     — PrintConfig, stage invalidation tracking           │
│  pipeline/   — SlicePipeline (6-stage state machine)              │
│                                                                   │
│  Depends on: Qt6::Core, Clipper2, TBB                             │
└──────────────────────────────────────────────────────────────────┘

外部依赖 (文件 I/O & 网格处理):
  ● VTK I/O: vtkSTLReader/Writer, vtkOBJReader, vtkPLYReader
  ● VTK Filters: vtkFillHolesFilter, vtkCleanPolyData, vtkPolyDataNormals
  ● 以上仅在 mesh/import/ 中使用，不暴露到引擎其他模块
```

**关键原则**：
- **引擎是纯数据**：`libslicingcore` 不依赖任何渲染库（VTK/Qt3D 都不依赖）
- **渲染是可插拔的**：VTK 和 Qt3D 都是渲染后端，通过 Bridge 层与引擎数据对接
- **渐进迁移**：Plater 视图暂留 VTK，Preview/GCode 新视图用 Qt3D

## 3. 渲染层设计：双后端 + 渐进迁移

### 3.1 为什么引入 Qt3D

| 维度 | VTK | Qt3D | 选择 |
|------|-----|------|------|
| Qt 集成 | QVTKOpenGLNativeWidget，非原生 | 原生 Qt，与 QML/Widgets 无缝 | Qt3D |
| 线程安全 | 严格非线程安全，只能主线程操作 | Qt 消息循环原生支持 | Qt3D |
| 渲染性能 | 针对科学可视化优化，非实时 | 基于 OpenGL 的实时渲染，帧率高 | Qt3D |
| Mesh I/O | 内置丰富的读写器 | 需自建或借助其他库 | VTK (保留) |
| Mesh Repair | 内置修复过滤器 | 需自建 | VTK (保留) |
| QML 支持 | 无 | 原生支持，声明式 UI | Qt3D |
| Entity 模型 | Pipeline 模式（复杂） | ECS (Entity-Component-System)，直观 | Qt3D |
| 切片预览 | 非原生场景（需手动构造管线） | 2D 多边形在 3D 空间的渲染很自然 | Qt3D |
| GCode 路径 | 非原生场景 | 线条实体，颜色/宽度可配置 | Qt3D |

### 3.2 双后端共存策略

```
视图                   后端           状态
─────────────────────────────────────────────
Plater View (3D 布局)   VTK           保留，渐进迁移
Slice Preview (切片预览)  Qt3D + QML    新建
GCode View (路径预览)    Qt3D + QML    新建
```

### 3.3 迁移路线图

```
Phase 4 (当前目标):
  ├── Plater View: 保留 VTK (不动现有代码)
  ├── Slice Preview: Qt3D + QML (全新)
  └── GCode View: Qt3D + QML (全新)

Phase 5+ (未来):
  └── Plater View: 迁移到 Qt3D + QML
      └── 届时 VTK 退化为纯 mesh I/O 库
```

### 3.4 Qt3D + QML 集成进 QWidget 主窗口

```cpp
// MainWindow 使用 QQuickWidget 嵌入 QML 3D 视图
class MainWindow : public QMainWindow {
    QStackedWidget* _viewStack;

    // Tab 0: Plater View (保留 VTK)
    PlaterWidget* _platerView;  // QVTKOpenGLNativeWidget

    // Tab 1: Slice Preview (新建, Qt3D)
    QQuickWidget* _previewView;  // 加载 SlicePreview.qml

    // Tab 2: GCode View (新建, Qt3D)
    QQuickWidget* _gcodeView;    // 加载 GCodeView.qml
};
```

## 4. 坐标系

使用 **int64 微米** 坐标（1 单位 = 0.001mm）：

```cpp
using coord_t = int64_t;
constexpr double SCALE = 0.001;

struct Point  { coord_t x, y; };
struct Point3D { coord_t x, y, z; };
```

理由：匹配 Clipper2 原生格式，避免浮点累积误差。

## 5. 核心数据结构

### 5.1 网格层（替代 SceneObjectV2）

```cpp
class TriangleMesh {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    double volume() const;          // 真实体积（四面体分解）
    bool isWatertight() const;
    void repair();                  // 自动修复
    vtkSmartPointer<vtkPolyData> toVTK() const;   // VTK 桥接 (legacy)
    QByteArray toQt3DBuffer() const;              // Qt3D 桥接 (new)
};

class ModelVolume {
    TriangleMesh mesh;
    std::string name;
    bool isModifier = false;
};

class ModelObject {
    std::vector<ModelVolume> volumes;
    Transform3D placement;          // 在打印平台上的摆放
};
```

### 5.2 切片层

```cpp
struct ExPolygon {
    Polygon contour;                // 外轮廓 (CCW)
    Polygons holes;                 // 内孔 (CW)
};

class Layer {
    int id;
    coord_t z;                      // Z 高度 (微米)
    std::vector<ExPolygon> slices;
};
```

### 5.3 挤出路径

```cpp
enum class ExtrusionRole {
    Perimeter, ExternalPerimeter, OverhangPerimeter,
    Infill, SolidInfill, TopSolidInfill, BottomSolidInfill,
    SupportMaterial, SupportInterface,
    Skirt, Brim, GapFill
};

struct ExtrusionPath {
    Points points;
    coord_t width;
    ExtrusionRole role;
};
```

## 6. Qt3D 渲染桥接

### 6.1 数据 → Entity 映射

```
引擎数据                    Qt3D Entity Tree
─────────────────────────────────────────────────────
ModelObject      ──→  Qt3DEntity (root)
  ModelVolume[0] ──→    Qt3DEntity
    TriangleMesh ──→      Qt3DMesh (geometry)
                      Qt3DTransform (placement)
                      Qt3DMaterial (appearance)
  ModelVolume[1] ──→    Qt3DEntity ...

Layer            ──→  Qt3DEntity (z = layer.z)
  ExPolygon[0]   ──→    Qt3DEntity
                      Qt3DMesh (triangulated polygon)
                      Qt3DMaterial (semi-transparent)
  ExPolygon[1]   ──→    Qt3DEntity ...

ExtrusionPath    ──→  Qt3DEntity
                      Qt3DLineGeometry (path points)
                      Qt3DMaterial (color by role)
```

### 6.2 Qt3DEntityMapper (Bridge 层)

```cpp
// libcsbridge/qt3d_entity_mapper.hpp
class Qt3DEntityMapper : public QObject {
    Q_OBJECT
public:
    // 从引擎数据构建 Qt3D Entity 树
    Qt3DEntity* buildModelEntity(ModelObject* obj, Qt3DEntity* parent);
    Qt3DEntity* buildLayerEntity(const Layer& layer, Qt3DEntity* parent);
    Qt3DEntity* buildExtrusionEntity(const ExtrusionPath& path,
                                      Qt3DEntity* parent);

    // 同步引擎数据变更到 Entity
    void syncTransform(ModelObject* obj, Qt3DEntity* entity);
    void syncSelection(const QList<ModelObject*>& selected);
};
```

### 6.3 QML 视图示例

```qml
// SlicePreview.qml
import QtQuick 2.15
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15

Entity {
    // Camera
    Camera { ... }

    // Light
    DirectionalLight { ... }

    // Layer entities populated from C++ via Qt3DEntityMapper
    NodeInstantiator {
        model: layerListModel       // C++ QAbstractListModel
        delegate: LayerEntity { }   // Custom QML component
    }

    // Layer slider (QML 2D overlay)
    Rectangle {
        Slider {
            from: 0; to: totalLayers
            onValueChanged: controller.setCurrentLayer(value)
        }
    }
}
```

## 7. 六阶段切片管线

```
IMPORT ──→ SLICE ──→ PERIMETERS ──→ INFILL ──→ SUPPORT ──→ GCODE
```

| 阶段 | 输入 | 输出 | 算法 |
|------|------|------|------|
| Import | 文件路径 | `ModelObject[]` + `TriangleMesh` | VTK Reader → TriangleMesh (自动修复) |
| Slice | `TriangleMesh` + 层高 | `Layer[]` 每层 `ExPolygon` | 网格-Z 平面求交 (Clipper2) |
| Perimeters | `ExPolygon` + 外墙数 | 偏移轮廓 | `Clipper2Lib::InflatePaths` N 次 |
| Infill | 外墙内剩余区域 | 填充路径 | Strategy: grid/honeycomb/gyroid |
| Support | 悬垂检测 | 支撑 ExPolygon | Tree/Grid 支撑生成 |
| GCode | 所有挤出路径 | G-code 文件 | 路径排序、速度/温度控制 |

### 状态机

```cpp
class SlicePipeline : public QObject {
    Q_OBJECT
    enum State { Idle, Importing, Slicing, GeneratingPerimeters,
                 GeneratingInfill, GeneratingSupport, GeneratingGCode,
                 Completed, Failed, Cancelled };
    void execute(const PrintConfig&, const std::vector<ModelObject>&);
    void cancel();  // 协作式取消 (std::atomic<bool>)
signals:
    void stageChanged(State);
    void progress(int percent);
    void finished(bool success, QString error);
};
```

## 8. 线程模型

```
MAIN THREAD                        SLICE WORKER THREAD
┌─────────────────────┐           ┌──────────────────────────┐
│ Qt Event Loop       │           │ SlicePipeline            │
│ Qt3D Rendering      │  queued   │  ├─ StageImport          │
│ QML Scene Graph     │◄─────────│  ├─ StageSlice           │
│ User Input          │  signals  │  ├─ StagePerimeters      │
│ SceneDocument       │           │  ├─ StageInfill          │
│ SlicingController   │           │  ├─ StageSupport         │
└─────────────────────┘           │  └─ StageGCode           │
                                  └──────────────────────────┘
```

- **主线程**：UI 更新、Qt3D/VTK 渲染、用户交互、SceneDocument 修改
- **Slice Worker (QThread)**：切片管线全部 6 阶段
- **协作式取消**：每阶段间检查 `std::atomic<bool>`
- **Qt3D 线程安全的优势**：Qt3D 的 ECS 通过 QNode 父子关系天然支持跨线程创建 Entity（在主线程组装即可）

## 9. UI 架构

### 9.1 技术分层

```
┌─────────────────────────────────────────┐
│  QML 3D 视图 (Preview, GCode)            │  ← 新功能用 QML + Qt3D
│  QQuickWidget 嵌入 QWidget 主窗口        │
├─────────────────────────────────────────┤
│  QWidget 面板 (Tree, Transform, Print)   │  ← 保留 QWidget，与现有代码一致
│  QDockWidget, QTreeView, QDoubleSpinBox  │
├─────────────────────────────────────────┤
│  QMainWindow (QStackedWidget 切换视图)   │  ← 顶层容器
└─────────────────────────────────────────┘
```

### 9.2 SceneDocument（替代双 SceneManager）

```cpp
class SceneDocument : public QObject {
    Q_OBJECT
    ModelObject* addModel(TriangleMesh mesh, const QString& name);
    void removeModels(const QList<ModelObject*>&);
    QList<ModelObject*> selection() const;
    void translate(const QList<ModelObject*>&, const Vec3d& delta);
    // 所有修改通过 Command 执行 → undo/redo
signals:
    void objectsAdded(QList<ModelObject*>);
    void selectionChanged(QList<ModelObject*>);
    void objectModified(ModelObject*);
};
```

### 9.3 MVC 模式

```
SceneDocument (MODEL)
    ├──→ ProjectTreeModel (QAbstractItemModel) → ProjectTree (QTreeView)
    ├──→ PlaterWidget (VIEW: 3D 布局, VTK/Qt3D)
    ├──→ TransformPanel (VIEW: QWidget 数值输入)
    └──→ Qt3DEntityMapper (BRIDGE: ModelObject → Qt3D Entity)
```

### 9.4 视图布局

```
MainWindow (QMainWindow)
├── MenuBar (MenuController 统一管理)
├── ToolBar (布局 + 视图 + 切片)
├── Central: QStackedWidget
│   ├── Plater View: 3D 布局工作区 (VTK, 保持现有)
│   │   ├── Left Dock: ProjectTree (QWidget)
│   │   ├── Central: QVTKOpenGLNativeWidget
│   │   ├── Right Dock: QTabWidget (默认可见)
│   │   │   ├── Model Info / Transform / Print / Support
│   │   └── Bottom: Status + Progress
│   ├── Preview View: 逐层切片预览 (Qt3D + QML, 新建)
│   │   └── QQuickWidget 加载 SlicePreview.qml
│   └── GCode View: 路径可视化 (Qt3D + QML, 新建)
│       └── QQuickWidget 加载 GCodeView.qml
```

## 10. 迁移路径

### Phase 1: Foundation (2-4 周)
- 创建 `libslicingcore` 库骨架
- `coord.hpp`（int64 微米）
- `TriangleMesh`（真实体积、`toVTK()`、`toQt3DBuffer()`）
- `ModelObject`、`ModelVolume`
- 单元测试

### Phase 2: SceneDocument (2 周)
- `SceneDocument` 替代双 SceneManager
- `CommandStack` undo/redo
- ProjectTree + PlaterWidget 接入 SceneDocument

### Phase 3: UI 整合 (2 周)
- MenuController 统一菜单
- 专用 TransformPanel widget
- 右侧面板默认可见

### Phase 4: Pipeline + Qt3D 视图 (4-6 周)
- SlicePipeline 6 阶段
- SlicingController 线程桥接
- **SlicePreview.qml** (Qt3D 逐层预览 + Z 滑块)
- **GCodeView.qml** (Qt3D 路径可视化)
- QQuickWidget 嵌入 MainWindow

### Phase 5: 打磨 (ongoing)
- 配置变更 → 增量重切
- Plater View VTK → Qt3D 渐进迁移
- 导入时自动修复
- Clipper2 插件重构

## 11. 代码映射

| 当前文件 | 目标 | 动作 |
|---------|------|------|
| `rs_scene_manager_v2.*` | `SceneDocument` | **重写** |
| `rs_scene_object_v2.*` | `ModelObject + TriangleMesh` | **重写** |
| `action_factory.cc` | `SceneDocument::addModel()` | **重写** |
| `action_statck.*` | `CommandStack` | **重写** |
| `plater_widget.cc` generateGCode | `SlicePipeline` | **重写** |
| `clipper_service.cc` | `libslicingcore/geometry/` | **重写** |
| `file_importer.*` | `libslicingcore/mesh/import/` | 重构 |
| `build_platform.*` | Keep VTK, 接入 SceneDocument | 重构 |
| `rs_interactor_v2.*` | Keep VTK, 接入 SceneDocument | 重构 |
| `rs_transform_gizmo.*` | Keep VTK, 接入 SceneDocument | 重构 |
| `tree_model.*` | `ProjectTreeModel` | 重构 |
| (新建) `SlicePreview.qml` | Qt3D 切片预览 | **新建** |
| (新建) `GCodeView.qml` | Qt3D GCode 视图 | **新建** |
| (新建) `qt3d_entity_mapper.*` | Bridge 层数据映射 | **新建** |
| `sdk/pdk/`, `sdk/qbus/` | 保持不变 | Keep |
| `plugin/` 基础设施 | 保持不变 | Keep |

## 12. 关键技术决策

| 决策 | 选择 | 理由 |
|------|------|------|
| 库结构 | `libslicingcore` (无渲染依赖) | 引擎可独立测试、CLI 复用 |
| 渲染后端 | 双后端渐进迁移 (VTK → Qt3D) | 不破坏已有功能，新功能享受 Qt3D 优势 |
| Panel 技术 | QWidget | 保留现有代码风格，成熟稳定 |
| 3D 视图技术 | Qt3D + QML | QML 对 Qt3D 支持最好，声明式 UI |
| QML 嵌入 | QQuickWidget | 无缝融入 QMainWindow 布局 |
| 坐标系 | int64 微米 | Clipper2 原生格式 |
| 场景管理 | 单一 SceneDocument | 消除双单例 |
| Transform | 数据属性，同步到渲染 | 渲染无关，修复互斥 Bug |
| 线程 | QThread 切片 + 主线程渲染 | Qt3D/VTK 都在主线程操作 |
| Undo/Redo | Command 模式 | 工业标准 |
| 填充/支撑/外墙 | Strategy 模式 | 可扩展 |

## 13. 相关文件

- `CLAUDE.md` — 项目构建与编码规范
- `.claude/skills/` — 调试与插件脚手架技能
- `src/sdk/pdk/` — 插件开发工具包
- `src/sdk/qbus/` — 微服务框架
