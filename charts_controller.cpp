#include "charts_controller.h"
#include <QSqlQuery>
#include <QDate>

chartsController::chartsController(QWidget* parent) : QWidget(parent), max_value(0)
{
    std::get<0>(line_chart) = new QChart();
    axisY = new QValueAxis();
    axisX = new QDateTimeAxis();
}

chartsController::~chartsController()
{
    delete std::get<0>(line_chart);
    foreach (QLineSeries* line, std::get<1>(line_chart)) {
        delete line;
    }
    delete std::get<2>(line_chart);
}

void chartsController::init()
{
    foreach (QLineSeries* series, std::get<1>(line_chart)) {
        std::get<0>(line_chart)->addSeries(series);
    }
    std::get<0>(line_chart)->legend()->hide();
    //![line chart settings]

    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Дата");

    axisY->setTitleText("Значение");
    axisY->setMin(0);
    axisY->setMax(6);
    std::get<0>(line_chart)->addAxis(axisX, Qt::AlignBottom);
    std::get<0>(line_chart)->addAxis(axisY, Qt::AlignLeft);
    axisY->setTickCount(4);
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
}

void chartsController::addNewLineSeries(QString line, QPen pen)
{
    QLineSeries* series = new QLineSeries();
    getLineData(line, series);
    series->setPen(pen);
    pen.setWidth(6);
    std::get<1>(line_chart).append(series);
}


QChartView *chartsController::line1_view()
{
    return std::get<2>(line_chart);
}

void chartsController::getLineData(QString qry_l, QLineSeries* series)
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

void chartsController::updateValues(QList<QString> lines)
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
    foreach (QLineSeries* series, std::get<1>(line_chart)) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    std::get<0>(line_chart)->update();
}
