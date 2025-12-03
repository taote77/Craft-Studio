#ifndef QBUS_APPLICATION_H
#define QBUS_APPLICATION_H

#include "global.h"
#include <QApplication>

class QBUS_API Application : public QApplication
{
  Q_OBJECT
public:
  Application(int& argc, char** argv);
};

#endif // QBUS_APPLICATION_H
