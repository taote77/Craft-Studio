#include <QApplication>

#include <pdk/app.h>

int main(int argc, char* argv[])
{

  pdk::Application app(argc, argv);

  return app.exec();
}
