/*
 * @Author: your name
 * @Date: 2021-01-25 17:18:21
 * @LastEditTime: 2021-02-19 15:30:49
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\sock\zsockd\z_local_sock_service.cpp
 */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "z_define.h"
#include "z_log.h"

#include "z_local_sock_define.h"
#include "z_local_sock_common.h"
#include "z_local_sock_service.h"

#define TAG ("SOCKD_SERVER")

static bool mReceiveEnable = false;


static int _createServerSock(){
    int s_ID = -1;
    if((s_ID = socket(AF_UNIX,SOCK_STREAM, 0)) < 0){
        Z_ERROR("create socket fail!\n");
        return s_ID;
    }
    return s_ID;  
}

static int _bindServerSock(int sock_id){
    int ret = -1;
    do{
        if(sock_id < 0){
            Z_ERROR("unlawful sock_id\n");
            break;
        }

        struct sockaddr_un srv_addr;
        srv_addr.sun_family = AF_UNIX;
        strncpy(srv_addr.sun_path, SOCK_FNAME, sizeof(srv_addr.sun_path) - 1);
        
        if((ret = bind(sock_id, (struct sockaddr *)&srv_addr, sizeof(srv_addr))) < 0){
            Z_ERROR("bind socket fail!\n");
            break;
        }
    }while(0);  
    return ret;  
}

static int _listenServerSock(int sock_id){
    int ret = -1;
    do{
        if(sock_id < 0){
            Z_ERROR("unlawful sock_id\n");
            break;
        }
        
        if((ret = listen(sock_id, 1)) < 0){
            Z_ERROR("listen socket fail!\n");
            break;
        }
    }while(0);  
    return ret;
}

static void _closeServerSock(int sock_id){
    Z_DEBUG("close socket : %d\n", sock_id);
    close(sock_id);
}

//根据客户端传来的字符串判断执行什么功能
static int _doCmd(char *buf,char *result){
    Z_VERIFY_RET_VAL(NULL == buf, -1);
    
    Z_DEBUG("client cmd is :%s\n",buf);
    if(0 == strcmp(buf, "test")){
        Z_DEBUG("yes, it's worked!!");
    }else if(0 == strcmp(buf, "exit")){
        Z_DEBUG("do zsockd stop!");
        mReceiveEnable = false;
        return 0;
    }
}



//工作线程
static int _WorkProc(int socket, char *buf, void *work_cb, void *param){
    Z_DEBUG("hello _WorkProc...\n");

    if(NULL != buf){
        _doCmd(buf, NULL);
    }
    if(NULL != work_cb){
        Z_DEBUG("Unrealized !!do work_cb");
    }

    return 0;
}

static int _clientReceiver(int sock_id, char* read_buff){
    if(sock_id < 0 || NULL == read_buff){
        Z_ERROR("Receiver args error!\n");
        return -1;
    }

    int mAccSockID = -1;
    int len = 0;
    
    while(mReceiveEnable){
        Z_DEBUG("waiting client call...\n");
        if((mAccSockID = accept(sock_id, NULL, NULL)) < 0){
            Z_ERROR("accept sock error, serverID is : %d\n", sock_id);
            continue;
        }

        memset(read_buff, 0, MAX_SOCK_BUFFSIZE);

        if((len = zSockCommon_socketReadData(mAccSockID, read_buff, MAX_SOCK_BUFFSIZE)) < 0){
            Z_ERROR("read data error! serverID is : %d\n", mAccSockID);
            continue;
        }
        Z_DEBUG("zsockd read data is :%s", read_buff);
        if(-1 == _WorkProc(mAccSockID, read_buff, NULL, NULL)){
            Z_ERROR("_WorkProc error! serverID is : %d\n", mAccSockID);
            continue;
        }
        
    }
    Z_DEBUG("clientReceiver stoped!!\n");
    return 0;
}




void zSockd_setReceiveEnable(bool enable){
    mReceiveEnable = enable;
}

//sockd守护线程
int zSockd_LooperThread(){
    int ret = -1;
    int mServerSockID = -1;
    char read_buf[MAX_SOCK_BUFFSIZE] = {0};
    char *cmd = NULL;

    do{
        if((mServerSockID = _createServerSock()) < 0){
            break;
        }
        if(ret = _bindServerSock(mServerSockID)){
            break;
        }
        if(ret = _listenServerSock(mServerSockID)){
            break;
        }
        if(ret = _clientReceiver(mServerSockID, read_buf)){
            break;
        }
    }while(0);

    _closeServerSock(mServerSockID);
    mReceiveEnable =false;
    return ret;
}