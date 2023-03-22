#include "icreadergui.h"
#include<QTime>
#include <QtGlobal>
#include <QTextCodec>
Tool_ICReader::Tool_ICReader(QObject *parent) : QObject(parent)
{
    m_nHandle = 0;
    this->moveToThread(&m_pThread);
    m_pThread.start();
}

/**
 * @brief Tool_ICReader::IC_API
 * @param APi_Name 请求名
 * @param json    解析的请求内容bizContent
 * @param baRes   响应的内容
 * @param ip      请求客户端IP
 * @return        返回0请求成功
 * @note          处理http请求(如果请求体解析失败，不会进入该函数)
 */
int Tool_ICReader::IC_API(const QString &APi_Name, const QJsonObject &json, QByteArray& baRes,QString ip)
{
    int nRet = -1;                                                                                /*给主线程发送当前请求地址IP*/
    QVariantMap qMapContent;
    m_lock.lock();

    baRes = QJsonDocument(QJsonObject::fromVariantMap(qMapContent)).toJson(QJsonDocument::Compact);
    m_lock.unlock();

    if (APi_Name == "JT_OpenCard" && (nRet == 0 || nRet == 4))
        return 0;
    return nRet;
}
/**
 * @brief  HexQString_to_QString
 * @param  utf-8编码的字符串
 * @return GBK 编码字符串
 */
QString Tool_ICReader::HexQString_to_QString(const QString &text)
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
QString Tool_ICReader::removeZeroChar(QString text)
{
    QString temp = text;
    int a = 0;
    int length = temp.size();
    for(int i =0 ; i< length; ++i)
    {
        if(temp.right(1)=='0')
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
