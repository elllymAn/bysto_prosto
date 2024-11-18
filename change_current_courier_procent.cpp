#include "change_current_courier_procent.h"
#include "ui_change_current_courier_procent.h"
#include "stylehelper.h"
#include <QSqlQuery>
#include <QMessageBox>

change_current_courier_procent::change_current_courier_procent(QWidget *parent, int procent_data, int order)
    : QDialog(parent)
    , ui(new Ui::change_current_courier_procent)
{
    order_id = order;
    ui->setupUi(this);
    ui->now->setText(QString::number(procent_data));
    ui->spinBox->setValue(procent_data);
    init();
}

change_current_courier_procent::~change_current_courier_procent()
{
    delete ui;
}

void change_current_courier_procent::init()
{
    QPalette palette = this->palette();
    QBrush brush(QColor(1, 50, 125));
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);

    ui->label->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label_2->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->now->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->spinBox->setStyleSheet(styleHelper::addTextStyle());
    ui->pushButton->setStyleSheet(styleHelper::addPushButtonStyle());
}

void change_current_courier_procent::on_pushButton_clicked()
{
    QSqlQuery qry;
    if(!qry.exec("UPDATE Заказ SET ПроцентКурьера = " + QString::number(ui->spinBox->value()) + " WHERE КодЗаказа = " + QString::number(order_id)))
    {
        QMessageBox::critical(nullptr, "Ошибка! ","Не удалось добавить отзыв!", QMessageBox::Apply);
    }
    else
    {
        QMessageBox::information(nullptr, "Успех ","Процент изменен!", QMessageBox::Apply);
        this->close();
        delete this;
    }
}

