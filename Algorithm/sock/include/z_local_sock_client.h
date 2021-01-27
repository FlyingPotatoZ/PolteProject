/*
 * @Author: your name
 * @Date: 2021-01-22 10:48:10
 * @LastEditTime: 2021-01-26 11:21:12
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\sock\z_local_sock_client.h
 */
#ifndef Z_LOCAL_SOCK_CLIENT_H
#define Z_LOCAL_SOCK_CLIENT_H


typedef enum{
    READ_ERR = -7,
    WRITE_ERR,
    CLOSE_ERR,
    NO_LINUX_MAKE_ADDRUN_ERROR,
    LINUX_MAKE_ADDRUN_ERROR,
    CONNECT_ERROR,
    CREATE_ERROR,
    NO_ERROR,
}ZSockError;

#ifdef __cplusplus
extern "C"{
#endif



/**
 * @description:本地 sock客户端
 * @param {*}
 * @return {*}
 */
int zSock_localSockSendClient(const char *cmd);


#ifdef __cplusplus
}
#endif

#endif