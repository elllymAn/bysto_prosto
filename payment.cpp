#include "payment.h"
#include "ui_payment.h"
#include "stylehelper.h"
#include <QRegularExpressionValidator>

Payment::Payment(QWidget *parent, QString price)
    : QDialog(parent)
    , ui(new Ui::Payment)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &QDialog::accept);
    ui->price->setText(price + " RUB");
    init();
}

void Payment::init()
{
    QPalette palette = this->palette();
    QBrush brush(QColor(1, 50, 125));
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);

    ui->label_3->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->price->setStyleSheet(styleHelper::addProjectFont("#69E0FE"));
    ui->pushButton->setStyleSheet(styleHelper::addPushButtonStyle());
    ui->lineEdit->setStyleSheet("QLineEdit{"
                                "background: none;"
                                "background-color: #999999;"
                                "border: none;"
                                "font-family: Marmelad;"
                                "}");
    ui->lineEdit_2->setStyleSheet("QLineEdit{"
                                  "background: none;"
                                  "background-color: #999999;"
                                  "border: none;"
                                  "font-family: Marmelad;"
                                  "}");
    ui->lineEdit_3->setStyleSheet("QLineEdit{"
                                  "background: none;"
                                  "background-color: #999999;"
                                  "border: none;"
                                  "font-family: Marmelad;"
                                  "}");
    ui->lineEdit_4->setStyleSheet("QLineEdit{"
                                  "background: none;"
                                  "background-color: #999999;"
                                  "border: none;"
                                  "font-family: Marmelad;"
                                  "}");


    ui->lineEdit->setInputMask("0000 0000 0000 0000");
    ui->lineEdit_2->setInputMask("000");
    ui->lineEdit_3->setValidator(new QRegularExpressionValidator(QRegularExpression("^0[0-9]|1[0-2]$"), ui->lineEdit_3));
    ui->lineEdit_4->setValidator(new QRegularExpressionValidator(QRegularExpression("^(19\\d{2}|20[0-2][0-4])$"), ui->lineEdit_4));   // ui->lineEdit->setPlaceholderText("Номер карты");
}

Payment::~Payment()
{
    delete ui;
}

void Payment::on_pushButton_clicked()
{
    this->accept();
}

