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

class charts_controller : public QWidget
{
    Q_OBJECT
public:
    charts_controller(QWidget* parent = nullptr);
    ~charts_controller();
    void init();
    void addNewLineSeries(QString line, QPen pen);
    void addNewDataLineSeries(QString line, QPen pen);
    void updateValues(QList<QString> lines, QList<QString> lines_date);


    QChartView* line1_view();
    QChartView* line2_view();
private:
    void getLineData(QString qry, QLineSeries* series);
    int max_value;
    std::tuple<QChart*, QList<QLineSeries*>, QChartView*> line_chart;
    std::tuple<QChart*, QList<QLineSeries*>, QChartView*> line_chart_date_reg;
};

#endif // CHARTS_CONTROLLER_H


