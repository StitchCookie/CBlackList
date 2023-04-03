#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "httprequest.h"
#include "httpresponse.h"
#include "logrecord/logrecord.h"
/* HTTP请求内容 */
struct Http_Request_Info
{
    QString path;
    QString body;
    QString ip;
    Http_Request_Info()
    {
        path.clear();
        body.clear();
        ip.clear();
    }
};

/* HTTP响应内容 */
struct Http_Respone_Info
{
    int code;
    QString apiType;
    QString apiName;
    QString body;
    Http_Respone_Info()
    {
        code = 0;
        apiType.clear();
        apiName.clear();
        body.clear();
    }
};

/* 回调函数 */
typedef int (*callback)(const Http_Request_Info &, Http_Respone_Info &);

class RequestHandler : public QObject
{
    Q_OBJECT
public:
    RequestHandler(QObject* parent=nullptr);
    ~RequestHandler();

    /* 请求处理函数 */
    void handler(HttpRequest& request, HttpResponse& response);

private:
    /* 响应请求 */
    void respone(int nSubCode, const QByteArray &data, const QString &apiType, const QString &apiName, HttpResponse &response);

public:
    /* 回调函数 */
    callback m_callback;
};

#endif // REQUESTHANDLER_H
