#ifndef LISTEN_H
#define LISTEN_H

#include <QObject>
#include <QThread>
#include "utilities.h"
#include "httpserver.h"

class HttpStartListen : public QObject, public enable_singleton<HttpStartListen>
{
    Q_OBJECT
public:
    explicit HttpStartListen();
    ~HttpStartListen();

    /* 启动线程 */
    void start(int port, callback cbfun);

private slots:
    /* 线程初始化 */
    void slt_init();
    /* 线程结束 */
    void slt_finish();

private:
    /* 工作线程 */
    QThread m_pThread;
    /* 监听端口 */
    int m_nPort;

    /* HTTP服务实例 */
    HttpServer *m_pHttpListener;
    /* 请求处理实例 */
    RequestHandler *m_pRequestHandler;
    /* 缓存回调函数 */
    callback m_callback;
};

#endif // LISTEN_H
