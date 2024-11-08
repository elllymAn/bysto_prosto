#include "freezetablewidget.h"

#include <QScrollBar>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include "reviewform.h"
#include <QPainter>
//! [constructor]
FreezeTableWidget::FreezeTableWidget()
{
    //setModel(model);
    frozenTableView = new QTableView(this);
    sql_model = new QSqlQueryModel();
    delegate = new CustomDelegateView(this);
   // sql_model->insertColumn(5);
    //connect the headers and scrollbars of both tableviews together
    connect(horizontalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionWidth);
    connect(verticalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionHeight);

    connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);
    //connect(sql_model, &QSqlQueryModel::dataChanged, this, &FreezeTableWidget::createButton);

}
//! [constructor]

FreezeTableWidget::~FreezeTableWidget()
{
    delete frozenTableView;
    delete delegate;
}

void FreezeTableWidget::setModel()
{
    QTableView::setModel(sql_model);
    frozenTableView->setModel(model());
    //frozenTableView->setSelectionModel(selectionModel());
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

void FreezeTableWidget::init_data(int id_user)
{
    QSqlQuery qry;
    qry.exec("SELECT КодЗаказа, СтатусЗаказа, АдресПолученияТовара, АдресДоставки FROM Заказ WHERE "//СтатусЗаказа = 'Выполнен' AND "
             "КодКлиента = " + QString::number(id_user));
    sql_model->setQuery(std::move(qry));
    sql_model->insertColumns(4, 1);
    sql_model->setHeaderData(4, Qt::Horizontal, tr("                      "));



    //init buttons for making feedback

    QSqlQuery qry_review;
    qry_review.exec("SELECT КодЗаказа FROM Заказ WHERE КодКлиента =" + QString::number(id_user) + " AND КодЗаказа "
                    "NOT IN (SELECT КодЗаказа FROM Отзыв WHERE КодКлиента = " + QString::number(id_user) + ");");
     QSet<int> existingOrderIds;
     while(qry_review.next()) {
         existingOrderIds.insert(qry_review.value(0).toInt());
     }

     for (int row = 0; row < this->model()->rowCount(); ++row) {
         int orderId = this->model()->data(this->model()->index(row, 0)).toInt();
         QString position = this->model()->data(this->model()->index(row, 1)).toString();
         if (!existingOrderIds.contains(orderId) and position == "Выполнен") {
             delegate->addButtonIndexes(this->model()->index(row, 4));
         }
     }
    this->setItemDelegateForColumn(4, delegate);
    connect(delegate, &CustomDelegateView::signalClicked, delegate, [this](QModelIndex index) {
        ReviewForm* form = new ReviewForm();
        form->exec();
    });
    //delete delegate;
}


