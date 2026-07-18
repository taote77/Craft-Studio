# 代码提交规范（Commit Convention）

> 本规范为本仓库从零设计的提交标准，对所有提交生效。
> **机器可解析部分（type、scope、标记符）一律英文；供人阅读部分（subject、body）使用中文。**
> 由 `scripts/git-hooks/commit-msg` 强制校验，不合规的提交会被直接拒绝。

## 1. 设计目标

1. **可追溯** — 任何一次提交都能独立回答"改了什么、为什么改"，不依赖 PR 或口头上下文
2. **可检索** — type / scope 取值封闭且为英文，`git log --grep`、`git log --oneline | grep` 可直接过滤
3. **可自动化** — 结构兼容 [Conventional Commits](https://www.conventionalcommits.org/)，未来可直接接入 changelog 生成与语义化版本
4. **低摩擦** — 供人阅读的部分用中文，写作与评审无翻译负担

## 2. 消息结构

```
<type>(<scope>)<!>: <subject>
                                 ← 空行（有 body 时必须）
<body>                           ← 可选：动机、影响、行为变化
                                 ← 空行（有 footer 时必须）
<footer>                         ← 可选：BREAKING CHANGE / Refs / Closes
```

- **header 必填**；`(<scope>)` 推荐填写；`!` 仅在破坏性变更时使用
- **body** 对非平凡改动必填（判定标准见 §6）
- **footer** 按需

## 3. type — 改动性质（必填，封闭集合）

| type | 含义 | 判定标准（自问） |
| ------ | ------ | ------------------ |
| `feat` | 新功能 | 用户或调用方获得了新能力？ |
| `fix` | 缺陷修复 | 修正了与预期不符的行为？ |
| `refactor` | 重构 | 代码结构变了，外部行为完全不变？ |
| `perf` | 性能优化 | 行为不变，但更快 / 更省内存？ |
| `docs` | 文档 | 只改了文档或注释？ |
| `style` | 格式 | 只改了排版（clang-format、空白、换行），语义零变化？ |
| `test` | 测试 | 只改了测试代码？ |
| `build` | 构建 | CMake、第三方依赖、编译选项、打包脚本？ |
| `ci` | 持续集成 | CI 配置与流水线脚本？ |
| `chore` | 杂务 | 以上都不是，且不影响 `src/` 逻辑（如 `.gitignore`、工具配置）？ |
| `revert` | 回滚 | 撤销某次既有提交？ |

**规则：**

- 一次提交只能有一个 type
- 同时像 `feat` 又像 `fix` → 说明提交应该拆分（见 §9）
- 判定有歧义时按此优先级取高者：`revert` > `feat` > `fix` > `perf` > `refactor` > 其余

## 4. scope — 影响范围（推荐填写，封闭集合）

| scope | 对应目录 / 范围 |
| ------- | ----------------- |
| `pdk` | `src/sdk/pdk` — 插件开发套件 |
| `qbus` | `src/sdk/qbus` — 微服务框架 |
| `clipper` | `src/plugin/q_clipper` |
| `sys` | `src/plugin/q_sys`、`src/plugin/cs_sys` |
| `cloud` | `src/plugin/q_cloud` |
| `db` | `src/plugin/cs_db` |
| `studio` | `src/app/CraftsStudio`（UI、主程序） |
| `engine` | `src/app/CraftsStudio/engine` — 场景引擎 |
| `backend` | `src/app/Backend` |
| `libs` | `src/libs` — 领域算法库 |
| `cmake` | `cmake/`、各级 `CMakeLists.txt`、`CMakePresets.json` |
| `deps` | 第三方依赖（`3rd-party/`、外部库版本） |
| `conventions` | `docs/conventions/` 规范文档 |
| `skills` | `.claude/skills/` AI 技能文件 |

**规则：**

- 只写**一个** scope，全小写，取值必须在上表内
- 改动横跨 ≥ 3 个模块的全局性变更，可省略 scope：`refactor: 统一日志宏调用方式`
- 恰好涉及两个模块 → **优先拆成两次提交**；确实不可拆时取"被改动的主体"一方
- 新增模块时，须在同一 PR 内把新 scope 登记到本表（`docs(conventions)`）

## 5. subject — 一句话摘要（必填）

- **中文陈述，动词开头**：增加 / 修复 / 重构 / 移除 / 优化 / 升级 / 调整 …
- **代码标识符保留英文原文**：类名、函数名、文件名、第三方库名不翻译
- 中文与英文、数字之间留一个空格：`增加 Clipper2 布尔运算的 TBB 并发支持`
- **不以句号或任何标点结尾**
- 长度：整个 header 不超过 **72 显示列**（hook 按 100 字节做硬校验，约 30 个汉字）
- 描述**做了什么**，而非怎么做的；实现细节放 body

| ❌ 反例 | ✅ 正例 | 问题 |
| --------- | --------- | ------ |
| `update: request 代码` | `refactor(qbus): 重构 Request 的超时处理逻辑` | type 无信息量、看不出改了什么 |
| `fix: 修复 bug。` | `fix(engine): 修复 SceneManagerV2 删除对象后悬垂指针` | 无信息量、句号结尾 |
| `feat: 增加了一个新的用于处理模型导入之后自动布局摆放的功能模块` | `feat(studio): 增加模型导入后的自动布局` | 过长、赘述 |

## 6. body — 动机与影响

**以下情况必须写 body：**

- 新功能（`feat`）
- 修复原因不显而易见的 bug（`fix`）
- 任何行为变化，或 reviewer 可能会问"为什么这么改"的改动

**写法：**

- 与 header 之间空一行
- 写**为什么改**（动机）和**行为层面变了什么**（影响），不要复述 diff
- 建议每行不超过 36 个汉字（约 72 列），可使用 `-` 列表
- 中文行文，标识符保留英文

## 7. footer — 元信息

| 条目 | 格式 | 说明 |
| ------ | ------ | ------ |
| 破坏性变更 | `BREAKING CHANGE: <说明与迁移方式>` | 同时 header 加 `!`（见 §8） |
| 关联 issue | `Refs: #123` / `Closes: #123` | Closes 会自动关闭 issue |
| 共同作者 | `Co-authored-by: Name <email>` | 结对 / AI 辅助提交 |

## 8. 破坏性变更

凡是**改变对外接口或既有行为**（pdk / qbus 公共 API、消息 topic 语义、插件加载契约、文件格式）的提交：

1. header 的 type/scope 之后加 `!`
2. footer 写 `BREAKING CHANGE:` 说明影响面与迁移方式

```
refactor(qbus)!: 将 ServiceBus::Request 的超时参数改为 std::chrono

BREAKING CHANGE: Request(topic, msg, int ms) 已移除，
调用方需改为 Request(topic, msg, std::chrono::milliseconds)。
所有插件需要重新编译。
```

## 9. 原子性与拆分

- **一次提交 = 一个不可再分的逻辑变更**，能独立编译、能独立回滚
- 禁止混装：
  - 新功能 + 顺手格式化 → 先提 `style`，再提 `feat`
  - 重构 + 修 bug → 先提 `fix`（基于旧结构最小修复），再提 `refactor`
- 改动已经混在工作区时，用 `git add -p` 按逻辑分批暂存

## 10. 禁止事项

- ❌ `update:`、`wip`、`临时提交`、`fix bug`、`修改代码` 等无信息量消息
- ❌ 用 `--no-verify` 绕过 hook（紧急止血时例外，须在 PR 描述中注明并事后 `commit --amend` 补正）
- ❌ 一次提交横跨多层（sdk + plugin + app）且无单一主题
- ❌ 在 subject 里写 issue 号代替描述（issue 号放 footer）

## 11. 校验与安装

`scripts/git-hooks/commit-msg` 校验以下各项，任一不满足即拒绝提交：

1. header 匹配 `^(type)(\(scope\))?!?: subject`，type 在 §3 集合内
2. header ≤ 100 字节
3. subject 不以 `。` / `.` 结尾
4. 有 body 时，第 2 行必须为空行
5. merge 提交、`revert`/`fixup!`/`squash!` 自动生成的消息直接放行

**每个成员克隆仓库后执行一次：**

```bash
git config core.hooksPath scripts/git-hooks
```

## 12. 完整示例

```
feat(qbus): 增加服务总线广播分发机制

此前服务之间必须逐个注册点对点 handler，新增订阅者
需要修改所有发布方。增加广播通道后，一次 Publish()
即可送达所有订阅者。

- ServiceBus 新增 Broadcast() 接口
- Dispatcher 按 topic 维护订阅者列表

Refs: #42
```

```
fix(engine): 修复 SceneManagerV2 删除对象后悬垂指针

RemoveObject() 只从 _objects 移除了条目，但拾取缓存
仍持有裸指针，随后一次 hover 触发 use-after-free。
删除时同步失效拾取缓存。

Closes: #57
```

```
style(clipper): 对 clipper_widget 执行 clang-format
```
