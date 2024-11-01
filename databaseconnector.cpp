#include "databaseconnector.h"

DatabaseConnector::DatabaseConnector()
{
    db = QSqlDatabase::addDatabase("QPSQL");
}

DatabaseConnector::~DatabaseConnector()
{
    qDebug() << "Destroy Database!";
}

QSqlDatabase DatabaseConnector::connect(const QString &server, const QString &databaseName, const QString &userName, const QString &password, const int& port)
{
    db.setConnectOptions("sslmode=disable");
    db.setHostName(server);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);
    db.setPort(port);

    if(db.open()) {
        qDebug() << "Connected to database!";
        return db;
    }
    else {
        qDebug() << "Failed to connect to database!";
        return QSqlDatabase();
    }
}

void DatabaseConnector::disConnect()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "Disconnected from Database!";
    }
}

QSqlDatabase DatabaseConnector::getDB()
{
    return db;
}
