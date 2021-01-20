/*
 * @Author: your name
 * @Date: 2021-01-20 15:45:05
 * @LastEditTime: 2021-01-20 17:53:58
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

    sem_destory((sem_t*)signal);
    free(signal);
}

