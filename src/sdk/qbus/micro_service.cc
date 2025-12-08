#include <qbus/micro_service.h>

#include <QDebug>

namespace qbus
{
MicroService::MicroService()
{
  //
  qDebug() << Q_FUNC_INFO;
}

MicroService::~MicroService()
{
  //

  qDebug() << Q_FUNC_INFO;
}

void MicroService::Initialize()
{
  //
}

void MicroService::Cleanup()
{
  //
}

}