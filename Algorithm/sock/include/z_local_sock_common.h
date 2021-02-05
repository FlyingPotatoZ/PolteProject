/*
 * @Author: your name
 * @Date: 2021-02-05 17:16:41
 * @LastEditTime: 2021-02-05 17:44:28
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\sock\include\z_local_sock_common.h
 */


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

/**
 * @description: 从socket文件中读数据
 * @param {int} fd
 * @param {void} *buffer
 * @param {int} length
 * @return {*}
 */
int zSockCommon_socketReadData(int fd, void *buffer, int length);

/**
 * @description:给socket文件中写数据 
 * @param {*}
 * @return {*}
 */
int zSockCommon_socketWriteData(int fd, const void *buffer, int length);