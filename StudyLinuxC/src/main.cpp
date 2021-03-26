/*
 * @Author: your name
 * @Date: 2021-03-24 14:54:01
 * @LastEditTime: 2021-03-26 16:54:31
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \StudyLinuxC\src\main.cpp
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "z_log.h"
#include "z_define.h"

#define TAG ("MAIN")

#define TE (1ULL<<32)
/**
 * @description:测试从标准输入读出字符，包括EOF标准 
 * @param {*}
 * @return {*}
 */
static void _getCforStdin(){
    int c ;
    while((c = getchar()) != EOF){
        putchar(c);
    }
    Z_DEBUG("exit~");
}

/**
 * @description:通过从标准输入读入字符串，将其当成可程序进行执行，主要使用execlp接口 
 * @param {*}
 * @return {*}
 */
static int  _execl(){
    char buf[Z_MAXLINE];
    pid_t pid;
    int status;

    Z_INFO("%% ");
    while(fgets(buf,Z_MAXLINE,stdin) != NULL){
        Z_DEBUG("do something!\n");
        if(buf[strlen(buf) - 1] == '\n'){
            buf[strlen(buf) - 1] = 0;
        }

        if((pid = fork()) < 0){
            Z_ERROR("fork error\n");
        }else if(0 == pid){
            execlp(buf, buf, (char*)0);
            Z_ERROR("could not execute %s", buf);
            return -1;
        }

        if((pid = waitpid(pid, &status, 0)) < 0){
            Z_ERROR("waitpid error\n");
        }
        Z_INFO("%% ");
    }
    return 0;
}

int main(int argv, char** argc){
    Z_DEBUG("hello this process ID is [%d]", getpid());
    _execl();
    return 0;
}
