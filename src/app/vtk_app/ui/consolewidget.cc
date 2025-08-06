#include "consolewidget.h"
#include <qboxlayout.h>
#include <qstringview.h>
#include <qtextedit.h>

namespace GUI
{

ConsoleWidget::ConsoleWidget(QWidget* parent)
  : QDockWidget(parent)
{
  Init();
}

void ConsoleWidget::Append(const QString& text)
{
  if (_content)
  {
    _content->insertPlainText(text);
  }
}

void ConsoleWidget::Clear() {}

void ConsoleWidget::Init()
{
  QString win_title = "Console";
  this->setWindowTitle(win_title);

  _content = new QTextEdit(this);
  _content->setReadOnly(true);
  this->setWidget(_content);

  auto features = this->features();
  features = features & ~QDockWidget::DockWidgetClosable;
  this->setFeatures(features);

  this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
}

} // namespace GUI
