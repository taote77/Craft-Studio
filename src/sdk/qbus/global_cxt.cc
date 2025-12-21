#include <qbus/global_cxt.h>

namespace qbus
{

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

}