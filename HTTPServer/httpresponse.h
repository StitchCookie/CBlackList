#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QMap>
#include <QString>
#include <QTcpSocket>
#include "logrecord/logrecord.h"
class HttpResponse
{
public:
    HttpResponse(QTcpSocket *socket);

    /* 设置消息头 */
    void setHeader(QByteArray baKey, QByteArray baValue);
    /* 获取消息头 */
    QMap<QByteArray,QByteArray>& getHeaders();
    /* 发送数据 */
    void write(QByteArray data, bool lastPart=false);
    /* 判断发送是否完整 */
    bool hasSentLastPart() const;

private:
    /* TcpSocket句柄 */
    QTcpSocket* m_pSocket;

    /* 消息头 */
    QMap<QByteArray,QByteArray> headers;

    /* 发送标记 */
    bool m_isSentHeaders;
    bool m_isSentLastPart;

    /* 发送数据 */
    bool writeToSocket(QByteArray data);
    void writeHeaders();
};

#endif // HTTPRESPONSE_H
