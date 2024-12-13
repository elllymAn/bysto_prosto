#include "checkorderinfo.h"
#include "ui_checkorderinfo.h"
#include "stylehelper.h"
#include <QSqlQuery>


checkOrderInfo::checkOrderInfo(QWidget *parent, int id_order)
    : QDialog(parent)
    , ui(new Ui::checkOrderInfo), order(id_order)
{
    ui->setupUi(this);
    init();
}

checkOrderInfo::~checkOrderInfo()
{
    delete ui;
}

void checkOrderInfo::on_pushButton_clicked()
{
    this->close();
    delete this;
}

void checkOrderInfo::init()
{
    QPalette palette = this->palette();
    QBrush brush(QColor(1, 50, 125));
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);

    this->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->pushButton->setStyleSheet(styleHelper::addPushButtonStyle());

    QSqlQuery qry;
    qry.prepare("SELECT КодЗаказа, АдресПолученияТовара, АдресДоставки, Вес, ДатаЗаказа FROM Заказ WHERE КодЗаказа = :order");
    qry.bindValue(":order", order);
    qry.exec(); qry.next();
    ui->label_2->setText(qry.value(0).toString());
    ui->label_4->setText(qry.value(1).toString());
    ui->label_6->setText(qry.value(2).toString());
    ui->label_8->setText(qry.value(3).toString());
    ui->label_10->setText(qry.value(4).toString());
}

