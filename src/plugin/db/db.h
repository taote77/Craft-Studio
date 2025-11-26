#ifndef CS_DB_H
#define CS_DB_H

#include <QCoreApplication>
#include <QFileInfo>
#include <QSharedPointer>
#include <QTimer>
#include <QUuid>

#include <pdk/object.h>

class Db : public pdk::Object
{
  Q_OBJECT
  PDK_OBJECT_INIT
  {
    PDK_BIND(0, Startup);

    PDK_BIND(1, OnFetchResult);
  }

public:
  Db();

  static QString kModuleAddress;

  void Run() override;

  void Startup(const QVariant& data);

  QVariant OnFetchResult(const QVariant& data);

public slots:

protected slots:
  void Dispatched(quint16 id, const std::string& data);

protected:
};

#endif // CS_DB_H
