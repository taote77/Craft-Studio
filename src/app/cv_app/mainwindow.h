#ifndef MAINWINOW_H
#define MAINWINOW_H

#include "actionfactory.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void setupUI();

    void exit();

signals:

private:
    ActionFactory* _action_factory{nullptr};

};

#endif // MAINWINOW_H
