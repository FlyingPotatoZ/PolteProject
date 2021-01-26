/*
 * @Author: your name
 * @Date: 2021-01-22 10:47:05
 * @LastEditTime: 2021-01-26 11:23:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\sock\z_local_sock_client.cpp
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

#include "z_local_sock_client.h"

/**
 * @description:构建本地sock addr结构体 
 * @param {*}
 * @return {*}
 */
static int _makeSockaddr_un(const char *name, struct sockaddr_un *p_addr, socklen_t *socklen, int namespaceID = 0){
    size_t namelen;
    memset(p_addr, 0, sizeof(*p_addr));

#ifdef HAVE_LINUX_LOCAL_SOCKET_NAMESPACE
    namelen = strlen(name);
    if((namelen + 1) > sizeof(p_addr->sun_path)){
        return NO_LINUX_MAKE_ADDRUN_ERROR;
    }
    p_addr->sun_path[0] = 0;
    memcpy(p_addr->sun_path + 1, name, namelen);

#else
    namelen = strlen(name) + strlen(FILESYSTEM_SOCKET_PREFIX);

    /* unix_path_max appears to be missing on linux */
    if (namelen > (sizeof(*p_addr) - offsetof(struct sockaddr_un, sun_path) - 1))
    {
        return NO_LINUX_MAKE_ADDRUN_ERROR;
    }

    strcpy(p_addr->sun_path, FILESYSTEM_SOCKET_PREFIX);
    strcat(p_addr->sun_path, name);

#endif

    p_addr->sun_family = AF_LOCAL;
    *socklen = namelen + offsetof(struct sockaddr_un, sun_path) + 1;

    return NO_ERROR;
}

/**
 * @description:从socket文件中读数据 
 * @param {*}
 * @return {*}
 */
static int _socketReadData(int fd, void *buffer, int length){
    
    if(fd < 0){
        printf("fd is error\n");
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
static int _socketWriteData(int fd, const void *buffer, int length){
    
    if(fd < 0){
        printf("fd is error\n");
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

/**
 * @description:连接socket 
 * @param {*}
 * @return {*}
 */
static int _socketConnectClient(const char *name, int type, int namespaceID = 0){
    int socketID;
    int ret;
    int flags;
    static struct sockaddr_un server_addr;
    
    socketID = socket(PF_UNIX, type, 0);
    if(socketID < 0){
        printf("creat socket fail\n");
        return CREATE_ERROR;
    }

    flags = fcntl(socketID, F_GETFL, 0);
    fcntl(socketID, F_SETFL, flags | O_NONBLOCK);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, name);

    ret = connect(socketID, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(-1 == ret){
        printf("can't connect socket\n");
        close(socketID);
        return CONNECT_ERROR;
    }
    return socketID;
}


int zSock_localSockSendClient(const char *cmd){
    char buf[MAX_SOCK_BUFFSIZE] = {0};
    int socketID;
    int ret;
    int len;
    do{
        socketID = _socketConnectClient(SOCK_FNAME, SOCK_STREAM);
        if(socketID < 0){
            printf("CONENT ERROR\n");
            ret = socketID;
            break;
        }

        ret = _socketWriteData(socketID, cmd, strlen(cmd));
        if(ret < 0){
            printf("WRITE ERROR\n");
            ret = WRITE_ERR;
            break;
        }

        ret = _socketReadData(socketID, buf, MAX_SOCK_BUFFSIZE);
        if(ret < 0){
            printf("READ ERROR\n");
            ret = READ_ERR;
            break;
        }
    }while (0);

    printf("%s\n", buf);
    close(socketID);
    return ret;
}

int main(int argv, char **argc){
    const char * cmd = "hello";
    int ret = 0;
    ret = zSock_localSockSendClient(cmd);
    printf("####bye###\n");
    return ret;
}