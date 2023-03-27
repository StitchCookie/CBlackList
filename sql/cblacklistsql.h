#ifndef CBLACKLISTSQL_H
#define CBLACKLISTSQL_H

#include <QObject>
#include <QSqlDatabase>
#include "sqltypedefine.h"
#include <QSqlQuery>
class CBlackListSql : public QObject
{
    Q_OBJECT
public:

    explicit CBlackListSql(QObject *parent = 0);
    /*加载MYSQL驱动设置并连接信息*/
    bool setConnectBase(QString ip, quint16 port,QString user,QString passwd,QString databaseName,QString type = sqlConnectType::mysql);
    /*执行sql语句------短连接*/
    QSqlQuery getHandleData(const QString selectSql,bool &ok);
    ~CBlackListSql();

signals:

public slots:
private:
    QSqlDatabase DB;
};

#endif // CBLACKLISTSQL_H
