#include "apihandler.h"
#pragma execution_character_set(“utf-8”)
apihandler::apihandler()
{
    this->moveToThread(&m_pThread);
    m_pThread.start();
}

int apihandler::handler(const Http_Request_Info & req, Http_Respone_Info & reply)
{
    //解析请求地址
    QStringList pathList = req.path.split('/');
    if(pathList.length() != 5)
    {
          vLogDebug("[ApiHandler] 请求地址出错: %s", req.path.toUtf8().data());
          qDebug()<<"请求地址失败:";
        respone(-1, "{\"info\":\"请求地址出错\"}", "null", "null", reply);
        return reply.code;
    }

    QString API_Type = pathList.at(3);
    QString API_Name = pathList.at(4);
    QByteArray body = req.body.toUtf8();
     vLogInfo("[ApiHandler] 收到请求,接口名:%s,请求名:%s,请求体:%s", API_Type.toUtf8().data(), API_Name.toUtf8().data(), body.data());
    //解析请求体
    QJsonObject json;
    if (false == analysisHttpBody(body, json))
    {
         qDebug()<<"请求内容失败:";
        QString aaa = "{\"info\":\"请求内容解析失败\"}";
        QByteArray bbb = aaa.toUtf8();
        respone(-1, bbb, API_Type, API_Name, reply);
        return reply.code;
    }
    //    //状态监测接口
    if (API_Type == "reader")
    {
        QByteArray p_OutputData = "";
        int ret = Tool_ICReader::getInstance().IC_API(API_Name, json, p_OutputData,req.ip);
        qDebug()<<"响应读卡器打开";
        //respone(ret == IC_NO_ERR ? 0 : 1, p_OutputData, API_Type, API_Name, reply);
    }

    return reply.code;
}



bool apihandler::analysisHttpBody(const QByteArray &pBody, QJsonObject &json)
{
    QJsonParseError err_rpt;
    QJsonDocument root_Doc = QJsonDocument::fromJson(pBody, &err_rpt);
    if(err_rpt.error != QJsonParseError::NoError)
    {
        vLogError("[ApiHandler] JSON格式错误");
        return false;
    }
    if (root_Doc.isObject())
    {
        QJsonObject object = root_Doc.object();
        if(object.contains("bizContent"))
        {
            QJsonValue value = object.value("bizContent");
            json = QJsonDocument::fromJson(value.toString().toUtf8()).object();
        }
    }
    return true;
}

void apihandler::respone(int nSubCode, const QByteArray &data, const QString &apiType, const QString &apiName, Http_Respone_Info &reply)
{
    reply.code = nSubCode;
    reply.body = data;
    reply.apiType = apiType;
    reply.apiName = apiName;
}
