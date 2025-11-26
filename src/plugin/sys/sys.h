#ifndef SYS_DB_H
#define SYS_DB_H

#include <pdk/object.h>

class Sys : public pdk::Object
{
  Q_OBJECT
  PDK_OBJECT_INIT
  {
    PDK_BIND(3, Startup);

    PDK_BIND(4, OnFetchResult);
  }

public:
  Sys();

  static QString kModuleAddress;

  void Run() override;

  void Startup(const QVariant& data);

  QVariant OnFetchResult(const QVariant& data);

public slots:

protected slots:

protected:
};

#endif // SYS_DB_H
