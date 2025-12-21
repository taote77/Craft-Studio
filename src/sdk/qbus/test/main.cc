#include <qbus/application.h>

#include <qbus/global_cxt.h>

int main(int argc, char** argv)
{

  qbus::Application app(argc, argv);

  qbus::GlobalCxt::GetInstance();

  return app.exec();
}
