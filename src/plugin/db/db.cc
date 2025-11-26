#include "db.h"

QString Db::kModuleAddress = "";

Db::Db()
{
  //   QObject::connect(&uploader_, &Uploader::preauth, this, &Cloud::UploadStart,
  //   Qt::QueuedConnection);
}

void Db::Run()
{
  kModuleAddress = Path();
}

void Db::Startup(const QVariant& data)
{
  qInfo() << "Db service startup";
}

QVariant Db::OnFetchResult(const QVariant& data)
{
  return 0;
}
