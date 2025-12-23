#include "clipper_service.h"

#include <QDebug>
#include <QThread>
#include <QVariant>

#include <qglobal.h>

ClipperService::ClipperService()
{
  //
}

QString ClipperService::serviceName() const
{
  return "clipper.service";
};

void ClipperService::init()
{
  qDebug() << Q_FUNC_INFO << QThread::currentThread();

  registerNotifyHandler("sys/state/change",
    [this](const QVariant& data)
    {
      int state = data.toInt();
      qDebug() << Q_FUNC_INFO << "ClipperService sys/state/change" << state;
    });

  registerNotifyHandler("clipper/upload", [this](const QVariant& data) { OnUpload(data); });
}

void ClipperService::startup()
{
  qDebug() << "ClipperService::startup";
}

void ClipperService::cleanup() {}

void ClipperService::OnStartup(const QVariant& data)
{
  qDebug() << "ClipperService::OnStartup";
}

void ClipperService::OnUpload(const QVariant& data)
{
  auto path = data.value<QString>();
  qDebug() << Q_FUNC_INFO << path;
}