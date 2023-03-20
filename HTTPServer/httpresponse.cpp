#include "httpresponse.h"

HttpResponse::HttpResponse(QTcpSocket *socket)
    : m_pSocket(socket)
    , m_isSentHeaders(false)
    , m_isSentLastPart(false)
{
}

void HttpResponse::setHeader(QByteArray baKey, QByteArray baValue)
{
    headers.insert(baKey, baValue);
}

QMap<QByteArray,QByteArray>& HttpResponse::getHeaders()
{
    return headers;
}

void HttpResponse::write(QByteArray baData, bool isLastPart)
{
    if (false == m_isSentHeaders)
    {
        if (isLastPart)
            headers.insert("Content-Length", QByteArray::number(baData.size()));
        writeHeaders();
    }

    if (baData.size() > 0)
        writeToSocket(baData);

    if (isLastPart)
    {
        m_pSocket->flush();
        m_isSentLastPart = true;
    }
}

bool HttpResponse::hasSentLastPart() const
{
    return m_isSentLastPart;
}

bool HttpResponse::writeToSocket(QByteArray baData)
{
    int nRemaining = baData.size();
    char* pData = baData.data();
    while (m_pSocket->isOpen() && nRemaining>0)
    {
        if (m_pSocket->bytesToWrite() > (16*1024))
            m_pSocket->waitForBytesWritten(-1); //待发送数据过大时，加等待操作

        qDebug()<<"发送的数据"<<pData;
        qint64 written = m_pSocket->write(pData, nRemaining);
        if (written == -1)
            return false;

        pData += written;
        nRemaining -= written;
    }

    return true;
}

void HttpResponse::writeHeaders()
{
    qDebug()<<"HttpResponse: 响应头已发送,不再重复发送";
    if(m_isSentHeaders)
    {
        qDebug()<<"HttpResponse: 响应头已发送,不再重复发送";
        vLogError("HttpResponse: 响应头已发送,不再重复发送.");
        return;
    }
    QByteArray buffer;
    buffer.append("HTTP/1.1 ");
    buffer.append("200");
    buffer.append(' ');
    buffer.append("OK");
    buffer.append("\r\n");
    foreach (QByteArray name, headers.keys())
    {
        buffer.append(name);
        buffer.append(": ");
        buffer.append(headers.value(name));
        buffer.append("\r\n");
        qDebug()<<name<<headers.value(name);
    }
    buffer.append("\r\n");
    qDebug()<<buffer;
    writeToSocket(buffer);
    m_pSocket->flush();
    m_isSentHeaders = true;
}
