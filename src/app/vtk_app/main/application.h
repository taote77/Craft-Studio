#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

namespace APP {

class Application : public QApplication
{
public:
    explicit Application(int &argc, char **argv);

    void OpenFiles(const QString &filename);

    void CloseFiles(const QString &filename);
};

} // namespace APP

#endif // APPLICATION_H
