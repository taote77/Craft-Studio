#include <qbus/service_export.h>
#include <qbus/service_manager.h>

#include <QDebug>
#include <QLibrary>

namespace qbus
{

ServiceManager::ServiceManager()
{
  //
}

ServiceManager::~ServiceManager()
{
  //
}

void ServiceManager::loadServices(const QDir& plugin_dir)
{
  for (auto filename : getPluginsLoadOrder(plugin_dir))
  {
    qDebug() << "load plugin:" << plugin_dir.absoluteFilePath(filename);
    QLibrary plugin(plugin_dir.absoluteFilePath(filename), this);
    if (plugin.load())
    {
      auto get_services_meta = reinterpret_cast<plugin_api_t>(plugin.resolve("getServicesMeta"));
      if (get_services_meta)
      {
        PluginExport plugin_export;
        get_services_meta(&plugin_export);
        for (const QMetaObject* meta_obj : plugin_export.meta_objects)
        {
          _services_meta.push_back(meta_obj);
        }
      }
      else
      {
        qCritical() << "get_services_meta failed";
      }
    }
    else
    {
      qCritical() << "load plugin failed," << plugin.fileName() << "," << plugin.errorString();
    }
  }
}

QStringList ServiceManager::getPluginsLoadOrder(const QDir& plugin_dir)
{
  QStringList load_order;
  QStringList filters;
  filters << "*.mx";
  for (auto file : plugin_dir.entryList(filters))
  {
    load_order.push_back(plugin_dir.absoluteFilePath(file));
  }

  qDebug() << "plugins load order: " << load_order;
  return load_order;
}

void ServiceManager::Start()
{
  //
}

void ServiceManager::Stop()
{
  //
}
}