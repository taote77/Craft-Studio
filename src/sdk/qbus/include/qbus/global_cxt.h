#ifndef QBUS_GLOBAL_CXT_H
#define QBUS_GLOBAL_CXT_H

class GlobalCxt
{
public:
  static GlobalCxt* GetInstance();

private:
  GlobalCxt();
  ~GlobalCxt();
};

#endif // QBUS_GLOBAL_CXT_H