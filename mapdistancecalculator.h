#ifndef MAPDISTANCECALCULATOR_H
#define MAPDISTANCECALCULATOR_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPair>
#include <QVector>

class MapDistanceCalculator : public QObject
{
    Q_OBJECT
public:
    explicit MapDistanceCalculator(QObject* parent = nullptr, QString distance_key = "", QString geocode_key = "");
    ~MapDistanceCalculator();
    void setCoordinatesOfAddress(QString address);
    void setDistance();
    int getResult();
private slots:
    void handleDistanceResponse(QNetworkReply *reply);
    void handleGeocodeResponse(QNetworkReply *reply);


private:
    int result;
    QVector<QPair<double, double>> current_way;
    QNetworkAccessManager* geocode_manager;
    QNetworkAccessManager* distance_network_manager;
    QString api_geocode_key;
    QString api_distance_key;
signals:
    void API_answer();
};

#endif // MAPDISTANCECALCULATOR_H
