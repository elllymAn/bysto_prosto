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
#include "change_current_courier_procent.h"
#include <QEventLoop>
#include "checkorderinfo.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), id_user(-1),
    optionManager(nullptr),
    db_helper(nullptr)
{
    ui->setupUi(this);
    QFontDatabase::addApplicationFont(":/fonts/Marmelad-Regular.ttf");
    db_helper = new DatabaseConnector();
    controller = new charts_controller(this);
    new_password = new PasswordLine(this);
    old_password = new PasswordLine(this);
    db_helper->connect("127.0.0.1", "courier_db", "postgres", "1234", 5432);
    initDefaultStyle();
    authorization();
    initAccountMode();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete db_helper;
    delete optionManager;
    foreach (QWidget* key, tables) {
        delete tables[key];
    }
    delete controller;
    foreach (QWidget* child, childs) {
        delete child;
    }
    delete yandex_map_length_controller;
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

    QPixmap pixmap5(":/resources/personal_account_icon.png");
    QIcon ButtonIcon5(pixmap5);
    ui->pushButton_5->setIcon(ButtonIcon5);
    ui->pushButton_5->setIconSize(ui->pushButton_5->size());

    //order menu design
    ui->tab->setStyleSheet(styleHelper::addProjectFont("white"));
    optionManager = new optionButton();
    QList<QPushButton*> buttons = ui->tab->findChildren<QPushButton*>();
    foreach (QPushButton* button , buttons) {
        if(button != ui->enterOrder)
        {
            optionManager->addButton(button);
            button->setStyleSheet(styleHelper::addPushButtonStyle2());
        }
    }
    ui->enterOrder->setStyleSheet(styleHelper::addPushButtonStyle());

    //line_edit settings
    QList<QLineEdit*> line_edit = ui->tab->findChildren<QLineEdit*>();
    foreach (QLineEdit* le, line_edit) {
        le->setStyleSheet(styleHelper::addTextStyle());
    }
    ui->telephone->setInputMask("80000000000");
    ui->weight->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d{1,3}(\\.\\d{1,2})?$"), ui->weight));
    // QSqlQuery qry;
    // qry.prepare("SELECT АдресКлиента FROM Клиент WHERE КодКлиента = :id");
    // qry.bindValue(":id", id_user);
    // qry.exec(); qry.next();
    // ui->your_address->setText(qry.value(0).toString());
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
    connect(ui->pushButton_5, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(9);
            });
    //![init menu buttons functional]


    //![init user's tables]
    tables[ui->tab_2] = new FreezeTableWidget(this);
    tables[ui->tab_3] = new FreezeTableWidget(this);

    tables[ui->tab_2]->init("SELECT КодЗаказа, СтатусЗаказа, ДатаЗаказа, ДатаСдачи FROM ИсторияПользователей WHERE "
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
    tables[ui->tab_3]->init("SELECT КодЗаказа, СтатусЗаказа, АдресПолученияТовара, АдресДоставки, ДатаЗаказа FROM ИсторияПользователей WHERE "
                         "КодКлиента = " + QString::number(id_user) + " AND СтатусЗаказа <> 'Выполнен'",
                        {
                            "SELECT КодЗаказа FROM ИсторияПользователей WHERE "
                            "КодКлиента = " + QString::number(id_user) + " AND СтатусЗаказа = 'Не принят' ",
                            "SELECT КодЗаказа FROM ИсторияПользователей WHERE "
                            "КодКлиента = " + QString::number(id_user) + " AND СтатусЗаказа = 'Не принят' "
                        },
                        {
                            [table = tables[ui->tab_3], parent = this](QModelIndex index)
                            {
                                correctOrderForm* form = new correctOrderForm(parent, table->model()->data(table->model()->index(index.row(), 0)).toInt());
                                connect(form, &QDialog::finished, table,
                                        [table]()
                                        {
                                            table->updateValues();
                                        });
                                form->exec();
                            },
                            [table = tables[ui->tab_3], parent = this](QModelIndex index)
                            {
                                QSqlQuery qry;
                                qry.prepare("DELETE FROM Заказ WHERE КодЗаказа = :order");
                                qry.bindValue(":order", table->model()->data(table->model()->index(index.row(), 0)).toInt());
                                if(!qry.exec()) QMessageBox::critical(parent, "Ошибка!","Не удалось принять заказ! ", QMessageBox::Apply);
                                else QMessageBox::information(parent, "Успех!","Заказ успешно принят!", QMessageBox::Apply);
                                table->updateValues();
                            }
                        },
                        {
                            "Изменить",
                            "Удалить"
                        });

    foreach(QWidget* key, tables.keys())
    {
        tables[key]->setModel();
    }

    ui->tab_2->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_7->addWidget(tables[ui->tab_2], 2, 0, 2, 4);

    ui->tab_3->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_11->addWidget(tables[ui->tab_3], 2, 0, 2, 4);
    //![init user's tables]


    ui->tabWidget->setCurrentIndex(0);
    this->on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void MainWindow::initCourierMode(int id)
{
    //![init menu buttons]
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

    QPixmap pixmap5(":/resources/personal_account_icon.png");
    QIcon ButtonIcon5(pixmap5);
    ui->pushButton_5->setIcon(ButtonIcon5);
    ui->pushButton_5->setIconSize(ui->pushButton_5->size());
    //![init menu buttons]
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
    connect(ui->pushButton_5, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(9);
            });
    //![init menu buttons functional]

    //![init functional tables]
    tables[ui->tab_4] = new FreezeTableWidget(this);
    tables[ui->tab_4]->init("SELECT Заказ.КодЗаказа, Заказ.НазваниеТарифа, Заказ.АдресПолученияТовара, "
                            "Заказ.АдресДоставки, Заказ.Вес, Заказ.ПроцентКурьера*СтоимостьЗаказа/100 AS Ставка, Заказ.ДатаЗаказа "
                            "FROM Заказ "
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
                            "Заказ.АдресПолученияТовара, Заказ.АдресДоставки, Заказ.Вес, Заказ.ДатаЗаказа "
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
    //![init functional tables]

    ui->tabWidget->setCurrentIndex(3);
    this->on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void MainWindow::initManagerMode(int id)
{
    //![init menu button]
    QPixmap pixmap1(":/resources/data_analysis_icon.png");
    QIcon ButtonIcon1(pixmap1);
    ui->pushButton_4->setIcon(ButtonIcon1);
    ui->pushButton_4->setIconSize(ui->pushButton_4->size());

    QPixmap pixmap2(":/resources/get_order_icon.png");
    QIcon ButtonIcon2(pixmap2);
    ui->pushButton_2->setIcon(ButtonIcon2);
    ui->pushButton_2->setIconSize(ui->pushButton_2->size());

    QPixmap pixmap3(":/resources/search_order_icon.png");
    QIcon ButtonIcon3(pixmap3);
    ui->pushButton_3->setIcon(ButtonIcon3);
    ui->pushButton_3->setIconSize(ui->pushButton_3->size());

    QPixmap pixmap4(":/resources/re-enter-icon.png");
    QIcon ButtonIcon4(pixmap4);
    ui->pushButton->setIcon(ButtonIcon4);
    ui->pushButton->setIconSize(ui->pushButton->size());

    QPixmap pixmap5(":/resources/personal_account_icon.png");
    QIcon ButtonIcon5(pixmap5);
    ui->pushButton_5->setIcon(ButtonIcon5);
    ui->pushButton_5->setIconSize(ui->pushButton_5->size());
    //![init menu button]

    //![init menu buttons functional]
    connect(ui->pushButton_4, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(7);
            });
    connect(ui->pushButton_2, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(8);
            });
    connect(ui->pushButton_3, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(6);
            });
    connect(ui->pushButton, &QPushButton::clicked, this,
            [this]()
            {
                authorization();
            });
    connect(ui->pushButton_5, &QPushButton::clicked, this,
            [this]()
            {
                ui->tabWidget->setCurrentIndex(9);
            });
    //![init menu buttons functional]

    //![init functional elements]
    tables[ui->tab_7] = new FreezeTableWidget(this);
    tables[ui->tab_7]->init("SELECT Заказ.КодЗаказа, Заказ.НазваниеТарифа, Заказ.ПроцентКурьера "
                            "FROM Заказ WHERE СтатусЗаказа = 'Не принят'",
                            {},
                            {
                                [table = tables[ui->tab_7], parent = this](QModelIndex index)
                                {
                                    change_current_courier_procent* form = new change_current_courier_procent(parent, table->model()->data(table->model()->index(index.row(), 2)).toInt(), table->model()->data(table->model()->index(index.row(), 0)).toInt());
                                    connect(form, &QDialog::finished, table,
                                        [table]()
                                        {
                                            table->updateValues();
                                        });
                                    form->exec();
                                },
                                [table = tables[ui->tab_7], parent = this](QModelIndex index)
                                {
                                    checkOrderInfo* form = new checkOrderInfo(parent, table->model()->data(table->model()->index(index.row(), 0)).toInt());
                                    connect(form, &QDialog::finished, table,
                                            [table]()
                                            {
                                                table->updateValues();
                                            });
                                    form->exec();
                                }
                            },
                            {
                                "Изменить проц-т",
                                "Смотреть заказ"
                            });

    tables[ui->tab_9] = new FreezeTableWidget(this);
    tables[ui->tab_9]->init("select Клиент.ФИО AS ФИОКлиента, Клиент.НомерТелефона AS НомерТелефонаКлиента, "
                            "Заказ.КодЗаказа, Отзыв.Описание AS ОписаниеОтзыва, Отзыв.Оценка AS ОценкаДоставки "
                            "FROM Отзыв JOIN Заказ ON Отзыв.КодЗаказа = Заказ.КодЗаказа "
                            "JOIN Клиент ON Клиент.КодКлиента = Отзыв.КодКлиента",
                            {},
                            {},
                            {});


    foreach(QWidget* key, tables.keys())
    {
        tables[key]->setModel();
    }

    ui->tab_7->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_17->addWidget(tables[ui->tab_7], 2, 0, 2, 4);

    ui->tab_8->setStyleSheet(styleHelper::addProjectFont("white"));
    // ui->gridLayout_19->addWidget(controller->line_view(), 2, 0, 2, 4);
    // ui->gridLayout_19->addWidget(controller->bar_view(), 4, 0, 4, 4);

    ui->tab_9->setStyleSheet(styleHelper::addProjectFont("white"));
    ui->gridLayout_21->addWidget(tables[ui->tab_9], 2, 0, 2, 4);
    //![init functional elements]

    ui->tabWidget->setCurrentIndex(6);
    this->on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

