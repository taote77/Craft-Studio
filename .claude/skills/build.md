---
name: build
description: Build CraftsStudio and its sub-targets — handling CMake config, common errors, and incremental builds
metadata:
  type: project
---

# Building CraftsStudio

## Quick Builds

```bash
# Full project (all targets)
cmake --build build --config Debug

# With presets
cmake --build --preset debug

# Single targets (fastest for iteration)
cmake --build build --config Debug --target CraftsStudio
cmake --build build --config Debug --target Backend
cmake --build build --config Debug --target clipper_app
cmake --build build --config Debug --target q_clipper
cmake --build build --config Debug --target qbus
cmake --build build --config Debug --target pdk
```

## Reconfigure (when adding files or changing dependencies)

```bash
cmake -S . -B build -G "Ninja Multi-Config"
```

Run this when:
- You added new `.cc`/`.h` files
- You changed `CMakeLists.txt` in any subdirectory
- You modified `target_link_libraries` or `target_sources`

## Build Output

| Target type | Location |
|-------------|----------|
| Executables | `build/output/bin/<Config>/` |
| Shared libs | `build/output/lib/<Config>/` |
| Plugins (.mx) | `build/output/plugin/<Config>/` |
| Static libs | `build/staticlib/` |

## Common Build Errors

### "No rule to make target"
→ You added a new file but didn't reconfigure. Run `cmake -S . -B build -G "Ninja Multi-Config"`.

### "undefined reference to Qt..."
→ Qt version mismatch. Check `CMAKE_PREFIX_PATH` points to your Qt6 install:
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/home/shane/Qt/6.11.1/gcc_64
```

### "VTK not found"
→ VTK must be built with Qt support. Rebuild VTK:
```bash
cmake -S /home/shane/workspace/VTK -B /home/shane/workspace/VTK/build \
  -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DVTK_QT_VERSION=6 \
  -DCMAKE_PREFIX_PATH=/home/shane/Qt/6.11.1/gcc_64 \
  -DVTK_GROUP_ENABLE_Qt=YES \
  -DCMAKE_INSTALL_PREFIX=/home/shane/.local \
  -DVTK_MODULE_ENABLE_VTK_GUISupportQt=YES \
  -DVTK_MODULE_ENABLE_VTK_RenderingQt=YES
cmake --build /home/shane/workspace/VTK/build
cmake --install /home/shane/workspace/VTK/build
```

### "plugin export symbol missing"
→ Plugin module must be `MODULE` type (not `SHARED`), have `SUFFIX ".mx"`, and `export.cc` must use `extern "C"` on `getServicesMeta`.

## Incremental Build Tips

- **Edit only .cc files**: Just run the target build — CMake handles dependencies
- **Edit a header used everywhere**: Rebuild from top to ensure all consumers recompile
- **Plugin-only changes**: Just build the plugin target, then trigger reload in a running app
- **Add/remove files**: Always reconfigure CMake first

## Code Formatting

```bash
# Format all source files before committing
find src -name "*.h" -o -name "*.cc" -o -name "*.cpp" | xargs clang-format -i -style=file

# Check a single file
clang-format -style=file --dry-run src/app/CraftsStudio/ui/mainwindow.cc
```
