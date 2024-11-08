#include "authorizationform.h"
#include "ui_authorizationform.h"
#include <QSqlError>
#include <QSqlQuery>
#include "stylehelper.h"
#include "passwordline.h"
#include <QMessageBox>

AuthorizationForm::AuthorizationForm(MainWindow *parent)
    : QWidget(nullptr)
    , ui(new Ui::AuthorizationForm)
{
    ui->setupUi(this);

    db_helper = parent->getDBConnector();
    setWindowStyle();
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
    ui->password->setStyleSheet(styleHelper::addProjectFont("black"));
    ui->telephone->setStyleSheet(styleHelper::addProjectFont("black"));
    this->setStyleSheet(styleHelper::addTextStyle());
    ui->enter_button->setStyleSheet(styleHelper::addPushButtonStyle());

    //forms design
    ui->lineEdit_2->setClearButtonEnabled(true);
    ui->lineEdit_2->setInputMask(QString("8XXXXXXXXXX"));
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
    if(qry.exec("SELECT НомерТелефона, Пароль FROM Клиент"))
    {
        while(qry.next())
        {
            qDebug() << qry.value(0).toString();
            qDebug() << qry.value(1).toString();
            if(qry.value(0).toString() == ui->lineEdit_2->text() && qry.value(1).toString() == passwordLine->text())
            {
                QSqlQuery qry_check;
                if(qry_check.exec("SELECT КодКлиента FROM Клиент WHERE НомерТелефона = '" + ui->lineEdit_2->text() + "'"))
                {
                    qry_check.next();
                    if(qry_check.value(0).toString() == "151")
                    {
                        emit RoleDefine(Role::MANAGER, qry_check.value(0).toInt());
                        return;
                    }
                }
                emit RoleDefine(Role::USER, qry_check.value(0).toInt());
                return;
            }
        }
    }
    if(qry.exec("SELECT НомерТелефона, Пароль FROM Курьер"))
    {
        while(qry.next())
        {
            qDebug() << qry.value(0).toString();
            qDebug() << qry.value(1).toString();
            if(qry.value(0).toString() == ui->lineEdit_2->text() && qry.value(1).toString() == passwordLine->text())
            {
                emit RoleDefine(Role::COURIER, qry.value(0).toInt());
                return;
            }
        }
    }
    else
    {
        qDebug() << "PostgreSQL_authorization_error: " << db_helper->getDB().lastError();
    }
    QMessageBox::information(this, "Ошибка","Неверно введены данные!", QMessageBox::Apply);
}

