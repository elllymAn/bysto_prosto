#include "freezetablewidget.h"
#include <QScrollBar>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QFunctionPointer>
#include "buttondelegate.h"


FreezeTableWidget::FreezeTableWidget() : data_query(""), query_sort("") {}

//! [constructor]
void FreezeTableWidget::init(QString data_query, QString query_sort, std::function<void(QModelIndex)> func_delegate, QString text_on_delegate)
{
    this->data_query = data_query;
    this->query_sort = query_sort;

    frozenTableView = new QTableView(this);
    sql_model = new QSqlTableModel(this);
    delegate = new CustomDelegateView(this, text_on_delegate);
    proxyModel = new QSortFilterProxyModel(sql_model);


    //connect the headers and scrollbars of both tableviews together
    connect(horizontalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionWidth);
    connect(verticalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionHeight);
    connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);
    connect(delegate, &CustomDelegateView::signalClicked, delegate, func_delegate);
}
//! [constructor]

FreezeTableWidget::~FreezeTableWidget()
{
    delete frozenTableView;
    delete delegate;
    delete sql_model;
    delete proxyModel;
}

void FreezeTableWidget::setModel()
{
    this->setSortingEnabled(true);
    QTableView::setModel(sql_model);
    frozenTableView->setModel(model());
    proxyModel->setSourceModel(sql_model);
    QTableView::setModel(proxyModel);
    frozenTableView->setModel(model());
    init_style();
}


void FreezeTableWidget::updateValues(QString data_qry, QString qry_sort)
{
    if(data_qry == "" and qry_sort == "")
        init_data(this->data_query, this->query_sort);
    else
        init_data(data_qry, qry_sort);
}

//! [init part1]
void FreezeTableWidget::init_style()
{
   // frozenTableView->setModel(model());
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    frozenTableView->verticalHeader()->hide();
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   // this->setSortingEnabled(true);
   // frozenTableView->setSortingEnabled(true);
    this->verticalHeader()->hide();
    this->setStyleSheet("QTableView { border: none;"
                        "background-color: #D9D9D9;"
                        "font-family: Marmelad;"
                        "font-size: 20px;"
                        "selection-background-color: #995067"
                        "}"
                        "QHeaderView::section"
                        "{"
                        "background-color: #D9D9D9;"
                        "color: black;"
                        "font-family: Marmelad;"
                        "font-size: 20px;"
                        "border: none;"
                        "}"
                        );
    viewport()->stackUnder(frozenTableView);
    //! [init part1]

    //! [init part2]
    frozenTableView->setStyleSheet("QTableView { border: none;"
                                   "background-color: rgba(242, 133, 255, 255);"
                                   "font-family: Marmelad;"
                                   "font-size: 20px;"
                                   "selection-background-color: rgba(242, 133, 255, 255);"
                                    "}"
                                   "QHeaderView::section"
                                   "{"
                                   "background-color: #D9D9D9;"
                                   "color: black;"
                                   "font-family: Marmelad;"
                                   "font-size: 20px;"
                                   "border: none;"
                                   "}"); //for demo purposes
    //frozenTableView->setSelectionModel(selectionModel());
    for (int col = 1; col < model()->columnCount(); ++col)
        frozenTableView->setColumnHidden(col, true);

    frozenTableView->setColumnWidth(0, columnWidth(0) );

    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->show();
   // this->setSelectionBehavior(QAbstractItemView::SelectRows);
    updateFrozenTableGeometry();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenTableView->setVerticalScrollMode(ScrollPerPixel);
}
//! [init part2]


//! [sections]
void FreezeTableWidget::updateSectionWidth(int logicalIndex, int /* oldSize */, int newSize)
{
    if (logicalIndex == 0){
        frozenTableView->setColumnWidth(0, newSize);
        updateFrozenTableGeometry();
    }
}

void FreezeTableWidget::updateSectionHeight(int logicalIndex, int /* oldSize */, int newSize)
{
    frozenTableView->setRowHeight(logicalIndex, newSize);
}

//! [sections]


//! [resize]
void FreezeTableWidget::resizeEvent(QResizeEvent * event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}
//! [resize]


//! [navigate]
QModelIndex FreezeTableWidget::moveCursor(CursorAction cursorAction,
                                          Qt::KeyboardModifiers modifiers)
{
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

    if (cursorAction == MoveLeft && current.column() > 0
        && visualRect(current).topLeft().x() < frozenTableView->columnWidth(0) ){
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
        - frozenTableView->columnWidth(0);
        horizontalScrollBar()->setValue(newValue);
    }
    return current;
}
//! [navigate]

void FreezeTableWidget::scrollTo (const QModelIndex & index, ScrollHint hint){
    if (index.column() > 0)
        QTableView::scrollTo(index, hint);
}


//! [geometry]
void FreezeTableWidget::updateFrozenTableGeometry()
{
    frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                 frameWidth(), columnWidth(0),
                                 viewport()->height()+horizontalHeader()->height());
}

void FreezeTableWidget::init_data(QString data_qry, QString qry_sort)
{
    //qDebug() << this->data_query;
   // qDebug() << this->query_sort;
   // qDebug() << data_qry;
   // qDebug() << qry_sort;
    delegate->clearData();
    sql_model->clear();
    QSqlQuery qry;
    qry.exec(data_qry);
    sql_model->setQuery(std::move(qry));
    sql_model->insertColumns(sql_model->columnCount(), 1);
    sql_model->setHeaderData(sql_model->columnCount()-1, Qt::Horizontal, tr("                      "));


    //init buttons for making feedback
    QSqlQuery qry_review;
    qry_review.exec(qry_sort);
    QSet<int> existingOrderIds;
    while(qry_review.next()) {
        existingOrderIds.insert(qry_review.value(0).toInt());
    }
    for (int row = 0; row < this->model()->rowCount(); ++row) {
        int orderId = this->model()->data(this->model()->index(row, 0)).toInt();
        if (existingOrderIds.contains(orderId)) {
            delegate->addButtonIndexes(this->model()->index(row, 4));
        }
    }
    this->setItemDelegate(delegate);
}

