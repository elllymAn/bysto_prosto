#include "charts_controller.h"
#include <QSqlQuery>
#include <QDate>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QBarCategoryAxis>

charts_controller::charts_controller(QWidget* parent) : QWidget(parent), max_value(0)
{
    std::get<0>(line_chart) = new QChart();
    std::get<1>(line_chart) = new QLineSeries();
    std::get<0>(bar_chart) = new QChart();
    std::get<1>(bar_chart) = new QBarSeries();
    std::get<4>(bar_chart) = new QBarSet("Количество заказов");
}

charts_controller::~charts_controller()
{
    delete std::get<0>(line_chart);
    delete std::get<1>(line_chart);
    delete std::get<2>(line_chart);
    delete std::get<0>(bar_chart);
    delete std::get<1>(bar_chart);
    delete std::get<2>(bar_chart);
    delete std::get<4>(bar_chart);
}

void charts_controller::init(QString line_qry, QString bar_query)
{
    std::get<3>(line_chart) = line_qry;
    std::get<3>(bar_chart) = bar_query;

    init_data();
    std::get<1>(bar_chart)->append(std::get<4>(bar_chart));
    std::get<0>(line_chart)->addSeries(std::get<1>(line_chart));
    std::get<0>(bar_chart)->addSeries(std::get<1>(bar_chart));
    std::get<0>(bar_chart)->setAnimationOptions(QChart::SeriesAnimations);
    std::get<0>(line_chart)->legend()->hide();

    //![line chart settings]
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Дата");

    // // Ось Y (значения)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Значение");
    //axisY->setLabelFormat("%d");
    axisY->setMin(0);
    // // Устанавливаем оси на график
    std::get<0>(line_chart)->addAxis(axisX, Qt::AlignBottom);
    std::get<0>(line_chart)->addAxis(axisY, Qt::AlignLeft);
    axisY->setTickCount(4);
    // Привязываем серию данных к осям
    std::get<1>(line_chart)->attachAxis(axisX);
    std::get<1>(line_chart)->attachAxis(axisY);

    std::get<0>(line_chart)->setVisible(true);
    std::get<2>(line_chart) = new QChartView(std::get<0>(line_chart));
    std::get<2>(line_chart)->setRenderHint(QPainter::Antialiasing);
    std::get<2>(line_chart)->setRubberBand(QChartView::HorizontalRubberBand);
    std::get<2>(line_chart)->setVisible(true);
    //![line chart settings]

    //![bar chart settings]

    QBarCategoryAxis* axeX = new QBarCategoryAxis();
    axeX->append(std::get<5>(bar_chart));
    std::get<0>(bar_chart)->addAxis(axeX,Qt::AlignBottom);
    std::get<1>(bar_chart)->attachAxis(axeX);

    QValueAxis* axeY = new QValueAxis();
    std::get<0>(bar_chart)->addAxis(axeY,Qt::AlignLeft);
    std::get<1>(bar_chart)->attachAxis(axeY);

    std::get<0>(bar_chart)->legend()->setVisible(true);
    std::get<0>(bar_chart)->setVisible(true);
    std::get<0>(bar_chart)->legend()->setAlignment(Qt::AlignBottom);

    std::get<2>(bar_chart) = new QChartView(std::get<0>(bar_chart));
    std::get<2>(bar_chart)->setRenderHint(QPainter::Antialiasing);
    //![bar chart settings]
}

QChartView *charts_controller::line_view()
{
    return std::get<2>(line_chart);
}

QChartView *charts_controller::bar_view()
{
    return std::get<2>(bar_chart);
}

void charts_controller::init_data()
{
    QSqlQuery qry;
    qry.exec(std::get<3>(line_chart));
    QDateTime prevDate = QDateTime();
    while(qry.next())
    {
        QDateTime date = QDateTime::fromString(qry.value(0).toString(), "yyyy-MM-dd");

        // Если есть промежуток между предыдущей и текущей датой, заполняем пропуски
        if (prevDate.isValid()) {
            // Цикл для заполнения пропущенных дат
            while (prevDate.addDays(1) < date) {
                prevDate = prevDate.addDays(1);
                std::get<1>(line_chart)->append(prevDate.toMSecsSinceEpoch(), 0); // Добавляем дату с нулевым значением
            }
        }

        // Добавляем текущие данные
        max_value = std::max(max_value, qry.value(1).toInt());
        std::get<1>(line_chart)->append(date.toMSecsSinceEpoch(), qry.value(1).toInt());

        // Обновляем предыдущую дату
        prevDate = date;
    }
    qry.exec(std::get<3>(bar_chart));
    while(qry.next())
    {
        std::get<4>(bar_chart)->append(qry.value(1).toInt());
        std::get<5>(bar_chart).append(qry.value(0).toString());
    }
}

void charts_controller::updateValues()
{
    std::get<1>(line_chart)->clear();
    init_data();
    std::get<0>(line_chart)->axes(Qt::Vertical).first()->setMax(max_value);
    std::get<0>(line_chart)->update();
}
