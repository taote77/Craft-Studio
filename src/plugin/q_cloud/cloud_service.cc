#include "cloud_service.h"

#include <QDebug>
#include <QVariant>
#include <qglobal.h>

CloudService::CloudService()
{
  //   QObject::connect(&uploader_, &Uploader::preauth, this, &Cloud::UploadStart,
  //   Qt::QueuedConnection);
}

QString CloudService::serviceName() const
{
  return "cloud";
};

void CloudService::init()
{
  //
  subscribe("sys/state/change", false);
  qDebug() << "CloudService::init";
}

void CloudService::startup()
{
  qDebug() << "CloudService::startup";
}

void CloudService::cleanup() {}

void CloudService::OnStartup(const QVariant& data)
{
  qDebug() << "CloudService::OnStartup";
  return;
}
