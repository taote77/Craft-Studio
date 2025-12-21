#ifndef QSYS_DB_H
#define QSYS_DB_H

#include <QObject>
#include <qbus/micro_service.h>

class QVariant;

class CloudService : public qbus::MicroService
{
  Q_OBJECT
public:
  Q_INVOKABLE CloudService();

  QString serviceName() const;

public slots:

  void init() override;

  void startup() override;

  void cleanup() override;

protected:
  // service interface

  void OnStartup(const QVariant& data);

  void OnUpload(const QVariant& data);

signals:
  void pub(const QString& topic, const QVariant& data);

protected slots:
  void OnPub(const QString& topic, const QVariant& data);
};

#endif // QSYS_DB_H
