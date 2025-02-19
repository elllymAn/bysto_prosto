#include "buttondelegate.h"
#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QDebug>

CustomDelegateView::CustomDelegateView(QObject *parent, QString line) :
    QItemDelegate(parent), text(line)
{

}

void CustomDelegateView::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);

    QRect buttonRect = QRect(option.rect.left() + 1, option.rect.top() + 1, option.rect.width() - 2, option.rect.height() - 2);
    QStyleOptionButton button;
    button.rect = buttonRect;
    button.state = QStyle::State_Enabled;
    button.text = text;

    QLinearGradient gradient(buttonRect.topLeft(), buttonRect.bottomLeft());
    if(m_buttonIndexes.contains(index))
    {
        gradient.setColorAt(0, QColor(242,133,255));
        gradient.setColorAt(0.8, QColor(145,80,153));
    }
    else
    {
        gradient.setColorAt(0, QColor(125,124,125));
        gradient.setColorAt(0.8, QColor(105,81,108));
    }

    // Установка цвета фона и закругленных углов
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(gradient); // Цвет фона
    painter->setPen(Qt::NoPen); // Убираем обводку
    painter->drawRoundedRect(button.rect, 10, 10); // Рисуем кнопку с закругленными углами

    // Рисуем текст кнопки
    painter->setPen(Qt::white); // Цвет текста
    painter->drawText(button.rect, Qt::AlignCenter, button.text);
}

bool CustomDelegateView::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonRelease && m_buttonIndexes.contains(index)) {
        emit signalClicked(index);
    }
    else
        QItemDelegate::editorEvent(event, model, option,index);
    return true;
}

void CustomDelegateView::addButtonIndexes(const QModelIndex& indexes)
{
    m_buttonIndexes.insert(indexes);
}

void CustomDelegateView::clearData()
{
    m_buttonIndexes.clear();
}

