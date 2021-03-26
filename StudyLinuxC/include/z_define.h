/*
 * @Author: your name
 * @Date: 2021-01-13 15:57:11
 * @LastEditTime: 2021-03-26 15:26:25
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_define.h
 */
#ifndef Z_DEFINE_H
#define Z_DEFINE_H 

#define PATH ("../res/Algorithm.txt")

#define KEY (19951006)

typedef void *HANDLE;

#define Z_TIMEOUT_INFINITY (0xffffffff)
#define Z_TIMEOUT_IMMEDIATE (0x00)
#define Z_MAXLINE (256)

#define Z_VERIFY_RET_VOID(condition) \
    do { \
        if (condition) {printf("VERIFY: condition "#condition" is not permitted.\n"); return;} \
    } while(0)

#define Z_VERIFY_RET_VAL(condition, val) \
    do { \
        if (condition) {printf("VERIFY: condition "#condition" is not permitted.\n"); return (val);} \
    } while(0)
#endif