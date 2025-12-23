#ifndef Q_CLIPPER_WIDGET_H
#define Q_CLIPPER_WIDGET_H

#include <QObject>
#include <QWidget>

#include <qbus/micro_service.h>

class QVariant;

class ClipperWidget : public qbus::MicroService
{

  // 带有gui 的插件，必须声明 withGui 属性，以便将其初始化在 QApplication gui 主线程
  Q_PROPERTY(QString withGui READ withGui CONSTANT)

  Q_OBJECT
public:
  Q_INVOKABLE ClipperWidget();

  QString serviceName() const override;

public slots:

  void init() override;

  void startup() override;

  void cleanup() override;

protected:
  // service interface

  Q_INVOKABLE bool withGui() { return true; };

  void OnStartup(const QVariant& data);

  void OnUpload(const QVariant& data);

signals:
  void pub(const QString& topic, const QVariant& data);

protected slots:
  void OnPub(const QString& topic, const QVariant& data);

private:
  QWidget* _main_widget = nullptr;
};

#endif // Q_CLIPPER_WIDGET_H
