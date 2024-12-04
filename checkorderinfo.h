#ifndef CHECKORDERINFO_H
#define CHECKORDERINFO_H

#include <QDialog>

namespace Ui {
class checkOrderInfo;
}

class checkOrderInfo : public QDialog
{
    Q_OBJECT

public:
    explicit checkOrderInfo(QWidget *parent = nullptr, int id_order = -1);
    ~checkOrderInfo();

private slots:
    void on_pushButton_clicked();

private:
    Ui::checkOrderInfo *ui;
    void init();
    int order;
};

#endif // CHECKORDERINFO_H
