#include <qbus/application.h>

int main(int argc, char** argv)
{

  qbus::Application app(argc, argv);

  return app.exec();
}
