#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "z_define.h"
#include "z_lock.h"
#include "z_thread.h"
#include "z_log.h"
#include "z_signal.h"


#define TAG ("THREAD")

typedef struct {
    /* 线程描述 */
    char name[32];              /* 线程名称 */

    /* 线程参数 */
    Z_THRD_PFN func;          /* 线程回调函数 */
    void *wparam;               /* 回调函数传参 */

    /*线程属性*/
    pthread_t id;               /* 线程ID */
    pid_t pid;                  /* PID */
    pid_t ppid;                 /* 父线程PID */

    uint32_t policy;            /* 调度策略*/
    uint32_t priority;          /* 优先级 */
    uint32_t stack_size;        /* 栈大小 */

    /* 线程控制属性 */
    uint8_t index;              /* 线程管理数组index, attention: 线程数大于MAX_THRD_NUM是无法直接使用index值*/
    uint8_t status;             /* 线程状态 */
    bool is_exit;               /* 表示当前线程是否退出 */

#if USE_THRD_POOL
    /* 线程池属性 */
    int seq;
    bool in_pool;               /* 是否位于线程池 */
    bool pool_wait;             /* 暂停标志 */
    HANDLE pool_signal;
#endif
} ZThread;

#if USE_THRD_POOL
#define MAX_THRD_NUM        (32)
#define MAX_POOL_THRD_NUM   (16)

static ZThread *mThrdList[MAX_THRD_NUM];
static int mThrdListSeq[MAX_THRD_NUM];

static int mCount = -1;
static ZMutex mPollMutex;
static int mMaxPoolThrd = MAX_POOL_THRD_NUM;
#endif

#if 0
static const char *_state2Name(int state) {
    switch(state) {
        case Z_THRD_STATE_IDLE :
            return "IDLE";
            break;
        case Z_THRD_STATE_PAUSE:
            return "PAUSE";
            break;
        case Z_THRD_STATE_RUNNING :
            return "RUNNING";
            break;
        case Z_THRD_STATE_READY:
            return "READY";
            break;
        case Z_THRD_STATE_ZOMBIE:
            return "ZOMBIE";
            break;
        default:
            return "INVALID";
    }
}
#endif

#if USE_THRD_POOL
static void _delThread(ZThread *thrd) {
    Z_VERIFY_RET_VOID(NULL == thrd);
    ZAutoMutex _l(mPollMutex);

    ZSignal_Destroy(thrd->pool_signal);

    if(thrd->index < MAX_THRD_NUM) {
        mThrdList[thrd->index] = NULL;
        mThrdListSeq[thrd->index] = 0;
    }
    mCount--;

    free(thrd);
}

static void _addThread(ZThread *thrd) {
    Z_VERIFY_RET_VOID(NULL == thrd);
    int i;
    
    ZAutoMutex _l(mPollMutex);

    do {
        // 超过最大数目,不加入索引表
        if(mCount >= MAX_THRD_NUM) {
            thrd->index = MAX_THRD_NUM + 1;
            Z_ERROR("Thread reach max number!\n");
            break;
        }

        /* 保存线程句柄 */
        for(i = 0; i < MAX_THRD_NUM; i++) {
            if(NULL == mThrdList[i]) {
                mThrdList[i] = thrd;
                mThrdListSeq[i] = thrd->seq;
                thrd->index = i;
                mCount++;
                break;
            }
        }

        if(mCount < mMaxPoolThrd) {
            break;
        }

        for(i = 0; i < MAX_THRD_NUM; i++) {
            if(mThrdList[i] && Z_THRD_STATE_READY == mThrdList[i]->status) {
                ZThrd_Destroy(mThrdList[i], 100);
            }
        }

        /* 释放分离线程的资源 */
        if(mCount > MAX_THRD_NUM - 2) {
            for(i = 0; i < MAX_THRD_NUM; i++) {
                if(mThrdList[i] && Z_THRD_STATE_ZOMBIE == mThrdList[i]->status) {
                    _delThread(mThrdList[i]);
                }
            }
        }
    }while(0);
}

