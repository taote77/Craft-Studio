#include "sys_service.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringView>
#include <QThread>
#include <QVariant>

#include <qglobal.h>
#include <qjsonobject.h>

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
  qDebug() << Q_FUNC_INFO << QThread::currentThread();

  // QString topic_sys_state_change = "sys/resource/info";
  // registerNotifyHandler(topic_sys_state_change,
  //   [this](const QVariant& data)
  //   {
  //     int state = data.toInt();

  //     qDebug() << Q_FUNC_INFO << state;
  //   });
}

void SysService::startup()
{
  qDebug() << "SysService::startup";

  _timer = new QTimer(this);

  QString topic = "sys/resource/info";
  connect(_timer, &QTimer::timeout, this,
    [this, topic]()
    {
      QJsonObject json_info;
      json_info["cpu"] = 40;
      json_info["mem"] = 40;

      publish(topic, json_info);
    });

  _timer->start(1000 * 5);
}

void SysService::cleanup() {}

QVariant SysService::OnFetchResult(const QVariant& data)
{
  return 0;
}
