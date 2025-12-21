#ifndef QBUS_GLOBAL_CXT_H
#define QBUS_GLOBAL_CXT_H

/*******************************************************************************
**                           应用全局上下文定义，全局单例
**
**
*******************************************************************************/

#include "qbus/qbus_global.h"

namespace qbus
{

class QBUS_API GlobalCxt
{
public:
  static GlobalCxt* GetInstance();

private:
  GlobalCxt();
  ~GlobalCxt();
};

}

#endif // QBUS_GLOBAL_CXT_H