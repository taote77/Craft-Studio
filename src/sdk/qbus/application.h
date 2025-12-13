#ifndef QBUS_APPLICATION_H
#define QBUS_APPLICATION_H

#include <QApplication>

#include <qbus/qbus_global.h>
#include <qbus/service_manager.h>

namespace qbus
{
class QBUS_API Application : public QApplication
{
  Q_OBJECT
public:
  Application(int& argc, char** argv);

  virtual ~Application();

protected:
  void init(const QDir& work_dir);

private:
  ServiceManager _service_manager;
};

#endif // QBUS_APPLICATION_H

} // namespace qbus
