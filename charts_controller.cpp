#include "charts_controller.h"
#include <QSqlQuery>
#include <QDate>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QBarCategoryAxis>

charts_controller::charts_controller(QWidget* parent) : QWidget(parent), max_value(0)
{
    std::get<0>(line_chart) = new QChart();
    std::get<0>(line_chart_date_reg) = new QChart();
}

charts_controller::~charts_controller()
{
    delete std::get<0>(line_chart);
    delete std::get<0>(line_chart_date_reg);
    foreach (QLineSeries* line, std::get<1>(line_chart)) {
        delete line;
    }
    foreach (QLineSeries* line, std::get<1>(line_chart_date_reg)) {
        delete line;
    }
    delete std::get<2>(line_chart);
    delete std::get<2>(line_chart_date_reg);
}

void charts_controller::init()
{
    foreach (QLineSeries* series, std::get<1>(line_chart)) {
        std::get<0>(line_chart)->addSeries(series);
    }
    foreach (QLineSeries* series, std::get<1>(line_chart_date_reg)) {
        std::get<0>(line_chart_date_reg)->addSeries(series);
    }
    std::get<0>(line_chart)->legend()->hide();
    std::get<0>(line_chart_date_reg)->legend()->hide();
    //![line chart settings]
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Дата");

    // // Ось Y (значения)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Значение");
    //axisY->setLabelFormat("%d");
    axisY->setMin(0);
    axisY->setMax(6);
    // // Устанавливаем оси на график
    std::get<0>(line_chart)->addAxis(axisX, Qt::AlignBottom);
    std::get<0>(line_chart)->addAxis(axisY, Qt::AlignLeft);
    axisY->setTickCount(4);
    // Привязываем серию данных к осям
    foreach (QLineSeries* series, std::get<1>(line_chart)) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    std::get<0>(line_chart)->setVisible(true);
    std::get<2>(line_chart) = new QChartView(std::get<0>(line_chart));
    std::get<2>(line_chart)->setRenderHint(QPainter::Antialiasing);
    std::get<2>(line_chart)->setRubberBand(QChartView::HorizontalRubberBand);
    std::get<2>(line_chart)->setVisible(true);
    //![line chart settings]


    //![bar chart settings]
    QDateTimeAxis *axX = new QDateTimeAxis();
    axX->setFormat("yyyy-MM-dd");
    axX->setTitleText("Дата");

    // // Ось Y (значения)
    QValueAxis *axY = new QValueAxis();
    axY->setTitleText("Значение");
    //axisY->setLabelFormat("%d");
    axY->setMin(0);
    axY->setMax(6);
    // // Устанавливаем оси на график
    std::get<0>(line_chart_date_reg)->addAxis(axX, Qt::AlignBottom);
    std::get<0>(line_chart_date_reg)->addAxis(axY, Qt::AlignLeft);
    axY->setTickCount(4);
    // Привязываем серию данных к осям
    foreach (QLineSeries* series, std::get<1>(line_chart_date_reg)) {
        series->attachAxis(axX);
        series->attachAxis(axY);
    }

    std::get<0>(line_chart_date_reg)->setVisible(true);
    std::get<2>(line_chart_date_reg) = new QChartView(std::get<0>(line_chart_date_reg));
    std::get<2>(line_chart_date_reg)->setRenderHint(QPainter::Antialiasing);
    std::get<2>(line_chart_date_reg)->setRubberBand(QChartView::HorizontalRubberBand);
    std::get<2>(line_chart_date_reg)->setVisible(true);
    //![bar chart settings]
}

void charts_controller::addNewLineSeries(QString line, QPen pen)
{
    QLineSeries* series = new QLineSeries();
    getLineData(line, series);
    series->setPen(pen);
    pen.setWidth(6);
    std::get<1>(line_chart).append(series);
}

void charts_controller::addNewDataLineSeries(QString line, QPen pen)
{
    QLineSeries* series = new QLineSeries();
    getLineData(line, series);
    series->setPen(pen);
    pen.setWidth(6);
    std::get<1>(line_chart_date_reg).append(series);
}


QChartView *charts_controller::line1_view()
{
    return std::get<2>(line_chart);
}

QChartView *charts_controller::line2_view()
{
    return std::get<2>(line_chart_date_reg);
}

void charts_controller::getLineData(QString qry_l, QLineSeries* series)
{
    QSqlQuery qry;
    qry.exec(qry_l);
    QDateTime prevDate = QDateTime();
    while(qry.next())
    {
        QDateTime date = QDateTime::fromString(qry.value(0).toString(), "yyyy-MM-dd");

        if (prevDate.isValid()) {
            while (prevDate.addDays(1) < date) {
                prevDate = prevDate.addDays(1);
                series->append(prevDate.toMSecsSinceEpoch(), 0);
            }
        }

        max_value = std::max(max_value, qry.value(1).toInt());
        series->append(date.toMSecsSinceEpoch(), qry.value(1).toInt());

        prevDate = date;
    }
}

void charts_controller::updateValues(QList<QString> lines, QList<QString> lines_date)
{
    for(int i = 0; i < std::get<1>(line_chart).length(); ++i)
    {
        std::get<0>(line_chart)->removeSeries(std::get<1>(line_chart)[i]);
    }

    for (int i = 0; i < lines.size(); ++i)
    {
        if (i < std::get<1>(line_chart).size())
        {
            std::get<1>(line_chart)[i]->clear();
            getLineData(lines[i], std::get<1>(line_chart)[i]);
            std::get<0>(line_chart)->addSeries(std::get<1>(line_chart)[i]);
        }
    }

    std::get<0>(line_chart)->axes(Qt::Vertical).first()->setMax(max_value);
    std::get<0>(line_chart)->update();



    for(int i = 0; i < std::get<1>(line_chart_date_reg).length(); ++i)
    {
        std::get<0>(line_chart_date_reg)->removeSeries(std::get<1>(line_chart_date_reg)[i]);
    }

    for (int i = 0; i < lines_date.size(); ++i)
    {
        if (i < std::get<1>(line_chart_date_reg).size())
        {
            std::get<1>(line_chart_date_reg)[i]->clear();
            getLineData(lines_date[i], std::get<1>(line_chart_date_reg)[i]);
            std::get<0>(line_chart_date_reg)->addSeries(std::get<1>(line_chart_date_reg)[i]);
        }
    }

    std::get<0>(line_chart_date_reg)->axes(Qt::Vertical).first()->setMax(max_value);
    std::get<0>(line_chart_date_reg)->update();
}
