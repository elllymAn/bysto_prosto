#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "databaseconnector.h"
#include <QMainWindow>
#include "freezetablewidget.h"
#include "roles.h"
#include "optionbutton.h"
#include "freezetablewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    DatabaseConnector* getDBConnector();
private:
    Ui::MainWindow *ui;
    void authorization();
    DatabaseConnector* db_helper;
    QList <QWidget*> childs;
    void setOrderTabStyle();
    void setHistoryTabStyle();
    void setCurrentOrderTabStyle();
    optionButton* optionManager;
    void correctPrice();
    FreezeTableWidget* freeze_table;
    FreezeTableWidget* current_orders;
    int id_user;
private slots:
    void checkRole(Role type, int id);
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_enterOrder_clicked();
    void on_tabWidget_currentChanged(int index);
};
#endif // MAINWINDOW_H
