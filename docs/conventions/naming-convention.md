# 代码命名规范（Naming Convention）

> 适用于所有**新增代码**。历史代码不做批量改名，仅在大幅修改某文件时顺势统一。

## 1. 命名总表

| 元素 | 规则 | 示例 |
| ------ | ------ | ------ |
| 类 / 结构体 | `PascalCase` | `SceneManager`、`MainWindow`、`PluginExport` |
| 方法 / 函数 | `PascalCase` | `SetupUI()`、`AddObject()`、`LoadPlugin()` |
| 成员变量 | `_camelCase`（前置下划线） | `_dispatcher`、`_vtkRenderWindow` |
| 局部变量 / 参数 | `camelCase` | `modelPath`、`sliceCount` |
| 文件名 | `snake_case` | `scene_manager.h`、`main_window.cc` |
| 命名空间 | `lowercase` | `pdk`、`qbus` |
| 宏 / 编译期常量 | `UPPER_CASE` | `PDK_OBJECT_INIT`、`DECLARE_PLUGIN` |
| 枚举类型 | `PascalCase`（优先 `enum class`） | `enum class MeshRepairMode` |
| 枚举值 | `PascalCase` | `MeshRepairMode::FillHoles` |
| Qt 信号 / 槽 | `camelCase` / `on` + 事件名 | `objectAdded` / `onPub` |
| Q_PROPERTY | `camelCase` | `Q_PROPERTY(bool withGui ...)` |

## 2. 历史例外：pdk 库

`src/sdk/pdk` 是稳定基础库，成员变量使用**后置下划线**（`dispatcher_`、`objects_`）。

- **不要重命名** pdk 内的既有成员
- 在 pdk 内新增代码时，**跟随 pdk 自身风格**（后置下划线）
- pdk 之外的一切新代码使用前置下划线 `_camelCase`

## 3. 语义要求

- 名字表达**意图**而非类型：`_pendingModels` 优于 `_modelList2`
- 布尔量用 is/has/can/with 前缀：`isWatertight`、`hasSupport`、`withGui`
- 避免缩写，行业公认者除外（`stl`、`gcode`、`vtk`、`ui`、`db`）
- 一个头文件一个类（小型辅助类除外），文件名与主类名对应：`SceneManager` → `scene_manager.h`

## 4. 插件与服务命名

| 元素 | 规则 | 示例 |
| ------ | ------ | ------ |
| 插件目录 | `q_<domain>` 或 `cs_<domain>` | `q_clipper`、`cs_db` |
| 插件产物 | 与目录同名，后缀 `.mx` | `q_clipper.mx` |
| 服务类 | `<Domain>Service` 或 `<Domain>Widget`(GUI) | `SysService`、`ClipperWidget` |
| 消息 topic（字符串） | `<域>/<事件>`，全小写 | `"model/imported"`、`"slice/complete"` |
| 消息 topic（整型） | protobuf 枚举中定义的公共协议号 | — |

## 5. 与提交规范的衔接

纯改名（不改行为）的提交使用 `refactor` type，并在 body 说明改名范围与动机，方便追溯。参见 [commit-convention.md](commit-convention.md)。
