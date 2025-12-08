#include <qbus/global_cxt.h>

GlobalCxt* GlobalCxt::GetInstance()
{
  static GlobalCxt instance;
  return &instance;
}

GlobalCxt::GlobalCxt()
{
  //
}

GlobalCxt::~GlobalCxt()
{
  //
}