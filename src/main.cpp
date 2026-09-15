#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Lucky Seven Slots"));
    QApplication::setOrganizationName(QStringLiteral("Qt Slots"));

    MainWindow window;
    window.show();
    return app.exec();
}
