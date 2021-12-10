/*-----------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/10/2021  -  11:03 AM  |
 *-----------------------------------------------------
 *| main.cpp               | start                    |
 *-----------------------------------------------------*/

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.move((QApplication::desktop()->width() - w.width())/2,(QApplication::desktop()->height() - w.height())/2);
    return a.exec();
}
