#ifndef PASSWORDLINE_H
#define PASSWORDLINE_H


#include <QAction>
#include <QLineEdit>
#include <QToolButton>

class PasswordLine : public QLineEdit
{
    Q_OBJECT
public:
    PasswordLine(QWidget* parent = nullptr);
private slots:
    void onPressed();
    void onReleased();

protected:
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
private:
    QToolButton *button;
};

#endif // PASSWORDLINE_H
