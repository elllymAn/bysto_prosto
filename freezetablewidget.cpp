#include "freezetablewidget.h"

#include <QScrollBar>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include "reviewform.h"
#include <QPainter>
#include "buttondelegate.h"
//! [constructor]
FreezeTableWidget::FreezeTableWidget(int client)
{
    //setModel(model);
    frozenTableView = new QTableView(this);
    sql_model = new QSqlTableModel(this);
    delegate = new CustomDelegateView(this);
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
    connect(delegate, &CustomDelegateView::signalClicked, delegate, [this, client](QModelIndex index) {
        ReviewForm* form = new ReviewForm(nullptr, client, this->model()->data(this->model()->index(index.row(), 0)).toInt());
        connect(form, &QDialog::finished, this,
            [this, client]()
            {
            init_data(client);
            });
        form->exec();
    });
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
    init();
}


void FreezeTableWidget::updateValues(int id_user)
{
    init_data(id_user);
}

//! [init part1]
void FreezeTableWidget::init()
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

// void FreezeTableWidget::sortSelectedColumn()
// {
//     QModelIndex currentIndex = this->currentIndex();
//     if (!currentIndex.isValid() && currentIndex.column() == 4) {
//         return;
//     }
//      // create proxy

//    // qDebug() << "сюда попадаем " + QString::number(currentIndex.column());
//     //sql_model->sort(currentIndex.column(), Qt::AscendingOrder);
// }
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

void FreezeTableWidget::init_data(int id_user)
{
    delegate->clearData();
    sql_model->clear();
    QSqlQuery qry;
    qry.exec("SELECT КодЗаказа, СтатусЗаказа, АдресПолученияТовара, АдресДоставки FROM ИсторияПользователей WHERE "//СтатусЗаказа = 'Выполнен' AND "
             "КодКлиента = " + QString::number(id_user));
    sql_model->setQuery(std::move(qry));
    sql_model->insertColumns(4, 1);
    sql_model->setHeaderData(4, Qt::Horizontal, tr("                      "));
    //sql_model->set
    //init buttons for making feedback
    QSqlQuery qry_review;
    qry_review.exec("SELECT КодЗаказа FROM ИсторияПользователей WHERE КодКлиента =" + QString::number(id_user) + " AND КодЗаказа "
                    "NOT IN (SELECT КодЗаказа FROM Отзыв WHERE КодКлиента = " + QString::number(id_user) + ");");
    QSet<int> existingOrderIds;
    while(qry_review.next()) {
        existingOrderIds.insert(qry_review.value(0).toInt());
    }
    for (int row = 0; row < this->model()->rowCount(); ++row) {
        int orderId = this->model()->data(this->model()->index(row, 0)).toInt();
        QString position = this->model()->data(this->model()->index(row, 1)).toString();
        if (existingOrderIds.contains(orderId) and position == "Выполнен") {
            delegate->addButtonIndexes(this->model()->index(row, 4));
        }
        //this->model()->setData(this->model()->index(row, 4), "Заказ еще не доставлен");
    }
    this->setItemDelegate(delegate);
    //delete delegate;
}

