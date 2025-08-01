#include "actionfactory.h"

ActionFactory::ActionFactory(QObject *parent) : QObject(parent)
{

}

void ActionFactory::exitApp()
{
    Q_EMIT sigExit();
}

