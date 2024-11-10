#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QObject>
#include <QItemDelegate>

class CustomDelegateView : public QItemDelegate
{
    Q_OBJECT
signals:
    QModelIndex signalClicked(QModelIndex index);
public:
    CustomDelegateView(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    void addButtonIndexes(const QModelIndex& indexes);
    void clearData();
private:
    QSet<QModelIndex> m_buttonIndexes;
};

#endif // BUTTONDELEGATE_H
