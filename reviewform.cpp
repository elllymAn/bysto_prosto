#include "reviewform.h"
#include "ui_reviewform.h"
#include "stylehelper.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QVariant>

ReviewForm::ReviewForm(QWidget *parent, int id_client, int id_order)
    : QDialog(parent),
    ui(new Ui::ReviewForm), client(id_client), id_order_review(id_order), enteredRating(-1)
{
    ui->setupUi(this);
    qDebug() << client;
    // Находим все кнопки на форме
    for (int i = 0; i < ui->horizontalLayout->count(); ++i) {
        QWidget* widget = ui->horizontalLayout->itemAt(i)->widget();
        if (QPushButton* btn = qobject_cast<QPushButton*>(widget)) {
            stars.append(btn);
        }
    }

    // Подключаем все кнопки к слоту
    for (QPushButton* btn : stars) {
        connect(btn, &QPushButton::clicked, this, &ReviewForm::paintGivingRating);
    }
    init();
}

ReviewForm::~ReviewForm()
{
    delete ui;
}

void ReviewForm::on_pushButton_clicked()
{
    if(ui->plainTextEdit->toPlainText().length() > 190)
    {
        QMessageBox::critical(nullptr, "Ошибка! ","Слишком большой отзыв (максимум 190 символов)!", QMessageBox::Apply);
        return;
    }
    if(enteredRating == -1)
    {
        QMessageBox::critical(nullptr, "Ошибка! ","Необходимо установить оценку!", QMessageBox::Apply);
        return;
    }
    QSqlQuery qry;
    qry.prepare("INSERT INTO Отзыв (Описание, Оценка, КодКлиента, КодЗаказа) "
                "VALUES (:info, :mark, :client, :order)");

    // Привязка значений
    if(ui->plainTextEdit->toPlainText() == "")
        qry.bindValue(":info", QVariant(QVariant::Invalid));
    else
        qry.bindValue(":info", ui->plainTextEdit->toPlainText());
    qry.bindValue(":mark", enteredRating);
    qry.bindValue(":client", client);
    qry.bindValue(":order", id_order_review);
    if(!qry.exec())
    {
        QMessageBox::critical(nullptr, "Ошибка! ","Не удалось добавить отзыв!", QMessageBox::Apply);
    }
    else
    {
        QMessageBox::information(nullptr, "Успех ","Отзыв успешно добавлен!", QMessageBox::Apply);
        this->close();
        delete this;
    }
}

void ReviewForm::paintGivingRating()
{
    qDebug() << "попадаем сюда";

    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    if (!buttonSender) {
        qDebug() << "Error: sender is not a QPushButton!";
        return;
    }

    // Обновляем иконки
    for (int i = 0; i < stars.size(); ++i) {
        if (buttonSender == stars[i]) {
            enteredRating = i + 1;
            qDebug() << "Rating entered:" << enteredRating;

            // Обновляем иконки
            for (int j = 0; j <= i; ++j) {
                QIcon starIcon(":/resources/icons8-star-48-full.png");
                if (starIcon.isNull()) {
                    qDebug() << "Icon not loaded!";
                }
                stars[j]->setIcon(starIcon);
            }
            for (int j = i+1; j < stars.size(); ++j) {
                QIcon starIcon(":/resources/icons8-star-48-.png");
                if (starIcon.isNull()) {
                    qDebug() << "Icon not loaded!";
                }
                stars[j]->setIcon(starIcon);
            }
        }
    }
}

void ReviewForm::init()
{
    QPalette palette = this->palette();
    QBrush brush(QColor(1, 50, 125));
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);

    ui->label->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->label_2->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->plainTextEdit->setStyleSheet(styleHelper::addTextStyle());
    ui->pushButton->setStyleSheet(styleHelper::addPushButtonStyle());
}

