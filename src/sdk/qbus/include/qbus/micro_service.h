#ifndef QBUS_MICRO_SERVICE
#define QBUS_MICRO_SERVICE

#include "qbus_global.h"
#include <QObject>

namespace qbus
{

class QBUS_API MicroService : public QObject
{
  Q_OBJECT
public:
  MicroService();
  virtual ~MicroService();

protected:
  virtual void Initialize();

  virtual void Cleanup();
};

} // namespace qbus

#endif // QBUS_MICRO_SERVICE