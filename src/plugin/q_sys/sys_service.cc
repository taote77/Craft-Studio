#include "sys_service.h"

#include <QVariant>

SysService::SysService()
{
  //   QObject::connect(&uploader_, &Uploader::preauth, this, &Cloud::UploadStart,
  //   Qt::QueuedConnection);
}

void SysService::Initialize() {}

void SysService::Cleanup() {}

QVariant SysService::OnFetchResult(const QVariant& data)
{
  return 0;
}
