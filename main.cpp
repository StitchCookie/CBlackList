#include <QCoreApplication>
#include <HTTPServer/listen.h>
#include "apihandler.h"
#include "pub/systemconfig.h"
#include <execinfo.h>
#include <signal.h>
#include <fcntl.h>
#include <mainmutual.h>

void dump(int signo)
{
    void *array[10];
    size_t size;
    char* *strings;
    size_t i;

    size = backtrace (array,10);
    strings = backtrace_symbols (array,size);

    char t_now[24] = {0};
    char szOut[500] = {0};

    memset(t_now, 0, sizeof(t_now));
    struct tm *systime;
    time_t now;
    now = time(NULL);
    systime = localtime(&now);
    snprintf(t_now,sizeof(t_now),"%d-%02d-%02d %02d:%02d:%02d",systime->tm_year+1900,systime->tm_mon+1,systime->tm_mday,systime->tm_hour,systime->tm_min,systime
             ->tm_sec);

    sprintf(szOut,"********New Segment OCCUR********\n%s\n*********************************\n",t_now);
    //创建文件
    FILE *p_LogFile = NULL;

    if ((p_LogFile = fopen("./segfault.log","a+")) != NULL)
    {
        fwrite((void *)szOut,strlen(szOut),1,p_LogFile);

        for (i=0; i<size; i++)
        {
            fwrite((void *)strings[i],strlen(strings[i]),1,p_LogFile);
            fwrite((void *)"\n",strlen("\n"),1,p_LogFile);
        }
        fwrite((void *)"\n",strlen("\n"),1,p_LogFile);
        fclose(p_LogFile);
    }

    free(strings);

    printf("程序发生崩溃,系统退出.\n");
    if(signo==-1)
        signo=0;
    exit(1);
}

int CheckOnly()
{
    const char filename[]  = "/root/Desktop/BlackLock1";
    int fd = open (filename, O_WRONLY | O_CREAT, 0644);
    int flock = lockf(fd, F_TLOCK, 0);

    if (fd == -1)
    {
        perror("open lockfile\n");
        return 0;
    }

    //给文件加锁
    if (flock == -1)
    {
        perror("lock file error\n");
        return 0;
    }

    //程序退出后，文件自动解锁
    return 1;
}

BaseInfo m_baseInfo;
QQueue<LineGrantrySQLData> m_qqueueData;
int main(int argc, char *argv[])
{
    if (!CheckOnly())
        return -1;
    QCoreApplication a(argc, argv);
    signal(SIGSEGV,&dump);
    if(initLogRecord(LL_DEBUG, "checkBlackLog","./checkBlackLog/",true) < 0)
    {
        qDebug() << "系统系统初始化失败";
    }
    if(!SystemConfig::getInstance()->set_Mysql_info(m_baseInfo)
            ||!SystemConfig::getInstance()->set_HttpRequest_info(m_baseInfo)
            ||!SystemConfig::getInstance()->set_HttpResponse_info(m_baseInfo)
            ||!SystemConfig::getInstance()->set_Redis_info(m_baseInfo)
            ||!SystemConfig::getInstance()->setLanebaseInfo(m_baseInfo))
    {
        qDebug()<<m_baseInfo.grantrayMysqlConfig.ip;
        vLogDebug("系统配置文件读取失败--程序退出");
        qDebug("系统配置文件读取失败--程序退出");
        return 0;
    }
    vLogDebug("系统配置文件读取成功");
    qDebug("系统配置文件读取成功");
    //HttpStartListen::getInstance().start(6666,apihandler::getInstance().handler);
    sqlRequestHandle *sqlDatabase = new sqlRequestHandle();
    HttpStartListen::getInstance().start(m_baseInfo.cBLackResponse.responsePort,apihandler::getInstance().handler);
    return a.exec();
}
