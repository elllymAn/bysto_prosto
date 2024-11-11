#ifndef FREEZETABLEWIDGET_H
#define FREEZETABLEWIDGET_H
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include "buttondelegate.h"
#include <QSortFilterProxyModel>
#include <functional>

class FreezeTableWidget : public QTableView {
    Q_OBJECT

public:
    FreezeTableWidget();
    ~FreezeTableWidget();
    void setModel();
    void updateValues(QString data_query = "", QString query_sort = "");
    void init(QString data_query,
              QString query_sort, std::function<void(QModelIndex)> func_delegate, QString text_on_delegate = "");
protected:
    void resizeEvent(QResizeEvent *event) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible) override;

private:
    QString data_query;
    QString query_sort;
    QTableView *frozenTableView;
    void init_style();
    void updateFrozenTableGeometry();
    QSqlTableModel* sql_model;
    void init_data(QString data_query, QString query_sort);
    CustomDelegateView* delegate;
    QSortFilterProxyModel *proxyModel;
private slots:
    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
};

#endif // FREEZETABLEWIDGET_H
