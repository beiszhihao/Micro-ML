/*-----------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/10/2021  -  11:03 AM  |
 *-----------------------------------------------------
 *| waiting.cpp            | Excessive animation      |
 *-----------------------------------------------------*/

#include "waiting.h"
#include "ui_waiting.h"
#include <QMovie>
waiting::waiting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::waiting)
{
    ui->setupUi(this);
    QMovie *movie = new QMovie(QCoreApplication::applicationDirPath()+"/waiting.gif");
    ui->waiting_ico->setMovie(movie);
    movie->start();
    setWindowFlags(Qt::FramelessWindowHint);//无边框
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::ApplicationModal);
}

waiting::~waiting()
{
    delete ui;
}
