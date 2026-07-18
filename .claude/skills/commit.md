---
name: commit
description: Compose and create commits following the project commit convention — English type/scope skeleton with Chinese subject/body, atomic splitting, hook-validated
metadata:
  type: project
---

# Writing Commits for CraftsStudio

Authoritative spec: `docs/conventions/commit-convention.md`. This skill is the working procedure; when in doubt, the spec wins.

## Message Format

```
<type>(<scope>)<!>: <subject>      ← type/scope in English, subject in Chinese

<body>                              ← Chinese, explains WHY + behavior change

<footer>                            ← BREAKING CHANGE / Refs / Closes
```

- Language policy: **machine-parsable parts (type, scope, markers) in English; human-readable parts (subject, body) in Chinese**. Code identifiers stay in English inside Chinese text.
- Subject: starts with a verb (增加/修复/重构/移除/优化…), no trailing punctuation, one space between CJK and Latin/digits.
- Header ≤ 100 bytes (hook-enforced, ≈30 汉字).

## Types (closed set)

`feat` `fix` `refactor` `perf` `docs` `style` `test` `build` `ci` `chore` `revert`

Ambiguity priority: `revert` > `feat` > `fix` > `perf` > `refactor` > rest.
If a change looks like both `feat` and `fix` → split into two commits.

## Scopes (closed set)

| scope | area | scope | area |
| ------ | ------ | ------ | ------ |
| `pdk` | src/sdk/pdk | `engine` | CraftsStudio/engine |
| `qbus` | src/sdk/qbus | `backend` | src/app/Backend |
| `clipper` | plugin/q_clipper | `libs` | src/libs |
| `sys` | plugin/q_sys, cs_sys | `cmake` | CMake files |
| `cloud` | plugin/q_cloud | `deps` | 3rd-party deps |
| `db` | plugin/cs_db | `conventions` | docs/conventions |
| `studio` | app/CraftsStudio | `skills` | .claude/skills |

One scope only. ≥3 modules touched → omit scope. New module → register the scope in the spec table within the same PR.

## Workflow

1. **Inspect** — run in parallel:
   ```bash
   git status
   git diff --staged        # what will be committed
   git diff                 # what would be left behind
   git log --oneline -10    # recent context
   ```

2. **Split check** — one commit = one logical change that compiles and reverts independently. If staged changes mix themes (e.g. feature + drive-by formatting, refactor + bugfix), propose a split and use `git add -p`. Never mix `style` noise into a `feat`/`fix` commit.

3. **Format gate** — staged C++ files must be clang-formatted:
   ```bash
   git diff --name-only --cached | grep -E '\.(h|cc|cpp)$' | xargs -r clang-format -i -style=file
   ```
   If formatting touches files beyond the current theme, that goes in a separate `style` commit.

4. **Compose** — pick type (see priority rule) and scope, write Chinese subject describing WHAT changed (not how). Body is required for `feat`, non-obvious `fix`, and any behavior change: explain motivation and behavioral impact, do not paraphrase the diff. Breaking public API / topic semantics / plugin contract → add `!` and a `BREAKING CHANGE:` footer.

5. **Commit** — use heredoc for multi-line messages:
   ```bash
   git commit -m "$(cat <<'EOF'
   feat(qbus): 增加服务总线广播分发机制

   此前服务之间必须逐个注册点对点 handler。
   增加广播通道后，一次 Publish() 即可送达所有订阅者。

   Refs: #42
   EOF
   )"
   ```
   The `commit-msg` hook (scripts/git-hooks/) validates the header. If rejected, fix the message — never suggest `--no-verify`.
   If the hook is not installed (`git config core.hooksPath` prints nothing), remind the user to run `git config core.hooksPath scripts/git-hooks`.

## Never

- `update:` / `wip` / `修改代码` / `fix bug` — banned as messages
- Subject ending with `。` or `.`
- Issue number as the subject (put it in `Refs:`/`Closes:` footer)
- One commit spanning sdk + plugin + app without a single theme

## Good Examples

```
fix(engine): 修复 SceneManagerV2 删除对象后悬垂指针

RemoveObject() 只从 _objects 移除了条目，但拾取缓存
仍持有裸指针，随后一次 hover 触发 use-after-free。
删除时同步失效拾取缓存。

Closes: #57
```

```
refactor(qbus)!: 将 ServiceBus::Request 的超时参数改为 std::chrono

BREAKING CHANGE: Request(topic, msg, int ms) 已移除，
调用方需改为 std::chrono::milliseconds，所有插件需重新编译。
```

```
style(clipper): 对 clipper_widget 执行 clang-format
```
