#include "cloud_service.h"

#include <QDebug>
#include <QVariant>

CloudService::CloudService()
{
  //   QObject::connect(&uploader_, &Uploader::preauth, this, &Cloud::UploadStart,
  //   Qt::QueuedConnection);
}

void CloudService::init() {}

void CloudService::cleanup() {}

void CloudService::OnStartup(const QVariant& data)
{
  qDebug() << "CloudService::OnStartup";
  return;
}
