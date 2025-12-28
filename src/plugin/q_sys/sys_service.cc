#include "sys_service.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringView>
#include <QThread>
#include <QVariant>

#include <qchar.h>
#include <qglobal.h>
#include <qjsonobject.h>
#include <qtimer.h>

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

  QString topic_sys_state_change = "sys/resource/info";
  registerNotifyHandler(topic_sys_state_change,
    [this](const QVariant& data)
    {
      auto state = data.toJsonObject();

      qDebug() << Q_FUNC_INFO << state;
    });
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

      static int c{ 0 };
      if (c >= 2)
      {
        _timer->stop();
      }
      c++;

      publish(topic, json_info);
    });

  _timer->start(1000 * 5);

  QTimer::singleShot(1000 * 5,
    [this]()
    {
      QString topic_bool{ "clipper/bool" };
      qDebug() << "request" << topic_bool;

      auto ret = request(topic_bool, 4);
      int ret_int = ret.toInt();
      qDebug() << "request ==========" << ret_int;
    });
}

void SysService::cleanup() {}

QVariant SysService::OnFetchResult(const QVariant& data)
{
  return 0;
}
