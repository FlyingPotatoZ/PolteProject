/*
 * @Author: your name
 * @Date: 2021-02-05 17:17:59
 * @LastEditTime: 2021-02-05 17:50:31
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\sock\z_local_sock_common.cpp
 */
#include "stdio.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "z_log.h"
#include "z_local_sock_define.h"
#include "z_local_sock_common.h"

#define TAG ("SOCK_COMMON")

/**
 * @description:从socket文件中读数据 
 * @param {*}
 * @return {*}
 */
int zSockCommon_socketReadData(int fd, void *buffer, int length){
    
    if(fd < 0){
        Z_ERROR("fd is error\n");
        return -1;
        
    }
    
    int bytes_left = 0;
    int bytes_read = 0;
    char *ptr = (char *)buffer;

    while(1){
        bytes_read = read(fd, ptr, length);
        if(-1 == bytes_read && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN){
            break;
        }

        if(bytes_read > 0){
            bytes_left += bytes_read;
            ptr += bytes_read;
            break;
        }
    }

    return bytes_left;
}

/**
 * @description:给socket文件中写数据 
 * @param {*}
 * @return {*}
 */
int zSockCommon_socketWriteData(int fd, const void *buffer, int length){
    
    if(fd < 0){
        Z_ERROR("fd is error\n");
        return -1;
    }
    
    int bytes_left = 0;
    int bytes_write = 0;
    char *ptr;
    ptr = (char *)buffer;
    bytes_left = length;

    while(bytes_left > 0){
        bytes_write = write(fd, ptr, bytes_left);
        if(bytes_write <= 0){
            if(errno == EINTR){
                bytes_write = 0;
            }else{
                return WRITE_ERR;
            }
        }
        bytes_left -= bytes_write;
        ptr += bytes_write;
    }
    return NO_ERROR;
}