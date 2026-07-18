# 开发规范目录（Conventions）

本目录存放 MasterWay / CraftsStudio 项目的团队开发规范，所有成员和 AI 工具（Claude Code、Copilot、Windsurf 等）都应遵守。

| 文档 | 内容 |
| ------ | ------ |
| [commit-convention.md](commit-convention.md) | 代码提交规范 — commit message 格式、type/scope 定义、提交拆分原则 |
| [naming-convention.md](naming-convention.md) | 代码命名规范 — 变量、方法、类、文件、信号槽的命名规则 |
| [code-style.md](code-style.md) | 代码风格规范 — 注释语言、头文件包含顺序、日志与错误处理、格式化工具 |

## 配套工具

- **格式化**: `.clang-format`（仓库根目录，Mozilla 风格 + Allman 大括号 + 100 列）
- **静态检查**: `.clang-tidy`（仓库根目录）
- **提交校验 hook**: `scripts/git-hooks/commit-msg` — 不合规的 commit message 直接拒绝提交
- **Claude Code skill**: `/commit` — 按照提交规范自动生成 commit message（见 `.claude/skills/commit.md`）

## 新成员上手（一次性）

```bash
git config core.hooksPath scripts/git-hooks   # 启用提交校验
```

## 修改规范

规范文件随代码一起走 Git 流程：修改需提交 PR，type 使用 `docs(conventions)`。
