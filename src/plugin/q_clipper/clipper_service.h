#ifndef QSYS_DB_H
#define QSYS_DB_H

#include <QObject>
#include <qbus/micro_service.h>

class QVariant;
class QRunnable; // #include <QRunnable>

class ClipperService : public qbus::MicroService
{
  Q_OBJECT
public:
  Q_INVOKABLE ClipperService();

  QString serviceName() const override;

protected slots:
  void init() override;

  void startup() override;

  void cleanup() override;

protected:
  // service interface

  void OnStartup(const QVariant& data);

  QVariant OnProcess(const QVariant& data);

  void process(const QVariant& data);

signals:
  void pub(const QString& topic, const QVariant& data);

  void finish(const QString& result);

protected slots:
  void OnPub(const QString& topic, const QVariant& data);

private:
  QRunnable* _work;
};

#endif // QSYS_DB_H
