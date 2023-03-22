#ifndef CBLACKLISTSQL_H
#define CBLACKLISTSQL_H

#include <QObject>
#include <QSqlDatabase>

class CBlackListSql : public QObject
{
    Q_OBJECT
public:
    explicit CBlackListSql(QObject *parent = 0);
    ~CBlackListSql();

signals:

public slots:
private:
    QSqlDatabase DB;
};

#endif // CBLACKLISTSQL_H
