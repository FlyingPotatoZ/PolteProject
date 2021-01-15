#ifndef Z_RUNCMD_H
#define Z_RUNCMD_H

/**
 * @description: 
 * @param {constchar} *cmd 待执行的shell指令
 * @param {char} *result 执行shell返回的结果
 * @param {int} len 返回result长度
 * @param {int} Row 需要返回结果的第几行，默认第一行
 * @return {*}
 */
int ZRunCmd(const char *cmd, char *result, int len, int Row = 0);



#endif