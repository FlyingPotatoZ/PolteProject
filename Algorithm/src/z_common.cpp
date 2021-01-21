/*
 * @Author: your name
 * @Date: 2021-01-14 10:19:47
 * @LastEditTime: 2021-01-21 09:29:35
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_common.cpp
 */
#include <stdio.h>
#include <time.h>

#include "z_define.h"
#include "z_common.h"


void ZCommom_AddTimeSpecMS(struct timespec *time, unsigned int add_ms /* ms */) {
    Z_VERIFY_RET_VOID(NULL == time);

    clock_gettime(CLOCK_MONOTONIC, time);

    //printf("CLOCK_MONOTONIC tv_sec:%ld, tv_nsec:%ld\n", time->tv_sec, time->tv_nsec);
    time->tv_sec = time->tv_sec + (add_ms / 1000);
    time->tv_nsec = (time->tv_nsec + ((add_ms) % 1000) * 1000000) % 1000000000;
}

unsigned long long ZCommom_GetSysUptimeUS(){
    struct timespec times = {0, 0};

    clock_gettime(CLOCK_MONOTONIC, &times);
    return times.tv_sec * 1000000 + times.tv_nsec / 1000;
}