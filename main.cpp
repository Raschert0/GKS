#include "mainwindow.h"
#include <QApplication>
																																																																																																																																																												//Author: Yaroslav Patalakha
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
