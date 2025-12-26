#include "sys.h"

QString Sys::kModuleAddress = "";

Sys::Sys() {}

void Sys::Run()
{
  kModuleAddress = Path();
}

void Sys::Startup(const QVariant& data)
{
  qInfo() << "Sys service startup";
}

QVariant Sys::OnFetchResult(const QVariant& data)
{
  return 0;
}
