#ifndef HTTPCLIENTHANDLEPOOL_H
#define HTTPCLIENTHANDLEPOOL_H

#include <QObject>
#include <QMutexLocker>
#include <QMutex>
#include <QTimer>
#include "pub/systemconfig.h"
#include "httpclienthandle.h"
#define MAX_CONNECT_THREAD 30
#define MIN_CONNECT_THREAD 0
#define CONNECT_BUSY_TIMEOUT m_baseInfo.cBlackListRequest.requestOvertime * 60    //连接处理超时,则认为异常
#define CONNECT_IDLE_TIMEOUT 6*10    //空闲超时,则可考虑销毁
class httpClientHandlePool : public QObject
{
    Q_OBJECT
public:
    explicit httpClientHandlePool(QObject *parent = 0);
    httpClientHandle* getHttpClientHandle();
    virtual ~httpClientHandlePool();

signals:

public slots:
private:
    QList<httpClientHandle*>   m_httpclientPool;
    QMutex  m_Mutex;
    QTimer m_cleanHanderTimer;
private slots:
    void slt_cleanup();
};

#endif // HTTPCLIENTHANDLEPOOL_H
