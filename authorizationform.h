#ifndef AUTHORIZATIONFORM_H
#define AUTHORIZATIONFORM_H

#include <QWidget>
#include "passwordline.h"
#include "mainwindow.h"


namespace Ui {
class AuthorizationForm;
}

class AuthorizationForm : public QWidget
{
    Q_OBJECT
public:
    explicit AuthorizationForm(MainWindow *parent = nullptr);
    ~AuthorizationForm();

protected:
    void paintEvent(QPaintEvent* event) override;
   // DatabaseConnector* db_helper;
private slots:
    void on_enter_button_clicked();
private:
    Ui::AuthorizationForm *ui;
    void addBackgroundGradient();
    void setWindowStyle();
    PasswordLine* passwordLine;
signals:
    void RoleDefine(Role, int);
};

#endif // AUTHORIZATIONFORM_H
