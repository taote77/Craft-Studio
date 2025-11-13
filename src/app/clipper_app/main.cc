#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("Clipper2 Demo");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("MasterWay");
    
    // 创建主窗口
    MainWindow window;
    window.show();
    
    return app.exec();
}