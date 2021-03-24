#ifndef Z_LOCK_H
#define Z_LOCK_H

#include <pthread.h>
/**
 * @description: 手动锁类，需要执行加锁解锁
 * @param {*}
 * @return {*}
 */
class ZMutex
{
public:
    ZMutex();
    ~ZMutex();
    int Lock();
    int TryLock();
    void UnLock();
    pthread_mutex_t *getMutex() {return &mMutex;}
private:
    pthread_mutex_t mMutex;
};


/**
 * @description:自动锁类，原理为构造函数加锁，析构函数解锁。使用方法就是在函数内部定义一个自动锁类即可，函数生命结束，自动调用析构解锁。 
 * @param {*}
 * @return {*}
 */
class ZAutoMutex
{
private:
    /* data */
    pthread_mutex_t *mMutex;
public:
    ZAutoMutex(ZMutex&);
    ZAutoMutex(pthread_mutex_t*);
    ~ZAutoMutex();
};

#endif

