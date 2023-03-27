#ifndef SQLREQUESTHANDLE_H
#define SQLREQUESTHANDLE_H

#include <QObject>
#include <QThread>
#include "cblacklistsql.h"
#include "sqltypedefine.h"
#include "pub/systemconfig.h"
#include <QTimer>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
class sqlRequestHandle : public CBlackListSql
{
    Q_OBJECT
public:
    explicit sqlRequestHandle(QObject *parent = 0);
    QString Utf8hexqstringToGbkhexqstring(const QString &text);

    ~sqlRequestHandle();

signals:

public slots:
    void dealSqlData();
    void sqlThreadWorking();
private:
    //HttpClient  *m_httpclient;
    QString m_selectSql;
    quint8 intervalTime;
    quint8 scopeTime;
    QThread m_thread;
    QSqlQuery m_query;
    QTimer   *m_interverTime;
    static int m_requestCount;
    bool    m_selectStatus;
    LineGrantrySQLData m_data;
};

#endif // SQLREQUESTHANDLE_H
