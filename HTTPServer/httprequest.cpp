#include "httprequest.h"

HttpRequest::HttpRequest()
    : m_nStatus(waitForRequest)
    , m_nCurrentSize(0)
    , m_nExpectedBodySize(0)
{
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::readFromSocket(QTcpSocket* pSocket)
{
    if (m_nStatus == waitForRequest)
        readRequest(pSocket);
    else if (m_nStatus == waitForHeader)
        readHeader(pSocket);
    else if (m_nStatus == waitForBody)
        readBody(pSocket);

    if (m_nCurrentSize > MAX_REQUEST_SIZE)
    {
        vLogError("HttpRequest: 请求数据长度[%d]过大.", m_nCurrentSize);
        m_nStatus = abort;
    }
}

void HttpRequest::readRequest(QTcpSocket* pSocket)
{
    int nToRead = MAX_REQUEST_SIZE - m_nCurrentSize + 1;
    qDebug()<<"MAX_REQUSE_SIZE = "<<MAX_REQUEST_SIZE <<"m_nCurrentSize = "<<m_nCurrentSize;
    m_baReqLineBuf.append(pSocket->readLine(nToRead));
    m_nCurrentSize += m_baReqLineBuf.size();
    qDebug()<<"开始读取请求行数据:";
    qDebug()<<"MAX_REQUSE_SIZE = "<<MAX_REQUEST_SIZE <<"nToRead = "<<nToRead;
    if (!m_baReqLineBuf.contains('\r') && !m_baReqLineBuf.contains('\n'))
    {
        if(m_baReqLineBuf.size() > 1024)
            vLogDebug("HttpRequest: 当前请求行数据不包含换行符,继续等待完整数据.");
        else
            vLogDebug("HttpRequest: 当前请求行数据不包含换行符,继续等待完整数据,数据内容=%s.", m_baReqLineBuf.data());
        return ;
    }

    QByteArray baRequestLine = m_baReqLineBuf.trimmed();
    m_baReqLineBuf.clear();

    if (!baRequestLine.isEmpty())
    {
        vLogDebug("HttpRequest: 收到请求行数据[来自%s]:%s.", qUtf8Printable(pSocket->peerAddress().toString()), baRequestLine.data());

        QList<QByteArray> list = baRequestLine.split(' ');
        if (list.count()!=3 || !list.at(2).contains("HTTP"))
        {
            vLogError("HttpRequest: 无效的请求行数据.");
            m_nStatus = abort;
        }
        else
        {
            m_baMethod = list.at(0).trimmed();
            m_baPath = list.at(1);
            m_baVersion = list.at(2);
            m_peerAddress = pSocket->peerAddress();
            m_nStatus = waitForHeader;
            m_baIP = m_peerAddress.toString().remove("::ffff:").toUtf8().data();
        }
    }
}

void HttpRequest::readHeader(QTcpSocket* pSocket)
{
    qDebug()<<"MAX_REQUSE_SIZE = "<<MAX_REQUEST_SIZE <<"m_nCurrentSize = "<<m_nCurrentSize;
    qDebug()<<"开始读取请求头数据:";
    int nToRead = MAX_REQUEST_SIZE - m_nCurrentSize + 1;
    m_baReqHeaderBuf.append(pSocket->readLine(nToRead));
    m_nCurrentSize += m_baReqHeaderBuf.size();
    qDebug()<<"MAX_REQUSE_SIZE = "<<MAX_REQUEST_SIZE <<"nToRead = "<<nToRead;
    if (!m_baReqHeaderBuf.contains('\r') && !m_baReqHeaderBuf.contains('\n'))
    {
        if(m_baReqHeaderBuf.size() > 1024)
            vLogDebug("HttpRequest: 当前请求头数据不包含换行符,继续等待完整数据.");
        else
            vLogDebug("HttpRequest: 当前请求头数据不包含换行符,继续等待完整数据,数据内容=%s.", m_baReqHeaderBuf.data());
        return ;
    }

    QByteArray baNewData = m_baReqHeaderBuf.trimmed();
    m_baReqHeaderBuf.clear();
    int nIndex = baNewData.indexOf(':');
    if (nIndex > 0)
    {
        //考虑大小写不一致问题,这里把所有请求头的key和value都转小写再保存
        m_baCurrentHeader = baNewData.left(nIndex).toLower();
        QByteArray baValue = baNewData.mid(nIndex+1).trimmed().toLower();
        m_mapHeaders.insert(m_baCurrentHeader, baValue);
        LOG_DETAIL3("HttpRequest: 收到请求头数据[%s: %s].", m_baCurrentHeader.data(), baValue.data());
    }
    else if (!baNewData.isEmpty())
    {
        //若不包含冒号且数据不为空,则认为是上一次头数据的追加
        if (m_mapHeaders.contains(m_baCurrentHeader))
            m_mapHeaders.insert(m_baCurrentHeader, m_mapHeaders.value(m_baCurrentHeader)+" "+baNewData);
    }
    else
    {
        vLogDebug("HttpRequest: 收到请求头的结尾：空白行.");

        QByteArray baContentLength = m_mapHeaders.value("content-length");
        if (!baContentLength.isEmpty())
            m_nExpectedBodySize = baContentLength.toInt();

        if (0 == m_nExpectedBodySize)
        {
            vLogDebug("HttpRequest: 本次请求无请求体.");
            qDebug()<<"本次请求无请求体";
            m_nStatus = complete;
        }
        else if ((m_nExpectedBodySize+m_nCurrentSize) > MAX_REQUEST_SIZE)
        {
            vLogDebug("HttpRequest: 请求体长度[%d]过大,无法接收.", m_nExpectedBodySize);
            m_nStatus = abort;
        }
        else
        {
            m_nStatus = waitForBody;
        }
    }
}

void HttpRequest::readBody(QTcpSocket* pSocket)
{
    qDebug()<<"开始读取请求体内容:";
    //判断一个传输是否结束,一是Content-Length;二是chunked编码最后的空chunked块。但这里不考虑分块传输。
    int nToRead = m_nExpectedBodySize - m_baBodyData.size();
    QByteArray baNewData = pSocket->read(nToRead);
    m_nCurrentSize += baNewData.size();
    m_baBodyData.append(baNewData);
    if (m_baBodyData.size() >= m_nExpectedBodySize)
        m_nStatus = complete;
}
