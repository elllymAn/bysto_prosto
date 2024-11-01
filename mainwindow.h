#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "databaseconnector.h"
#include <QMainWindow>
#include "roles.h"

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
private slots:
    void checkRole(Role type);
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
