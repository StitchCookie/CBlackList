// ***********************************************
//
// 项目名称(projectName)： datatransfer
//
// 文件名(FileName)：logrecord.h
//
// 功能描述(Description)：
//
// 作者(Author)：王锐煌
//
// 创建日期(Created Date)：2019-08-16
//
// 修改记录(Revision Record)：
//
// ***********************************************

#ifndef LOGLOCAL_HPP
#define LOGLOCAL_HPP

#define LOG_LINE_MAXBYTE 40960
#include <QCoreApplication>
/*！
 * 在工程的pro添加宏定义，支持异步写日志
 * DEFINES += USING_LOG_THREAD
 */
/**
 * @brief The EN_LOG_LEVEL enum
 * 在工程的pro添加宏定义，支持异步写日志
 * DEFINES += USING_LOG_THREAD
 */
enum EN_LOG_LEVEL{LL_DEBUG,LL_INFO,LL_WARN,LL_ERROR,LL_FATAL};
enum EN_LOG_OUTPUT{LOG_OUT_TXT=1, LOG_OUT_STD=2};
/**
 * @brief initLogRecord : 设置日志的级别，名称和路径。
 * @param nLevel        : 日志级别
 * @param sLogName      : 日志名称
 * @param sLogPath      : 日志路径
 * @param isMajor       : 是否主日志，默认为否
 * @param nMaxSize      : 日志最大容量(单位：KB)
 * @return              : 初始化成功，返回日志对象句柄(>=0)；失败返回-1。
 */
int  initLogRecord(int nLevel,const char* sLogName,const char* sLogPath="./log",
                   bool isMajor=false, int nMaxSize=10240, int nOutput=LOG_OUT_TXT);
/**
 * @brief deinitLogRecord : 回收日志资源
 * @param nLogHandle      : 日志句柄
 */
void deinitLogRecord(int nLogHandle=-1);
/**
 * @brief deinitLogRecordWithName : 回收日志资源
 * @param sLogName                : 日志名称
 */
void deinitLogRecordWithName(const char* sLogName=nullptr);

/**
 * @brief setMajorLogRecord : 设置主日志
 * @param nLogHandle        : 主日志句柄
 * @return                  : 设置成功返回true
 */
bool setMajorLogRecord(int nLogHandle);

/**
 * @brief setMajorLogRecord : 设置主目录
 * @param sLogName          : 主日志名称
 * @return                  : 设置成功返回true
 */
bool setMajorLogRecord(const char* sLogName);

/**
 * @brief getLogRecordHandle : 获取日志句柄
 * @param sLogName           : 日志名称
 * @return                   : 日志句柄
 */
int getLogRecordHandle(const char* sLogName=nullptr);

/**
 * @brief vLogLevel : 记录日志(默认写到主日志)
 * @param nLevel    : 日志等级
 * @param format    : 格式化消息
 */
void vLogLevel(int nLevel, const char *format,...);
void vLogDebug(            const char *format,...);
void vLogInfo (            const char *format,...);
void vLogWarn (            const char *format,...);
void vLogError(            const char *format,...);
void vLogFatal(            const char *format,...);

void vLogLevel1(int nLevel, const char *message);
void vLogDebug1(            const char *message);
void vLogInfo1 (            const char *message);
void vLogWarn1 (            const char *message);
void vLogError1(            const char *message);
void vLogFatal1(            const char *message);

void vLogLevelLen(int nLevel, const char *message,    int msglen);
void vLogLevelLen(int nLevel,        int  nLogHandle, const char *message, int msglen);
void vLogLevelLen(int nLevel, const char* sLogName,   const char *message, int msglen);

/**
 * @brief row        : 本次写入的日志在文件中的行数
 * @param logName    : 本次写入的日志名称 日志名称随着时间、文件大小的变化而变化
 * @param format     : 格式化消息
 */
void  vLogDebug2(int &row, QString & logName,  const char *format,...);
/**
 * @brief vLogLevel  : 支持以日志句柄作为选择写入日志文件，如没有找到，则输出到默认日志文件
 * @param nLevel     : 日志等级
 * @param nLogHandle : 日志句柄
 * @param format     : 格式化消息
 */
void vLogLevel(int nLevel, int nLogHandle, const char *format,...);

/**
 * @brief vLogLevel1
 * @param nLevel
 * @param nLogHandle
 * @param message
 */
void vLogLevel1(int nLevel, int nLogHandle, const char *message);

/**
 * @brief vLogLevel: 支持以文件前缀作为选择写入日志文件，如没有找到，则输出到默认日志文件。
 * @param nLevel
 * @param sLogName
 * @param format
 */
void vLogLevel(int nLevel, const char* sLogName, const char *format,...);

/**
 * @brief vLogLevel1
 * @param nLevel
 * @param sLogName
 * @param message
 */
void vLogLevel1(int nLevel, const char* sLogName, const char *message);

#ifdef LOG_DETAIL_FOR_TEST
#define LOG_DETAIL1(p1)                         vLogDebug1(p1)
#define LOG_DETAIL2(p1,p2)                      vLogDebug(p1,p2)
#define LOG_DETAIL3(p1,p2,p3)                   vLogDebug(p1,p2,p3)
#define LOG_DETAIL4(p1,p2,p3,p4)                vLogDebug(p1,p2,p3,p4)
#define LOG_DETAIL5(p1,p2,p3,p4,p5)             vLogDebug(p1,p2,p3,p4,p5)
#define LOG_DETAIL6(p1,p2,p3,p4,p5,p6)          vLogDebug(p1,p2,p3,p4,p5,p6)
#define LOG_DETAIL7(p1,p2,p3,p4,p5,p6,p7)       vLogDebug(p1,p2,p3,p4,p5,p6,p7)
#define LOG_DETAIL8(p1,p2,p3,p4,p5,p6,p7,p8)    vLogDebug(p1,p2,p3,p4,p5,p6,p7,p8)
#define LOG_DETAIL9(p1,p2,p3,p4,p5,p6,p7,p8,p9) vLogDebug(p1,p2,p3,p4,p5,p6,p7,p8,p9)
#else
#define LOG_DETAIL1(p1)
#define LOG_DETAIL2(p1,p2)
#define LOG_DETAIL3(p1,p2,p3)
#define LOG_DETAIL4(p1,p2,p3,p4)
#define LOG_DETAIL5(p1,p2,p3,p4,p5)
#define LOG_DETAIL6(p1,p2,p3,p4,p5,p6)
#define LOG_DETAIL7(p1,p2,p3,p4,p5,p6,p7)
#define LOG_DETAIL8(p1,p2,p3,p4,p5,p6,p7,p8)
#define LOG_DETAIL9(p1,p2,p3,p4,p5,p6,p7,p8,p9)
#endif

#endif // LOGINTERFACE_HPP
