#include "registrationform.h"
#include "ui_registrationform.h"
#include "stylehelper.h"
#include <QSqlQuery>
#include <QMessageBox>

registrationForm::registrationForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::registrationForm)
{
    ui->setupUi(this);
    button_controller = new optionButton();
    password = new PasswordLine(this);
    button_controller->addButton(ui->pushButton);
    button_controller->addButton(ui->pushButton_2);
    connect(ui->pushButton_2, &QPushButton::clicked,
        this, [&]()
        {
            ui->label_6->setVisible(true);
            ui->lineEdit_3->setVisible(true);
            user_role = Role::COURIER;
        });
    connect(ui->pushButton, &QPushButton::clicked,
            this, [&]()
            {
                ui->label_6->setVisible(false);
                ui->lineEdit_3->setVisible(false);
                user_role = Role::USER;
            });
    init();
}

registrationForm::~registrationForm()
{
    delete ui;
    delete button_controller;
}

void registrationForm::init()
{
    //set style option
    this->setStyleSheet(styleHelper::addTextStyle());
    ui->pushButton->setStyleSheet(styleHelper::addPushButtonStyle2());
    ui->pushButton_2->setStyleSheet(styleHelper::addPushButtonStyle2());
    password->setMinimumSize(QSize(0, 40));
    password->setMaximumSize(QSize(1111, 40));
    password->setFont(QFont("Marmelad", 16));
    ui->gridLayout_2->addWidget(password, 11, 2, 1, 2);
    ui->pushButton_3->setStyleSheet(styleHelper::addPushButtonStyle());
    ui->label->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label_2->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label_3->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label_4->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label_5->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label_6->setStyleSheet(styleHelper::addProjectFont("white"));
    QPalette palette = this->palette();
    QBrush brush(QColor(1, 50, 125));
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);
    ui->label_6->setVisible(false);
    ui->lineEdit_3->setVisible(false);

    //set functional option
    ui->lineEdit_2->setClearButtonEnabled(true);
    ui->lineEdit->setClearButtonEnabled(true);
    ui->lineEdit_3->setClearButtonEnabled(true);
    password->setClearButtonEnabled(true);
    ui->lineEdit_2->setInputMask(QString("80000000000"));
    ui->lineEdit_3->setInputMask(QString("00 00 000000"));
}

bool registrationForm::telephone_is_unique(QString telephone)
{
    QSqlQuery qry;
    qry.exec("SELECT НомерТелефона FROM Курьер UNION SELECT НомерТелефона FROM Клиент;");
    while(qry.next())
    {
        if(qry.value(0).toString() == telephone)
            return false;
    }
    return true;
}

void registrationForm::on_pushButton_3_clicked()
{
    if(ui->lineEdit->text() == "" or ui->lineEdit_2->text() == "8" or password->text() == "")
    {
        QMessageBox::critical(this, "Ошибка! ","Не все данные формы заполнены!", QMessageBox::Apply);
        return;
    }
    if(!telephone_is_unique(ui->lineEdit_2->text()))
    {
        QMessageBox::critical(this, "Ошибка! ","Данный номер уже зарегистрирован!", QMessageBox::Apply);
        return;
    }
    if (user_role == Role::USER)
    {
        if(!check_date(ui->dateEdit->date()))
        {
            QMessageBox::critical(this, "Ошибка! ","Регистрация для пользователя возможна с 14 лет!", QMessageBox::Apply);
            return;
        }
        QSqlQuery qry;
        qry.prepare("INSERT INTO Клиент(ФИО, НомерТелефона, ДатаРождения, Пароль) VALUES(:fio, :tel, :date, :pass)");
        qry.bindValue(":fio", ui->lineEdit->text());
        qry.bindValue(":tel", ui->lineEdit_2->text());
        qry.bindValue(":date", ui->dateEdit->date().toString("yyyy-MM-dd"));
        qry.bindValue(":pass", password->text());
        if(!qry.exec())
        {
            QMessageBox::critical(this, "Ошибка! ","Не удалось добавить аккаунт!", QMessageBox::Apply);
        }
        else
        {
            QMessageBox::information(this, "Успех ","Аккаунт добавлен!", QMessageBox::Apply);
            this->close();
            delete this;
        }
        return;
    }
    else if(user_role == Role::COURIER)
    {
        if(!check_date(ui->dateEdit->date()))
        {
            QMessageBox::critical(this, "Ошибка! ","Регистрация для курьера возможна с 18 лет!", QMessageBox::Apply);
            return;
        }
        if(ui->lineEdit_3->text() == "")
        {
            QMessageBox::critical(this, "Ошибка! ","Данные паспорта не заполнены!", QMessageBox::Apply);
            return;
        }
        QSqlQuery qry;
        qry.prepare("INSERT INTO Курьер(ФИО, ДатаРождения, ДанныеПаспорта, НомерТелефона, Пароль) VALUES(:fio, :date, :passport, :tel, :pass)");
        qry.bindValue(":fio", ui->lineEdit->text());
        qry.bindValue(":tel", ui->lineEdit_2->text());
        qry.bindValue(":date", ui->dateEdit->date().toString("yyyy-MM-dd"));
        qry.bindValue(":pass", password->text());
        qry.bindValue(":passport", ui->lineEdit_3->text());
        if(!qry.exec())
        {
            QMessageBox::critical(this, "Ошибка! ","Не удалось добавить аккаунт!", QMessageBox::Apply);
        }
        else
        {
            QMessageBox::information(this, "Успех ","Аккаунт добавлен!", QMessageBox::Apply);
            //this->close();
            //delete this;
        }
        return;
    }
    QMessageBox::critical(this, "Ошибка! ","Роль не выбрана!", QMessageBox::Apply);
}

bool registrationForm::check_date(QDate date)
{
    int daysDiff = date.daysTo(QDate::currentDate());
    double yearsDiff = daysDiff / 365.25;
    if (yearsDiff < 14 and user_role == Role::USER)
        return false;
    else if (yearsDiff < 18 and user_role == Role::COURIER)
        return false;
    return true;
}

