#include "freezetablewidget.h"
#include <QScrollBar>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QFunctionPointer>
#include "buttondelegate.h"
#include <QLineEdit>
#include <QHBoxLayout>

FreezeTableWidget::FreezeTableWidget(QWidget* parent) : data_query("")
{
    ParentX = parent->pos().x();
    ParentY = parent->pos().y();
}

//! [constructor]
void FreezeTableWidget::init(QString data_query,
                             QList<QString> queries_sort,
                             QList<std::function<void(QModelIndex)>> funcs_delegate,
                             QList<QString> texts_on_delegate)
{
    if(funcs_delegate.size() != texts_on_delegate.size())
        return;
    this->data_query = data_query;
    if(queries_sort.empty())
    {
        for(int i = 0; i < funcs_delegate.size(); ++i)
            this->queries_sort.append(data_query);
    }
    else this->queries_sort = std::move(queries_sort);
    frozenTableView = new QTableView(this);
    sql_model = new QSqlTableModel(this);
    foreach(QString line, texts_on_delegate)
        delegates.append(new CustomDelegateView(this, line));
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
    for(int i = 0; i < funcs_delegate.size(); ++i)
        connect(delegates[i], &CustomDelegateView::signalClicked, delegates[i], funcs_delegate[i]);
}
//! [constructor]

FreezeTableWidget::~FreezeTableWidget()
{
    delete frozenTableView;
    foreach(CustomDelegateView* delegate, delegates) {
        delete delegate;
    }
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
    connect(this->horizontalHeader(), &QHeaderView::sectionClicked, this, [this](int section)
    {
        QHeaderView* headerView = this->horizontalHeader();
        QWidget* sortWidget = new QWidget(this, Qt::Popup);

        // Устанавливаем стиль для sortWidget
        sortWidget->setStyleSheet("background-color: rgba(1, 50, 125, 128);"
                                  "color: white;"
                                  "font-family: Marmelad;"
                                  "font-size: 20px;");

        QHBoxLayout* lo = new QHBoxLayout(sortWidget);
        QLineEdit* le = new QLineEdit(sortWidget);

        connect(le, &QLineEdit::textChanged, this, [this, section](const QString& toSort) {
            if (toSort.isEmpty()) {
                proxyModel->setFilterFixedString("");
                proxyModel->sort(section);
            } else {
                proxyModel->setFilterKeyColumn(section);
                proxyModel->setFilterFixedString(toSort);
                proxyModel->sort(section);
            }
        });

        lo->addWidget(le);
        sortWidget->setLayout(lo);

        // Определяем размер и позицию столбца
        const int columnWidth = headerView->sectionSize(section);  // Ширина столбца
        const int columnPos = headerView->sectionPosition(section); // Позиция столбца на экране

        // Учитываем прокрутку таблицы
        const int horizontalOffset = this->horizontalScrollBar()->value();

        // Позиционируем sortWidget над столбцом
        const int sx = columnPos - horizontalOffset + (ParentX + this->pos().x());
        const int sy = ParentY + this->pos().y() - sortWidget->height();  // Отступ сверху для размещения над столбцом

        // Устанавливаем размер и показываем форму
        sortWidget->resize(columnWidth, 40);
        sortWidget->move(sx, sy);
        sortWidget->show();
        connect(this->horizontalHeader(), &QHeaderView::sectionClicked, this, [this, sortWidget, section](int clicked)
        {
            proxyModel->setFilterFixedString("");
            proxyModel->sort(section);
        });
    });
    init_style();
}


void FreezeTableWidget::updateValues()
{
    init_data();
}

//! [init part1]
void FreezeTableWidget::init_style()
{
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
                                   "}");
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

void FreezeTableWidget::init_data()
{
    foreach(CustomDelegateView* del, delegates)
        del->clearData();
    sql_model->clear();
    QSqlQuery qry;
    if(!qry.exec(data_query)) {qDebug() << "data_qry"; }
    sql_model->setQuery(std::move(qry));
    for(int i = 0; i < delegates.size(); ++i)
    {
        sql_model->insertColumns(sql_model->columnCount(), 1);
        sql_model->setHeaderData(sql_model->columnCount()-1, Qt::Horizontal, tr("                          "));
    }
    for(int i = 0; i < delegates.size(); ++i)
    {
        QSqlQuery qry_review;
        if(!qry_review.exec(queries_sort[i])) {qDebug() << "review errror";}
        QSet<int> existingOrderIds;
        while(qry_review.next()) {
            existingOrderIds.insert(qry_review.value(0).toInt());
        }
        for (int row = 0; row < this->model()->rowCount(); ++row) {
            int orderId = this->model()->data(this->model()->index(row, 0)).toInt();
            if (existingOrderIds.contains(orderId)) {
                delegates[i]->addButtonIndexes(this->model()->index(row, sql_model->columnCount()-(delegates.size()-i)));
            }
        }
        this->setItemDelegateForColumn(sql_model->columnCount()-(delegates.size()-i), delegates[i]);
    }
}
