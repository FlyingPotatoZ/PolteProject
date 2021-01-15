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
 * @Date: 2020-12-23 16:13:23
 * @LastEditTime: 2021-01-04 16:40:01
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_file.cpp
 */

#include <stdio.h>
#include <string.h>

#include "z_define.h"
#include "z_file.h"
#include "z_log.h"

#define TAG ("Z_FILE")
int z_write_1(const char* path, const char * str){
    FILE *pfile = NULL;
    pfile = fopen(path,"ab+");
    if(!pfile){
        Z_ERROR("open file fail!");
        return -1;
    }

}

int z_file(){
    FILE* pfile = NULL;
    pfile = fopen(PATH,"ab+");
    if(!pfile){
        Z_ERROR("open file fail\n");
        return -1;
    }
    Z_DEBUG("open ok!");
    char* str = "hello hello hello hello";
    fwrite(str, strlen(str)+1 , 1, pfile);
    for(int i = 0; i < 5 ; i++){
        fwrite(str, strlen(str)+1 , 1, pfile);
    }

    
    fclose(pfile);

    
}