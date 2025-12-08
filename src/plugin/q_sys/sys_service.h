#ifndef QSYS_DB_H
#define QSYS_DB_H

#include <QObject>
#include <qbus/micro_service.h>

class QVariant;

class SysService : public qbus::MicroService
{
  Q_OBJECT
public:
  SysService();

protected:
  void Initialize() override;

  void Cleanup() override;

  QVariant OnFetchResult(const QVariant& data);

public slots:

protected slots:
};

#endif // QSYS_DB_H
