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
    void init(QString line_query = "", QString bar_query = "");
    void updateValues();


    QChartView* line_view();
    QChartView* bar_view();
private:
    int max_value;
    void init_data();
    std::tuple<QChart*, QLineSeries*, QChartView*, QString> line_chart;
    std::tuple<QChart*, QBarSeries*, QChartView*, QString, QBarSet*, QStringList> bar_chart;
};

#endif // CHARTS_CONTROLLER_H


