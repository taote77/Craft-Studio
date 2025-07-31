#include "menu_manager.h"
#include <QMenuBar>

MenuManager::MenuManager(QObject *parent) : QObject(parent)
{

}

void MenuManager::init(QMainWindow& main_win, ActionFactory& action_fact)
{
    auto menu_bar = main_win.menuBar();

    auto menu_file =  menu_bar->addMenu(tr("File"));
    auto open_action = menu_file->addAction("&Open");
    auto exit_action = menu_file->addAction("&Exit");
    connect(exit_action, &QAction::triggered, &action_fact, &ActionFactory::sigExit);

}
