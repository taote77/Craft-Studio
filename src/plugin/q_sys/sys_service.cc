#include "sys_service.h"

#include <QString>
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

  _timer = new QTimer(this);

  QString topic = "sys/state/change";
  connect(_timer, &QTimer::timeout, this,
    [this, topic]()
    {
      int alpha = 0;
      QVariant data = alpha;
      qDebug() << topic << data;
      publish(topic, data);
    });

  _timer->start(1000 * 5);
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
