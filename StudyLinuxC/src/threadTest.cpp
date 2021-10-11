/*
 * @Author: your name
 * @Date: 2021-06-09 16:14:48
 * @LastEditTime: 2021-09-02 17:33:22
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\StudyLinuxC\src\threadTest.cpp
 */

#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <iostream>


#include "z_log.h"
#include "z_define.h"

#define TAG ("ThreadDemo")

using namespace std;

void _printPids(const char* str){
    pid_t pid;
    pthread_t tid;

    pid=getpid();
    tid=pthread_self();
    Z_DEBUG("%s pid %u tid %u (0x%x)\n",str,(unsigned int)pid,(unsigned int)tid,(unsigned int)tid);
}

void* _thr_fn(void* arg){
    _printPids("new thread 0:");
    return (void*)0;
}

void* _thr_fn1(void* arg){
    _printPids("new thread 1:");
    return (void*)0;
}

static int zThreadcreatejoinDemo(){
    
    int err;

    pthread_t mpthread,mpthread1;
    void *mret;


    err = pthread_create(&mpthread,NULL,_thr_fn,NULL);
    if(0!=err){
        Z_ERROR("can`t create thread 0 :%s\n",strerror(err));
    }
    err = pthread_create(&mpthread1,NULL,_thr_fn1,NULL);
    if(0!=err){
        Z_ERROR("can`t create thread 1 :%s\n",strerror(err));
    }

    err = pthread_join(mpthread,&mret);
    if(0!=err){
        Z_ERROR("can`t join thread 0 :%s\n",strerror(err));
    }
    Z_DEBUG("thread 0 exit code %d\n",mret);
    
    err = pthread_join(mpthread1,&mret);
    if(0!=err){
        Z_ERROR("can`t join thread 1 :%s\n",strerror(err));
    }
    Z_DEBUG("thread 1 exit code %d\n",mret);

    _printPids("main thread");
    sleep(1);
    return 0;
}

#define NHASH 29
#define HASH(fp) (((unsigned long)fp)%NHASH)


typedef struct foo{
    int f_count;
    pthread_mutex_t f_lock;
    foo* f_next;
    int f_id;
}Foo;

Foo *fh[NHASH];
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

Foo *foo_alloc(){
    Foo *fp;
    if((fp=(Foo*)malloc(sizeof(Foo))) != NULL){
        fp->f_count=1;
        if(pthread_mutex_init(&fp->f_lock,NULL)!=0){
            free(fp);
            return NULL;
        }
    }  
    return fp; 
}

Foo *foo_alloc2(){
    Foo *fp;
    int idx;
    if((fp=(Foo*)malloc(sizeof(Foo))) != NULL){
        fp->f_count=1;
        if(pthread_mutex_init(&fp->f_lock,NULL)!=0){
            free(fp);
            return NULL;
        }
        idx= HASH(fp);
        pthread_mutex_lock(&hashlock);
        fp->f_next=fh[idx];
        fh[idx]=fp->f_next;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);
    }  
    return fp; 
}

void foo_hold(Foo* fp){
    pthread_mutex_lock(&(fp->f_lock));
    fp->f_count++;
    pthread_mutex_unlock(&(fp->f_lock));
}

void foo_hold2(Foo* fp){
    pthread_mutex_lock(&hashlock);
    fp->f_count++;
    pthread_mutex_unlock(&hashlock);
}

Foo * foo_find(int id){
    Foo *fp;
    int idx;
    idx = HASH(fp);
    pthread_mutex_lock(&hashlock);
    for(fp = fh[idx];fp!=NULL;fp=fp->f_next){
        if(fp->f_id==id){
            fp->f_count++;
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}

void foo_rele(Foo* fp){
    pthread_mutex_lock(&(fp->f_lock));
    if(--fp->f_count==0){
        pthread_mutex_unlock(&(fp->f_lock));
        pthread_mutex_destroy(&(fp->f_lock));
        free(fp);
    }else{
        pthread_mutex_unlock(&(fp->f_lock));
    }   
}

void foo_rele2(Foo* fp){
    Foo *t_fp;
    int idx;
    pthread_mutex_lock(&hashlock);

    if(--fp->f_count==0){
        idx = HASH(fp);
        t_fp = fh[idx];
        if(t_fp == fp){
            fh[idx]=fp->f_next;
        }else{
            while(t_fp->f_next!=fp) t_fp=t_fp->f_next;
            t_fp->f_next=fp->f_next;
        }

        pthread_mutex_unlock(&hashlock);
        pthread_mutex_destroy(&(fp->f_lock));
        free(fp);
    }else{
        pthread_mutex_unlock(&hashlock);
    }  
}



int zThreadDemoMain(){
    Foo *fp = foo_alloc2();
    foo_hold2(fp);
    Foo *fp2 = foo_find(0);
    foo_rele2(fp);
    if(fp2 != NULL){
        foo_rele2(fp2);
    }
    

}