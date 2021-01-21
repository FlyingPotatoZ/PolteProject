/*
 * @Author: your name
 * @Date: 2021-01-20 15:45:05
 * @LastEditTime: 2021-01-21 09:57:15
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\z_signal.cpp
 */
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#include <algorithm>


#include "z_define.h"
#include "z_common.h"
#include "z_log.h"
#include "z_signal.h"

#define TAG ("Signal")

typedef struct{
    sem_t sem;
    int max_val;
}ZSignal;

HANDLE ZSignal_Create(int defval, int maxval){
    ZSignal *H = (ZSignal*)malloc(sizeof(ZSignal));
    if(NULL == H){
        Z_ERROR("malloc ZSignal error!\n");
        return NULL;
    }
    sem_init(&(H->sem), 0, defval);
    H->max_val = maxval;
    return H;
}

void ZSignal_Destroy(HANDLE signal){
    Z_VERIFY_RET_VOID(NULL == signal);

    sem_destroy((sem_t*)signal);
    free(signal);
}

int ZSignal_Wait(HANDLE signal, int timeout){
    Z_VERIFY_RET_VAL(NULL == signal, -1);
    int ret = 0;

    if(timeout < 0){
        sem_wait((sem_t *)signal);
    }else{
        //通过使用sem_trywait来规避sem_timedwait使用绝对时间的缺陷
        
        // 延时时间由毫秒转换为微秒
        const size_t timeoutUs = timeout * 1000; 
        // 每次最大的睡眠的时间为10000微秒(10毫秒)
        const size_t maxTimeWait = 10000; 

        // 睡眠时间，默认为1微秒
        size_t timeWait = 1; 
        // 剩余需要延时睡眠时间
        size_t delayUs = 0; 

        // 循环前的开始时间，单位微秒
        const uint64_t startUs = ZCommom_GetSysUptimeUS(); 
        // 过期时间，单位微秒
        uint64_t elapsedUs = 0;

        do{
            // 如果信号量大于0，则减少信号量并立刻退出循环
            if(0 == sem_trywait((sem_t *)signal)) {
                ret = 0;
                break;
            }
            // 系统信号立刻返回-1
            if(EAGAIN != errno) {
                ret = -1;
                break;
            }
            // delayUs一定是大于等于0的，因为do-while的条件是elapsedUs <= timeoutUs.
            delayUs = timeoutUs - elapsedUs;

            // 睡眠时间取最小的值
            timeWait = std::min(delayUs, timeWait);

            // 进行睡眠 单位是微秒
            ret = usleep(timeWait);
            if(0 != ret) {
                return -1;
            }

            // 每次睡眠延时时间双倍自增
            timeWait *= 2;

            // 每次睡眠延时时间不能超过最大值
            timeWait = std::min(timeWait, maxTimeWait);

            // 计算开始时间到现在的运行时间 单位是微秒
            elapsedUs = ZCommom_GetSysUptimeUS() - startUs;
        }while(elapsedUs <= timeoutUs);
        // 如果当前循环的时间超过预设延时时间则循环退出, 且设置返回值为-1
        if(elapsedUs > timeoutUs) {
            ret = -1;
        }

    }
    return ret;
}


void ZSignal_Post(HANDLE signal){
    Z_VERIFY_RET_VOID(NULL == signal);
    int val;
    sem_getvalue((sem_t *)signal, &val);
    if(val < ((ZSignal *)signal)->max_val) {
        sem_post((sem_t *)signal);
    }
}


void ZSignal_Reset(HANDLE signal){
    ZSignal_Post(signal);
}