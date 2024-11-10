#ifndef REVIEWFORM_H
#define REVIEWFORM_H

#include <QDialog>
#include <QSqlDatabase>
namespace Ui {
class ReviewForm;
}

class ReviewForm : public QDialog
{
    Q_OBJECT

public:
    explicit ReviewForm(QWidget *parent = nullptr, int id_client = -1, int id_order = -1);
    ~ReviewForm();
private slots:
    void on_pushButton_clicked();
    void paintGivingRating();
private:
    QList<QPushButton*> stars;
    int enteredRating;
    Ui::ReviewForm *ui;
    void init();
    int client;
    int id_order_review;
};

#endif // REVIEWFORM_H
