// ***********************************************
//
// 项目名称(projectName)： datatransfer
//
// 文件名(FileName)：logrecord.cpp
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

#include "logrecord/logrecord.h"
#include <signal.h>
#ifndef __unix
#include <processthreadsapi.h>
#else
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <string.h>
#include <list>
#include <string>
#include <time.h>
#include <stdarg.h>
#include <map>
#include <unordered_map>
#include <memory>
#include <QReadWriteLock>
#include <QStringList>
#ifdef USING_LOG_THREAD
#include <thread>
#include <functional>

#include <condition_variable>
#include <atomic>
#endif

class TTestLog
{
public:
#ifdef USING_LOG_THREAD
class TWriteLogThread
{
public:
    TWriteLogThread(TTestLog* log)
        :m_thread()
        ,m_mutex()
        ,m_cond()
        ,m_mapMessage()
        ,m_isTerminate(false)
        ,m_log(log)
    {
        m_thread = std::thread(&TWriteLogThread::run,this);
    }

    ~TWriteLogThread()
    {
        stop();
    }

    void push(const std::string& date, const std::string& sMsg)
    {
        if(true==m_isTerminate.load()) return ;
        std::unique_lock<std::mutex> lk(m_mutex);
        if(m_mapMessage.count(date)>0)
            m_mapMessage[date].push_back(sMsg);
        else
        {
            std::list<std::string> lst;
            lst.push_back(sMsg);
            m_mapMessage.insert(std::make_pair(date,lst));
        }
        m_cond.notify_all();
    }

    void stop(void)
    {
        if(true==m_isTerminate.load()) return ;
        m_isTerminate.store(true);
        m_cond.notify_all();
        if(m_thread.joinable())
            m_thread.join();
    }
protected:
    void run(void)
    {
        try
        {
            while(false==m_isTerminate.load())
            {
                writeMessage();
            }
            writeMessage();
        }
        catch(...)
        {}
    }
    void writeMessage(void)
    {
        try
        {
            std::map<std::string,std::list<std::string> >  mapMessage;
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                if(m_mapMessage.empty())
                {
                    if(true==m_isTerminate.load()) return ;
                    m_cond.wait_for(lk,std::chrono::seconds(5));
                    return ;
                }
                m_mapMessage.swap(mapMessage);
            }

            for(auto iter = mapMessage.begin();
                iter!=mapMessage.end();++iter)
            {
                m_log->writebatch(iter->first,iter->second);
            }
        }
        catch(...)
        {}
    }
protected:
    std::thread m_thread;
    std::mutex  m_mutex;
    std::condition_variable m_cond;
    std::map<std::string,std::list<std::string> > m_mapMessage;
    std::atomic_bool m_isTerminate;

