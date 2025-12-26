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
