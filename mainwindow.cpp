#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include "authorizationform.h"
#include "stylehelper.h"
#include <QTabBar>
#include <QFontDatabase>
#include <QRegularExpressionValidator>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include "reviewform.h"
#include "correctorderform.h"
#include "payment.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), id_user(-1),
    optionManager(nullptr), //freeze_table(nullptr),
    db_helper(nullptr)
{
    ui->setupUi(this);
    QFontDatabase::addApplicationFont(":/fonts/Marmelad-Regular.ttf");
    db_helper = new DatabaseConnector();
    db_helper->connect("127.0.0.1", "courier_db", "postgres", "1234", 5432);
    initDefaultStyle();
    authorization();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete db_helper;
    delete optionManager;
    foreach (QWidget* key, tables) {
        delete tables[key];
    }
  //  delete freeze_table;
 //   delete current_orders;
    foreach (QWidget* child, childs) {
        delete child;
    }
}

DatabaseConnector *MainWindow::getDBConnector()
{
    return db_helper;
}

void MainWindow::authorization()
{
    foreach (QWidget* child, childs) {
        delete child;
    }
    childs.clear();
    this->hide();
    AuthorizationForm* form = new AuthorizationForm(this);
    childs.append(form);
    form->show();
    connect(form, SIGNAL(RoleDefine(Role,int)), this, SLOT(checkRole(Role,int)));
}

void MainWindow::initUserMode(int id)
{
    ui->tabWidget->setCurrentIndex(0);
    //![init styles]
    QPixmap pixmap1(":/resources/clip_icon.png");
    QIcon ButtonIcon1(pixmap1);
    ui->pushButton_4->setIcon(ButtonIcon1);
    ui->pushButton_4->setIconSize(ui->pushButton_4->size());

    QPixmap pixmap2(":/resources/delivery_icon.png");
    QIcon ButtonIcon2(pixmap2);
    ui->pushButton_2->setIcon(ButtonIcon2);
    ui->pushButton_2->setIconSize(ui->pushButton_2->size());

    QPixmap pixmap3(":/resources/done_icon.png");
    QIcon ButtonIcon3(pixmap3);
    ui->pushButton_3->setIcon(ButtonIcon3);
    ui->pushButton_3->setIconSize(ui->pushButton_3->size());

    QPixmap pixmap4(":/resources/re-enter-icon.png");
    QIcon ButtonIcon4(pixmap4);
    ui->pushButton->setIcon(ButtonIcon4);
    ui->pushButton->setIconSize(ui->pushButton->size());

    //order menu design
    ui->tab->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->price->setStyleSheet("QLabel{"
                             "font-family: Marmelad;"
                             "font-size : 20 px;"
                             "color: #69E0FE;"
                             "}");

    optionManager = new optionButton();
    QList<QPushButton*> buttons = ui->tab->findChildren<QPushButton*>();
    foreach (QPushButton* button , buttons) {
        if(button != ui->enterOrder)
        {
            optionManager->addButton(button);
            button->setStyleSheet(styleHelper::addPushButtonStyle2());
            connect(button, &QPushButton::clicked, this,
                [this]()
                {
                        if(optionManager->active == ui->base)
                        {
                            ui->price->setText("2200");
                        }
                        else if(optionManager->active == ui->profi)
                        {
                            ui->price->setText("5500");
                        }
                        else if(optionManager->active == ui->products)
                        {
                            ui->price->setText("1500");
                        }
                        else if(optionManager->active == ui->express)
                        {
                            ui->price->setText("4000");
                        }
                        else if(optionManager->active == ui->documents)
                        {
                            ui->price->setText("1500");
                        }
                });
        }
    }
    ui->enterOrder->setStyleSheet(styleHelper::addPushButtonStyle());

    //line_edit settings
    QList<QLineEdit*> line_edit = ui->tab->findChildren<QLineEdit*>();
    foreach (QLineEdit* le, line_edit) {
        le->setStyleSheet(styleHelper::addTextStyle());
    }
    ui->telephone->setInputMask("80000000000");
    ui->weight->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d+(\\.\\d+)?$"), ui->weight));
    //![init styles]




    //![init menu buttons functional]
    connect(ui->pushButton_2, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(2);
            });
    connect(ui->pushButton_3, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(1);
            });
    connect(ui->pushButton_4, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(0);
            });

    connect(ui->pushButton, &QPushButton::clicked, this,
            [this]()
            {
                authorization();
            });
    //![init menu buttons functional]


    //![init user's tables]
    tables[ui->tab_2] = new FreezeTableWidget(this);
    tables[ui->tab_3] = new FreezeTableWidget(this);

    // freeze_table = new FreezeTableWidget();
    // current_orders = new FreezeTableWidget();

    tables[ui->tab_2]->init("SELECT КодЗаказа, СтатусЗаказа, АдресПолученияТовара, АдресДоставки FROM ИсторияПользователей WHERE "
                       "КодКлиента = " + QString::number(id_user),
                        {"SELECT КодЗаказа FROM ИсторияПользователей WHERE СтатусЗаказа = 'Выполнен' AND КодКлиента = " +
                           QString::number(id_user) + " AND КодЗаказа NOT IN (SELECT КодЗаказа FROM Отзыв WHERE КодКлиента = " + QString::number(id_user) + ");"
                        },
                        {
                            [id_user = id, table = tables[ui->tab_2], parent = this](QModelIndex index)
                            {
                                ReviewForm* form = new ReviewForm(parent, 1, table->model()->data(table->model()->index(index.row(), 0)).toInt());
                                connect(form, &QDialog::finished, table,
                                       [table]()
                                       {
                                           table->updateValues();
                                       });
                                form->exec();
                            }
                        },
                        {
                            "Дать отзыв"
                        });
    tables[ui->tab_3]->init("SELECT КодЗаказа, СтатусЗаказа, АдресПолученияТовара, АдресДоставки FROM ИсторияПользователей WHERE "
                         "КодКлиента = " + QString::number(id_user) + " AND СтатусЗаказа <> 'Выполнен'",
                        {
                            "SELECT КодЗаказа FROM ИсторияПользователей WHERE "
                            "КодКлиента = " + QString::number(id_user) + " AND СтатусЗаказа = 'Не принят' "
                        },
                        {
                            [table = tables[ui->tab_3], db = db_helper, parent = this](QModelIndex index)
                            {
                                correctOrderForm* form = new correctOrderForm(parent, table->model()->data(table->model()->index(index.row(), 0)).toInt());
                                connect(form, &QDialog::finished, table,
                                        [table, db]()
                                        {
                                            table->updateValues();
                                        });
                                form->exec();
                            }
                        },
                        {
                            "Изменить"
                        });

    foreach(QWidget* key, tables.keys())
    {
        tables[key]->setModel();
    }

    // freeze_table->setModel();
    // current_orders->setModel();

    ui->tab_2->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_7->addWidget(tables[ui->tab_2], 2, 0, 2, 4);

    ui->tab_3->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_11->addWidget(tables[ui->tab_3], 2, 0, 2, 4);
    this->on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
    //![init user's tables]
}