static bool _attachThread(ZThread *thrd, const char *name, ZThrd_Attr *attr, const Z_THRD_PFN pfn, void *wparam) {
    thrd->func = pfn;
    thrd->wparam = wparam;

    if(name) {
        strcpy(thrd->name, name);
        pthread_setname_np(thrd->id, name);
    }

    if(NULL != attr) {
        if(attr->policy != thrd->policy) {
            return false;
        }

        if(attr->stack_size != thrd->stack_size) {
            return false;
        }

        if(attr->priority != thrd->priority) {
            if(0 != ZThrd_SetPriority(thrd, attr->priority)) {
                return false;
            }
        }
    }

    if(thrd->index < MAX_THRD_NUM) {
        thrd->seq = mThrdListSeq[thrd->index];
    }

    thrd->pool_wait = false;
    ZSignal_Post(thrd->pool_signal);

    return true;
}

static bool _detachThread(ZThread *thrd) {
    thrd->func = NULL;
    thrd->name[0] = '\0';
    thrd->pool_wait = true;

    if(thrd->index < MAX_THRD_NUM && mThrdListSeq[thrd->index] == thrd->seq) {
        mThrdListSeq[thrd->index]++;
    }

    return true;
}

static ZThread *_getIdleThread() {
    ZAutoMutex _l(mPollMutex);
    int i = 0;
    for(i = 0; i < MAX_THRD_NUM; i++) {
        if(NULL != mThrdList[i] && Z_THRD_STATE_READY == mThrdList[i]->status) {
            break;
        }
    }

    if(i >= MAX_THRD_NUM) {
        return NULL;
    }

    mThrdList[i]->status = Z_THRD_STATE_ATTACH;

    return mThrdList[i];
}
#endif

/*工作线程 */
static void *_wordThread(void *wparam) {
    Z_VERIFY_RET_VAL(NULL == wparam, NULL);
    ZThread *thrd = (ZThread *)wparam;
    uint64_t ret_code = 0;

    thrd->pid = getpid();
    thrd->ppid = getppid();

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);        /* 设置取消属性 */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);   /* 设置立即取消 */

    do {
#if USE_THRD_POOL
        if(thrd->pool_wait) {
            thrd->status = Z_THRD_STATE_READY;
            ZSignal_Wait(thrd->pool_signal, -1);
        }
#endif
        if(thrd->is_exit) {
            break;
        }

        thrd->status = Z_THRD_STATE_RUNNING;

        if(NULL != thrd->func) {
            ret_code = (uint64_t)thrd->func(thrd->wparam);
        }

#if USE_THRD_POOL
        if(thrd->in_pool) {
            _detachThread(thrd);
        }
#else
        thrd->is_exit = true;
#endif
        Z_DEBUG("del self-exit thrd, id:%u, name:[%s].\n", thrd->id, thrd->name);
    }while(!thrd->is_exit
#if USE_THRD_POOL
        && thrd->in_pool
#endif
        );

    thrd->status = Z_THRD_STATE_ZOMBIE;

    Z_DEBUG("Thread:[%s] Release self-exit ok!%d\n", thrd->name, ret_code);

    pthread_detach(pthread_self());
    pthread_exit((void *)&ret_code);
}

#if USE_THRD_POOL
/**
 * @brief 初始化线程池信息
 */
int ZThrd_Pool_Init(unsigned int max_threads) {
    mMaxPoolThrd = max_threads;
    return 0;
}

/**
 * @brief 销毁线程池, 池中所有线程强制停止
 */
void ZThrd_Pool_Uninit() {
    int i = 0;
    for(i = 0; i < MAX_THRD_NUM; i++) {
        if(mThrdList[i] != NULL) {
            ZThrd_Destroy(mThrdList[i], 100);
        }
    }

    mCount = -1;
}

/**
 * @brief 按名字查找线程
 *
 * @param name 名字
 *
 * @return 线程句柄
 */
HANDLE ZThrd_FindByName(char *name) {
    int i;
    for(i = 0; i < MAX_THRD_NUM; i++) {
        if(mThrdList[i] && !strcmp(name, mThrdList[i]->name)) {
            return mThrdList[i];
        }
    }
    return NULL;
}
#endif

