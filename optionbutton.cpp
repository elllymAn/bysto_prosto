#include "optionbutton.h"

optionButton::optionButton(QObject *parent) : active(nullptr), QObject(parent) {}

void optionButton::addButton(QPushButton *object)
{
    connect(object, &QPushButton::clicked, this, [this, object]() {
        handleButtonClicked(object);
    });
    names[object] = object->text();
}

QString optionButton::name_of_button(QPushButton *btn)
{
    return names[btn];
}

void optionButton::handleButtonClicked(QPushButton *button)
{
    if (active == button) {
        active = nullptr;
        button->setStyleSheet("QPushButton{"
                              "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(105, 224, 254, 255), stop:1 rgba(63, 134, 152, 255));"
                              "border-radius: 20px;"
                              "font-family: Marmelad;"
                              "color: white;"
                              "}");
        return;
    }
    if (active) {
        active->setStyleSheet("QPushButton{"
                              "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(105, 224, 254, 255), stop:1 rgba(63, 134, 152, 255));"
                              "border-radius: 20px;"
                              "font-family: Marmelad;"
                              "color: white;"
                              "}");
    }

    active = button;

    active->setStyleSheet(
        "QPushButton{"
        "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(242, 133, 255, 255), stop:1 rgba(63, 134, 152, 255));"
        "border-radius: 20px;"
        "font-family: Marmelad;"
        "color: white;"
        "}");
}
