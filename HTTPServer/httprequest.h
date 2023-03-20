#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QByteArray>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include "logrecord/logrecord.h"
#define MAX_REQUEST_SIZE 4*1024*1024     //一次请求允许的最大数据长度

class HttpRequest
{
public:
    /* 请求状态 */
    enum RequestStatus {waitForRequest, waitForHeader, waitForBody, complete, abort};

    HttpRequest();
    virtual ~HttpRequest();

    /* 读取请求内容 */
	void readFromSocket(QTcpSocket *pSocket);

    /* 获取请求状态/信息 */
    RequestStatus getStatus(){return m_nStatus;}
    QByteArray getMethod(){return m_baMethod;}
    QByteArray getPath(){return m_baPath;}
    QByteArray getVersion(){return m_baVersion;}
    QHostAddress getPeerAddress(){return m_peerAddress;}
    QByteArray getIP(){return m_baIP;}
    QByteArray getHeader(const QByteArray& name){return m_mapHeaders.value(name.toLower());}
    QList<QByteArray> getHeaders(const QByteArray& name){return m_mapHeaders.values(name.toLower());}
    QMultiMap<QByteArray,QByteArray> getHeaderMap(){return m_mapHeaders;}
    QByteArray getBody(){return m_baBodyData;}

private:
    /* 读取请求信息 */
    void readRequest(QTcpSocket* socket);
    /* 读取请求头 */
    void readHeader(QTcpSocket* socket);
    /* 读取请求体 */
    void readBody(QTcpSocket* socket);

private:
    /* 请求状态 */
    RequestStatus m_nStatus;

    /* 请求客户端信息 */
    QByteArray m_baMethod;
    QByteArray m_baPath;
    QByteArray m_baVersion;
    QHostAddress m_peerAddress;
    QByteArray m_baIP;
    /* 请求头数据 */
    QMultiMap<QByteArray,QByteArray> m_mapHeaders;
    /* 请求体数据 */
    QByteArray m_baBodyData;
    /* 缓存数据 */
    QByteArray m_baReqLineBuf;
    QByteArray m_baReqHeaderBuf;
    /* 当前请求头值 */
    QByteArray m_baCurrentHeader;

    /* 请求数据长度 */
    int m_nCurrentSize;
    int m_nExpectedBodySize;
};

#endif // HTTPREQUEST_H
