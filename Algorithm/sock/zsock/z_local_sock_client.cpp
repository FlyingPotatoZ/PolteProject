/*
 * @Author: your name
 * @Date: 2021-01-22 10:47:05
 * @LastEditTime: 2021-01-25 18:08:40
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

int main(int argv, char **argc){
    printf("hah\n");
}