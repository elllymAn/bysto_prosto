#ifndef PAYMENT_H
#define PAYMENT_H

#include <QDialog>

namespace Ui {
class Payment;
}

class Payment : public QDialog
{
    Q_OBJECT

public:
    explicit Payment(QWidget *parent = nullptr, QString price = "0");
    void init();
    ~Payment();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Payment *ui;
};

#endif // PAYMENT_H
