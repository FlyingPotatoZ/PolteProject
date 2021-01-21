/*
 * 
 * 　　┏┓　　　┏┓+ +
 * 　┏┛┻━━━┛┻┓ + +
 * 　┃　　　　　　　┃ 　
 * 　┃　　　━　　　┃ ++ + + +
 *  ████━████ ┃+
 * 　┃　　　　　　　┃ +
 * 　┃　　　┻　　　┃
 * 　┃　　　　　　　┃ + +
 * 　┗━┓　　　┏━┛
 * 　　　┃　　　┃　　　　　　　　　　　
 * 　　　┃　　　┃ + + + +
 * 　　　┃　　　┃
 * 　　　┃　　　┃ +  神兽保佑
 * 　　　┃　　　┃    代码无bug　　
 * 　　　┃　　　┃　　+　　　　　　　　　
 * 　　　┃　 　　┗━━━┓ + +
 * 　　　┃ 　　　　　　　┣┓
 * 　　　┃ 　　　　　　　┏┛
 * 　　　┗┓┓┏━┳┓┏┛ + + + +
 * 　　　　┃┫┫　┃┫┫
 * 　　　　┗┻┛　┗┻┛+ + + +
 * 
 * 
 * @Author: your name
 * @Date: 2020-12-24 11:20:54
 * @LastEditTime: 2021-01-04 16:38:00
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_log.cpp
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include "z_define.h"
#include "z_log.h"


static Z_LogLevel mLogLevel = Z_LOG_DEBUG;

void zLog_setLogLevel(Z_LogLevel level){
    mLogLevel = level;
}

Z_LogLevel zLog_getLogLevel(){
    return mLogLevel;
}

void zLog_PrintLog(Z_LogLevel level,const char* tag, const char* fmt, ...){
    if(level < mLogLevel){
        return ;
    }

    char buf[1024] = {0};//存放打印信息
    char timer[32] = {0};//存放时间信息
    char level_tag[16] = {0};//显示打印级别

    unsigned int end_pos = 0;

    va_list ap;
    va_start(ap,fmt);
    vsnprintf(buf,sizeof(buf),fmt,ap);
    va_end(ap);

    switch (level)
    {
    case Z_LOG_TRACE:
        sprintf(level_tag,"ALL");
        break;
    case Z_LOG_INFO:
        sprintf(level_tag,"INFO");
        break;
    case Z_LOG_DEBUG:
        sprintf(level_tag,"DEBUG");
        break;
    case Z_LOG_WARN:
        sprintf(level_tag,"WARN");
        break;
    case Z_LOG_ERROR:
        sprintf(level_tag,"ERROR");
        break;
    case Z_LOG_FATAL:
        sprintf(level_tag,"FATAL");
        break;
    default:
        break;
    }

    struct tm *timebuf, tmbuf;
    time_t tsec = time(0);
    timebuf = localtime_r(&tsec,&tmbuf);//Linux下专用，线程安全
    int ret = snprintf(timer, sizeof(timer), "%04d-%02d-%02d %02d:%02d:%02d", 
        1900+timebuf->tm_year, timebuf->tm_mon + 1, timebuf->tm_mday, timebuf->tm_hour, timebuf->tm_min, timebuf->tm_sec);
    if(ret < 0)
    {
        memset(timer, 0, sizeof(timer));
    }

    if(NULL == strchr(buf, '\n')){
        end_pos = strlen(buf);
        end_pos = end_pos > (sizeof(buf) - 2) ? (sizeof(buf) - 2) : end_pos;
        buf[end_pos] = '\n';
        buf[end_pos + 1] = '\0';
    }

    printf("[%s] %s [%s] %s", tag, timer, level_tag, buf);

}

