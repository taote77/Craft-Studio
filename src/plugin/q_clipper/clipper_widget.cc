#include "clipper_widget.h"

#include <QDebug>
#include <QThread>
#include <QVariant>

#include <qglobal.h>
#include <qwidget.h>

ClipperWidget::ClipperWidget()
{
  //
}

QString ClipperWidget::serviceName() const
{
  return "clipper.ui";
};

void ClipperWidget::init()
{
  qDebug() << Q_FUNC_INFO << QThread::currentThread();

  _main_widget = new QWidget(nullptr, Qt::WindowFlags());

  _main_widget->resize(800, 600);
  _main_widget->setWindowTitle("Clipper");
  _main_widget->show();

  registerNotifyHandler("clipper/interface/bool",
    [this](const QVariant& data)
    {
      int state = data.toInt();
      qDebug() << Q_FUNC_INFO << "ClipperWidget clipper/interface/bool" << state;
    });
}

void ClipperWidget::startup()
{
  qDebug() << "ClipperWidget::startup";
}

void ClipperWidget::cleanup() {}

void ClipperWidget::OnStartup(const QVariant& data)
{
  qDebug() << "ClipperWidget::OnStartup";
}

void ClipperWidget::OnUpload(const QVariant& data)
{
  auto path = data.value<QString>();
  qDebug() << Q_FUNC_INFO << path;
}