void MainWindow::initCourierMode(int id)
{
    QPixmap pixmap1(":/resources/delivery_icon.png");
    QIcon ButtonIcon1(pixmap1);
    ui->pushButton_4->setIcon(ButtonIcon1);
    ui->pushButton_4->setIconSize(ui->pushButton_4->size());

    QPixmap pixmap2(":/resources/get_order_icon.png");
    QIcon ButtonIcon2(pixmap2);
    ui->pushButton_2->setIcon(ButtonIcon2);
    ui->pushButton_2->setIconSize(ui->pushButton_2->size());

    QPixmap pixmap3(":/resources/done_icon.png");
    QIcon ButtonIcon3(pixmap3);
    ui->pushButton_3->setIcon(ButtonIcon3);
    ui->pushButton_3->setIconSize(ui->pushButton_3->size());

    QPixmap pixmap4(":/resources/re-enter-icon.png");
    QIcon ButtonIcon4(pixmap4);
    ui->pushButton->setIcon(ButtonIcon4);
    ui->pushButton->setIconSize(ui->pushButton->size());

    //![init menu buttons functional]
    connect(ui->pushButton_2, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(3);
            });
    connect(ui->pushButton_3, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(5);
            });
    connect(ui->pushButton_4, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(4);
            });
    connect(ui->pushButton, &QPushButton::clicked, this,
            [this]()
            {
                authorization();
            });
    //![init menu buttons functional]
    //qDebug() << id;
    tables[ui->tab_4] = new FreezeTableWidget(this);
    tables[ui->tab_4]->init("SELECT Заказ.КодЗаказа, Заказ.НазваниеТарифа, Заказ.АдресПолученияТовара, Заказ.АдресДоставки, Заказ.Вес, Заказ.ПроцентКурьера*Тариф.Цена/100 AS Ставка "
                            "FROM Заказ JOIN Тариф ON Заказ.НазваниеТарифа = Тариф.НазваниеТарифа "
                            "WHERE СтатусЗаказа = 'Не принят'",
                            {
                                "SELECT КодЗаказа FROM Заказ WHERE "
                                "СтатусЗаказа = 'Не принят' AND (НазваниеТарифа <> 'Экспресс' OR( НазваниеТарифа = 'Экспресс' AND "
                                "(SELECT Курьер.КоличествоВыполненныхЗаказов FROM Курьер WHERE КодКурьера = " +QString::number(id) + ") >= 20))"
                            },
                            {
                                [table = tables[ui->tab_4], db = db_helper, parent = this, id_cur = id](QModelIndex index)
                                {
                                    db->getDB().transaction();
                                    QSqlQuery qry;
                                    if(qry.exec("CALL public.courier_take_order(" + QString::number(id_cur) + ", " + table->model()->data(table->model()->index(index.row(), 0)).toString() +");"))
                                    {
                                        db->getDB().commit();
                                        QMessageBox::information(parent, "Успех!","Заказ успешно принят!", QMessageBox::Apply);
                                    }
                                    else
                                    {
                                        db->getDB().rollback();
                                        qDebug() << db->getDB().lastError().text();
                                        qDebug() << qry.lastQuery();
                                        QMessageBox::critical(parent, "Ошибка!","Не удалось принять заказ! ", QMessageBox::Apply);
                                    }
                                    table->updateValues();
                                }
                            },
                            {
                                "Взять заказ"
                            });

    tables[ui->tab_5] = new FreezeTableWidget(this);
    tables[ui->tab_5]->init("SELECT Заказ.КодЗаказа, Заказ.НазваниеТарифа, "
                            "Заказ.АдресПолученияТовара, Заказ.АдресДоставки, Заказ.Вес "
                                "FROM Заказ INNER JOIN ВРаботе ON Заказ.КодЗаказа = ВРаботе.КодЗаказа "
                                  "WHERE Заказ.СтатусЗаказа = 'В работе' AND ВРаботе.КодКурьера = " +QString::number(id),
                            {},
                            {
                                [table = tables[ui->tab_5], parent = this](QModelIndex index)
                                {
                                    QSqlQuery qry;
                                    qry.prepare("UPDATE Заказ "
                                                "SET СтатусЗаказа = 'Выполнен' "
                                                "WHERE КодЗаказа = :order;");
                                    qry.bindValue(":order", table->model()->data(table->model()->index(index.row(), 0)));
                                    if(qry.exec())
                                        QMessageBox::information(parent, "Успех!","Заказ успешно завершен!", QMessageBox::Apply);
                                    else
                                        QMessageBox::critical(parent, "Ошибка!","Не удалось завершить заказ! ", QMessageBox::Apply);
                                    table->updateValues();
                                },
                                [table = tables[ui->tab_5], parent = this](QModelIndex index)
                                {
                                    QSqlQuery qry;
                                    qry.prepare("UPDATE Заказ "
                                                "SET СтатусЗаказа = 'Не принят' "
                                                "WHERE КодЗаказа = :order;");
                                    qry.bindValue(":order", table->model()->data(table->model()->index(index.row(), 0)));
                                    if(qry.exec())
                                        QMessageBox::information(parent, "Успех!","Заказ успешно отменен!", QMessageBox::Apply);
                                    else
                                        QMessageBox::critical(parent, "Ошибка!","Не удалось отменить заказ! ", QMessageBox::Apply);
                                    table->updateValues();
                                }
                            },
                            {
                                "Завершить заказ",
                                "Отменить заказ"
                            });


    tables[ui->tab_6] = new FreezeTableWidget(this);
    tables[ui->tab_6]->init("SELECT Отзыв.КодЗаказа,Отзыв.Оценка, Отзыв.Описание "
                            "FROM ВРаботе JOIN Отзыв ON ВРаботе.КодЗаказа = Отзыв.КодЗаказа  "
                            "WHERE ВРаботе.КодКурьера = " + QString::number(id),
                            {},
                            {},
                            {});

    foreach(QWidget* key, tables.keys())
    {
        tables[key]->setModel();
    }

    ui->tab_4->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_10->addWidget(tables[ui->tab_4], 2, 0, 2, 4);

    ui->tab_5->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_13->addWidget(tables[ui->tab_5], 2, 0, 2, 4);

    ui->tab_6->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_15->addWidget(tables[ui->tab_6], 7, 0, 7, 6);


    this->on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void MainWindow::initManagerMode(int id)
{

}

