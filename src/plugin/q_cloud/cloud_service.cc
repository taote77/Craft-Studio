#include "cloud_service.h"

#include <QDebug>
#include <QThread>
#include <QVariant>
#include <qglobal.h>

CloudService::CloudService()
{
  //
}

QString CloudService::serviceName() const
{
  return "cloud";
};

void CloudService::init()
{
  qDebug() << Q_FUNC_INFO << QThread::currentThread();

  registerNotifyHandler("sys/state/change",
    [this](const QVariant& data)
    {
      int state = data.toInt();
      qDebug() << Q_FUNC_INFO << "CloudService sys/state/change" << state;
    });

  registerNotifyHandler("cloud/upload", [this](const QVariant& data) { OnUpload(data); });
}

void CloudService::startup()
{
  qDebug() << "CloudService::startup";
}

void CloudService::cleanup() {}

void CloudService::OnStartup(const QVariant& data)
{
  qDebug() << "CloudService::OnStartup";
}

void CloudService::OnUpload(const QVariant& data)
{
  auto path = data.value<QString>();
  qDebug() << Q_FUNC_INFO << path;
}