/**
 * @brief 获取创建线程的默认属性 默认值:Z_THRD_SCHED_OTHER, Z_THRD_PRIORITY_NORMAL, Z_THRD_DEFAULT_STACK_SIZE
 * @param attr 属性句柄
 */
void ZThrd_GetDefAttr(ZThrd_Attr *attr) {
    if(NULL != attr) {
        attr->policy = Z_THRD_SCHED_OTHER;
        attr->priority = Z_THRD_DEFAULT_PRIORIOTY;
        attr->stack_size = Z_THRD_DEFAULT_STACK_SIZE;
    }
}

/**
 * @brief 销毁线程
 *
 * @param p_thrd 线程句柄
 * @param timeout 等待的时间(单位为毫秒)
 */
void ZThrd_Destroy_Direct(HANDLE p_thrd, int timeout /* ms */) {
    Z_VERIFY_RET_VOID(NULL == p_thrd);
    ZThread *thrd = (ZThread *)(p_thrd);
    int ret = 0;
    void *code = NULL;

    Z_DEBUG("[%s] pid:%d destroy ...\n", thrd->name, thrd->pid);

    /* 等待线程自己退出 */
    thrd->is_exit = true;
#if USE_THRD_POOL
    thrd->pool_wait = false;
    ZSignal_Post(thrd->pool_signal);
#endif
    while(Z_THRD_STATE_ZOMBIE != thrd->status && timeout > 0) {
        usleep(1 * 1000);
        timeout -= 1;
    }

    if(Z_THRD_STATE_ZOMBIE != thrd->status) {
        /* 强制退出 */
        if(!pthread_cancel(thrd->id)) {
            Z_ERROR("Thread<%s> is canneled !!!\n",  thrd->name);
        }
    }

    ret = pthread_join(thrd->id, &code);
    if(0 == ret) {
        if(PTHREAD_CANCELED == code) {
            Z_ERROR("Thread canceled!\n"); //线程被取消
        } else {
            Z_ERROR("ret:%lx\n", (uint64_t)code);
        }
    }
    
#if USE_THRD_POOL
    _delThread(thrd);
#else
    free(thrd);
#endif
}

/**
 * @brief 创建线程, 使用前,建议通过ZThrd_GetDefAttr获取优化过的属性; thrdattr为NULL时使用系统默认属性
 *
 * @param name      线程名
 * @param thrdattr  线程属性
 * @param thrd_pfn  线程函数指针
 * @param wParam    线程函数参数
 *
 * @return 成功,返回线程句柄;失败,返回空值
 */
