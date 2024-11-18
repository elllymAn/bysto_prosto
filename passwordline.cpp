#include "passwordline.h"

PasswordLine::PasswordLine(QWidget *parent) : QLineEdit(parent) {
    setEchoMode(QLineEdit::Password);
    setClearButtonEnabled(true);
    QAction *action = addAction(QIcon(":/resources/icons8-close-eye-48.png"), QLineEdit::TrailingPosition);
    button = qobject_cast<QToolButton *>(action->associatedObjects().last());
    button->setCursor(QCursor(Qt::PointingHandCursor));
    connect(button, &QToolButton::pressed, this, &PasswordLine::onPressed);
    connect(button, &QToolButton::released, this, &PasswordLine::onReleased);
}

void PasswordLine::onPressed() {
    QToolButton *button = qobject_cast<QToolButton *>(sender());
    button->setIcon(QIcon(":/resources/icons8-eye-64.png"));
    setEchoMode(QLineEdit::Normal);
}

void PasswordLine::onReleased() {
    QToolButton *button = qobject_cast<QToolButton *>(sender());
    button->setIcon(QIcon(":/resources/icons8-close-eye-48.png"));
    setEchoMode(QLineEdit::Password);
}

void PasswordLine::enterEvent(QEnterEvent *event)
{
    QLineEdit::enterEvent(event);
}

void PasswordLine::leaveEvent(QEvent *event) {
    QLineEdit::leaveEvent(event);
}

void PasswordLine::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
}

void PasswordLine::focusOutEvent(QFocusEvent *event) {
    QLineEdit::focusOutEvent(event);
}