void MainWindow::initDefaultStyle()
{
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setStyleSheet("border-style: none");
    ui->tabWidget->setDocumentMode(true);
}

//![init main window mode]
void MainWindow::checkRole(Role user, int id)
{
    qDebug() << id;
    childs[0]->close(); //close authorization window
    this->show();
    role_user = user;
    id_user = id;
    switch(user)
    {
    case Role::USER:
        initUserMode(id);
        break;
    case Role::COURIER:
        initCourierMode(id);
        break;
    case Role::ADMINISTRATOR:
        break;
    case Role::MANAGER:
        initManagerMode(id);
        break;
    default:
        break;
    }
}
//![init main window mode]


void MainWindow::on_enterOrder_clicked()
{
    if(optionManager->active == nullptr)
    {
        QMessageBox::critical(this, "Ошибка! ","Не выбран способ доставки! ", QMessageBox::Apply);
        return;
    }
    if(ui->weight->text() == "" or ui->your_address->text() == "" or ui->Address->text() == "" or ui->FIO->text() == "" or ui->telephone->text().length() != 11)
    {
        QMessageBox::critical(this, "Ошибка! ","Не все поля заполнены корректно", QMessageBox::Apply);
        return;
    }
    Payment* form = new Payment(nullptr, ui->price->text());
    if(form->exec() != QDialog::Accepted)
    {
        QMessageBox::critical(this, "Ошибка! ","Заказ не был оплачен", QMessageBox::Apply);
        delete form;
        return;
    }
    delete form;
    QSqlQuery qry;
    qry.prepare("INSERT INTO Заказ (КодКлиента,НазваниеТарифа, Вес, АдресПолученияТовара, АдресДоставки, ФИОПолучателя, НомерТелефонаПолучателя) "
                "VALUES (:id_client, :tariff, :weight, :address_get, :address_del, :fio, :telephone)");

    //qDebug() << id_user;
   // qDebug() << optionManager->name_of_button(optionManager->active);
   // qDebug() << ui->weight->text();
   // qDebug() << ui->your_address->text();
   // qDebug() << ui->Address->text();
   // qDebug() << ui->FIO->text();
   // qDebug() << ui->telephone->text();
    // Привязка значений

    qry.bindValue(":id_client", id_user);
    qry.bindValue(":tariff", optionManager->name_of_button(optionManager->active));
    qry.bindValue(":weight", ui->weight->text().toDouble());
    qry.bindValue(":address_get", ui->your_address->text());
    qry.bindValue(":address_del", ui->Address->text());
    qry.bindValue(":fio", ui->FIO->text());
    qry.bindValue(":telephone", ui->telephone->text());

    ui->weight->setText("");
    ui->your_address->setText("");
    ui->Address->setText("");
    ui->FIO->setText("");
    ui->telephone->setText("");

    if(!qry.exec())
    {
        qDebug() << db_helper->getDB().lastError();
        QMessageBox::critical(this, "Ошибка! ","Не удалось добавить заказ! "+ db_helper->getDB().lastError().text(), QMessageBox::Apply);
    }
    else
    {
        QMessageBox::information(this, "Успех ","Заказ успешно добавлен!", QMessageBox::Apply);
    }
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index == 1)
        tables[ui->tabWidget->widget(index)]->updateValues();
    else if(index == 2)
        tables[ui->tabWidget->widget(index)]->updateValues();
    else if(index == 3)
        tables[ui->tabWidget->widget(index)]->updateValues();
    else if(index == 4)
        tables[ui->tabWidget->widget(index)]->updateValues();
    else if(index == 5)
    {
        QSqlQuery qry;
        qry.exec("SELECT AVG(Отзыв.Оценка) "
                 "FROM ВРаботе JOIN Отзыв ON ВРаботе.КодЗаказа = Отзыв.КодЗаказа  "
                 "WHERE ВРаботе.КодКурьера = " + QString::number(id_user));
        qry.next();
        ui->rating->setText(qry.value(0).toString());
        qry.exec("SELECT КоличествоВыполненныхЗаказов FROM Курьер WHERE КодКурьера = " + QString::number(id_user));
        qry.next();
        ui->do_order->setText(qry.value(0).toString());
        tables[ui->tabWidget->widget(index)]->updateValues();
    }
}


