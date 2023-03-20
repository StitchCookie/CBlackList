#include "httpserver.h"

HttpServer::HttpServer(int nPort, RequestHandler *pRequestHandler, QObject *parent)
    : QTcpServer(parent)
    , m_nPort(nPort)
    , m_pRequestHandler(pRequestHandler)
    , m_pPool(nullptr)
{
    vLogDebug("HttpServer: 创建.");
    qRegisterMetaType<tSocketDescriptor>("tSocketDescriptor");
}

HttpServer::~HttpServer()
{
    close();
    vLogDebug("HttpServer: 销毁.");
}

bool HttpServer::startListen()
{
    if (nullptr == m_pPool)
        m_pPool = new HttpConnectionHandlerPool(m_pRequestHandler);

    quint16 nPort = m_nPort & 0xFFFF;
    QTcpServer::listen(QHostAddress::Any, nPort);
    if (!isListening())
	{
        vLogFatal("HttpServer: 监听端口: %d 失败: %s", nPort, qUtf8Printable(errorString()));
		return false;
	}

    vLogDebug("HttpServer: 监听端口%d成功.", nPort);
	return true;
}

void HttpServer::close()
{
    vLogDebug("HttpServer: 关闭监听.");
    DEL_INSTANCE(m_pPool)
}

void HttpServer::incomingConnection(tSocketDescriptor socketDescriptor)
{
   vLogDebug("HttpServer: 收到新连接,socketDescriptor=%d.", static_cast<int>(socketDescriptor));

    HttpConnectionHandler *pFreeHandler = nullptr;
    if (m_pPool)
        /*从线程池中获取空闲的线程来处理当前客户端请求*/
        pFreeHandler = m_pPool->getConnectionHandler();

    //由HttpConnectionHandler来处理连接
    if (pFreeHandler)
    {
        //通过事件队列来传递socket描述符(跨线程)
        QMetaObject::invokeMethod(pFreeHandler, "slt_handleConnection", Qt::QueuedConnection, Q_ARG(tSocketDescriptor, socketDescriptor));
    }
    else
    {
      vLogError("HttpServer: 连接数过多,无可用的HttpConnectionHandler来处理本次连接.");
        QTcpSocket *pSocket = new QTcpSocket(this);
        pSocket->setSocketDescriptor(socketDescriptor);
        connect(pSocket, SIGNAL(disconnected()), pSocket, SLOT(deleteLater()));
        pSocket->write("HTTP/1.1 503 too many connections\r\nConnection: close\r\n\r\nToo many connections\r\n");
        pSocket->disconnectFromHost();
        DEL_INSTANCE(pSocket)
    }
}
