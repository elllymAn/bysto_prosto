#include "stylehelper.h"

QString styleHelper::addProjectFont(QString color_text)
{
    return "QLabel{"
           "font-family: Marmelad;"
           "font-size : 20 px;"
           "color: "+color_text +";"
                          "}";
}

QString styleHelper::addTextStyle()
{
    return "QLineEdit{"
           "background: none;"
           "background-color: rgb(217, 217, 217);"
           "border: none;"
           "font-family: Marmelad;"
           "}";
}

QString styleHelper::addPushButtonStyle()
{
    return "QPushButton{"
           "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.157303 rgba(242, 133, 255, 255), stop:1 rgba(145, 80, 153, 255));"
           "border-radius: 20px;"
           "font-family: Marmelad;"
           "color: white;"
           "}";
}

QString styleHelper::addPushButtonStyle2()
{
    return "QPushButton{"
           "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(105, 224, 254, 255), stop:1 rgba(63, 134, 152, 255));"
           "border-radius: 20px;"
           "font-family: Marmelad;"
           "color: white;"
           "}";
}

QString styleHelper::addPushButtonStyleUse()
{
    return "QPushButton:hover{"
           "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(242, 133, 255, 255), stop:1 rgba(63, 134, 152, 255));"
           "}";
}

QString styleHelper::userStyle()
{
    return "QPushButton{"
           "qproperty-icon: url(qrc:/resources/rocket.png);"
           "}";
}
