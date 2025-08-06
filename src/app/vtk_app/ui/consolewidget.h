#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QDockWidget>
#include <QTextEdit>
#include <qboxlayout.h>
#include <qtextedit.h>

namespace GUI
{

class ConsoleWidget : public QDockWidget
{
  Q_OBJECT
public:
  explicit ConsoleWidget(QWidget* parent = nullptr);

  void Append(const QString& text);

  void Clear();

protected:
  void Init();

private:
  QTextEdit* _content;
};

} // namespace GUI

#endif // CONSOLEWIDGET_H
