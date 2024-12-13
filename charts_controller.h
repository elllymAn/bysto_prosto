#ifndef CHARTS_CONTROLLER_H
#define CHARTS_CONTROLLER_H

#include <QObject>
#include <QChart>
#include <QLineSeries>
#include <QBarSeries>
#include <QWidget>
#include <QChartView>
#include <tuple>
#include <QBarSet>
#include <QStringList>
#include <QValueAxis>
#include <QDateTimeAxis>

class chartsController : public QWidget
{
    Q_OBJECT
public:
    chartsController(QWidget* parent = nullptr);
    ~chartsController();
    void init();
    void addNewLineSeries(QString line, QPen pen);
    void updateValues(QList<QString> lines);


    QChartView* line1_view();
    //QChartView* line2_view();
private:
    void getLineData(QString qry, QLineSeries* series);
    int max_value;
    QValueAxis *axisY;
    QDateTimeAxis *axisX;
    std::tuple<QChart*, QList<QLineSeries*>, QChartView*> line_chart;
};

#endif // CHARTS_CONTROLLER_H


