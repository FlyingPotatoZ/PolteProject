/*
 * @Author: your name
 * @Date: 2021-01-07 09:36:13
 * @LastEditTime: 2021-01-15 11:31:20
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\main.cpp
 */
/*
 * 
 * 　　┏┓　　　┏┓+ +
 * 　┏┛┻━━━┛┻┓ + +
 * 　┃　　　　　　　┃ 　
 * 　┃　　　━　　　┃ ++ + + +
 *  ████━████ ┃+
 * 　┃　　　　　　　┃ +
 * 　┃　　　┻　　　┃
 * 　┃　　　　　　　┃ + +
 * 　┗━┓　　　┏━┛
 * 　　　┃　　　┃　　　　　　　　　　　
 * 　　　┃　　　┃ + + + +
 * 　　　┃　　　┃
 * 　　　┃　　　┃ +  神兽保佑
 * 　　　┃　　　┃    代码无bug　　
 * 　　　┃　　　┃　　+　　　　　　　　　
 * 　　　┃　 　　┗━━━┓ + +
 * 　　　┃ 　　　　　　　┣┓
 * 　　　┃ 　　　　　　　┏┛
 * 　　　┗┓┓┏━┳┓┏┛ + + + +
 * 　　　　┃┫┫　┃┫┫
 * 　　　　┗┻┛　┗┻┛+ + + +
 * 
 * 
 * @Author: your name
 * @Date: 2020-12-23 15:45:19
 * @LastEditTime: 2021-01-04 16:32:38
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\main.cpp
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#include "z_define.h"
#include "z_file.h"
#include "z_log.h"
#include "z_lock.h"
#include "z_runcmd.h"
#include "z_message.h"
#include "z_thread.h"

/**
 * @description: 
 * @param {int} argc
 * @param {char} *
 * @return {*}
 */

#define TAG ("MAIN")

static ZMutex mMainMutex;
static ZMsgQue_ID demo_id = 0;
static bool mWorkProcRthreadRun = false;
static HANDLE mWorkProcRthread = 0;

static void _test1(int*){
    ZAutoMutex _l(mMainMutex);
    Z_INFO("int*");
}
static void _test2(int&){
    Z_INFO("int&");
}


static void* _msg_demo_receive(void *param){
    ZMsg msg = {0};
    ZMsgQue_ID *demo_id = (ZMsgQue_ID*)param;
    while(mWorkProcRthreadRun){
        if(0 == ZMsgQue_Receive(*demo_id,&msg,1000)){
            switch (msg.what)
            {
            case 1:
                Z_DEBUG("msg demo ->msg.what is :%d\n",msg.what);
                break;
            
            default:
                break;
            }
        }
        sleep(10);
    }
    Z_DEBUG("_msg_demo_receive thread is Stop!\n");
}

int main(int argc, char **argv){
    z_file();
    int A[50];
    int *a = new int[100];
    char *aa;
    int AA;
    char C[100];
    const char* str = "1234678g711";
    printf("%d\n",sizeof(aa));
    printf("%d\n",sizeof(C));
    Z_DEBUG_T(TAG,"hello");
    Z_DEBUG("no TAG");

    _test1(a);
    _test2(AA);

    const char *cmd = "ifconfig";
    char buf[1024] = {0};
    
    ZRunCmd(cmd, buf, sizeof(buf), 3);

    Z_DEBUG("buf is %s",buf);
    
    ZMsg *mq = (ZMsg*)malloc(sizeof(ZMsg));
    unsigned long id = 0;
    unsigned long * const ip = &id;

    mq->what = 0x00000000;
    mq->arg1 = 0x11111111;
    Z_DEBUG("id1 is %x\n",id);
    Z_DEBUG("ip1 is %x\n",ip);
    *ip = (unsigned long )mq;
    Z_DEBUG("id2 is %x\n",id);
    Z_DEBUG("ip2 is %x\n",*ip);

    ZMsg *c = (ZMsg*)id;
    Z_DEBUG("c is %x\n",c->what);
    Z_DEBUG("c is %d\n",7%6);

    //###################Msg Demo######
    if(0 != ZMsgQue_Create(&demo_id,"main_msg",10)){
        Z_ERROR("create msgque error!");
    }
    ZMsg msg = {0};
    msg.what = 1;
    ZMsgQue_Send(demo_id, &msg, 1000);
    usleep(1000*1000);
    mWorkProcRthreadRun = true;   
    if(NULL == mWorkProcRthread){
        mWorkProcRthread = ZThrd_Create("Message_demo",NULL,_msg_demo_receive,&demo_id);
    }
    getchar();
    //###################Msg Demo######

}