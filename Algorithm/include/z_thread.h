/*
 * @Author: your name
 * @Date: 2021-01-14 15:23:43
 * @LastEditTime: 2021-01-21 15:42:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_thread.h
 */
#ifndef Z_THREAD_H
#define Z_THREAD_H
//是否启用线程池 
#define USE_THRD_POOL (0) 

/* 线程调度策略 */
enum ZThrd_Sched{
    Z_THRD_SCHED_OTHER,   /* 分时调度策略, 系统默认 */
    Z_THRD_SCHED_FIFO,    /* 实时调度策略, 先到先服务 */
    Z_THRD_SCHED_RR,      /* 实时调度策略, 时间片轮转 */
};

/* 推荐线程堆栈大小(字节) */
#define Z_THRD_DEFAULT_STACK_SIZE (24 * 1024)  /* 在linux中这个值是最小的 */

/* 只对实时调度策略有效 */
/* 在linux系统上, 线程优先级取值范围为1~99, 1最低, 99最高 */
#define Z_THRD_DEFAULT_PRIORIOTY  (64) /* 推荐线程优先级 */

#define Z_THRD_PRIORITY_LOWEST    (Z_THRD_DEFAULT_PRIORIOTY - 24)   /*!<最低的优先级*/
#define Z_THRD_PRIORITY_LOW       (Z_THRD_DEFAULT_PRIORIOTY - 12)   /*!<低的优先级*/
#define Z_THRD_PRIORITY_NORMAL    (Z_THRD_DEFAULT_PRIORIOTY)        /*!<普通的优先级*/
#define Z_THRD_PRIORITY_HIGH      (Z_THRD_DEFAULT_PRIORIOTY + 12)   /*!<高的优先级*/
#define Z_THRD_PRIORITY_HIGHEST   (Z_THRD_DEFAULT_PRIORIOTY + 24)   /*!<最高的优先级*/

/* 线程属性 */
typedef struct {
    unsigned int priority;   /* 调度策略 */
    unsigned int policy;     /* 优先级 */
    unsigned int stack_size; /* 堆栈的大小 */
} ZThrd_Attr;

/* 线程状态 */
enum ZThrd_State {
    Z_THRD_STATE_IDLE,         /* 空闲 */
    Z_THRD_STATE_PAUSE,        /* 暂停,休眠 */
    Z_THRD_STATE_RUNNING,      /* 正在运行 */
    Z_THRD_STATE_ATTACH,
    Z_THRD_STATE_READY,        /* 就绪 */
    Z_THRD_STATE_ZOMBIE,       /* 僵尸 */
};

/**
* @brief  线程执行体的函数指针
*
* @param *sk_thrd_pfn_t 指向回调函数的指针
* @param wparam 回调函数的第一个参数
*
* @return 线程返回值
*/
typedef void*(*Z_THRD_PFN)(void *wparam);

#if USE_THRD_POOL
/**
 * @brief 初始化线程池信息
 */
int ZThrd_Pool_Init(unsigned int max_threads);

/**
 * @brief 销毁线程池, 池中所有线程强制停止
 */
void ZThrd_Pool_Uninit();

/**
 * @brief 按名字查找线程
 *
 * @param name 名字
 * @return 线程句柄
 */
HANDLE ZThrd_FindByName(char *name);
#endif

/**
 * @brief 获取创建线程的默认属性 默认值:Z_THRD_SCHED_OTHER, Z_THRD_PRIORITY_NORMAL, Z_THRD_DEFAULT_STACK_SIZE
 * @param attr  属性句柄
 */
void ZThrd_GetDefAttr(ZThrd_Attr *attr);

/**
 * @brief 创建线程, 使用前,建议通过ZThrd_GetDefAttr获取优化过的属性; attr为NULL时使用系统默认属性
 *
 * @param name      线程名
 * @param thrd_attr      线程属性
 * @param pfn       线程函数指针
 * @param wParam    线程函数参数
 *
 * @return 成功,返回线程句柄;失败,返回空值
 */
HANDLE ZThrd_Create_Direct(const char *name, ZThrd_Attr *thrd_attr, const Z_THRD_PFN pfn, void *wparam);

/**
 * @brief 销毁线程
 *
 * @param p_thrd 线程句柄
 * @param timeout 等待的时间(单位为毫秒)
 */
void ZThrd_Destroy_Direct(HANDLE p_thrd, int timeout /* ms */);

/**
 * @brief 从线程池中获取线程,没有空闲则使用sk_thrd_create_ex创建
 *
 * @param name      线程名
 * @param thrd_attr  线程属性
 * @param pfn  线程函数指针
 * @param wParam    线程函数参数
 *
 * @return 成功,返回线程句柄;失败,返回空值
 */
HANDLE ZThrd_Create(const char *name, ZThrd_Attr *thrd_attr, const Z_THRD_PFN pfn, void *wparam);

/**
 * @brief 分离线程
 *
 * @param p_thrd 线程句柄
 * @param timeout 等待的时间(单位为毫秒)
 */
void ZThrd_Destroy(HANDLE p_thrd, int timeout /*ms*/);

/**
 * @brief 线程延迟
 *
 * @param timeout 超时时间(单位为毫秒)
 */
void ZThrd_Delay(unsigned int timeout /* ms */);

/**
 * @brief 设置线程优先级
 *
 * @param p_thrd 线程句柄
 * @param priority 线程优先级的大小
 */
int ZThrd_SetPriority(HANDLE p_thrd, int priority);

/**
 * @brief 取得线程优先级
 *
 * @param p_thrd 线程句柄
 * @return 当前线程的优先级
 */
int ZThrd_GetPriority(HANDLE p_thrd);

/**
 * @brief 获取线程状态
 */
int ZThrd_GetStatus(HANDLE p_thrd);

#endif