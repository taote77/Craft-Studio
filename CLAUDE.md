# CLAUDE.md

## Project Identity

- **Project name**: MasterWay
- **Product name**: CraftsStudio
- **Description**: C++20/Qt6/VTK desktop 3D printing slicer & pre-processing application with a microservice plugin architecture. Core features:
  - **Mesh repair**: defect detection (残缺检测), hole filling, non-manifold fixing, watertight conversion
  - **Shell operations**: shell detection (套壳检测), shell splitting, hollowing, boolean operations (via Clipper2)
  - **Slicing**: model slicing, support generation, G-code generation
  - **Model preprocessing**: import (STL/OBJ/3MF/AMF/PLY), transform, layout on build plate, auto-arrangement
- **Build system**: CMake 3.20+ with Ninja Multi-Config generator
- **Standard**: C++20, Qt 5/6 (prefer Qt6), VTK 9+ (with Qt support)
- **Remote**: `git@github.com:taote77/Craft-Studio.git`

## Build Commands

```bash
# Full debug build (all targets)
cmake --build build --config Debug

# Full release build
cmake --build build --config Release

# Build specific targets
cmake --build build --config Debug --target CraftsStudio
cmake --build build --config Debug --target Backend
cmake --build build --config Debug --target clipper_app
cmake --build build --config Debug --target qbus
cmake --build build --config Debug --target pdk

# Build a single plugin
cmake --build build --config Debug --target q_clipper
cmake --build build --config Debug --target q_sys
cmake --build build --config Debug --target q_cloud

# Reconfigure CMake (after adding files, changing deps)
cmake -S . -B build -G "Ninja Multi-Config"

# Run applications
build/output/bin/Debug/CraftsStudio       # Main 3D GUI app
build/output/bin/Debug/Backend            # Headless backend service
build/output/bin/Debug/clipper_app        # Clipper2 test/demo app

# Code quality
find src -name "*.h" -o -name "*.cc" -o -name "*.cpp" | xargs clang-format -i -style=file
clang-tidy <file> -p build -- -std=c++20
```

**Output layout** (configured in `cmake/build.cmake`):
| Type | Path |
|------|------|
| Executables | `build/output/bin/` |
| Libraries | `build/output/lib/` |
| Plugins (.mx) | `build/output/plugin/` |
| Static libs | `build/staticlib/` |

## Architecture

```
┌──────────────────────────────────────────┐
│  Applications (CraftsStudio, Backend,     │
│  clipper_app, vtk_app)                    │
│  Depends on: SDKs, VTK, Qt6, TBB, Clipper2│
└──────────────────┬───────────────────────┘
                   │
┌──────────────────┴───────────────────────┐
│  Plugins (.mx hot-reloadable modules)     │
│  q_clipper, q_sys, q_cloud, cs_db, cs_sys│
│  Depends on: qbus, pdk, domain libs      │
│  Communicate through service bus ONLY     │
└──────────────────┬───────────────────────┘
                   │
┌──────────────────┴───────────────────────┐
│  sdk/qbus — Qt Microservice Framework    │
│  Service bus, dispatcher, router,        │
│  hot-reload, service lifecycle           │
│  Depends on: Qt6, pdk                    │
└──────────────────┬───────────────────────┘
                   │
┌──────────────────┴───────────────────────┐
│  sdk/pdk — Plugin Development Kit        │
│  Object model, messaging, plugin loading │
│  Depends on: protobuf                    │
└──────────────────────────────────────────┘
```

### Layer Dependency Rules

1. **pdk** must NOT depend on qbus, any plugin, or any app
2. **qbus** must NOT depend on any plugin or app
3. **Plugins** must NOT directly depend on other plugins — communicate through the service bus
4. **Apps** can depend on all SDKs but should prefer bus-based communication with plugins
5. **No layer** should include headers from `3rd-party/` directly except through wrappers

### Plugin Interface Contract

Every plugin MUST have an `export.cc` with the canonical entry point:

```cpp
#include "qbus/service_export.h"
#include "my_service.h"

extern "C" void getServicesMeta(void* pex)
{
  PluginExport* pexport = reinterpret_cast<PluginExport*>(pex);
  pexport->meta_objects.push_back(&MyService::staticMetaObject);
}
```

**Lifecycle** (called in order by the service manager):
1. `MicroService::init()` — register handlers, allocate resources
2. `MicroService::startup()` — start background work, timers
3. `MicroService::cleanup()` — release resources

