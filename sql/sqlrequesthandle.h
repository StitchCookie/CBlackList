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
    static sqlRequestHandle* getInstance(){
        static sqlRequestHandle m_task;
        return &m_task;
    }
    QString Utf8hexqstringToGbkhexqstring(const QString &text);

    ~sqlRequestHandle();

signals:

public slots:
    void dealSqlData();
    void sqlThreadWorking();
private:   
    sqlRequestHandle();
    sqlRequestHandle(const sqlRequestHandle& T) = delete;
    sqlRequestHandle& operator=(const sqlRequestHandle& T) = delete;
    QString m_selectSql;
    quint8 intervalTime;
    quint8 scopeTime;
    QThread m_thread;
    QSqlQuery m_query;
    QTimer   *m_interverTime;
    bool    m_selectStatus;
    LineGrantrySQLData m_data;
};

#endif // SQLREQUESTHANDLE_H
