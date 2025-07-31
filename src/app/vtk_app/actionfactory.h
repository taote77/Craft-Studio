#ifndef ACTIONFACTORY_H
#define ACTIONFACTORY_H

#include <QObject>

class ActionFactory : public QObject
{
    Q_OBJECT
public:
    explicit ActionFactory(QObject *parent = nullptr);

signals:
    void sigExit();

};

#endif // ACTIONFACTORY_H