**GUI plugins** MUST declare: `Q_PROPERTY(bool withGui READ withGui CONSTANT)` (forces execution on Qt GUI main thread).

**Headless plugins** (non-GUI services) MUST NOT depend on Qt::Widgets.

### Message Patterns (pdk::Message)

Defined in `src/sdk/pdk/message.h`:

| Type | Method | Thread Safety | Use Case |
|------|--------|---------------|----------|
| `Async` | `Publish()` / `registerNotifyHandler()` | Safe | Fire-and-forget notifications, status updates |
| `FakeSync` | `Request()` (event loop pumps) | User-thread safe | Most service calls |
| `Sync` | `BlockRequest()` | Thread safe | Background thread calls |
| `DangerSync` | `Invoke()` | **NOT** thread safe | Same-thread only — can deadlock! |

**Topic conventions:**
- String topics: descriptive names like `"model/imported"`, `"slice/complete"`
- Integer topics: well-known protocol topics (defined in protobuf enums)
- Use `PDK_BIND(topic, method)` macro for convenient handler registration

## Coding Conventions

Full team conventions live in `docs/conventions/` — commit messages ([commit-convention.md](docs/conventions/commit-convention.md), hook-enforced), naming, and code style. The tables below are quick references.

### Commit Messages (enforced by `scripts/git-hooks/commit-msg`)

- Format: `<type>(<scope>)!?: <subject>` — type/scope in **English**, subject/body in **Chinese**
- Types: `feat` `fix` `refactor` `perf` `docs` `style` `test` `build` `ci` `chore` `revert`
- Scopes (closed set): `pdk` `qbus` `clipper` `sys` `cloud` `db` `studio` `engine` `backend` `libs` `cmake` `deps` `conventions` `skills`
- Enable the hook once per clone: `git config core.hooksPath scripts/git-hooks`
- Use the `/commit` skill to compose commits — full spec in `docs/conventions/commit-convention.md`

### Naming (standard for ALL new code)

| Element | Convention | Example |
|---------|------------|---------|
| Member variables | `_camelCase` | `_dispatcher`, `_vtkRenderWindow` |
| Methods | `PascalCase` | `setupUI()`, `addObject()` |
| Classes | `PascalCase` | `SceneManager`, `MainWindow` |
| Files | `snake_case` | `scene_manager.h`, `main_window.cc` |
| Namespaces | `lowercase` | `pdk`, `qbus` |
| Macros / Constants | `UPPER_CASE` | `PDK_OBJECT_INIT`, `DECLARE_PLUGIN` |
| Signals / slots | `camelCase` / `camelCase` | `objectAdded`, `onPub` |

**Exception**: pdk library uses `trailing_underscore_` (e.g., `dispatcher_`, `objects_`). This is a stable library — do NOT rename. New code outside pdk uses `_leading_underscore`.

### Comments

- **New code**: Write comments in **English**
- **Existing Chinese comments**: Keep them; translate when significantly modifying the file

### Include Order

Within each `.cc` file, group includes with a blank line between groups:

1. Own header (e.g., `#include "my_class.h"`)
2. Project headers — grouped by layer: pdk, qbus, then app/plugin
3. Qt headers
4. VTK headers
5. Third-party (Clipper2, protobuf, TBB)
6. Standard library
7. System headers

### Error Handling & Logging

Use the logging macros from `pdk::Object` (defined in `src/sdk/pdk/object.h`):

```cpp
Debug() << "Processing model:" << modelName;
Info() << "Plugin loaded:" << pluginName;
Warning() << "Memory usage high:" << usage << "MB";
Critical() << "Failed to load file:" << filePath;
```

For recoverable errors, return an error struct rather than throwing exceptions.

### File Organization

```
src/
  app/<app_name>/
    <component>.h / .cc     — One class per header (small helpers excepted)
    CMakeLists.txt

  plugin/<plugin_name>/
    export.cc               — MANDATORY plugin entry point
    <service_name>.h / .cc  — Service implementation
    CMakeLists.txt           — Must produce .mx suffix for modern plugins

  sdk/<sdk_name>/
    <module>.h / .cc
    CMakeLists.txt
```

## Key Files

