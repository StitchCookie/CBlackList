#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QBasicTimer>
#include "httpconnectionhandler.h"
#include "httpconnectionhandlerpool.h"
#include "requesthandler.h"

using namespace httpConnectionHandlerPool;

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    HttpServer(int nPort, RequestHandler* requestHandler, QObject* parent=nullptr);
    ~HttpServer();

    /* 监听 */
    bool startListen();

protected:
    /* 有新连接 */
    void incomingConnection(tSocketDescriptor socketDescriptor);
    /* 断开连接 */
    void close();

private:
    /* 监听端口 */
    int m_nPort;
    /* 请求处理实例 */
    RequestHandler* m_pRequestHandler;
    /* 线程池 */
    HttpConnectionHandlerPool* m_pPool;
};

#endif // HTTPSERVER_H
