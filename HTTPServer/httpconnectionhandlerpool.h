#ifndef HTTPCONNECTIONHANDLERPOOL_H
#define HTTPCONNECTIONHANDLERPOOL_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QMutex>
#include "pub/systemconfig.h"
#include "httpconnectionhandler.h"

namespace httpConnectionHandlerPool
{
#define MAX_CONNECT_THREAD m_baseInfo.cBLackResponse.MAX_ERUPT
#define MIN_CONNECT_THREAD 1
#define CONNECT_BUSY_TIMEOUT 6*10    //连接处理超时,则认为异常
#define CONNECT_IDLE_TIMEOUT 3*10    //空闲超时,则可考虑销毁

class HttpConnectionHandlerPool : public QObject
{
    Q_OBJECT
public:
    HttpConnectionHandlerPool(RequestHandler *requestHandler);
    virtual ~HttpConnectionHandlerPool();
    HttpConnectionHandler* getConnectionHandler();

private:
    RequestHandler* m_pRequestHandler;
    QList<HttpConnectionHandler*> m_pool;
    QTimer m_cleanupTimer;
    QMutex m_Mutex;

private slots:
    /* 定期清理 */
    void slt_cleanup();
};
}

#endif // HTTPCONNECTIONHANDLERPOOL_H
