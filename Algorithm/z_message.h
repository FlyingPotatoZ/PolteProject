/*
 * @Author: your name
 * @Date: 2021-01-07 16:23:16
 * @LastEditTime: 2021-01-14 10:27:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Algorithm\z_message.h
 */
#ifndef Z_MESSAGE_H
#define Z_MESSAGE_H

typedef unsigned long ZMsgQue_ID;//这个值是一个地址值，存的是一个指向堆区消息队列的指针的值，也就是消息队列的首地址 
                                //通过(ZMsgQue *)ZMsgQue_ID即可得到指向消息队列的指针

typedef struct{
    int what;
    int arg1;
    int arg2;
    void *obj;
}ZMsg;

/**
 * @description: 创建一个消息队列
 * @param {ZMsgQue_ID} *队列ID，为const的
 * @param {constchar} *que_name 
 * @param {constunsignedint} Max_msg_num 定义队列承载最大消息数
 * @return {*}
 */
int ZMsgQue_Create(ZMsgQue_ID * const que_ID, const char *que_name, const unsigned int Max_msg_num);

/**
 * @description:销毁消息队列 
 * @param {constZMsgQue_ID} que_ID
 * @return {*}
 */
int ZMsgQue_Destory(const ZMsgQue_ID que_ID);

/**
 * @description:给指定消息队列发送一个消息 
 * @param {constZMsgQue_ID} que_ID
 * @param {constZMsg} *msg
 * @param {constunsignedint} time_ms 超时时间， 可以创建支持立即超时和无限等待
 * @return {*}
 */
int ZMsgQue_Send(const ZMsgQue_ID que_ID, const ZMsg *msg, const unsigned int time_ms);

/**
 * @description: 从指定消息队列读取一个消息
 * @param {constZMsgQue_ID} que_ID
 * @param {ZMsg} *msg
 * @param {constunsignedint} time_ms
 * @return {*}
 */
int ZMsgQue_Receive(const ZMsgQue_ID que_ID, ZMsg *msg, const unsigned int time_ms);
#endif