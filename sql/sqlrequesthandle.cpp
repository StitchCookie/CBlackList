#include "sqlrequesthandle.h"
#include <QDateTime>
#include <QMetaObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextCodec>
#include <QByteArray>
int sqlRequestHandle::m_requestCount = 0;
sqlRequestHandle::sqlRequestHandle(QObject *parent) : CBlackListSql(parent)
{
    this->moveToThread(&m_thread);
    m_thread.start();
    connect(&m_thread,&QThread::started,this,&sqlRequestHandle::sqlThreadWorking);
    qDebug()<<"当前mysql线程id为"<<QThread::currentThreadId();
    m_interverTime = nullptr;
    m_selectSql = QString("select CONCAT(CPUNetID,CPUCardId),OBUIssueID,VehiclePlate from `%1` where TransTime between date_add(now(), interval - %2 minute) and now()AND CPUNetID is not NULL AND CPUNetID!='0000' LIMIT 500;")
            .arg(m_baseInfo.grantrayMysqlConfig.tableName)
            .arg(m_baseInfo.grantrayMysqlConfig.scopeTime);
}

QString sqlRequestHandle::Utf8hexqstringToGbkhexqstring(const QString &text)
{
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
}

void sqlRequestHandle::sqlThreadWorking()
{
    //m_httpclient = new HttpClient;
    //[0]设置数据库连接信息
    if(setConnectBase(m_baseInfo.grantrayMysqlConfig.ip,m_baseInfo.grantrayMysqlConfig.port,m_baseInfo.grantrayMysqlConfig.user,
                      m_baseInfo.grantrayMysqlConfig.passwd,m_baseInfo.grantrayMysqlConfig.databaseName,sqlConnectType::mysql))
    {
        m_interverTime = new QTimer(this);
    }
    //[0]设置数据库连接信息

    if(m_interverTime != nullptr)
    {
        dealSqlData();
        connect(m_interverTime,&QTimer::timeout,this,&sqlRequestHandle::dealSqlData);
    }
}
sqlRequestHandle::~sqlRequestHandle()
{
    m_thread.quit();
    m_thread.wait();
    m_thread.deleteLater();
   // m_httpclient->deleteLater();
}

void sqlRequestHandle::dealSqlData()
{
    m_interverTime->stop();
    m_query  = getHandleData(m_selectSql,m_selectStatus);
    if(!m_selectStatus)
    {
        qDebug()<<"mysql扫描失败"<<m_query.lastError().text()<< "扫描时间"<<QDateTime::currentDateTime().toString("yyyy-MM-ddthh:mm:ss");
    }else{
        qDebug()<<"mysql扫描成功"<<QDateTime::currentDateTime().toString("yyyy-MM-ddthh:mm:ss");
    }
    while(m_query.next())
    {
      //  QByteArray m_bytearray;
      //  QVariantMap m_map;
     //   m_map["checkType"] = 7;
     //   m_map["cpcCardId"] = "";
     //   m_map["etcCardId"] = m_query.value(0).toString();
  //      m_map["laneId"] = m_baseInfo.laneBase_Info.laneId;
//        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
//        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//        QTextCodec *gbk = QTextCodec::codecForName("GBK");
       // QString strUnicoude = utf8->toUnicode(m_query.value(2).toString().toLocal8Bit().data());
       // QByteArray gbk_byte = gbk->fromUnicode(strUnicoude);
       // m_map["license"] = gbk_byte.toHex();
       // m_map["obuId"] = m_query.value(1).toString();
       // m_map["provId"] = m_baseInfo.laneBase_Info.provid;
       // m_map["stationId"] = m_baseInfo.laneBase_Info.statinid;
       // m_bytearray = QJsonDocument(QJsonObject::fromVariantMap(m_map)).toJson(QJsonDocument::Compact);
        //qDebug()<<m_bytearray.toHex();
        //qDebug()<<QByteArray::fromHex(m_bytearray.toHex());
        m_data.etcCardId = m_query.value(0).toString();
        m_data.obuid = m_query.value(1).toString();
        m_data.license = m_query.value(2).toString();
        m_qqueueData.enqueue(m_data);
        qDebug()<<"enqueue size is :"<<m_qqueueData.size();
       // m_httpclient->incoming_MysqlData(m_bytearray);
    }
    m_interverTime->setInterval(m_baseInfo.grantrayMysqlConfig.intervalTime * 1000 * 60);
    m_interverTime->start();
}


