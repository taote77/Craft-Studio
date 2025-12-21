#include "wifi_service.h"

#include <QString>
#include <QThread>
#include <QVariant>

#include <qglobal.h>

WifiService::WifiService()
{
  //
}

QString WifiService::serviceName() const
{
  return "sys.wifi";
};

void WifiService::init()
{

  qDebug() << Q_FUNC_INFO << QThread::currentThread();

  // _timer = new QTimer(this);

  // QString topic = "sys/wifi/change";
  // connect(_timer, &QTimer::timeout, this,
  //   [this, topic]()
  //   {
  //     QVariant data{ 1 };
  //     // qDebug() << topic << data;
  //     // publish(topic, data);
  //   });

  // _timer->start(1000 * 20);
}

void WifiService::startup()
{
  qDebug() << "WifiService::startup";
}

void WifiService::cleanup() {}

QVariant WifiService::OnFetchResult(const QVariant& data)
{
  return 0;
}
