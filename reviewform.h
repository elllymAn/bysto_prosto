#ifndef REVIEWFORM_H
#define REVIEWFORM_H

#include <QDialog>

namespace Ui {
class ReviewForm;
}

class ReviewForm : public QDialog
{
    Q_OBJECT

public:
    explicit ReviewForm(QWidget *parent = nullptr);
    ~ReviewForm();

private:
    Ui::ReviewForm *ui;
};

#endif // REVIEWFORM_H
