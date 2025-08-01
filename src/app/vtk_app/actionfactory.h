#ifndef ACTIONFACTORY_H
#define ACTIONFACTORY_H

#include <QObject>

class ActionFactory : public QObject
{
    Q_OBJECT
public:
    explicit ActionFactory(QObject *parent = nullptr);

public slots:

    void exitApp();

signals:
    void sigExit();

};

#endif // ACTIONFACTORY_H
