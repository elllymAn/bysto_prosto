#ifndef OPTIONBUTTON_H
#define OPTIONBUTTON_H

#include <QObject>
#include <QPushButton>

class optionButton : public QObject
{
    Q_OBJECT
public:
    optionButton(QObject *parent = nullptr);
    void addButton(QPushButton* object);
    //~optionButton();
    QPushButton* active;
    QString name_of_button(QPushButton* btn);
private:
    QMap<QPushButton*, QString> names;
    void handleButtonClicked(QPushButton *button);
};

#endif // OPTIONBUTTON_H
