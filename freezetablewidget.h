#ifndef FREEZETABLEWIDGET_H
#define FREEZETABLEWIDGET_H
#include <QTableView>
#include <QSqlQueryModel>
#include "buttondelegate.h"
class FreezeTableWidget : public QTableView {
    Q_OBJECT

public:
    FreezeTableWidget();
    ~FreezeTableWidget();
    void setModel();
    void updateValues(int id_user);
protected:
    void resizeEvent(QResizeEvent *event) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible) override;
 //   void setItemDelegateForColumn(int column, QAbstractItemDelegate* delegate ) override;

private:
    QTableView *frozenTableView;
    void init();
    void updateFrozenTableGeometry();
    QSqlQueryModel* sql_model;
    void init_data(int id_user);
    CustomDelegateView* delegate;
private slots:
    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);

};

#endif // FREEZETABLEWIDGET_H
