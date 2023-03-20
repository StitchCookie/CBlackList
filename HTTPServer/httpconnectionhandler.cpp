#include "httpconnectionhandler.h"

HttpConnectionHandler::HttpConnectionHandler(RequestHandler *pRequestHandler)
    : QObject()
    , m_pSocket(nullptr)
    , m_pThread(nullptr)
    , m_pCurrentRequest(nullptr)
    , m_pRequestHandler(pRequestHandler)
    , m_isBusy(false)
    , m_dtBusyBeginTime(QDateTime::currentDateTime())
    , m_dtBusyEndTime(QDateTime::currentDateTime())
    , m_isCloseConnection(false)
    , m_isWaitingNextRequest(false)
{
    m_pThread = new QThread();
    m_pThread->setObjectName("HttpConnectionHandler");
    m_pThread->start();
    this->moveToThread(m_pThread);

    m_Timer.moveToThread(m_pThread);
    m_Timer.setSingleShot(true);  //设置单次定时器

    m_pSocket = new QTcpSocket();
    m_pSocket->moveToThread(m_pThread);

    connect(&m_Timer, SIGNAL(timeout()), SLOT(slt_timeout()));
    connect(m_pSocket, SIGNAL(readyRead()), SLOT(slt_read()));
    connect(m_pSocket, SIGNAL(disconnected()), SLOT(slt_disconnected()));
    connect(m_pThread, SIGNAL(finished()), this, SLOT(slt_threadFinished()));

    vLogDebug("HttpConnectionHandler (%p):创建.", static_cast<void*>(this));
}

HttpConnectionHandler::~HttpConnectionHandler()
{
    m_pThread->quit();
    m_pThread->wait();
    m_pThread->deleteLater();
    vLogDebug("HttpConnectionHandler (%p):销毁.", static_cast<void*>(this));
}

void HttpConnectionHandler::slt_threadFinished()
{
    //销毁线程内创建的资源
    DEL_INSTANCE(m_pCurrentRequest)
            m_Timer.stop();
    if(m_pSocket)
    {
        m_pSocket->close();
        delete m_pSocket;
        m_pSocket = nullptr;
    }
}

bool HttpConnectionHandler::isBusy()
{
    if (m_pSocket->isOpen())
        m_isBusy = true;
    return m_isBusy;
}

void HttpConnectionHandler::setBusy(bool isBusy)
{
    m_isBusy = isBusy;
    if (m_isBusy)
        m_dtBusyBeginTime = QDateTime::currentDateTime();
    else
        m_dtBusyEndTime = QDateTime::currentDateTime();
}

qint64 HttpConnectionHandler::busyTime()
{
    return m_dtBusyBeginTime.secsTo(QDateTime::currentDateTime());
}

qint64 HttpConnectionHandler::idleTime()
{
    return m_dtBusyEndTime.secsTo(QDateTime::currentDateTime());
}

void HttpConnectionHandler::slt_handleConnection(const tSocketDescriptor socketDescriptor)
{
    vLogDebug("HttpConnectionHandler (%p): 处理新连接,socketDescriptor=%d.",
              static_cast<void*>(this), static_cast<int>(socketDescriptor));
    if (m_pSocket->isOpen())
    {
        vLogError("HttpConnectionHandler (%p): socket为已打开状态,无法处理本次连接.");
        return ;
    }

    //TODO 网传connectToHost会内存泄露,但测了不会
    m_pSocket->connectToHost("", 0);
    //重置并丢弃缓存中的数据
    m_pSocket->abort();

    if (!m_pSocket->setSocketDescriptor(socketDescriptor))
    {
        vLogFatal("HttpConnectionHandler (%p): 初始化socket失败: %s.",
                  static_cast<void*>(this), qUtf8Printable(m_pSocket->errorString()));
        return;
    }

    m_Timer.start(READ_TIMEOUT);
    DEL_INSTANCE(m_pCurrentRequest)
}