//![init default style settings for form]
void MainWindow::initDefaultStyle()
{
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setStyleSheet("border-style: none");
    ui->tabWidget->setDocumentMode(true);
}

void MainWindow::initAccountMode()
{
    old_password->setText("");
    new_password->setText("");
    switch(role_user)
    {
        case Role::COURIER:
            ui->label_28->setVisible(true);
            ui->lineEdit_7->setVisible(true);
            ui->user_role_lk->setText("Курьер");
            ui->label_28->setText("Номер паспорта");
            ui->lineEdit_7->setInputMask(QString("00 00 000000"));
            break;
        case Role::USER:
            ui->label_28->setVisible(true);
            ui->lineEdit_7->setVisible(true);
            ui->label_28->setText("Ваш адрес");
            ui->user_role_lk->setText("Пользователь");
            ui->lineEdit_7->setInputMask(QString());
            break;
        case Role::MANAGER:
            ui->label_28->setVisible(false);
            ui->lineEdit_7->setVisible(false);
            ui->user_role_lk->setText("Менеджер");
            break;
        default:
            break;
    }

    //![init personal account style settings]
    ui->tab_10->setStyleSheet(styleHelper::addProjectFont("white") + '\n' + styleHelper::addTextStyle());
    ui->user_role_lk->setStyleSheet(styleHelper::addProjectFont("rgba(242, 133, 255, 255)"));
    ui->pushButton_6->setStyleSheet(styleHelper::addPushButtonStyle());
    ui->pushButton_7->setStyleSheet(styleHelper::addPushButtonStyle());

    ui->lineEdit_3->setInputMask(QString("80000000000"));


    new_password->setMinimumSize(QSize(0, 40));
    new_password->setMaximumSize(QSize(1111111, 40));
    new_password->setFont(QFont("Marmelad", 16));
    ui->gridLayout_23->addWidget(new_password, 19, 1);

    old_password->setMinimumSize(QSize(0, 40));
    old_password->setMaximumSize(QSize(1111111, 40));
    old_password->setFont(QFont("Marmelad", 16));
    ui->gridLayout_23->addWidget(old_password, 21, 1);

    //![init personal account style settings]

    //![init pushbuttons action]
    disconnect(ui->pushButton_6, nullptr, nullptr, nullptr);
    disconnect(ui->pushButton_7, nullptr, nullptr, nullptr);
    connect(ui->pushButton_6, &QPushButton::clicked, this,
            [this]()
            {
                if(ui->lineEdit_3->text().length() != 11)
                {
                    QMessageBox::critical(this, "Ошибка! ","Неверный формат номера телефона!", QMessageBox::Apply);
                    return;
                }
                if(role_user == Role::COURIER)
                {
                    if(ui->lineEdit_7->text().length() != 12)
                    {
                        QMessageBox::critical(this, "Ошибка! ","Неверный формат данных паспорта!", QMessageBox::Apply);
                        return;
                    }


                    QSqlQuery qry1;
                    qry1.prepare("SELECT НомерТелефона FROM Курьер WHERE КодКурьера <> :curier UNION SELECT НомерТелефона FROM Клиент;");
                    qry1.bindValue(":curier", id_user);
                    qry1.exec();
                    while(qry1.next())
                    {
                        if(qry1.value(0).toString() == ui->lineEdit_3->text())
                        {
                            QMessageBox::critical(this, "Ошибка! ","Данный номер уже используется!", QMessageBox::Apply);
                            return;
                        }
                    }


                    QSqlQuery qry;
                    qry.prepare("UPDATE Курьер "
                                "SET ФИО = :fio, "
                                "НомерТелефона = :tel, "
                                "ДанныеПаспорта = :pass "
                                "WHERE КодКурьера = :curier");
                    qry.bindValue(":fio", ui->lineEdit->text());
                    qry.bindValue(":tel", ui->lineEdit_3->text());
                    qry.bindValue(":pass", ui->lineEdit_7->text());
                    qry.bindValue(":curier", id_user);


                    if(!qry.exec()) QMessageBox::critical(this, "Ошибка! ","Не удалось обновить данные", QMessageBox::Apply);
                    else QMessageBox::information(this, "Успех ","Данные успешно обновлены!", QMessageBox::Apply);
                }
                else
                {
                    QSqlQuery qry1;
                    qry1.prepare("SELECT НомерТелефона FROM Курьер UNION SELECT НомерТелефона FROM Клиент WHERE КодКлиента <> :user;");
                    qry1.bindValue(":user", id_user);
                    qry1.exec();
                    while(qry1.next())
                    {
                        if(qry1.value(0).toString() == ui->lineEdit_3->text())
                        {
                            QMessageBox::critical(this, "Ошибка! ","Данный номер уже используется!", QMessageBox::Apply);
                            return;
                        }
                    }



                    QSqlQuery qry;
                    qry.prepare("UPDATE Клиент "
                                "SET ФИО = :fio, "
                                "НомерТелефона = :tel, "
                                "АдресКлиента = :address "
                                "WHERE КодКлиента = :user");
                    qry.bindValue(":fio", ui->lineEdit->text());
                    qry.bindValue(":tel", ui->lineEdit_3->text());
                    qry.bindValue(":address", ui->lineEdit_7->text());
                    qry.bindValue(":user", id_user);



                    if(!qry.exec()) QMessageBox::critical(this, "Ошибка! ","Не удалось обновить данные", QMessageBox::Apply);
                    else QMessageBox::information(this, "Успех ","Данные успешно обновлены!", QMessageBox::Apply);
                }
            });
    connect(ui->pushButton_7, &QPushButton::clicked, this,
            [this]()
            {
                if(new_password->text() == "" or old_password->text() == "")
                {
                    QMessageBox::critical(this, "Ошибка! ","Некорректный формат данных!", QMessageBox::Apply);
                    return;
                }
                if(role_user == Role::COURIER)
                {
                    QSqlQuery qry;
                    qry.prepare("SELECT Пароль FROM Курьер WHERE КодКурьера = :curier");
                    qry.bindValue(":curier", id_user);
                    qry.exec();
                    if(qry.next())
                    {
                        QMessageBox::critical(this, "Ошибка! ","Не удалось обновить пароль", QMessageBox::Apply);
                        return;
                    }


                    if(qry.value(0).toString() == old_password->text())
                    {
                        QSqlQuery qry1;
                        qry1.prepare("UPDATE Курьер SET Пароль = :password WHERE КодКурьера = :curier");
                        qry1.bindValue(":password", new_password->text());
                        qry1.bindValue(":curier", id_user);
                        if(!qry1.exec())
                            QMessageBox::critical(this, "Ошибка! ","Не удалось обновить пароль", QMessageBox::Apply);
                        else
                            QMessageBox::information(this, "Успех ","Данные успешно обновлены!", QMessageBox::Apply);
                    }
                    else QMessageBox::critical(this, "Ошибка! ","Неверно введен старый пароль", QMessageBox::Apply);
                }
                else
                {
                    QSqlQuery qry;
                    qry.prepare("SELECT Пароль FROM Клиент WHERE КодКлиента = :user");
                    qry.bindValue(":user", id_user);
                    qry.exec();
                    if(!qry.next())
                    {
                        QMessageBox::critical(this, "Ошибка! ","Не удалось обновить пароль", QMessageBox::Apply);
                        return;
                    }


                    if(qry.value(0).toString() == old_password->text())
                    {
                        QSqlQuery qry1;
                        qry1.prepare("UPDATE Клиент SET Пароль = :password WHERE КодКлиента = :user");
                        qry1.bindValue(":password", new_password->text());
                        qry1.bindValue(":user", id_user);
                        if(!qry1.exec())
                            QMessageBox::critical(this, "Ошибка! ","Не удалось обновить пароль", QMessageBox::Apply);
                        else
                        {
                            QMessageBox::information(this, "Успех ","Данные успешно обновлены!", QMessageBox::Apply);
                        }
                    }
                    else QMessageBox::critical(this, "Ошибка! ","Неверно введен старый пароль", QMessageBox::Apply);
                }
            });
    //![init pushbuttons action]
}