    TTestLog* m_log;
};
#endif
public:
    TTestLog()
        :m_nLevel(0)
        ,m_sLogPath("./log")
        ,m_sLogName("log")
        ,m_nLogIndex(m_nLogSerialNo++)
        ,m_tmHourLine()
        ,m_nLogMaxSize(10240<<10)
        ,m_nLogSize(0)
        ,m_nLogNumber(1)
        ,m_nLogOut(LOG_OUT_TXT)
    {
        memset(&m_tmHourLine,0,sizeof(m_tmHourLine));
        m_nLogRowNumber = 0;
#ifdef USING_LOG_THREAD
        m_thread = std::make_shared<TWriteLogThread>(this);
#endif
    }

    ~TTestLog()
    {
    }

    int setLogPath(const char* sLogPath)
    {
        m_sLogPath = sLogPath;
        return checkDirExist(m_sLogPath.c_str());
    }
    void setLogName(const char* sLogName,int nMaxSize = 10240)
    {
        m_sLogName = sLogName;
        auto numSize  = readLogNumber(m_sLogPath,m_sLogName);
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char buf[64]={0};
        strftime(&buf[0], sizeof(buf), "%Y%m%d",&curtime);
        m_date = buf;
        m_nLogNumber  = numSize.first;
        m_nLogSize    = numSize.second;
        m_nLogMaxSize = nMaxSize << 10;
    }
    void setLevel(int nLevel){m_nLevel=nLevel%5;}
    int  getLevel(void){return m_nLevel;}
    void setLogOut(int nOutput){m_nLogOut = nOutput;}
    void setLogRowNumber()
    {
        //m_nLogRowNumber = ;
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char date[64]={0};
        makedate(date,sizeof(date),curtime);
        addhourline(date,curtime);
        std::string Name = rName(date);
        if((access(Name.c_str(),F_OK)) != -1)
            setLogRowNumber(GetFileRowNumber(rName(date)));
        else
            clearLogRowNumber();
    }
    void setLogRowNumber(int LogRowNumber)
    {
        QWriteLocker lk(&m_rdwrLock);
        m_nLogRowNumber = LogRowNumber;
    }
    void addLogRowNumber()
    {
        QWriteLocker lk(&m_rdwrLock);
        ++m_nLogRowNumber;
    }
    void clearLogRowNumber()
    {
        QWriteLocker lk(&m_rdwrLock);
        m_nLogRowNumber = 0;
    }
    int getLogRowNumber()
    {
        QReadLocker lk(&m_rdwrLock);
        return m_nLogRowNumber;
    }
    void writeLog(int nLevel,const char *format,...)
    {
        if(nLevel<m_nLevel) return ;
        char message[LOG_LINE_MAXBYTE+1] = {0};
        va_list ap;
        va_start(ap, format);
        int msglen=::vsnprintf(message,LOG_LINE_MAXBYTE,format,ap);
        va_end(ap);
        if(msglen>LOG_LINE_MAXBYTE)
            msglen = LOG_LINE_MAXBYTE;
        writeLogLen(nLevel,message,msglen);
    }

    void writeLog(int nLevel, const char *format,va_list ap, int msglen)
    {
        if(nLevel<m_nLevel) return ;
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char head[128] = {0};
        makehead(head,sizeof(head),curtime,tp,nLevel);
        int hdlen = strlen(head);
        char message[LOG_LINE_MAXBYTE] = {0};
        if(msglen>LOG_LINE_MAXBYTE-1)msglen=LOG_LINE_MAXBYTE-1;
        ::vsnprintf(message,LOG_LINE_MAXBYTE-1,format,ap);
        makeline(curtime,head,hdlen,message,msglen);
    }

    void writeLog1(int nLevel, const char *message)
    {
        if(nLevel<m_nLevel) return ;
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char head[128] = {0};
        makehead(head,sizeof(head),curtime,tp,nLevel);
        int hdlen  = strlen(head);
        int msglen = strlen(message);
        makeline(curtime,head,hdlen,message,msglen);
    }

    void writeLogLen(int nLevel, const char *message, int msglen)
    {
        if(nLevel<m_nLevel) return ;
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char head[128] = {0};
        makehead(head,sizeof(head),curtime,tp,nLevel);
        int hdlen  = strlen(head);
        makeline(curtime,head,hdlen,message,msglen);
    }

    void writeLogLen(int nLevel, const char *message, int msglen,int & Rownumber,QString & logName)
    {
        if(nLevel<m_nLevel) return ;
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char head[128] = {0};
        makehead(head,sizeof(head),curtime,tp,nLevel);
        int hdlen  = strlen(head);
#ifndef __unix
        std::string sMsg(hdlen+msglen+2,0);
#else
        std::string sMsg(hdlen+msglen+1,0);
#endif
        memcpy(&sMsg[0],head,hdlen);
        memcpy(&sMsg[hdlen],message,msglen);
        hdlen += msglen;
#ifndef __unix
        sMsg[hdlen++] = '\r';
#endif
        sMsg[hdlen++] = '\n';
        char date[64]={0};
        makedate(date,sizeof(date),curtime);
        addhourline(date,curtime);
        writeFile(date ,sMsg);
        Rownumber = getLogRowNumber();
        logName = rName(date).c_str();
        QStringList tmp = logName.split("/");
        logName = tmp[tmp.size() -1];
    }

    void stop(void)
    {
#ifdef USING_LOG_THREAD
        m_thread->stop();
#endif
    }

