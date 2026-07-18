# 代码风格规范（Code Style）

> 与 `.clang-format`（Mozilla 基底、Allman 大括号、100 列）和 `.clang-tidy` 配合使用。
> 工具能自动做的交给工具；本文只约定工具管不到的部分。

## 1. 注释语言

- **新代码注释一律英文**
- 既有中文注释保留；当对该文件做大幅修改时，顺势翻译为英文
- 注释解释**为什么**，不复述代码在做什么

## 2. 头文件包含顺序

每个 `.cc` 文件内按以下分组，**组间空一行**：

```cpp
#include "my_class.h"          // 1. 自己的头文件

#include "pdk/object.h"        // 2. 项目头文件（按层：pdk → qbus → app/plugin）
#include "qbus/micro_service.h"

#include <QWidget>             // 3. Qt

#include <vtkRenderer.h>       // 4. VTK

#include <clipper2/clipper.h>  // 5. 第三方（Clipper2、protobuf、TBB）

#include <memory>              // 6. 标准库
#include <vector>

#include <sys/stat.h>          // 7. 系统头文件
```

## 3. 日志与错误处理

使用 `pdk::Object` 提供的日志宏（`src/sdk/pdk/object.h`）：

```cpp
Debug()    << "Processing model:" << modelName;   // 开发期诊断
Info()     << "Plugin loaded:" << pluginName;     // 正常里程碑
Warning()  << "Memory usage high:" << usage;      // 可继续但需关注
Critical() << "Failed to load file:" << filePath; // 功能失败
```

- 可恢复错误**返回错误结构体**，不抛异常
- 不要用 `qDebug()` / `std::cout` 直接输出

## 4. 架构层级红线

1. `pdk` 不得依赖 `qbus`、任何插件或应用
2. `qbus` 不得依赖任何插件或应用
3. 插件之间**只通过服务总线通信**，禁止直接依赖
4. 任何层不得直接 include `3rd-party/` 的头文件，须经封装层
5. GUI 插件必须声明 `Q_PROPERTY(bool withGui READ withGui CONSTANT)`；无头插件不得依赖 `Qt::Widgets`

## 5. 提交前自查

```bash
# 只格式化本次改动的文件（避免噪音扩散到无关文件）
git diff --name-only --cached | grep -E '\.(h|cc|cpp)$' | xargs -r clang-format -i -style=file

# 静态检查单个文件
clang-tidy <file> -p build -- -std=c++20
```

- 格式化产生的无关改动**不要混入功能提交**——单独提 `style` 类型提交
- 提交拆分与消息格式见 [commit-convention.md](commit-convention.md)
