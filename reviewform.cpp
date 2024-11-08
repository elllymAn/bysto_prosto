#include "reviewform.h"
#include "ui_reviewform.h"

ReviewForm::ReviewForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReviewForm)
{
    ui->setupUi(this);
}

ReviewForm::~ReviewForm()
{
    delete ui;
   // delete this;
}
