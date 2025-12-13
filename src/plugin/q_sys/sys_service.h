#ifndef QSYS_DB_H
#define QSYS_DB_H

#include <QDebug>
#include <QObject>
#include <qbus/micro_service.h>

class QVariant;

class SysService : public qbus::MicroService
{
  Q_OBJECT
public:
  Q_INVOKABLE SysService();

  QString serviceName() const override;

public slots:
  void init() override;

  void startup() override;

  void cleanup() override;

protected:
  QVariant OnFetchResult(const QVariant& data);

public slots:

protected slots:
};

#endif // QSYS_DB_H
