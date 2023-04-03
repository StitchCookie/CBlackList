#include "CBlack.h"
#include<QTime>
#include <QtGlobal>
#include <QStringList>
#include <QTextCodec>
#include <hiredis/hiredis.h>
#include "pub/systemconfig.h"
#include <QDebug>
CBlack::CBlack(QObject *parent) : QObject(parent)
  ,m_redisStatus(false)
{

    m_nHandle = 0;
    this->moveToThread(&m_pThread);
    m_pThread.start();
}

CBlack::~CBlack()
{
    m_pThread.quit();
    m_pThread.wait();
    m_pThread.deleteLater();
}

/**
 * @brief CBlack::IC_API
 * @param APi_Name 请求名
 * @param json    解析的请求内容bizContent
 * @param baRes   响应的内容
 * @param ip      请求客户端IP
 * @return        返回0请求成功
 * @note          处理http请求(如果请求体解析失败，不会进入该函数)
 */
int CBlack::IC_API(const QString &APi_Name, const QString &array, QByteArray& baRes)
{
    int nRet = -1;
    QVariantMap qMapContent;
    if(APi_Name.compare(m_baseInfo.cBLackResponse.responseAddr.split("/").last()) ==0)
    {
        m_lock.lock();
        redisContext *m_ctx = redisConnect(m_baseInfo.loaclRedis.redisIp.toStdString().c_str(),m_baseInfo.loaclRedis.redisPort);

        if(m_ctx == nullptr || m_ctx->err)
        {
            if(m_ctx)
            {
                QString str = QString("Error%q").arg(m_ctx->errstr);
                redisFree(m_ctx);
                m_ctx = nullptr;
                qDebug()<<str;
            }else{
                QString str = "can't allocate redis context";
                qDebug()<<str;
            }
            return -1;
        }
        if(m_baseInfo.sys_Switch.redisPasswdForbid == 1)
        {
            redisReply * reply = (redisReply *)redisCommand(m_ctx, "AUTH %s", m_baseInfo.loaclRedis.redisPasswd.toStdString().c_str());
            if (reply->type == REDIS_REPLY_ERROR)
            {
                qDebug()<<"Redis密码验证失败如果本地Reids不使用密码登录，并不会影响程序(建议修改配置文件redis_UsePassd = 0)！";
            }
            else
            {
            }
            freeReplyObject(reply);
            reply = nullptr;
        }
        QString selectNo = QString("select %1").arg(m_baseInfo.loaclRedis.redisNo);
        redisReply *reply = (redisReply*)redisCommand(m_ctx,selectNo.toStdString().c_str());
        if(reply->type == REDIS_REPLY_STATUS)
        {
            m_redisStatus = true;
        }else{
            m_redisStatus = false;
            qDebug()<<"Redis No enter failed";
        }
        freeReplyObject(reply);
        reply = nullptr;
        QString selectData = QString("get %1").arg(array);
        redisReply *reply1 = (redisReply*)redisCommand(m_ctx,selectData.toStdString().c_str());
        if(reply1->type == REDIS_REPLY_STRING && reply1 != NULL)
        {
            nRet = 0;
            m_redisStatus = true;
            bool ok;
            int tempint =  QString(reply1->str).toInt(&ok);
            qMapContent["result"]=tempint;
            qMapContent["infos"]="BlackListcheck";

            freeReplyObject(reply1);
            reply1 = nullptr;
        }else{
            nRet = 0;
            m_redisStatus = false;
            qMapContent["result"]= -1;
            qMapContent["infos"]="BlackListcheck";
        }

        baRes = QJsonDocument(QJsonObject::fromVariantMap(qMapContent)).toJson(QJsonDocument::Compact);
        redisFree(m_ctx);
        m_ctx = nullptr;
        m_lock.unlock();
    }
    return nRet;

}
/**
 * @brief  HexQString_to_QString
 * @param  utf-8编码的字符串
 * @return GBK 编码字符串
 */
QString CBlack::HexQString_to_QString(const QString &text)
{
    QTextCodec *tc = QTextCodec::codecForName("GBK");
    // 将Hex字符串转换为Ascii字符串
    QString temp;
    QByteArray ascii_data;
    // QStringList temp_list = text.split('0', QString::SkipEmptyParts);
    foreach(QString str, text)
    {
        ascii_data.append(str);
    }
    // 将ascii_data中的16进制数据转化为对应的字符串，比如\x31转换为"1"
    temp = tc->toUnicode(QByteArray::fromHex(ascii_data));
    return temp;
}
/**
 * @brief removeZeroChar
 * @param 车牌号码GBK16进制编码
 * @return 去掉尾部与编码无关的占位字符0
 */
QString CBlack::removeZeroChar(QString text)
{
    QString temp = text;
    int a = 0;
    int length = temp.size();
    for(int i =0 ; i< length; ++i)
    {
        if(temp.right(1).compare("0")==0)
        {
            ++a;
            temp = temp.left(temp.length()-1);

        }
    }
    if(a%2!=0)
    {
        temp = temp.append("0");
    }
    return temp;
}
