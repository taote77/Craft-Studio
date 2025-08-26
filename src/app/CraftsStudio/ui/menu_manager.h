#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "mainwindow.h"

class ActionFactory;

class MenuManager : public QObject
{
  Q_OBJECT
public:
  explicit MenuManager(QObject* parent = nullptr);

  void init(MainWindow& main_win, ActionFactory& action_fact);

signals:
};

#endif // MENUMANAGER_H
