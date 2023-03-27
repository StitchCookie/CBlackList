#ifndef HTTPCLIENTHANDLE_H
#define HTTPCLIENTHANDLE_H

#include <QObject>
#include <pub/systemconfig.h>
#include <QDateTime>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "httpdefine.h"
#include <QVariantMap>
#include "hiredis/hiredis.h"
#include  <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTextCodec>
/*delete object*/
#define DEL_OBJECT(OBJECT) {if(OBJECT) delete OBJECT; OBJECT = nullptr;}
#define REQUEST_TIMEOUT m_baseInfo.cBlackListRequest.requestOvertime
class httpClientHandle : public QObject
{
    Q_OBJECT
public:
    static httpClientHandle* getInstance()
    {
        static httpClientHandle task; //会用当前的无参构造
        return &task;
    }

    bool dealData(const QByteArray &array,QJsonObject &jsondata);
    void packRequset(QByteArray &postdata);
    void setRedisDatabase(QString m_ip,int m_port,int m_No,QString m_Passwd,int m_exitTime);
    bool connectRedis();
    QString Utf8hexqstringToGbkhexqstring(const QString &text);


    /*线程工作状态*/
    bool isBusy();
    /**/
    void setBusy(bool isBusy = true);
    ~httpClientHandle();

signals:
public slots:
    void dealPost();

private:
    explicit httpClientHandle();
    httpClientHandle(const httpClientHandle& T) = delete;
    httpClientHandle& operator=(const httpClientHandle& T)=delete;
    QVariantMap m_map;
    /* 工作线程 */
    QThread m_pThread;
    /**/
    QNetworkAccessManager   *m_NetManager;
    QString                 m_URL;
    QNetworkRequest         m_request;
    QNetworkReply          *m_reply;
    bool                    m_redisStatus;
    /*redis handle  Maintain a connection one per thread*/
    QString m_ip;
    int m_port;
    int m_No;
    QString m_Passwd;
    int m_exitTime;
    redisContext *m_ctx;
    LineGrantrySQLData m_LineGrantrySQLData;
private slots:
};
#endif // HTTPCLIENTHANDLE_H
