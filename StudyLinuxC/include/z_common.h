/*
 * @Author: your name
 * @Date: 2021-01-14 10:16:37
 * @LastEditTime: 2021-01-22 10:56:23
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_common.h
 */
#ifndef Z_COMMON_H
#define Z_COMMUN_H

/**
 * @description:时间+ 单位毫秒 
 * @param {structtimespec} *time
 * @param {unsignedint} add_ms
 * @return {*}
 */
void ZCommom_AddTimeSpecMS(struct timespec *time, unsigned int add_ms /* ms */);


/**
 * @description: 获取系统启动至今的时间, 单位:微秒
 * @param {*}
 * @return {*}
 */
unsigned long long ZCommom_GetSysUptimeUS();
#endif