HANDLE ZThrd_Create_Direct(const char *name, ZThrd_Attr *thrd_attr, const Z_THRD_PFN pfn, void *wparam) {
    Z_VERIFY_RET_VAL(NULL == pfn, NULL);
    int ret = 0;
    ZThread *thrd = NULL;
    pthread_attr_t attr;
    struct sched_param param;
    uint32_t max_priority;
    uint32_t min_priority;

    uint32_t policy = Z_THRD_SCHED_OTHER;
    uint32_t priority = Z_THRD_PRIORITY_NORMAL;
    size_t stack_size = Z_THRD_DEFAULT_STACK_SIZE;
    do {
        thrd = (ZThread *)malloc(sizeof(ZThread));
        if(NULL == thrd) {
            Z_ERROR("Malloc error:%s !\n", strerror(errno));
            ret = -1;
            break;
        }
        memset(thrd, 0, sizeof(ZThread));

        thrd->func = pfn;
        thrd->wparam = wparam;

#if USE_THRD_POOL
        thrd->pool_signal = ZSignal_Create(0, 1);
        if(NULL == thrd->pool_signal) {
            Z_ERROR("Create signal error!\n");
            ret = -1;
            break;
        }
        thrd->pool_wait = false;
        thrd->in_pool = false;
#endif
        thrd->status = Z_THRD_STATE_IDLE;

        /* 初始化线程属性对象 */
        pthread_attr_init(&attr);

        if(thrd_attr) {
            policy = thrd_attr->policy;
            priority = thrd_attr->priority;
            stack_size = thrd_attr->stack_size;

            /* 设置线程堆栈 */
            if(stack_size) {
                if(stack_size <  PTHREAD_STACK_MIN) {
                    stack_size = PTHREAD_STACK_MIN;
                }

                if(pthread_attr_setstacksize(&attr, stack_size) != 0) {
                    pthread_attr_getstacksize(&attr, &stack_size);
                    Z_ERROR("Set default stack size [%d] !\n", stack_size);
                }

                thrd->stack_size = stack_size;
            }

            /* 设置分离属性 */
            //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

            /* 设置调度策略*/
            thrd->priority = priority;
            switch(policy) {
                case Z_THRD_SCHED_OTHER:
                {
                    policy = SCHED_OTHER;
                }
                break;
                case Z_THRD_SCHED_RR:
                {
                    policy = SCHED_RR;
                }
                break;
                case Z_THRD_SCHED_FIFO:
                {
                    policy = SCHED_FIFO;
                }
                break;
                default:
                {
                    policy = SCHED_OTHER;
                }
            }

            pthread_attr_setschedpolicy(&attr, policy);
            thrd->policy = policy;

            /* 设置优先级 */
            /*在linux系统上, 线程优先级取值范围为1~99, 1最低, 99最高*/
            max_priority = sched_get_priority_max(policy);
            min_priority = sched_get_priority_min(policy);

            priority = (priority > max_priority) ? max_priority : priority;
            priority = (priority < min_priority) ? min_priority : priority;
            //priority = max_priority - (priority * (max_priority - min_priority)) / 255; //???

            /* SCHED_OTHER 不支持优先级 */
            if(SCHED_RR == policy || SCHED_FIFO == policy) {
                param.sched_priority = priority;
                //设置成实时策略后必须设定优先级，否则线程将创建失败
                pthread_attr_setschedparam(&attr, &param);
                //必须把继承属性修改为PTHREAD_EXPLICIT_SCHED才能有效,否则将继续继承父进程的策略
                pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
            }
        }

        /* 创建线程 */
        if(0 != pthread_create(&(thrd->id), &attr, _wordThread, (void *)thrd)) {
            Z_ERROR("Create Thread error:%s!\n", strerror(errno));
            ret = -1;
            break;
        }

        if(name) {
            strcpy(thrd->name, name);
            pthread_setname_np(thrd->id, name);
        }
    }while(0);

    pthread_attr_destroy(&attr);

    if(0 != ret && thrd) {
#if USE_THRD_POOL
        if(thrd->pool_signal) {
            ZSignal_Destroy(thrd->pool_signal);
        }
#endif
        free(thrd);
        thrd = NULL;

#if USE_THRD_POOL
    } else {
        _addThread(thrd);
        Z_DEBUG("Thread:[%s] id:%d pid:%u create ok.\n", thrd->name, thrd->id, thrd->pid);
#endif
    }

    return thrd;
}

/**
 * @brief 从线程池中获取线程,没有空闲则使用ZThrd_Create_Direct创建
 *
 * @param name      线程名
 * @param thrdattr  线程属性
 * @param thrd_pfn  线程函数指针
 * @param wParam    线程函数参数
 *
 * @return 成功,返回线程句柄;失败,返回空值
 */
HANDLE ZThrd_Create(const char *name, ZThrd_Attr *thrd_attr, const Z_THRD_PFN pfn, void *wparam) {
    Z_VERIFY_RET_VAL(NULL == pfn, NULL);
    ZThread *thrd = NULL;

#if USE_THRD_POOL
    /* 初始化全局线程索引表 */
    if(mCount <= 0) {
        memset(mThrdList, 0, sizeof(mThrdList));
        memset(mThrdListSeq, 0, sizeof(mThrdListSeq));
        mCount = 0;
    }

    /* 获取空闲线程 */
    thrd = _getIdleThread();
    if(NULL != thrd) {
        if(_attachThread(thrd, name, thrd_attr, pfn, wparam)) {
            Z_DEBUG("[%s] id:%d pid:%u create ok.\n", thrd->name, thrd->id, thrd->pid);
            return thrd;
        }
    }
#endif
    /* 没有则创建 */
    if(NULL == thrd_attr) {
        ZThrd_Attr def_attr;
        ZThrd_GetDefAttr(&def_attr);
        thrd_attr = &def_attr;
    }

    thrd = (ZThread *)ZThrd_Create_Direct(name, thrd_attr, pfn, wparam);
#if USE_THRD_POOL
    if(NULL != thrd) {
        thrd->in_pool = true;
    }
#endif
    return thrd;
}

