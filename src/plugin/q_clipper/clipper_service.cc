#include "clipper_service.h"

#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QVariant>

#include <qglobal.h>
#include <qthread.h>
#include <qthreadpool.h>
#include <thread>

ClipperService::ClipperService()
{
  //
}

QString ClipperService::serviceName() const
{
  return "clipper.service";
};

void ClipperService::init()
{
  qDebug() << Q_FUNC_INFO << QThread::currentThread();

  registerNotifyHandler("sys/state/change",
    [this](const QVariant& data)
    {
      int state = data.toInt();
      qDebug() << Q_FUNC_INFO << "ClipperService sys/state/change" << state;
    });

  registerRequestHandler("clipper/bool", [this](const QVariant& data) { return OnProcess(data); });
}

void ClipperService::startup()
{
  qDebug() << "ClipperService::startup";

  _work = QRunnable::create(
    [this]()
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      QString fff = "fff";
      qDebug() << Q_FUNC_INFO << fff << QThread::currentThreadId();
      Q_EMIT finish(fff);
    });
  _work->setAutoDelete(false);

  QThreadPool::globalInstance()->tryStart(_work);

  QThreadPool::globalInstance()->start(_work);

  QObject::connect(this, &ClipperService::finish, this,
    [this](const QString& data) { qDebug() << Q_FUNC_INFO << data << QThread::currentThreadId(); });
}

void ClipperService::cleanup() {}

void ClipperService::OnStartup(const QVariant& data)
{
  qDebug() << "ClipperService::OnStartup";
}

QVariant ClipperService::OnProcess(const QVariant& data)
{
  qDebug() << Q_FUNC_INFO;
  std::this_thread::sleep_for(std::chrono::seconds(4));

  qDebug() << Q_FUNC_INFO << "done";

  return 12;
}