protected:

    void writeFile(const std::string& date, const std::string& sMsg)
    {
        try
        {
            if(date.length()==0) return ;
#ifdef USING_LOG_THREAD
            m_thread->push(date,sMsg);
#else
            if(m_nLogOut&LOG_OUT_TXT)
            {
                outputtxt(date,sMsg);
            }
            if(m_nLogOut&LOG_OUT_STD)
            {
                printf(sMsg.data());
            }
#endif
        }
        catch(...)
        {
        }
    }

    void outputtxt(const std::string& date, const std::string& sMsg)
    {
        FILE* fp = open(date);
        if(nullptr==fp) return ;
        append(fp,sMsg);
        close(fp);
    }

    void makecurtime(tm& curtime,timespec &tp)
    {
        clock_gettime(CLOCK_REALTIME,&tp);
        time_t tm_t = (time_t)tp.tv_sec;
#ifndef __unix
        localtime_s(&curtime,&tm_t);
#else
        localtime_r(&tm_t,&curtime);
#endif
    }

    void makehead(char* head,int hdsize,tm& curtime,timespec& tp,int nLevel)
    {
        static std::string sLevel[] = {"DEBUG","INFO","WARN","ERROR","FATAL"};
#ifndef __unix
        unsigned nthrd = GetCurrentThreadId();
#else
        unsigned nthrd = (unsigned)syscall(__NR_gettid);
#endif
        snprintf(head,hdsize-1,"%02d:%02d:%02d.%03d\t[%s]\t[%08X]\t"
                ,curtime.tm_hour,curtime.tm_min,curtime.tm_sec,
                (int)(tp.tv_nsec/1000000),sLevel[nLevel].c_str(),nthrd);
    }

    void addhourline(char* date, tm& curtime)
    {
        if(curtime.tm_yday!=m_tmHourLine.tm_yday ||
                curtime.tm_hour!=m_tmHourLine.tm_hour)
        {
            char buf[64]={0};
            strftime(&buf[0], sizeof(buf), "%Y-%m-%d %H",&curtime);
            std::string sLine(buf);
            sLine += std::string(256-strlen(buf),'=');
#ifndef __unix
            sLine.push_back('\r');
#endif
            sLine.push_back('\n');
            writeFile(date ,sLine);
            m_tmHourLine = curtime;
        }
    }

    void makeline(tm& curtime, const char* head, int hdlen,
                  const char* message, int msglen)
    {
#ifndef __unix
        std::string sMsg(hdlen+msglen+2,0);
#else
        std::string sMsg(hdlen+msglen+1,0);
#endif
        memcpy(&sMsg[0],head,hdlen);
        memcpy(&sMsg[hdlen],message,msglen);
        hdlen += msglen;
#ifndef __unix
        sMsg[hdlen++] = '\r';
#endif
        sMsg[hdlen++] = '\n';
        char date[64]={0};
        makedate(date,sizeof(date),curtime);
        addhourline(date,curtime);
        writeFile(date ,sMsg);
    }

    int makelogpath(char *logpath, int size, const char* date)
    {
        snprintf(logpath,size,"%s/%s",m_sLogPath.data(),date);
        if(0!=checkDirExist(logpath))
            return -1;
        return strlen(logpath);
    }

    void makelogname(char *logname, int size, const char* date)
    {
        snprintf(logname,size,"/%s_%s_%03d.log",m_sLogName.data(),date,m_nLogNumber);
    }

    void makedate(char *date, int size, tm& curtime)
    {
        strftime(date, size, "%Y%m%d",&curtime);
    }

    FILE* open(const std::string& date)
    {
        char sFileName[260]={0};
        int len = makelogpath(sFileName,256,date.data());
        if(len<0)
            return nullptr;
        if(m_nLogSize>m_nLogMaxSize)
        {
            m_nLogNumber++;
            m_nLogSize = 0;
            clearLogRowNumber();
        }

        if(m_date!=date)
        {
            m_nLogNumber=1;
            m_date = date;
            clearLogRowNumber();
        }
        makelogname(&(sFileName[len]),256-len,date.data());
        return fopen(sFileName,"ab");
    }
    std::string rName(const std::string& date)
    {
        char sFileName[260]={0};
        int len = makelogpath(sFileName,256,date.data());
        if(len<0)
            return nullptr;
        if(m_nLogSize>m_nLogMaxSize)
        {
            m_nLogNumber++;
            m_nLogSize = 0;
            clearLogRowNumber();
        }

        if(m_date!=date)
        {
            m_nLogNumber=1;
            m_date = date;
            clearLogRowNumber();
        }
        makelogname(&(sFileName[len]),256-len,date.data());
        return sFileName;
    }
    int GetFileRowNumber(const std::string& sFileName)
    {
        if(sFileName.empty())
            return 0;
        char Cmd[300] = {0};
        sprintf(Cmd,"wc -l %s |awk '{print $1}'",sFileName.c_str());
        //signal(SIGCHLD, SIG_IGN);
        char buff[50] = {0};
        char buf[50] = {0};
        FILE *  _shellFd = popen(Cmd,"r");
        if(_shellFd == NULL)
            return 0;
        if(NULL != fgets(buff, sizeof(buff), _shellFd))
        {
            if((strlen(buff)-1) >0)
            {
                strncpy(buf,buff,strlen(buff)-1);
            }
        }
        if(_shellFd != NULL)
            pclose(_shellFd);
        _shellFd = NULL;
        return atoi(buf);
    }
    FILE* checkreopen(FILE* fp, const std::string& date)
    {
        if(m_nLogSize<m_nLogMaxSize)
            return fp;
        char sFileName[260]={0};
        close(fp);
        fp=nullptr;
        int len = makelogpath(sFileName,256,date.data());
        if(len<0)
            return nullptr;
        m_nLogNumber++;
        m_nLogSize = 0;
        if(m_date!=date)
        {
            m_nLogNumber=1;
            m_date = date;
        }
        makelogname(&(sFileName[len]),256-len,date.data());
        return fopen(sFileName,"ab");
    }

    void writebatch(const std::string& date, std::list<std::string>& records)
    {
        if(m_nLogOut&LOG_OUT_TXT)
        {
            outputtxtbatch(date,records);
        }
        if(m_nLogOut&LOG_OUT_STD)
        {
            outputstdbatch(records);
        }
    }

    void outputtxtbatch(const std::string& date, std::list<std::string>& records)
    {
        FILE* fp = open(date);
        if(nullptr==fp) return ;
        for(auto& item : records)
        {
            fp = checkreopen(fp,date);
            if(nullptr==fp) return ;
            append(fp,item);
        }
        close(fp);
    }

    void outputstdbatch(std::list<std::string>& records)
    {
        std::string sMsg;
        int i=1;
        for(auto& item :records)
        {
            sMsg.append(item);
            if(0==i%10)
            {
                printf(sMsg.data());
                sMsg.clear();
            }
            i++;
        }
        if(false==sMsg.empty())
            printf(sMsg.data());
    }

    bool append(FILE* fp, const std::string& sMsg)
    {
        size_t size = sMsg.length();
        fwrite(sMsg.c_str(),size,1,fp);

        char * date = new char[size+1];
        memset(date,0,size+1);
        strncpy(date,sMsg.c_str(),size);
        for(size_t i = 0; i < size;++i)
        {
            if(date[i] == '\n')
                addLogRowNumber();
        }
        if(date != NULL) delete date;

        //m_nLogSize += size;

        return true;
    }

    void close(FILE* fp)
    {
        if(nullptr==fp) return ;
        fflush(fp);
        fseek(fp,0L,SEEK_END);
        m_nLogSize = ftell(fp);
        fclose(fp);
    }

    int createDir(const   char   *sPathName)
    {
        char dirName[256]={0};
        strncpy(dirName, sPathName,256);
        int len = strlen(dirName);
        if(dirName[len-1]!='/')
            strcat(dirName, "/");
        len = strlen(dirName);
        for(int i=1;i<len;i++)
        {
            if(dirName[i]=='/')
            {
                dirName[i]   =   0;
                if(access(dirName,0)!=0   )
                {
                    #ifndef __unix
                    if(-1==mkdir(dirName))
                    #else
                    if(-1==mkdir(dirName,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
                    #endif
                    {
                        return -1;
                    }
                }
                dirName[i] = '/';
            }
        }
        return 0;
    }

    int checkDirExist(const char* dir)
    {
        if (access(dir,0)!=0)
        {
            return createDir(dir);
        }
        return 0;
    }
#ifndef __unix
    std::pair<int,int> readLogNumber(const std::string& sPath, const std::string& sName)
    {
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char tmp[64]={0};
        makedate(tmp,sizeof(tmp),curtime);
        std::string sFile = sPath+"/"+std::string(tmp)+"/"
                +sName+"_"+std::string(tmp)+"_*.log";
        std::pair<int,int> numSize;
        numSize.first  = 1;
        numSize.second = 0;
        struct _finddata_t fdata;
        memset(&fdata,0,sizeof(fdata));
        intptr_t handle = _findfirst(sFile.data(),&fdata);
        if(-1==handle)
            return numSize;
        do{
            std::string fname(fdata.name);
            size_t nbegin = fname.find_last_of("_")+1;
            size_t nend = fname.find_last_of(".");
            fname = fname.substr(nbegin,nend-nbegin);
            int number = atoi(fname.data());
            if(numSize.first<=number)
            {
                numSize.first=number;
                numSize.second = fdata.size;
            }
            memset(&fdata,0,sizeof(fdata));
            if(0!=_findnext(handle,&fdata))
                break;
        }while(1);
        _findclose(handle);
        return numSize;
    }
#else
    std::pair<int,int> readLogNumber(const std::string& sPath, const std::string& sName)
    {
        timespec tp;
        tm curtime;
        makecurtime(curtime,tp);
        char tmp[64]={0};
        makedate(tmp,sizeof(tmp),curtime);
        std::string sDir  = sPath+"/"+std::string(tmp);
        std::string sFile = sName+"_"+std::string(tmp);
        sFile.append("_");
        std::pair<int,int> numSize;
        numSize.first  = 1;
        numSize.second = 0;
        DIR* dir = opendir(sDir.data());
        if(nullptr==dir) return numSize;
        struct dirent *d_ent;
        for(d_ent=readdir(dir);d_ent!=nullptr;d_ent=readdir(dir))
        {
            std::string fname(d_ent->d_name);
            if(0==fname.find(sFile))
            {
                size_t nbegin = fname.find_last_of("_")+1;
                size_t nend = fname.find_last_of(".");
                std::string tmp = fname.substr(nend);
                if(tmp!=".log")
                    continue;
                tmp = fname.substr(nbegin,nend-nbegin);
                int number = atoi(tmp.data());
                if(numSize.first<=number)
                {
                    numSize.first=number;
                    fname = sDir+ "/" +fname;
                    struct stat statbuf;
                    if(stat(fname.data(),&statbuf)==0)
                        numSize.second = statbuf.st_size;
                }
            }
        }
        closedir(dir);
        dir=nullptr;
        return numSize;
    }
#endif
public:
    static std::unordered_map<int,std::shared_ptr<TTestLog> > m_mapLogIns;
    static std::unordered_map<std::string, int> m_mapLogHandle;
    static int m_nLogSerialNo;
    static int m_majorLogHandle;
private:
    int         m_nLevel;
    std::string m_sLogPath;
    std::string m_sLogName;
    int         m_nLogIndex;

    tm          m_tmHourLine;
    int         m_nLogMaxSize;
    long        m_nLogSize;
    int         m_nLogNumber;
    int         m_nLogOut;
    int         m_nLogRowNumber;
    QReadWriteLock          m_rdwrLock;
    std::string m_date;
#ifdef USING_LOG_THREAD
    std::shared_ptr<TWriteLogThread> m_thread;
    friend class TWriteLogThread;
#endif
};

std::unordered_map<int,std::shared_ptr<TTestLog> > TTestLog::m_mapLogIns;
std::unordered_map<std::string, int> TTestLog::m_mapLogHandle;
int TTestLog::m_nLogSerialNo=0;
int TTestLog::m_majorLogHandle = 0;

#define LOG_FORMAT_STRING(level,handle,format) \
    if(TTestLog::m_mapLogIns.count(handle)<=0) return ; \
    if(level<TTestLog::m_mapLogIns[handle]->getLevel()) return ; \
    char message[LOG_LINE_MAXBYTE+1]={0}; \
    va_list ap; \
    va_start(ap, format); \
    int msglen = ::vsnprintf(message,LOG_LINE_MAXBYTE,format,ap); \
    va_end(ap); \
    if(msglen>LOG_LINE_MAXBYTE)\
        msglen=LOG_LINE_MAXBYTE;\
    TTestLog::m_mapLogIns[handle]->writeLogLen(level,message,msglen);

#define LOG_NORMAL_STRING(level,handle,message) \
    if(TTestLog::m_mapLogIns.count(handle)<=0) return ; \
    if(level<TTestLog::m_mapLogIns[handle]->getLevel()) return ; \
    TTestLog::m_mapLogIns[handle]->writeLog1(level,message);

#define LOG_STRING_LENGTH(level,handle,message,msglen) \
    if(TTestLog::m_mapLogIns.count(handle)<=0) return ; \
    if(level<TTestLog::m_mapLogIns[handle]->getLevel()) return ; \
    TTestLog::m_mapLogIns[handle]->writeLogLen(level,message,msglen);

void vLogLevel(int nLevel, const char *format,...)
{
    LOG_FORMAT_STRING(nLevel,TTestLog::m_majorLogHandle,format)
}

void vLogLevel(int nLevel, int nLogHandle, const char *format,...)
{
    LOG_FORMAT_STRING(nLevel,nLogHandle,format)
}

void vLogLevel(int nLevel, const char* sLogName, const char *format,...)
{
    int nLogHandle = 0;
    if(TTestLog::m_mapLogHandle.count(sLogName)>0)
        nLogHandle = TTestLog::m_mapLogHandle[sLogName];
    LOG_FORMAT_STRING(nLevel,nLogHandle,format)
}

void vLogDebug(const char *format,...)
{
    LOG_FORMAT_STRING(LL_DEBUG,TTestLog::m_majorLogHandle,format)
}

void vLogDebug2(int & row , QString &logName , const char *format,...)
{
    if(TTestLog::m_mapLogIns.count(TTestLog::m_majorLogHandle)<=0) return;
    if(LL_DEBUG < TTestLog::m_mapLogIns[TTestLog::m_majorLogHandle]->getLevel()) return;
    char message[LOG_LINE_MAXBYTE+1]={0};
    va_list ap;
    va_start(ap, format);
    int msglen = ::vsnprintf(message,LOG_LINE_MAXBYTE,format,ap);
    va_end(ap);
    if(msglen>LOG_LINE_MAXBYTE)
        msglen=LOG_LINE_MAXBYTE;
    TTestLog::m_mapLogIns[TTestLog::m_majorLogHandle]->writeLogLen(LL_DEBUG,message,msglen,row,logName);
}

void vLogInfo(const char *format,...)
{
    LOG_FORMAT_STRING(LL_INFO,TTestLog::m_majorLogHandle,format)
}
void vLogWarn(const char *format,...)
{
    LOG_FORMAT_STRING(LL_WARN,TTestLog::m_majorLogHandle,format)
}
void vLogError(const char *format,...)
{
    LOG_FORMAT_STRING(LL_ERROR,TTestLog::m_majorLogHandle,format)
}
void vLogFatal(const char *format,...)
{
    LOG_FORMAT_STRING(LL_FATAL,TTestLog::m_majorLogHandle,format)
}

void vLogLevel1(int nLevel, const char *message)
{
    LOG_NORMAL_STRING(nLevel,TTestLog::m_majorLogHandle,message)
}
void vLogDebug1(            const char *message)
{
    LOG_NORMAL_STRING(LL_DEBUG,TTestLog::m_majorLogHandle,message)
}
void vLogInfo1 (            const char *message)
{
    LOG_NORMAL_STRING(LL_INFO,TTestLog::m_majorLogHandle,message)
}
void vLogWarn1 (            const char *message)
{
    LOG_NORMAL_STRING(LL_WARN,TTestLog::m_majorLogHandle,message)
}
void vLogError1(            const char *message)
{
    LOG_NORMAL_STRING(LL_ERROR,TTestLog::m_majorLogHandle,message)
}
void vLogFatal1(            const char *message)
{
    LOG_NORMAL_STRING(LL_FATAL,TTestLog::m_majorLogHandle,message)
}

void vLogLevel1(int nLevel, int nLogHandle, const char *message)
{
    LOG_NORMAL_STRING(nLevel,nLogHandle,message)
}
void vLogLevel1(int nLevel, const char* sLogName, const char *message)
{
    int nLogHandle = 0;
    if(TTestLog::m_mapLogHandle.count(sLogName)<=0) return;
    nLogHandle = TTestLog::m_mapLogHandle[sLogName];
    LOG_NORMAL_STRING(nLevel,nLogHandle,message);
}

void vLogLevelLen(int nLevel, const char *message,    int msglen)
{
    LOG_STRING_LENGTH(nLevel,TTestLog::m_majorLogHandle,message,msglen)
}
void vLogLevelLen(int nLevel,        int  nLogHandle, const char *message, int msglen)
{
    LOG_STRING_LENGTH(nLevel,nLogHandle,message,msglen)
}

void vLogLevelLen(int nLevel, const char* sLogName,   const char *message, int msglen)
{
    if(TTestLog::m_mapLogHandle.count(sLogName)<=0) return;
    int nLogHandle = TTestLog::m_mapLogHandle[sLogName];
    vLogLevel(nLevel,nLogHandle,message,msglen);
}

int  initLogRecord(int nLevel, const char *sLogName,
                   const char *sLogPath, bool isMajor,
                   int nMaxSize, int nOutput)
{
    if(TTestLog::m_mapLogHandle.count(sLogName)>0)
    {
        int nLogIndex = TTestLog::m_mapLogHandle[sLogName];
        if(isMajor)
            TTestLog::m_majorLogHandle = nLogIndex;
        return nLogIndex;
    }
    int nLogIndex = TTestLog::m_nLogSerialNo;
    std::shared_ptr<TTestLog> log = std::make_shared<TTestLog>();
    if(nullptr==log) return -1;
    log->setLevel(nLevel);
    if(0!=log->setLogPath(sLogPath))
        return -1;
    log->setLogName(sLogName,nMaxSize);
    log->setLogOut(nOutput);
    log->setLogRowNumber();
    TTestLog::m_mapLogIns[nLogIndex] = log;
    TTestLog::m_mapLogHandle[sLogName] = nLogIndex;
    if(isMajor)
        TTestLog::m_majorLogHandle = nLogIndex;

    return nLogIndex;
}
void deinitLogRecord(int nLogHandle)
{
    if(-1==nLogHandle)
    {
        for(auto& item : TTestLog::m_mapLogIns)
            item.second->stop();
        TTestLog::m_mapLogIns.clear();
        TTestLog::m_mapLogHandle.clear();
        return ;
    }
    if(TTestLog::m_mapLogIns.count(nLogHandle)==0) return ;
    TTestLog::m_mapLogIns[nLogHandle]->stop();
    TTestLog::m_mapLogIns.erase(nLogHandle);
    if(TTestLog::m_mapLogIns.empty())
        TTestLog::m_mapLogHandle.clear();
}

void deinitLogRecordWithName(const char* sLogName)
{
    if(nullptr==sLogName)
    {
        deinitLogRecord(-1);
        return ;
    }
    int nLogHandle=0;
    if(TTestLog::m_mapLogHandle.count(sLogName)==0) return ;
    nLogHandle = TTestLog::m_mapLogHandle[sLogName];
    TTestLog::m_mapLogIns[nLogHandle]->stop();
    TTestLog::m_mapLogIns.erase(nLogHandle);
    TTestLog::m_mapLogHandle.erase(sLogName);
}

bool setMajorLogRecord(int nLogHandle)
{
    if(TTestLog::m_mapLogIns.count(nLogHandle)<=0) return false;
    TTestLog::m_majorLogHandle = nLogHandle;
    return true;
}

bool setMajorLogRecord(const char* sLogName)
{
    if(TTestLog::m_mapLogHandle.count(sLogName)<=0) return false;
    TTestLog::m_majorLogHandle = TTestLog::m_mapLogHandle[sLogName];
    return true;
}

int getLogRecordHandle(const char* sLogName)
{
    if(nullptr==sLogName)
        return TTestLog::m_majorLogHandle;
    if(TTestLog::m_mapLogHandle.count(sLogName)>0)
        return TTestLog::m_mapLogHandle[sLogName];
    return -1;
}


