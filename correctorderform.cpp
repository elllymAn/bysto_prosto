#include "correctorderform.h"
#include "ui_correctorderform.h"
#include "stylehelper.h"
#include <QSqlQuery>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QSqlError>

correctOrderForm::correctOrderForm(QWidget *parent, int order)
    : QDialog(parent)
    , ui(new Ui::correctOrderForm), id_order(order)
{
    ui->setupUi(this);
    init();
}

correctOrderForm::~correctOrderForm()
{
    delete ui;
}

void correctOrderForm::init()
{
    QPalette palette = this->palette();
    QBrush brush(QColor(1, 50, 125));
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);

    //this->setStyleSheet(styleHelper::addProjectFont("white"));
    foreach (QLabel* l, this->findChildren<QLabel*>()) {
        l->setStyleSheet(styleHelper::addProjectFont("white"));
    }
    ui->weight->setStyleSheet(styleHelper::addTextStyle());
    ui->address_get->setStyleSheet(styleHelper::addTextStyle());
    ui->address_del->setStyleSheet(styleHelper::addTextStyle());
    ui->fio->setStyleSheet(styleHelper::addTextStyle());
    ui->telephone->setStyleSheet(styleHelper::addTextStyle());
    ui->telephone->setInputMask("800000000");
    ui->weight->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d+(\\.\\d+)?$"), ui->weight));

    ui->pushButton->setStyleSheet(styleHelper::addPushButtonStyle2());
    setDataIntoLineEdit();
}

void correctOrderForm::setDataIntoLineEdit()
{
    QSqlQuery qry;
    qry.exec("SELECT Вес, АдресПолученияТовара, АдресДоставки, ФИОПолучателя, НомерТелефонаПолучателя "
             "FROM Заказ WHERE КодЗаказа = " + QString::number(id_order));
    qry.next();
    ui->weight->setText(qry.value(0).toString());
    ui->address_get->setText(qry.value(1).toString());
    ui->address_del->setText(qry.value(2).toString());
    ui->fio->setText(qry.value(3).toString());
    ui->telephone->setText(qry.value(4).toString());
}

void correctOrderForm::on_pushButton_clicked()
{
    //qDebug() << id_order;
    int decision = QMessageBox::warning(this, "Вы уверены?","Вы уверены, что хотите изменить данные?", QMessageBox::Yes | QMessageBox::No);
    if (decision == QMessageBox::Yes) {
        QSqlQuery qry;
        qry.prepare("UPDATE Заказ SET "
                    "Вес = :weight, "
                    "АдресПолученияТовара = :a_get, "
                    "АдресДоставки = :a_del, "
                    "ФИОПолучателя = :fio, "
                    "НомерТелефонаПолучателя = :telephone "
                    "WHERE КодЗаказа = :id_order;");

        qDebug() << ui->weight->text().toDouble();
        qDebug() << ui->address_get->text();
        qDebug() << ui->address_del->text();
        qDebug() << ui->fio->text();
        qDebug() << ui->telephone->text();
        qDebug() << id_order;

        qry.bindValue(":weight", ui->weight->text().toDouble());  // Привязываем вес
        qry.bindValue(":a_get", ui->address_get->text());        // Привязываем адрес получения товара
        qry.bindValue(":a_del", ui->address_del->text());        // Привязываем адрес доставки
        qry.bindValue(":fio", ui->fio->text());                  // Привязываем ФИО получателя
        qry.bindValue(":telephone", ui->telephone->text());      // Привязываем номер телефона
        qry.bindValue(":id_order", id_order);                    // Привязываем id заказа


        if(!qry.exec())
        {
            QMessageBox::critical(this, "Ошибка! ","Не удалось добавить заказ! ", QMessageBox::Apply);
        }
        else
        {
            QMessageBox::information(this, "Успех ","Заказ успешно добавлен!", QMessageBox::Apply);
            this->close();
            //qDebug() << qry.lastError().driverText();
            delete this;
        }
    }
}

