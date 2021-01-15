/*
 * @Author: your name
 * @Date: 2021-01-07 14:47:51
 * @LastEditTime: 2021-01-07 16:21:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_runcmd.cpp
 */
#include <stdio.h>
#include <string.h>

#include "z_log.h"
#include "z_runcmd.h"


#define TAG ("RunCmd")

int ZRunCmd(const char *cmd, char *result, int len, int Row){
    int ret = -1;
    if(NULL == cmd ||NULL == result || len <= 0){
        Z_ERROR("%s param is error", __FUNCTION__);
        return ret;
    }
    
    FILE *file = popen(cmd, "r");
    if(file < 0 ){
        Z_ERROR("%s popen is error", __FUNCTION__);
        return -1;
    }
    ret = 0;
    memset(result, 0, len);
    int i = 0;
    while((fgets(result, len, file)) != NULL){
        //Z_DEBUG("%s\n",result);
        if(i >= Row) break;
        i++;
    }      
    pclose(file);
    return ret;

}