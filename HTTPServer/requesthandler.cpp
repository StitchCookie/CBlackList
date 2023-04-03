#include "requesthandler.h"
#include <QThread>
RequestHandler::RequestHandler(QObject* parent)
    : QObject(parent)
{
    vLogDebug("RequestHandler: created");
}

RequestHandler::~RequestHandler()
{
    vLogDebug("RequestHandler: deleted");
}
/**
 * @brief RequestHandler::handler
 * @param request   http请求的内容
 * @param response  http响应内容
 */
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
        if(reply.code == 0)
        respone(reply.code, reply.body.toUtf8(), reply.apiType, reply.apiName, response);
        {
            vLogDebug("当前收到来自[%s]请求的车牌------------%s：",req.ip.toUtf8().data(),req.body.toUtf8().data());
            vLogDebug("当前响应的数据------------%s：",reply.body.toUtf8().data());
        }

    }
}
/**
 * @brief RequestHandler::respone
 * @param nSubCode  响应状态码 0为正确响应
 * @param data      响应数据
 * @param apiType
 * @param apiName
 * @param response
 */
void RequestHandler::respone(int nSubCode, const QByteArray &data, const QString &apiType, const QString &apiName, HttpResponse &response)
{
    QVariantMap qmapData;

    if(nSubCode)
    {
        qmapData["error:"] = "请检查URL请求地址或Redis数据库配置信息";
        QByteArray baRspData = QJsonDocument(QJsonObject::fromVariantMap(qmapData)).toJson(QJsonDocument::Compact);
        response.write(baRspData, true);
        return;
    }
    response.write(data, true);
}