void MainWindow::initChartController()
{
    controller->addNewLineSeries("SELECT ДатаЗаказа, COUNT(*) FROM Заказ GROUP BY ДатаЗаказа ORDER BY ДатаЗаказа ASC", QPen(Qt::red));
    controller->addNewLineSeries("SELECT ДатаСдачи, COUNT(*) FROM Заказ WHERE ДатаСдачи IS NOT NULL GROUP BY ДатаСдачи ORDER BY ДатаСдачи ASC", QPen(Qt::green));
    controller->addNewDataLineSeries("SELECT ДатаЗаказа, COUNT(*) FROM Заказ GROUP BY ДатаЗаказа ORDER BY ДатаЗаказа ASC", QPen(Qt::darkBlue));
    controller->addNewDataLineSeries("SELECT ДатаРегистрации, COUNT(*) FROM Курьер GROUP BY ДатаРегистрации ORDER BY ДатаРегистрации ASC", QPen(Qt::green));
    controller->init();
    ui->gridLayout_19->addWidget(controller->line1_view(), 2, 0, 2, 4);
    ui->gridLayout_19->addWidget(controller->line2_view(), 4, 0, 4, 4);
    controller->line1_view()->setMinimumSize(500, 300);
}
//![init default style settings for form]

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
    case Role::MANAGER:
        initManagerMode(id);
        initChartController();
        break;
    default:
        break;
    }
    initAccountMode();
}
//![init main window mode]

