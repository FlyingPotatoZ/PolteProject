/*
 * @Author: your name
 * @Date: 2021-01-26 17:35:28
 * @LastEditTime: 2021-02-05 18:30:39
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\Algorithm\sock\zsockd\z_service_main.cpp
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "z_local_sock_define.h"
#include "z_local_sock_service.h"
#include "z_log.h"

#define TAG ("SOCKD_MAIN")

static int _initSockServer(){
    char cmd[MAX_SOCK_CMD] = {0};
    memset(cmd, 0, sizeof(cmd));

    sprintf(cmd ,"rm -rf %s", SOCK_FNAME);
    system(cmd);
    return 0;
}


int main(int argc, char **argv){
    zLog_setLogLevel(Z_LOG_ALL);
    
    Z_TRACE("***********************************************\n");
    Z_TRACE("*************Start SOCKD_SERVER****************\n");
    Z_TRACE("***********************************************\n");

    _initSockServer();

    int tmp = 0;
    zSockd_setReceiveEnable(true);
    tmp = zSockd_LooperThread();
    

    Z_TRACE("***********************************************\n");
    Z_TRACE("*************Stop SOCKD_SERVER****************\n");
    Z_TRACE("***********************************************\n");
    return tmp;
}
