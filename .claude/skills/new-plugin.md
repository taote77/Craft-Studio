---
name: new-plugin
description: Scaffolding guide for creating a new CraftsStudio plugin with correct file structure, CMake, and export pattern
metadata:
  type: project
---

# Creating a New CraftsStudio Plugin

## File Structure

```
src/plugin/<plugin_name>/
├── CMakeLists.txt        # Build definition — MUST produce .mx suffix
├── export.cc             # MANDATORY: plugin entry point
├── <service>.h           # Service class declaration
└── <service>.cc          # Service class implementation
```

## Step 1: CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)

set(SOURCES
    export.cc
    my_service.h
    my_service.cc
)

add_library(my_plugin MODULE ${SOURCES})

target_link_libraries(my_plugin
    PRIVATE
    qbus
    pdk
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# CRITICAL: .mx suffix for plugin loading
set_target_properties(my_plugin PROPERTIES
    SUFFIX ".mx"
    PREFIX ""
)

# Output to plugin directory
set_target_properties(my_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUT_PATH}"
    ARCHIVE_OUTPUT_DIRECTORY "${PLUGIN_OUT_PATH}"
)
```

Key points:
- Use `MODULE` (not `SHARED`) — plugins are runtime-loadable modules
- Set `SUFFIX ".mx"` — the plugin loader looks for `.mx` files
- Set `PREFIX ""` — prevent `lib` prefix on the output
- Use `${PLUGIN_OUT_PATH}` for output (defined in root `cmake/build.cmake`)

## Step 2: export.cc (MANDATORY)

```cpp
#include "qbus/service_export.h"
#include "my_service.h"

// If your plugin has a GUI widget:
// #include "my_widget.h"

extern "C" void getServicesMeta(void* pex)
{
    PluginExport* pexport = reinterpret_cast<PluginExport*>(pex);
    pexport->meta_objects.push_back(&MyService::staticMetaObject);

    // If you have multiple services/widgets, push all of them:
    // pexport->meta_objects.push_back(&MyWidget::staticMetaObject);
}
```

Rules:
- Function signature is exactly `extern "C" void getServicesMeta(void*)` — the `extern "C"` prevents C++ name mangling so `dlsym` can find it
- Push ALL service/wiget QMetaObjects — any you omit will not be loadable
- Include `"qbus/service_export.h"` for the `PluginExport` struct

## Step 3: Service Class Header

```cpp
#ifndef MY_SERVICE_H
#define MY_SERVICE_H

#include <qbus/micro_service.h>

class MyService : public qbus::MicroService
{
    Q_OBJECT

public:
    // If this is a GUI service (has widgets), add:
    // Q_PROPERTY(bool withGui READ withGui CONSTANT)

    QString serviceName() const override
    {
        return "my_service";  // Unique name used for lookup
    }

public slots:
    void init() override;       // Register handlers, allocate resources
    void startup() override;    // Start timers, background work
    void cleanup() override;    // Release resources

private:
    // Your internal state with _camelCase naming
    QString _config;
    NotifyHandler _onDataReceived;
};

#endif // MY_SERVICE_H
```

### GUI vs Headless

**GUI service** (has Qt Widgets):
```cpp
Q_PROPERTY(bool withGui READ withGui CONSTANT)
bool withGui() const { return true; }
```

**Headless service** (no GUI):
- Do NOT add the `withGui` property
- Do NOT depend on `Qt::Widgets` in CMake

The `withGui` property tells the service manager to run this service on the Qt GUI main thread.

## Step 4: Service Implementation

```cpp
#include "my_service.h"
#include <QDebug>

void MyService::init()
{
    // Register broadcast handler
    registerNotifyHandler("my_topic/data", [this](const QVariant& data) {
        qDebug() << "[my_service] received data:" << data;
        // Process the data...
    });

    // Register request handler
    registerRequestHandler("my_topic/query", [this](const QVariant& data) -> QVariant {
        // Process and return result
        QVariant result;
        result.setValue(QString("processed: %1").arg(data.toString()));
        return result;
    });

    qDebug() << "[my_service] initialized";
}

void MyService::startup()
{
    // Start timers, background threads, etc.
    // Publish initial state
    publish("my_service/started", QVariant(serviceName()));
}

void MyService::cleanup()
{
    // Stop timers, join threads, release resources
    qDebug() << "[my_service] cleaning up";
}
```

## Step 5: Register in Parent CMake

Add to `src/plugin/CMakeLists.txt`:

```cmake
add_subdirectory(my_plugin)
```

## Step 6: Build & Verify

```bash
# Build just your plugin
cmake --build build --config Debug --target my_plugin

# Verify the export symbol exists
nm -D build/output/plugin/Debug/my_plugin.mx | grep getServicesMeta

# Run the app with your plugin loaded
QBUS_PLUGIN_PATH=build/output/plugin/Debug build/output/bin/Debug/CraftsStudio
```

## Anti-Patterns to Avoid

- ❌ Including headers from other plugins directly — use the service bus
- ❌ Doing heavy work in `init()` — that blocks plugin loading. Use `startup()`
- ❌ Calling Qt GUI functions from a headless service — will crash
- ❌ Forgetting `extern "C"` on `getServicesMeta` — symbol won't be found
- ❌ Using `SHARED` library type instead of `MODULE` — plugins are modules, not libraries
- ❌ Exposing raw pointers across plugin boundaries — use the bus or shared ownership (`QSharedPointer`)

## Reference Implementations

- `src/plugin/q_clipper/` — Modern plugin with both a service AND a GUI widget
- `src/plugin/q_sys/` — Headless services (sys_service, wifi_service)
- `src/plugin/q_cloud/` — Headless service (cloud_service)
