#include "sys_service.h"

#include <QVariant>
#include <qglobal.h>

SysService::SysService()
{
  //   QObject::connect(&uploader_, &Uploader::preauth, this, &Cloud::UploadStart,
  //   Qt::QueuedConnection);
}

QString SysService::serviceName() const
{
  return "sys";
};

void SysService::init()
{
  qDebug() << "SysService::init";
}

void SysService::startup()
{
  qDebug() << "SysService::startup";
}

void SysService::cleanup() {}

QVariant SysService::OnFetchResult(const QVariant& data)
{
  return 0;
}
