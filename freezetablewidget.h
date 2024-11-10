#ifndef FREEZETABLEWIDGET_H
#define FREEZETABLEWIDGET_H
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include "buttondelegate.h"
#include <QSortFilterProxyModel>

class FreezeTableWidget : public QTableView {
    Q_OBJECT

public:
    FreezeTableWidget(int client);
    ~FreezeTableWidget();
    void setModel();
    void updateValues(int id_user);
   // void sortSelectedColumn();
protected:
    void resizeEvent(QResizeEvent *event) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible) override;
 //   void setItemDelegateForColumn(int column, QAbstractItemDelegate* delegate ) override;

private:
    QTableView *frozenTableView;
    void init();
    void updateFrozenTableGeometry();
    QSqlTableModel* sql_model;
    void init_data(int id_user);
    CustomDelegateView* delegate;
    QSortFilterProxyModel *proxyModel;
private slots:
    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
};

#endif // FREEZETABLEWIDGET_H
