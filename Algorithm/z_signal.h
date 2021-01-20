/*
 * @Author: your name
 * @Date: 2021-01-20 15:45:01
 * @LastEditTime: 2021-01-20 17:01:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\z_signal.h
 */
#ifndef Z_SIGNAL_H
#define Z_SIGNAL_H

/**
 * @brief 创建信号量
 *
 * @param defval 取值为0
 * @param maxval 取值为1
 *
 * @return 成功,返回信号量句柄; 否则,返回空值
 */
HANDLE ZSignal_Create(int defval, int maxval);

/**
 * @brief 销毁信号量
 *
 * @param signal 信号量句柄
 */
void ZSignal_Destroy(HANDLE signal);

/**
 * @brief 等待信号量
 *
 * @param signal 信号量句柄
 * @param timeout -1表示无限等待;其他值表示等待的时间(ms)
 *
 * @return 成功,返回0; 否则,返回-1
 */
int ZSignal_Wait(HANDLE signal, int timeout);

/**
 * @brief 释放信号量
 *
 * @param signal 信号量句柄
 */
void ZSignal_Post(HANDLE signal);

/**
 * @brief 重置信号量
 *
 * @param signal 信号量句柄
 */
void ZSignal_Reset(HANDLE signal);
#endif