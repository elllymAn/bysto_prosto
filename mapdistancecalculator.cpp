#include "mapdistancecalculator.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MapDistanceCalculator::MapDistanceCalculator(QObject *parent, QString distance_key, QString geocode_key) : QObject(parent), api_distance_key(distance_key), result(-1), api_geocode_key(geocode_key)
{
    geocode_manager = new QNetworkAccessManager();
    distance_network_manager = new QNetworkAccessManager();
    connect(geocode_manager, &QNetworkAccessManager::finished, this, &MapDistanceCalculator::handleGeocodeResponse);
    connect(distance_network_manager, &QNetworkAccessManager::finished, this, &MapDistanceCalculator::handleDistanceResponse);
}

MapDistanceCalculator::~MapDistanceCalculator()
{
    delete geocode_manager;
    delete distance_network_manager;
}

void MapDistanceCalculator::setDistance()
{
    if(current_way.size() != 2)
    {
        qDebug() << "need 2 coordinate for get distance. Use setCoordinatesOfAddress for set 1 point";
        return;
    }
    QNetworkRequest request(QString("https://api.distancematrix.ai/maps/api/distancematrix/json?"
                                    "origins=%1,%2"
                                    "&destinations=%3,%4"
                                    "&key=%5")
                                .arg(current_way[0].first)
                                .arg(current_way[0].second)
                                .arg(current_way[1].first)
                                .arg(current_way[1].second)
                                .arg(api_distance_key));
    distance_network_manager->get(request);
}

int MapDistanceCalculator::getResult()
{
    return this->result;
}

void MapDistanceCalculator::setCoordinatesOfAddress(QString address)
{
    if(current_way.size() == 2)
    {
        qDebug() << "you can add only two points";
        return;
    }
    QNetworkRequest request(QString("https://geocode-maps.yandex.ru/1.x/?apikey=%1&geocode=%2&results=1&format=json")
                                .arg(api_geocode_key)
                                .arg(QUrl::toPercentEncoding(address)));
    geocode_manager->get(request);
}


void MapDistanceCalculator::handleDistanceResponse(QNetworkReply *reply)
{
    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject rootObj = jsonDoc.object();

    // Извлекаем массив "rows"
    QJsonArray rowsArray = rootObj["rows"].toArray();

    // Перебираем элементы массива rows (в данном случае один элемент)
    for (const QJsonValue &rowValue : rowsArray) {
        QJsonObject rowObj = rowValue.toObject();

        // Извлекаем массив "elements"
        QJsonArray elementsArray = rowObj["elements"].toArray();

        if(elementsArray.empty()) emit error_distance_calculator();

        for (const QJsonValue &elementValue : elementsArray) {
            QJsonObject elementObj = elementValue.toObject();

            // Извлекаем объект "distance" и его значение "value"
            QJsonObject distanceObj = elementObj["distance"].toObject();
            int distanceValue = distanceObj["value"].toInt();

            qDebug() << "result найден: " << distanceValue;
            this->result = distanceValue;
        }
    }
    current_way.clear();
    emit API_answer();
}

void MapDistanceCalculator::handleGeocodeResponse(QNetworkReply *reply)
{
    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if (jsonDoc.isObject()) {
        QJsonObject rootObj = jsonDoc.object();
        QJsonObject responseObj = rootObj["response"].toObject();
        QJsonObject geoObjectCollectionObj = responseObj["GeoObjectCollection"].toObject();
        QJsonArray featureMemberArray = geoObjectCollectionObj["featureMember"].toArray();
        if (!featureMemberArray.isEmpty()) {
            QJsonObject geoObject = featureMemberArray[0].toObject()["GeoObject"].toObject();
            QJsonObject pointObj = geoObject["Point"].toObject();
            QString pos = pointObj["pos"].toString();
            QStringList coordinates = pos.split(" ");
            if (coordinates.size() == 2) {
                double latitude = coordinates[0].toDouble();
                double longitude = coordinates[1].toDouble();
                current_way.append(QPair<double, double>{latitude, longitude});
                qDebug() << "Latitude:" << latitude << "Longitude:" << longitude;
            } else qWarning() << "Ошибка: Неверный формат координат.";
        }
        else
        {
            emit enter_uncorrect_data();
            return;
        }
    }
    emit API_answer();
}
