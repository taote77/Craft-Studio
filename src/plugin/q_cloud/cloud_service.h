#ifndef QSYS_DB_H
#define QSYS_DB_H

#include <QObject>
#include <qbus/micro_service.h>

class QVariant;

class CloudService : public qbus::MicroService
{
  Q_OBJECT
public:
  CloudService();

protected:
  void init() override;

  void cleanup() override;

  // service interface

  void OnStartup(const QVariant& data);

public slots:

protected slots:
};

#endif // QSYS_DB_H