/**
 * @brief 分离线程
 *
 * @param p_thrd 线程句柄
 * @param timeout 等待的时间(单位为毫秒)
 */
void ZThrd_Destroy(HANDLE p_thrd, int timeout /*ms*/) {
    Z_VERIFY_RET_VOID(p_thrd == NULL);
    ZThread *thrd = (ZThread *)(p_thrd);

#if USE_THRD_POOL
    if(thrd->index < MAX_THRD_NUM) {
        /* 线程已退出 */
        if(thrd->seq != mThrdListSeq[thrd->index]) {
            Z_DEBUG("del self-exit p_thrd<%s> ok. seq=%d list.seq=%d\n", thrd->name, thrd->seq , mThrdListSeq[thrd->index]);
            return;
        }

        while(Z_THRD_STATE_READY != thrd->status && timeout > 0) {
            usleep(1 * 1000);
            timeout -= 1;
        }

        if(Z_THRD_STATE_READY == thrd->status) {
            Z_DEBUG("del self-exit thrd, id:%u, name:[%s] ok.\n", thrd->id, thrd->name);
            return;
        }
    }
#endif
    ZThrd_Destroy_Direct(thrd, timeout);
}

/**
 * @brief 线程延迟
 *
 * @param timeout 超时时间(单位为毫秒)
 */
void ZThrd_Delay(unsigned int timeout /* ms */) {
    usleep(timeout * 1000);
}

/**
 * @brief 设置线程优先级
 *
 * @param p_thrd 线程句柄
 * @param priority 线程优先级的大小
 */
int ZThrd_SetPriority(HANDLE p_thrd, int priority) {
    Z_VERIFY_RET_VAL(NULL == p_thrd, -1);
    ZThread *thrd = (ZThread *)p_thrd;
    struct sched_param param;
    int policy;
    int max_priority;
    int min_priority;

    switch(thrd->policy) {
        case Z_THRD_SCHED_OTHER:
        {
            policy = SCHED_OTHER;
        }
        break;
        case Z_THRD_SCHED_RR:
        {
            policy = SCHED_RR;
        }
        break;
        case Z_THRD_SCHED_FIFO:
        {
            policy = SCHED_FIFO;
        }
        break;
        default:
        {
            policy = SCHED_OTHER;
        }
    }

    if(SCHED_OTHER == policy) {
        return -1;
    }

    /*在linux系统上, 线程优先级取值范围为1~99, 1最低, 99最高*/
    max_priority = sched_get_priority_max(policy);
    min_priority = sched_get_priority_min(policy);

    priority = (priority > max_priority) ? max_priority : priority;
    priority = (priority < min_priority) ? min_priority : priority;

    param.__sched_priority = priority;
    if(0 != pthread_setschedparam(thrd->id, policy, &param)) {
        Z_ERROR("set thread sched param error!\n");
        return -1;
    }

    thrd->priority = priority;
    return 0;
}

/**
 * @brief 取得线程优先级
 *
 * @param p_thrd 线程句柄
 *
 * @return 当前线程的优先级
 */
int ZThrd_GetPriority(HANDLE p_thrd) {
    Z_VERIFY_RET_VAL(NULL == p_thrd, 0);
    ZThread *thrd = (ZThread *)p_thrd;
    return thrd->priority;

}

/**
 * @brief 获取线程状态
 */
int ZThrd_GetStatus(HANDLE p_thrd) {
    Z_VERIFY_RET_VAL(NULL == p_thrd, Z_THRD_STATE_IDLE);
    ZThread *thrd = (ZThread *)p_thrd;

    return thrd->status;
}


