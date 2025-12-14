#ifndef MOD_SYS_WIFI_H
#define MOD_SYS_WIFI_H

#include <QDebug>
#include <QObject>
#include <qbus/micro_service.h>
#include <qtimer.h>

class QVariant;

class WifiService : public qbus::MicroService
{
  Q_OBJECT
public:
  Q_INVOKABLE WifiService();

  QString serviceName() const override;

public slots:
  void init() override;

  void startup() override;

  void cleanup() override;

protected:
  QVariant OnFetchResult(const QVariant& data);

public:
private:
  QTimer* _timer;
};

#endif // MOD_SYS_WIFI_H
