#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font("Arial", 10);
    a.setFont(font);
    MainWindow w;
    w.show();
    return a.exec();
}
