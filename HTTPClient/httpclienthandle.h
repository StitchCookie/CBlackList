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
#include <QTextCodec>
#include "hiredis/hiredis.h"
/*delete object*/
#define DEL_OBJECT(OBJECT) {if(OBJECT) delete OBJECT; OBJECT = nullptr;}
#define REQUEST_TIMEOUT m_baseInfo.cBlackListRequest.requestOvertime
class httpClientHandle : public QObject
{
    Q_OBJECT
public:
    explicit httpClientHandle();
    inline void setHttpRequestOvertime(int overtime)
    {
        m_overTime = overtime;
    }
    bool dealData(const QByteArray &pArray, QJsonObject &pJson);

    QString Utf8hexqstringToGbkhexqstring(const QString &text);


    /*线程工作状态*/
    bool isBusy();
    /**/
    void setBusy(bool isBusy = true);
    ~httpClientHandle();

signals:

public slots:
    void dealPost(QByteArray lineGrantryCarData);

    /* 获取线程忙碌时间(超过一定阈值,很可能会出现异常,需要释放连接) */
    quint64 busyTime();
    quint64 idleTime();
private:

    /* 工作线程 */
    QThread* m_pThread;
    /* 忙碌状态 */
    bool m_isBusy;
    QDateTime m_dtBusyBeginTime;
    QDateTime m_dtBusyEndTime;
    /**/
    QNetworkAccessManager   *m_NetManager;
    QString                 m_URL;
    QNetworkRequest         m_request;
    QNetworkReply          *m_reply;
    bool                    m_redisStatus;
    /*  overTime*/
    int m_overTime;
    /*redis handle  Maintain a connection one per thread*/
    QString m_ip;
    int m_port;
    int m_No;
    QString m_Passwd;
    int m_exitTime;
    redisContext *m_ctx;
private slots:
};
#endif // HTTPCLIENTHANDLE_H