//![init get_usres_order_pushbutton_action]
void MainWindow::on_enterOrder_clicked()
{
    if(optionManager->active == nullptr)
    {
        QMessageBox::critical(this, "Ошибка! ","Не выбран способ доставки! ", QMessageBox::Apply);
        return;
    }
    if(ui->weight->text() == "" or ui->your_address->text() == "" or ui->Address->text() == "")// or ui->FIO->text() == "" or ui->telephone->text().length() != 11)
    {
        QMessageBox::critical(this, "Ошибка! ","Не все поля заполнены корректно", QMessageBox::Apply);
        return;
    }
    if(ui->weight->text().toDouble() > 10.0 and optionManager->name_of_button(optionManager->active) == "Экспресс")
    {
        QMessageBox::critical(this, "Ошибка! ","Максимально доступный вес в тарифе Экспресс равен 10 кг", QMessageBox::Apply);
        return;
    }
    yandex_map_length_controller = new MapDistanceCalculator(this, "CtSWNkTnMj92mAJJhcX15tWcko77VMgco6K1Y3DQTr5AnUA8XJS0c325EZkE0OCv", "fe6b37b2-0fdc-45b2-a93f-d4116fcb4c47");

    QEventLoop l;
    connect(yandex_map_length_controller, &MapDistanceCalculator::API_answer, &l, &QEventLoop::quit);
    connect(yandex_map_length_controller, &MapDistanceCalculator::enter_uncorrect_data, &l, &QEventLoop::quit);
    connect(yandex_map_length_controller, &MapDistanceCalculator::error_distance_calculator, &l, &QEventLoop::quit);
    connect(yandex_map_length_controller, &MapDistanceCalculator::enter_uncorrect_data, this,
            [this]()
            {
                QMessageBox::critical(this, "Ошибка! ","Указанные адреса неверны!", QMessageBox::Apply);
                return;
            });
    connect(yandex_map_length_controller, &MapDistanceCalculator::error_distance_calculator, this,
            [this]()
            {
                QMessageBox::critical(this, "Ошибка! ","Не удалось вычислить расстояние между адресами! Попробуйте позже.", QMessageBox::Apply);
                return;
            });

    //![get data from yndex map API]
    yandex_map_length_controller->setCoordinatesOfAddress(ui->Address->text());
    l.exec();
    yandex_map_length_controller->setCoordinatesOfAddress(ui->your_address->text());
    l.exec();
    yandex_map_length_controller->setDistance();
    l.exec();
    //![get data from yndex map API]

    QSqlQuery qry1;
    qDebug() << optionManager->active->text();
    qry1.prepare("SELECT Цена FROM Тариф WHERE НазваниеТарифа = :tariff");
    qry1.bindValue(":tariff", optionManager->active->text());
    qry1.exec(); qry1.next();
    Payment* form = new Payment(nullptr, QString::number(yandex_map_length_controller->getResult()/1000 * 5 + qry1.value(0).toInt()));
    if(form->exec() != QDialog::Accepted)
    {
        QMessageBox::critical(this, "Ошибка! ","Заказ не был оплачен", QMessageBox::Apply);
        delete form;
        return;
    }
    delete form;
    QSqlQuery qry;
    qry.prepare("INSERT INTO Заказ (КодКлиента,НазваниеТарифа, Вес, АдресПолученияТовара, АдресДоставки, ФИОПолучателя, НомерТелефонаПолучателя, СтоимостьЗаказа) "
                "VALUES (:id_client, :tariff, :weight, :address_get, :address_del, :fio, :telephone, :price)");

    qry.bindValue(":id_client", id_user);
    qry.bindValue(":tariff", optionManager->name_of_button(optionManager->active));
    qry.bindValue(":weight", ui->weight->text().toDouble());
    qry.bindValue(":address_get", ui->your_address->text());
    qry.bindValue(":address_del", ui->Address->text());
    qry.bindValue(":price", yandex_map_length_controller->getResult()/1000 * 5 + qry1.value(0).toInt());
    if(ui->FIO->text() == "")
        qry.bindValue(":fio", QVariant());
    else
        qry.bindValue(":fio", ui->FIO->text());
    if(ui->telephone->text() == "8")
        qry.bindValue(":telephone", QVariant());
    else
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
    else QMessageBox::information(this, "Успех ","Заказ успешно добавлен!", QMessageBox::Apply);
}
//![init get_usres_order_pushbutton_action]

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch(index)
    {
        case 5:
        {
            QSqlQuery qry;
            qry.exec("SELECT AVG(Отзыв.Оценка) "
                     "FROM ВРаботе JOIN Отзыв ON ВРаботе.КодЗаказа = Отзыв.КодЗаказа  "
                     "WHERE ВРаботе.КодКурьера = " + QString::number(id_user));
            qry.next();
            ui->rating->setText(qry.value(0).toString().left(4));
            qry.exec("SELECT КоличествоВыполненныхЗаказов FROM Курьер WHERE КодКурьера = " + QString::number(id_user));
            qry.next();
            ui->do_order->setText(qry.value(0).toString());
            tables[ui->tabWidget->widget(index)]->updateValues();
            break;
        }
        case 7:
        {
            controller->updateValues({"SELECT ДатаЗаказа, COUNT(*) FROM Заказ GROUP BY ДатаЗаказа ORDER BY ДатаЗаказа ASC",
                                      "SELECT ДатаСдачи, COUNT(*) FROM Заказ WHERE ДатаСдачи IS NOT NULL GROUP BY ДатаСдачи ORDER BY ДатаСдачи ASC"},
                                     {"SELECT ДатаЗаказа, COUNT(*) FROM Заказ GROUP BY ДатаЗаказа ORDER BY ДатаЗаказа ASC",
                                      "SELECT ДатаРегистрации, COUNT(*) FROM Курьер GROUP BY ДатаРегистрации ORDER BY ДатаРегистрации ASC"});
            break;
        }
        case 9:
        {
            if(role_user == Role::COURIER)
            {
                QSqlQuery qry;
                qry.prepare("SELECT ФИО, НомерТелефона, ДанныеПаспорта "
                            "FROM Курьер WHERE КодКурьера = :user_id");
                qry.bindValue(":user_id", id_user);
                qry.exec();
                if(qry.next())
                {
                    ui->lineEdit->setText(qry.value(0).toString());
                    ui->lineEdit_3->setText(qry.value(1).toString());
                    ui->lineEdit_7->setText(qry.value(2).toString());
                }
                else
                {
                    QMessageBox::critical(this, "Ошибка! ","Не получилось получить данные о пользователе", QMessageBox::Apply);
                }
            }
            else
            {
                QSqlQuery qry;
                qry.prepare("SELECT ФИО, НомерТелефона, АдресКлиента "
                            "FROM Клиент WHERE КодКлиента = :user_id");
                qry.bindValue(":user_id", id_user);
                qry.exec();
                if(qry.next())
                {
                    ui->lineEdit->setText(qry.value(0).toString());
                    ui->lineEdit_3->setText(qry.value(1).toString());
                    ui->lineEdit_7->setText(qry.value(2).toString());
                }
                else
                {
                    QMessageBox::critical(this, "Ошибка! ","Не получилось получить данные о пользователе", QMessageBox::Apply);
                }
            }
            break;
        }
        case 0:
        {
            QSqlQuery qry;
            qry.prepare("SELECT АдресКлиента FROM Клиент WHERE КодКлиента = :id");
            qry.bindValue(":id", id_user);
            qry.exec(); qry.next();
            ui->your_address->setText(qry.value(0).toString());
            break;
        }
        default:
        {
            tables[ui->tabWidget->widget(index)]->updateValues();
        }
    }
}


