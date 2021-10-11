/*
 * @Author: your name
 * @Date: 2021-09-16 10:34:38
 * @LastEditTime: 2021-09-16 15:35:46
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \StudyLinuxC\src\IPCTest.cpp
 */

#include <stdlib.h>
#include <unistd.h>

#include "z_log.h"
#include "z_define.h"

#define TAG ("IPCDEMO")

/**
 * @description:经由管道父进程向子进程传递信息 
 * @param {*}
 * @return {*}
 */
static void demo1(){
    int n;
    int fd[2];
    pid_t pid;
    char line[Z_MAXLINE];
    if(pipe(fd) < 0){
        Z_ERROR("pipe error!\n");
    }
    if((pid = fork())<0){
        Z_ERROR("fork error !\n");
    }else if(pid >0){//父进程
        close(fd[0]);
        write(fd[1],"hello pipe\n",11);
    }else{//子进程
        close(fd[1]);
        n = read(fd[0],line,Z_MAXLINE);
        write(STDOUT_FILENO,line,n);
    }
    exit(0);
}

int zIPCDemoMain(){
    demo1();
    return 0;
}