| File | Role |
|------|------|
| `cmake/build.cmake` | Output directory configuration |
| `src/sdk/pdk/object.h` | Base class for all plugin objects (logging, messaging, lifecycle) |
| `src/sdk/pdk/message.h` | Message struct with 4 communication types |
| `src/sdk/qbus/micro_service.h` | MicroService base class (init/startup/cleanup, pub/sub) |
| `src/sdk/qbus/service_bus.h` | Central communication hub |
| `src/sdk/qbus/service_export.h` | PluginExport struct definition |
| `src/plugin/q_clipper/export.cc` | Canonical modern plugin export example |
| `src/plugin/q_clipper/clipper_widget.h` | Example GUI microservice (note: `withGui` property) |
| `src/plugin/q_sys/sys_service.h` | Example headless microservice |
| `src/app/CraftsStudio/main.cc` | Main application entry point |
| `src/app/CraftsStudio/engine/rs_scene_manager_v2.h` | Scene graph singleton (current active engine) |

## Debugging Quick Reference

```bash
# Run with plugin path (needed for hot-reload)
QBUS_PLUGIN_PATH=build/output/plugin/Debug build/output/bin/Debug/CraftsStudio

# Trace VTK module loading
VTK_DEBUG_MODULE_LOADER=ON build/output/bin/Debug/CraftsStudio

# Verify plugin exports
nm -D build/output/plugin/Debug/q_clipper.mx | grep getServicesMeta

# Check library dependencies
ldd build/output/bin/Debug/CraftsStudio

# GDB with pretty-printing
gdb -ex "set print pretty on" build/output/bin/Debug/CraftsStudio

# Build one plugin and copy to output (quick iteration)
cmake --build build --config Debug --target q_clipper
```

## Code Quality Tools

- **Formatter**: `.clang-format` — Mozilla-based, Allman braces, 100 cols, C++17 standard macros
- **Linter**: `.clang-tidy` — Comprehensive checks enabled (boost, bugprone, clang-analyzer, modernize, performance, readability)
- **No CI/CD**: Currently none — local quality checks only

## Current State

- ~44 commits on `master`, synced with `origin/master`
- Branch `dev/new` currently identical to `master`
- Active development area: PlaterWidget + SceneManagerV2 + model import pipeline
- Recent features: model import, Clipper2 concurrent algorithms, UI service support, broadcast messaging
- Builds with Ninja Multi-Config; output to `build/output/`

## AI Infrastructure Setup (Team Sharing)

This project uses **symlinks** to share AI context across tools and team members.

### File layout

```
MasterWay/
├── CLAUDE.md                          ← Canonical project context (ALL tools read this)
├── AGENTS.md          → CLAUDE.md     ← Symlink (Codex, Copilot)
├── .windsurfrules     → CLAUDE.md     ← Symlink (Windsurf)
├── CONVENTIONS.md     → CLAUDE.md     ← Symlink (Aider) — create if needed
├── .github/
│   └── copilot-instructions.md → ../CLAUDE.md  ← Symlink (GitHub Copilot)
└── .claude/
    ├── settings.local.json            ← Claude Code permissions (local only)
    └── skills/                        ← Shared skill files (version controlled)
        ├── build.md
        ├── commit.md
        ├── debug.md
        └── new-plugin.md
```

### Team member setup

Each team member clones the repo, then creates one user-level symlink to share skills across all their projects:

```bash
# Clone the repo
git clone git@github.com:taote77/Craft-Studio.git

# Share skills globally (all Claude Code projects see them)
ln -s /path/to/Craft-Studio/.claude/skills ~/.claude/skills

# Optional: create additional cross-tool symlinks if your project doesn't have them
cd /path/to/other-project
ln -sf /path/to/Craft-Studio/CLAUDE.md AGENTS.md
```

After setup:
- **Skills** (`build`, `commit`, `debug`, `new-plugin`) are available in EVERY Claude Code project via `~/.claude/skills/`
- **Updates** to skills are `git pull`ed — everyone gets improvements immediately
- **New tools**: add a symlink pointing to `CLAUDE.md` (e.g., `.cursorrules → CLAUDE.md`)

### Adding a new skill

1. Create `MasterWay/.claude/skills/<name>.md`
2. Commit and push
3. Team members `git pull` to get it — no other config needed (symlink auto-resolves)

## Related Documentation

- `docs/conventions/` — Team conventions: commit messages, naming, code style
- `src/sdk/qbus/README.md` — QBus microservice framework overview
- `src/sdk/qbus/docs/Architecture.md` — Plugin types (GUI vs headless)
- `note-learn/vtk/` — VTK learning notes and examples
- `note-learn/cv/` — Computer vision learning notes
