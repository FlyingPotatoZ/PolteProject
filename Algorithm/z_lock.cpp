/*
 * @Author: your name
 * @Date: 2021-01-05 10:56:02
 * @LastEditTime: 2021-01-05 11:26:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_lock.cpp
 */
#include "z_lock.h"

#define TAG ("LOCK")

ZMutex::ZMutex(){
    pthread_mutex_init(&mMutex, NULL);
}

ZMutex::~ZMutex(){
    pthread_mutex_destroy(&mMutex);
}

int ZMutex::Lock(){
    return pthread_mutex_lock(&mMutex);
}

int ZMutex::TryLock(){
    return pthread_mutex_trylock(&mMutex);
}

void ZMutex::UnLock(){
    pthread_mutex_unlock(&mMutex);
}


ZAutoMutex::ZAutoMutex(ZMutex &mLock){
    mMutex = mLock.getMutex();
    pthread_mutex_lock(mMutex);
}

ZAutoMutex::ZAutoMutex(pthread_mutex_t *pt):mMutex(pt){
    pthread_mutex_lock(mMutex);
}

ZAutoMutex::~ZAutoMutex(){
    pthread_mutex_unlock(mMutex);
}