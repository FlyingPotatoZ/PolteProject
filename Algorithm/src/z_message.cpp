/*
 * @Author: your name
 * @Date: 2021-01-12 11:20:57
 * @LastEditTime: 2021-01-14 14:59:54
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_message.cpp
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#include "z_define.h"
#include "z_message.h"
#include "z_log.h"
#include "z_common.h"

#define TAG ("Message")

typedef struct {
    unsigned int magic;
    unsigned int maxmsg;
    unsigned int msgsize;
    unsigned int refcnt;
    
    pthread_mutex_t mutex;
    pthread_cond_t read_cond;
    pthread_cond_t write_cond;
    
    unsigned int read, write;
    char msgbuff[0];
}ZMsgQue;

#define MSG_SIZE sizeof(ZMsg)
#define MAGIC (0x19951006)
#define IS_VALID_ID(x) ((x > 0) ? 1 : 0)

int ZMsgQue_Create(ZMsgQue_ID * const que_ID, const char *que_name, const unsigned int Max_msg_num){
    ZMsgQue *msg_que = NULL;
    if(NULL == que_ID){
        Z_ERROR("que_ID is NULL\n");
        return -1;
    }

    int mq_size = sizeof(ZMsgQue) + Max_msg_num * MSG_SIZE;
    *que_ID = -1;

    msg_que = (ZMsgQue*)malloc(mq_size);
    if(NULL == msg_que){
        Z_ERROR("molloc msg_que fail\n");
        return -1;
    }

    memset(msg_que, 0, mq_size);

    msg_que->magic = MAGIC;
    msg_que->maxmsg = Max_msg_num;
    msg_que->read = msg_que->write = msg_que->refcnt = 0;
    msg_que->msgsize = MSG_SIZE;
    pthread_mutex_init(&msg_que->mutex,NULL);

    pthread_condattr_t condattr;
    pthread_condattr_init(&condattr);
    pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC);

    pthread_cond_init(&msg_que->read_cond,&condattr);
    pthread_cond_init(&msg_que->write_cond,&condattr);

    *que_ID = (ZMsgQue_ID)msg_que;
    return 0;
}


int ZMsgQue_Destory(const ZMsgQue_ID que_ID){
    ZMsgQue *mq = NULL;
    if(!IS_VALID_ID(que_ID)){
        Z_ERROR("que_ID is invalid!\n");
        return -1;
    }

    mq = (ZMsgQue*)que_ID;
    if(MAGIC != mq->magic){
        Z_ERROR("magic is invalid\n");
        return -1;
    }
    mq->magic = 0;

    while (0 != mq->refcnt) {
        pthread_cond_broadcast(&mq->read_cond);
        pthread_cond_broadcast(&mq->write_cond);
    }

    pthread_cond_destroy(&mq->read_cond);
    pthread_cond_destroy(&mq->write_cond);
    pthread_mutex_destroy(&mq->mutex);

    /* free space */
    free(mq);
    mq = NULL;
    return 0;
}

int ZMsgQue_Send(const ZMsgQue_ID que_ID, const ZMsg *msg, const unsigned int time_ms){
    ZMsgQue *mq = NULL;
    if(!IS_VALID_ID(que_ID)){
        Z_ERROR("que_ID is invalid!\n");
        return -1;
    }

    struct timespec ts;
    char empty = 0;
    int ret = 0;

    mq = (ZMsgQue*)que_ID;
    if(MAGIC != mq->magic||NULL == msg||MSG_SIZE != mq->msgsize){
        Z_ERROR("quene param is invalid\n");
        return -1;
    }

    mq->refcnt++;

    pthread_mutex_lock(&mq->mutex);

    if(Z_TIMEOUT_INFINITY == time_ms){
        ZCommom_AddTimeSpecMS(&ts, time_ms);
    }

    if ((mq->read + mq->maxmsg - mq->write) % mq->maxmsg == 1) {
        if(Z_TIMEOUT_INFINITY == time_ms) {
            ret = pthread_cond_wait(&mq->write_cond, &mq->mutex);  //old wait error.
        } else if(Z_TIMEOUT_IMMEDIATE == time_ms) {
            ret = ETIMEDOUT;
        } else {
            ret = pthread_cond_timedwait(&mq->write_cond, &mq->mutex, &ts); //old wait error.
        }
        
        if ((ETIMEDOUT == ret) || (MAGIC != mq->magic)) {
            pthread_mutex_unlock(&mq->mutex);
            mq->refcnt--;
            Z_ERROR("error to send msg %ld,magic num is %lx,ret is %d>>>>>\n", mq->refcnt, mq->magic, ret);
            return -1;
        }
    }

    memcpy (mq->msgbuff + (mq->write)* (mq->msgsize), (void *)msg, MSG_SIZE);
    mq->write++;
    if (mq->write == mq->maxmsg) {
        mq->write = 0;
    }
    empty = (mq->write == mq->read);
    if (!empty) {
        pthread_cond_signal(&mq->read_cond);
    }
    pthread_mutex_unlock(&mq->mutex);

    mq->refcnt--;
    return 0;
}


int ZMsgQue_Receive(const ZMsgQue_ID que_ID, ZMsg *msg, const unsigned int time_ms){
    ZMsgQue *mq = NULL;
    if(!IS_VALID_ID(que_ID)){
        Z_ERROR("que_ID is invalid!\n");
        return -1;
    }

    struct timespec ts;
    bool isfull = false;
    int ret = 0;

    mq = (ZMsgQue*)que_ID;
    if(MAGIC != mq->magic||NULL == msg||MSG_SIZE != mq->msgsize){
        Z_ERROR("quene param is invalid\n");
        return -1;
    }
    
    mq->refcnt++;

    pthread_mutex_lock(&mq->mutex);

    if(Z_TIMEOUT_INFINITY != time_ms) {
        ZCommom_AddTimeSpecMS(&ts,time_ms);
    }
    
    if (mq->read == mq->write) {
        if(Z_TIMEOUT_INFINITY == time_ms) {
            ret = pthread_cond_wait(&mq->read_cond, &mq->mutex);
        } else if(Z_TIMEOUT_IMMEDIATE == time_ms) {
            ret = ETIMEDOUT;
        } else {
            ret = pthread_cond_timedwait(&mq->read_cond, &mq->mutex, &ts);
        }
        
        if ((ret == ETIMEDOUT) || (MAGIC != mq->magic)) {
            pthread_mutex_unlock(&mq->mutex);
            mq->refcnt--;
            return -1;
        }
    }
    
    memcpy(msg, mq->msgbuff + mq->read * mq->msgsize, /*mq->msgsize*/MSG_SIZE);
    mq->read++;
    if (mq->read == mq->maxmsg) {
        mq->read = 0;
    }
    
    isfull = ((mq->read + mq->maxmsg - mq->write) % mq->maxmsg == 1);
    if (!isfull) {
        pthread_cond_signal(&mq->write_cond);
    }
    
    pthread_mutex_unlock(&mq->mutex);

    mq->refcnt--;
    return 0;
}

