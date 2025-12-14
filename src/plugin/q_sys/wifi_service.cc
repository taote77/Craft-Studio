#include "wifi_service.h"

#include <QString>
#include <QVariant>

#include <qglobal.h>

WifiService::WifiService()
{
  //   QObject::connect(&uploader_, &Uploader::preauth, this, &Cloud::UploadStart,
  //   Qt::QueuedConnection);
}

QString WifiService::serviceName() const
{
  return "sys.wifi";
};

void WifiService::init()
{
  qDebug() << "WifiService::init";

  _timer = new QTimer(this);

  QString topic = "sys/wifi/change";
  connect(_timer, &QTimer::timeout, this,
    [this, topic]()
    {
      QVariant data{ 1 };
      qDebug() << topic << data;
      publish(topic, data);
    });

  _timer->start(3000);
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
