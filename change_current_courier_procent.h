#ifndef CHANGE_CURRENT_COURIER_PROCENT_H
#define CHANGE_CURRENT_COURIER_PROCENT_H

#include <QDialog>

namespace Ui {
class change_current_courier_procent;
}

class change_current_courier_procent : public QDialog
{
    Q_OBJECT

public:
    explicit change_current_courier_procent(QWidget *parent = nullptr, int procent_data = 0, int order_id = 0);
    ~change_current_courier_procent();

private slots:
    void on_pushButton_clicked();

private:
    int order_id;
    Ui::change_current_courier_procent *ui;
    void init();
};

#endif // CHANGE_CURRENT_COURIER_PROCENT_H
