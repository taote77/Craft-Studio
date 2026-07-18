---
name: debug
description: Debugging guide for CraftsStudio — plugin hot-reload, VTK rendering, qbus message tracing, GDB tips
metadata:
  type: project
---

# CraftsStudio Debugging Guide

## Plugin Hot-Reload Debugging

### Verify plugin exports

Each `.mx` plugin must export a `getServicesMeta` symbol:

```bash
nm -D build/output/plugin/Debug/q_clipper.mx | grep getServicesMeta
# Expected output: T getServicesMeta  (T = text section, exported)
```

If the symbol is missing, check that `export.cc` is in the CMake target sources and has `extern "C"`.

### Plugin loading trace

```bash
QBUS_PLUGIN_PATH=build/output/plugin/Debug build/output/bin/Debug/CraftsStudio
```

Watch stderr for plugin loading/failure messages from the service manager.

### Breakpoint in a plugin

Since plugins (`.mx` files) are loaded at runtime via `dlopen`, GDB doesn't know about them at startup. Strategies:

1. **Conditional breakpoint**: Set a breakpoint on `dlopen` and step until your plugin loads, then set breakpoints in the plugin
2. **Pending breakpoints**: `gdb -ex "set breakpoint pending on"` then set breakpoints by function name — GDB will resolve them when the shared library loads
3. **Sleep trick**: Add a temporary `sleep(10)` after plugin loading in the app, attach GDB during that window

### Quick plugin iteration

```bash
# Build only the plugin (fast)
cmake --build build --config Debug --target q_clipper

# The .mx is already in build/output/plugin/Debug/ — trigger reload in the running app
```

## VTK Rendering Debugging

### Module loading trace

```bash
VTK_DEBUG_MODULE_LOADER=ON build/output/bin/Debug/CraftsStudio
```

Prints every VTK module as it's loaded — useful when VTK can't find a factory or reader.

### OpenGL issues

```bash
LIBGL_DEBUG=verbose build/output/bin/Debug/CraftsStudio
```

Useful for diagnosing shader compilation errors, framebuffer issues, or render window initialization failures.

### Inspect VTK objects in GDB

```gdb
(gdb) p *renderer
(gdb) p actor->GetMapper()
(gdb) p actor->GetProperty()->GetColor()
(gdb) p dataset->GetNumberOfPoints()
```

### Common VTK pitfalls

- **No render**: Check `QVTKOpenGLNativeWidget` is added to a layout and `show()` was called
- **Blank window**: Verify a `vtkRenderer` was added via `renderWindow->AddRenderer()`
- **Crashes in pipeline update**: Often caused by null input connections — check `GetInputConnection()` returns non-null
- **Coordinate mismatch**: `vtkCoordinate::SetCoordinateSystemToWorld()` vs `ToDisplay()` vs `ToViewport()`

## qbus Message Tracing

### Enable verbose message logging

In the qbus source (`micro_service.cc`), uncomment or add:

```cpp
qDebug() << "[qbus]" << serviceName() << "received:" << event.topic;
```

Rebuild qbus and run — all message traffic will be logged.

### Request timeout

Default request timeout is 20 seconds (`RequestImpl(msg, 20000)` in `pdk/object.h`). If services hang:

1. Check that the handler is registered: look for `registerRequestHandler(topic, ...)` in `init()`
2. Reduce timeout for debugging: `Request(topic, data, 5000)` (5 seconds)
3. Check for deadlocks: `DangerSync`/`Invoke()` must only be used same-thread

### Deadlock diagnosis

If the app freezes after a service call:

1. Attach GDB: `gdb -p $(pidof CraftsStudio)`
2. Get backtraces: `thread apply all bt`
3. Look for threads waiting on mutexes or `QWaitCondition`
4. Check if any thread called `Invoke()` (DangerSync) cross-thread

### Service registration verification

Add this diagnostic in the app startup or service manager:

```cpp
// After all plugins are loaded
qDebug() << "Registered services:";
for (auto& name : serviceManager->serviceNames()) {
    qDebug() << "  -" << name;
}
```

## GDB Tips

### Launch with Qt/VTK friendly settings

```bash
gdb -ex "set print pretty on" \
    -ex "set print elements 0" \
    -ex "handle SIG34 nostop noprint" \
    build/output/bin/Debug/CraftsStudio
```

- `set print pretty on` — readable struct/class output
- `set print elements 0` — no truncation of long strings
- `handle SIG34 nostop` — ignore Qt's internal signal used for thread synchronization

### Inspect Qt types

```gdb
(gdb) p qPrintable(myQString)
(gdb) p myQString.toStdString()
(gdb) p myQList.size()
```

### Inspect smart pointers

```gdb
(gdb) p *mySharedPtr.data()
(gdb) p *myWeakPtr.lock().data()
```

## Environment Variables Summary

| Variable | Purpose |
| -------- | ------- |
| `QBUS_PLUGIN_PATH` | Directory where `.mx` plugin files are found |
| `LD_LIBRARY_PATH` | Must include `build/output/lib/Debug` for libpdk.so and libqbus.so |
| `VTK_DEBUG_MODULE_LOADER` | Set to `ON` to trace VTK module loading |
| `LIBGL_DEBUG` | Set to `verbose` for OpenGL/Mesa debugging |
