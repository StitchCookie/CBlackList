#include "requesthandler.h"

RequestHandler::RequestHandler(QObject* parent)
    : QObject(parent)
{
    vLogDebug("RequestHandler: created");
}

RequestHandler::~RequestHandler()
{
    vLogDebug("RequestHandler: deleted");
}

void RequestHandler::handler(HttpRequest& request, HttpResponse& response)
{
    Http_Request_Info req;
    Http_Respone_Info reply;
    req.path = request.getPath();
    req.body = request.getBody();
    req.ip = request.getIP();
    if (m_callback == nullptr)
    {
        vLogFatal("RequestHandler: 请先注册回调函数");
        respone(-1, "{\"info\":\"服务异常\"}", "", "", response);
    }
    else
    {
        /*回调函数开始执行*/
        m_callback(req, reply);

        respone(reply.code, reply.body.toUtf8(), reply.apiType, reply.apiName, response);
    }
}

void RequestHandler::respone(int nSubCode, const QByteArray &data, const QString &apiType, const QString &apiName, HttpResponse &response)
{
    QVariantMap qMapContent;
    qMapContent["code"] = nSubCode;
    if(nSubCode)
    {
        qMapContent["message"] = data;
    }
    else
    {
        qMapContent["message"] = "success";
        qMapContent["bizContent"] = data;
    }
    QByteArray baRspData = QJsonDocument(QJsonObject::fromVariantMap(qMapContent)).toJson(QJsonDocument::Compact);
    response.write(baRspData, true);
    vLogInfo("RequestHandler: 回复数据,接口名:%s,请求名:%s,内容:%s", apiType.toUtf8().data(), apiName.toUtf8().data(), baRspData.data());
}
