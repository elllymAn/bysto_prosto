#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QDialog>
#include "optionbutton.h"
#include "passwordline.h"
#include "roles.h"
#include <QDate>

namespace Ui {
class registrationForm;
}

class registrationForm : public QDialog
{
    Q_OBJECT

public:
    explicit registrationForm(QWidget *parent = nullptr);
    ~registrationForm();
    optionButton* button_controller;
private slots:
    void on_pushButton_3_clicked();

private:
    Role user_role;
    bool check_date(QDate date);
    void init();
    bool telephone_is_unique(QString telephone);
    PasswordLine* password;
    Ui::registrationForm *ui;
};

#endif // REGISTRATIONFORM_H
