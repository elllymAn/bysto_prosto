#ifndef DATABASECONNECTOR_H
#define DATABASECONNECTOR_H

#include <QSqlDatabase>
#include <QSql>
#include <QDebug>


class DatabaseConnector : public QObject
{
public:
    DatabaseConnector();
    ~DatabaseConnector();
    QSqlDatabase connect(const QString& server,
                         const QString& databaseName,
                         const QString& userName,
                         const QString& password,
                         const int& port);
    void disConnect();
    QSqlDatabase getDB();

private:
    QSqlDatabase db;
};

#endif // DATABASECONNECTOR_H
