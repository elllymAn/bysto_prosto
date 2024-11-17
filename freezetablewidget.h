#ifndef FREEZETABLEWIDGET_H
#define FREEZETABLEWIDGET_H
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include "buttondelegate.h"
#include <QSortFilterProxyModel>
#include <functional>
#include <QList>

class FreezeTableWidget : public QTableView {
    Q_OBJECT

public:
    FreezeTableWidget(QWidget* parent = nullptr);
    ~FreezeTableWidget();
    void setModel();
    void updateValues();
    void init(QString data_query,
              QList<QString> queries_sort, QList<std::function<void(QModelIndex)>> funcs_delegate, QList<QString> texts_on_delegate);
protected:
    void resizeEvent(QResizeEvent *event) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible) override;

private:
    QString data_query;
    QList<QString> queries_sort;
    QTableView *frozenTableView;
    void init_style();
    void updateFrozenTableGeometry();
    QSqlTableModel* sql_model;
    void init_data();

    int ParentX;
    int ParentY;
    QList<CustomDelegateView*> delegates;
    QSortFilterProxyModel *proxyModel;
private slots:
    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
};

#endif // FREEZETABLEWIDGET_H