void HttpConnectionHandler::slt_read()
{
    //TODO 对于较大的数据,需要很多次读取,看看能否一次读取全部
    //TODO 目前是一个connect一个thread,而不是一个request一个thread,因此不支持HTTP pipelining
    while (m_pSocket->bytesAvailable())
    {
        //保持长连接期间,收到新请求
        if(m_isWaitingNextRequest)
        {
            setBusy(true);  //重置m_dtBusyBeginTime,以防被pool误判为请求处理超时而强制销毁
            m_isWaitingNextRequest = false;
        }

        //读取数据
        readFromSocket();

        //请求数据处理异常
        if (m_pCurrentRequest->getStatus() == HttpRequest::abort)
        {
            //TODO 完善异常时的回复内容
            m_pSocket->write("HTTP/1.1 413 entity too large\r\nConnection: close\r\n\r\n413 Entity too large\r\n");
            disconnectSocket();
            return;
        }

        //收到完整请求数据
        if (HttpRequest::complete == m_pCurrentRequest->getStatus())
        {
            QByteArray baIP = m_pCurrentRequest->getIP();
            vLogDebug("HttpConnectionHandler (%p): 收到来自[%s]设备的完整post请求.",
                      static_cast<void*>(this), baIP.data());

            //响应请求
            response();

            //客户端需要断开
            if (m_isCloseConnection)
            {
                disconnectSocket();
            }
            //保持长连接复用
            else
            {
                //在读到下次的body时会重置为READ_TIMEOUT,对于读到下次header后,body前刚好超时的情况,不考虑
                m_Timer.start(WAIT_NEXT_REQUEST_TIMEOUT);
                m_isWaitingNextRequest = true;
                vLogDebug("HttpConnectionHandler (%p): 本次请求处理结束,不主动断开连接,保持长连接复用,等待是否有新数据,等待超时时间=%d毫秒.",
                          static_cast<void*>(this), static_cast<int>(WAIT_NEXT_REQUEST_TIMEOUT));
            }
        }
    }
}

void HttpConnectionHandler::slt_timeout()
{
    vLogDebug("HttpConnectionHandler (%p): 连接超时时间已到,断开连接.", static_cast<void*>(this));
    disconnectSocket();
}

void HttpConnectionHandler::slt_disconnected()
{
    //即使客户端漏加"Connection"="close",只要对方有断开连接,本槽也会被触发,从而被动关闭连接
    vLogDebug("HttpConnectionHandler (%p): 连接断开.", static_cast<void*>(this));
    m_pSocket->close();
    m_Timer.stop();
    m_isBusy = false;
}

void HttpConnectionHandler::readFromSocket()
{
    if (!m_pCurrentRequest)
        m_pCurrentRequest = new HttpRequest();

    while (m_pSocket->bytesAvailable() &&
           m_pCurrentRequest->getStatus()!=HttpRequest::complete &&
           m_pCurrentRequest->getStatus()!=HttpRequest::abort)
    {
        m_pCurrentRequest->readFromSocket(m_pSocket);
        if (m_pCurrentRequest->getStatus() == HttpRequest::waitForBody)
            m_Timer.start(READ_TIMEOUT);    //重置超时时间,以防包体数据太大时间不够
    }
}

void HttpConnectionHandler::response()
{
    m_Timer.stop();
    HttpResponse response(m_pSocket);

    m_isCloseConnection = isCloseConnection();
    if(m_isCloseConnection)
        response.setHeader("Connection", "close");

    try
    {
        m_pRequestHandler->handler(*m_pCurrentRequest, response);
    }
    catch(std::exception& e)
    {
        vLogFatal("HttpConnectionHandler (%p): An exception occured in the request handler.exception=%s.",
                  static_cast<void*>(this), e.what());
    }
    catch(...)
    {
        vLogFatal("HttpConnectionHandler (%p): An unknown exception occured in the request handler.",
                  static_cast<void*>(this));
    }

    if (!response.hasSentLastPart())
        response.write(QByteArray(), true);

    vLogDebug("HttpConnectionHandler (%p): finished request", static_cast<void*>(this));

    DEL_INSTANCE(m_pCurrentRequest)
}

bool HttpConnectionHandler::isCloseConnection()
{
    //客户端明确断开连接
    if (0 == QString::compare(m_pCurrentRequest->getHeader("connection"), "close", Qt::CaseInsensitive))
        return true;

    //http1.0不支持长连接
    if (0 == QString::compare(m_pCurrentRequest->getVersion(), "HTTP/1.0", Qt::CaseInsensitive))
        return true;

    //还有一种是客户端主动断开连接,其自动会触发slt_disconnected断开连接

    return false;
}

void HttpConnectionHandler::disconnectSocket()
{
    vLogDebug("HttpConnectionHandler (%p): 主动发起断开连接.", static_cast<void*>(this));
    while(m_pSocket->bytesToWrite()) m_pSocket->waitForBytesWritten();
    m_pSocket->disconnectFromHost();
}
