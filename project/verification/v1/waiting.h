/*-----------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/10/2021  -  11:03 AM  |
 *-----------------------------------------------------
 *| waiting.h              | Excessive animation      |
 *-----------------------------------------------------*/

#ifndef WAITING_H
#define WAITING_H

#include <QDialog>

namespace Ui {
class waiting;
}

class waiting : public QDialog
{
    Q_OBJECT

public:
    explicit waiting(QWidget *parent = 0);
    ~waiting();

private:
    Ui::waiting *ui;
};

#endif // WAITING_H
