#ifndef CORRECTORDERFORM_H
#define CORRECTORDERFORM_H

#include <QDialog>

namespace Ui {
class correctOrderForm;
}

class correctOrderForm : public QDialog
{
    Q_OBJECT

public:
    explicit correctOrderForm(QWidget *parent = nullptr, int id_order = -1);
    ~correctOrderForm();

private slots:
    void on_pushButton_clicked();

private:
    int id_order;
    void init();
    void setDataIntoLineEdit();
    Ui::correctOrderForm *ui;
};

#endif // CORRECTORDERFORM_H
