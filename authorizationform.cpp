#include "authorizationform.h"
#include "ui_authorizationform.h"
#include <QSqlError>
#include <QSqlQuery>
#include "stylehelper.h"
#include "passwordline.h"
#include <QMessageBox>
#include "registrationform.h"

AuthorizationForm::AuthorizationForm(MainWindow *parent)
    : QWidget(nullptr)
    , ui(new Ui::AuthorizationForm)
{
    ui->setupUi(this);
    setWindowStyle();
    connect(ui->pushButton, &QPushButton::clicked, this, [this]()
            {
                registrationForm* form = new registrationForm();
                form->exec();
            });
}

AuthorizationForm::~AuthorizationForm()
{
    delete ui;
    delete passwordLine;
}

void AuthorizationForm::addBackgroundGradient()
{
    QPalette palette = this->palette();
    QLinearGradient gradient(0,0,0, this->height());
    gradient.setColorAt(0, QColor(0,74,140));
    gradient.setColorAt(0.7, QColor(0,242,244));

    QBrush brush(gradient);
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);
}

void AuthorizationForm::setWindowStyle()
{
    //making design
    addBackgroundGradient();
    ui->hi_label->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->enter_label->setStyleSheet(styleHelper::addProjectFont("rgba(0,0,0, 0.3)"));
    //ui->pushButton->setStyleSheet(styleHelper::addProjectFont("rgba(0,0,0, 0.3)"));
    ui->password->setStyleSheet(styleHelper::addProjectFont("black"));
    ui->telephone->setStyleSheet(styleHelper::addProjectFont("black"));
    this->setStyleSheet(styleHelper::addTextStyle());
    ui->enter_button->setStyleSheet(styleHelper::addPushButtonStyle());

    //forms design
    ui->lineEdit_2->setClearButtonEnabled(true);
    ui->lineEdit_2->setInputMask(QString("80000000000"));
    //configurate password form
    passwordLine = new PasswordLine();
    passwordLine->setMinimumSize(QSize(293, 40));
    passwordLine->setMaximumSize(QSize(300, 40));
    passwordLine->setFont(QFont("Marmelad", 16));
    ui->gridLayout_2->addWidget(passwordLine, 8, 1);

}

void AuthorizationForm::paintEvent(QPaintEvent *event)
{
    addBackgroundGradient();
}



void AuthorizationForm::on_enter_button_clicked()
{
    QSqlQuery qry;
    if(qry.exec("SELECT НомерТелефона, Пароль, КодКлиента FROM Клиент"))
    {
        while(qry.next())
        {
            if(qry.value(0).toString() == ui->lineEdit_2->text() && qry.value(1).toString() == passwordLine->text())
            {
                if(qry.value(2).toInt() == 151)
                {
                    emit RoleDefine(Role::MANAGER, qry.value(2).toInt());
                    return;
                }
                emit RoleDefine(Role::USER, qry.value(2).toInt());
                return;
            }
        }
    }
    if(qry.exec("SELECT НомерТелефона, Пароль, КодКурьера FROM Курьер"))
    {
        while(qry.next())
        {
            if(qry.value(0).toString() == ui->lineEdit_2->text() && qry.value(1).toString() == passwordLine->text())
            {
                emit RoleDefine(Role::COURIER, qry.value(2).toInt());
                return;
            }
        }
    }
    else
    {
        qDebug() << "PostgreSQL_authorization_error: ";
    }
    QMessageBox::information(this, "Ошибка","Неверно введены данные!", QMessageBox::Apply);
}

