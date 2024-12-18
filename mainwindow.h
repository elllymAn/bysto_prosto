#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "databaseconnector.h"
#include <QMainWindow>
#include "freezetablewidget.h"
#include "roles.h"
#include "optionbutton.h"
#include <QMap>
#include "charts_controller.h"
#include "mapdistancecalculator.h"
#include "passwordline.h"

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
    void authorization();
    void initUserMode(int id);
    void initCourierMode(int id);
    void initManagerMode(int id);
    void initDefaultStyle();
    void initAccountMode();
    void initChartController();
    void resetChangingUser();


    Ui::MainWindow *ui;
    QList <QWidget*> childs;
    QMap<QWidget*, FreezeTableWidget*> tables;
    DatabaseConnector* db_helper;
    optionButton* optionManager;
    chartsController* controller;
    MapDistanceCalculator* yandex_map_length_controller;
    PasswordLine* new_password;
    PasswordLine* old_password;
    int id_user;
    Role role_user;
private slots:
    void checkRole(Role type, int id);
    void on_enterOrder_clicked();
    void on_tabWidget_currentChanged(int index);
};
#endif // MAINWINDOW_H
