#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include "authorizationform.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db_helper = new DatabaseConnector();
    db_helper->connect("127.0.0.1", "courier_db", "postgres", "1234", 5432);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete db_helper;
    foreach (QWidget* child, childs) {
        delete child;
    }
}

DatabaseConnector *MainWindow::getDBConnector()
{
    return db_helper;
}

void MainWindow::on_pushButton_clicked()
{
    authorization();
}

void MainWindow::authorization()
{
    AuthorizationForm* form = new AuthorizationForm(this);
    childs.append(form);
    form->show();
    connect(form, SIGNAL(RoleDefine(Role)), this, SLOT(checkRole(Role)));
}

void MainWindow::checkRole(Role user)
{
    //if (user == Role::USER) QMessageBox::information(this, "Имя диалога","Юзер",QMessageBox::Save | QMessageBox::Discard| QMessageBox::Cancel, QMessageBox::Save);
    //else if (user == Role::COURIER) QMessageBox::information(this, "Имя диалога","Курьер",QMessageBox::Save | QMessageBox::Discard| QMessageBox::Cancel, QMessageBox::Save);
   // else if (user == Role::MANAGER) QMessageBox::information(this, "Имя диалога","Манагер",QMessageBox::Save | QMessageBox::Discard| QMessageBox::Cancel, QMessageBox::Save);
    childs[0]->close();
}

