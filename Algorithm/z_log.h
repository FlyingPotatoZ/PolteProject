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
 * @Date: 2021-01-04 10:15:29
 * @LastEditTime: 2021-01-04 16:32:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_log.h
 */

#ifndef Z_LOG_H
#define Z_LOG_H

enum Z_LogLevel {
    Z_LOG_ALL = 0,
    Z_LOG_TRACE,
    Z_LOG_INFO,
    Z_LOG_DEBUG,
    Z_LOG_WARN,
    Z_LOG_ERROR,
    Z_LOG_FATAL
};

/**
 * @description:设置日志打印级别 
 * @param {*}
 * @return {*}
 */
void zLog_setLogLevel(Z_LogLevel level);

/**
 * @description:获取日志打印级别 
 * @param {*}
 * @return {*}
 */
Z_LogLevel zLog_getLogLevel();

/**
 * @description:执行打印 
 * @param {*}
 * @return {*}
 */
void zLog_PrintLog(Z_LogLevel level,const char* tag, const char* fmt, ...);

#define Z_LOG_T(level,tag,fmt,...) zLog_PrintLog(level,tag,fmt,##__VA_ARGS__)

#define Z_TRACE_T(tag,fmt,...) Z_LOG_T(Z_LOG_TRACE,tag,fmt,##__VA_ARGS__)
#define Z_INFO_T(tag,fmt,...) Z_LOG_T(Z_LOG_INFO,tag,fmt,##__VA_ARGS__)
#define Z_DEBUG_T(tag,fmt,...) Z_LOG_T(Z_LOG_DEBUG,tag,fmt,##__VA_ARGS__)
#define Z_WARN_T(tag,fmt,...) Z_LOG_T(Z_LOG_WARN,tag,fmt,##__VA_ARGS__)
#define Z_ERROR_T(tag,fmt,...) Z_LOG_T(Z_LOG_ERROR,tag,fmt,##__VA_ARGS__)
#define Z_FATAL_T(tag,fmt,...) Z_LOG_T(Z_LOG_FATAL,tag,fmt,##__VA_ARGS__)


#define Z_TRACE(fmt,...) Z_TRACE_T(TAG,fmt,##__VA_ARGS__)
#define Z_INFO(fmt,...) Z_INFO_T(TAG,fmt,##__VA_ARGS__)
#define Z_DEBUG(fmt,...) Z_DEBUG_T(TAG,fmt,##__VA_ARGS__)
#define Z_WARN(fmt,...) Z_WARN_T(TAG,fmt,##__VA_ARGS__)
#define Z_ERROR(fmt,...) Z_ERROR_T(TAG,fmt,##__VA_ARGS__)
#define Z_FATAL(fmt,...) Z_FATAL_T(TAG,fmt,##__VA_ARGS__)


#endif




