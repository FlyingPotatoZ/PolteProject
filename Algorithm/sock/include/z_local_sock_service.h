/*
 * @Author: your name
 * @Date: 2021-02-04 22:13:48
 * @LastEditTime: 2021-02-05 17:58:03
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\sock\include\z_local_sock_service.h
 */
#ifndef Z_LOCAL_SOCK_SERVICE_H
#define Z_LOCAL_SOCK_SERVICE_H

/**
 * @description: 启动server监听线程
 * @param {*}
 * @return {*}
 */
int zSockd_LooperThread();

/**
 * @description:服务的监听使能 
 * @param {bool} enable
 * @return {*}
 */
void zSockd_setReceiveEnable(bool enable);

/**
 * @description:服务端工作线程，通过回调函数执行具体方法 
 * @param {int} socket
 * @param {char} *buf
 * @param {void} *work_cb
 * @param {void} *param
 * @return {*}
 */
int zSock_WorkProc(int socket, char *buf, void *work_cb, void *param);
#endif