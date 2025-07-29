#include "mainwindow.h"
#include "menu_manager.h"

#include <QDebug>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUI();
}

void MainWindow::setupUI()
{
    this->resize(1024, 980);

    _action_factory = new ActionFactory(this);

    connect(_action_factory,&ActionFactory::sigExit,this, &MainWindow::exit);

    auto menu_mgr = new MenuManager(this);
    menu_mgr->init(*this, *_action_factory);

}

void MainWindow::exit()
{
    qDebug()<< "exiout";

    QApplication::exit();

}
