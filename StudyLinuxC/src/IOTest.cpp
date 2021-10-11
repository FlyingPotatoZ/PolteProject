/*
 * @Author: your name
 * @Date: 2021-09-17 10:49:36
 * @LastEditTime: 2021-10-11 15:02:01
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \StudyLinuxC\src\IOTest.cpp
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "z_log.h"
#include "z_define.h"

#define TAG ("IODEMO")

/**
 * @description: 多路IO，主要测试select性能，在监听多路IO时得处理情况
 * @param {*}
 * @return {*}
 */
static void _multiIO(){
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);//标准输入
    FD_SET(2, &rfds);//标准出错


    /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(3, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true. */
    else
        printf("No data within five seconds.\n");

    exit(EXIT_SUCCESS);
} 

int zIODemoMain(){
    _multiIO();
    return 0;
}