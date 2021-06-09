/*
 * @Author: your name
 * @Date: 2021-03-24 14:54:01
 * @LastEditTime: 2021-04-28 10:39:50
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \StudyLinuxC\src\main.cpp
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <fcntl.h>

#include "z_log.h"
#include "z_define.h"
#include "classTest.h"

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
    zLog_setLogLevel(Z_LOG_INFO);
    printf("%% ");
    while(fgets(buf,Z_MAXLINE,stdin) != NULL){
        if(buf[strlen(buf) - 1] == '\n'){
            buf[strlen(buf) - 1] = 0;
        }

        if((pid = fork()) < 0){
            Z_ERROR("fork error\n");
        }else if(0 == pid){
            execlp(buf, buf, (char*)0);
            Z_ERROR("could not execute %s", buf);
            exit(127);
        }

        if((pid = waitpid(pid, &status, 0)) < 0){
            Z_ERROR("waitpid error\n");
        }
        printf("%% ");
    }
    return 0;
}

// 检查是否为字符串型数字, return 1为字符串型数字
int isDigit(const char *str)
{
	int i    = 0;
	int flag = 1;
    
    if(0 == strlen(str)){
        flag = 0;
    }

	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] >= '0' && str[i] <='9')
		{
			continue;
		}
		else
		{
			flag = 0;
			break;
		}
	}

	return flag;

}

/**
 * @description:不带缓冲的I/O函数：open read write lseek close 。这些都属于UNIX系统调用 
 * @param {*}
 * @return {*}
 */
int static _test_open(const char *pathName, int flag, ...);

int static _test_create(const char *pathName, mode_t mode);
//等效于 open(pathName, O_WRONNLY | O_CREAT | O_TRUNC, mode);


/**
 * @description: 测试在文件中留下“空洞”，即使用lseek将文件偏移量移动超过文件长度，这样做是容许的。
 * @param {*}
 * @return {*}
 */
static int _fileHole(const char *pathname){
    int fd;
    char buf1[] = "asdfghkl";
    char buf2[] = "ASDFGHJKL";
    if((fd = creat(pathname, 'rw')) < 0) Z_ERROR("create error\n");
    if(write(fd, buf1, 8) != 8) Z_ERROR("buf1 write error\n");
    if(lseek(fd, 16384, SEEK_SET) == -1) Z_ERROR("buf1 write error\n");//制造了一个16384大小的空洞，导致文件变得很大
    if(write(fd, buf2, 8) != 8) Z_ERROR("buf2 write error\n");

    return 0;
}

/**
 * @description:将标准输入复制到标准输出 
 * @param {*}
 * @return {*}
 */
static int _stdIN2stdOUT(){
    int t_size;
    char buf[Z_BUFSIZE];
    
    while((t_size = read(STDIN_FILENO, buf, Z_BUFSIZE)) > 0){
        if(write(STDOUT_FILENO, buf, t_size) != t_size) 
            Z_ERROR("write error!\n");
    }
    if(t_size < 0) Z_ERROR("read error!\n");
    return 0;
}

/**
 * @description: 改变文件状态标志
 * @param {int} fd 文件描述符
 * @param {int} flags
 * @return {*}
 */
static void _set_fl(int fd, int flags){
    int val;
    if((val = fcntl(fd, F_GETFL, 0)) < 0){
        Z_ERROR("fcntl F_GETFL %d error!", fd);
    }

    val |= flags;
    
    if(fcntl(fd,F_SETFL, val) < 0){
        Z_ERROR("fcntl F_SETFL %d error!", fd);
    }
}

/**
 * @description:打印出文件类型 
 * @param {const char} *path
 * @return {*}
 */
static void _stat(const char *path){
    int i;
    struct stat buf;
    char *ptr = {0};

    if(lstat(path, &buf) < 0){
        Z_ERROR("lstat error!\n");
        return ;
    }

    if(S_ISREG(buf.st_mode)){
        ptr = "regular";
    }else if(S_ISDIR(buf.st_mode)){
        ptr = "directory";
    }else if(S_ISCHR(buf.st_mode)){
        ptr = "character special";
    }else if(S_ISBLK(buf.st_mode)){
        ptr = "block special";
    }else if(S_ISFIFO(buf.st_mode)){
        ptr = "fifo";
    }else if(S_ISLNK(buf.st_mode)){
        ptr = "symbolic link";
    }else if(S_ISSOCK(buf.st_mode)){
        ptr = "socket";
    }else{
        ptr = "unknown mode";
    }
    Z_INFO("%s is %s\n", path, ptr);
}

void _cbsayhello(){
    Z_DEBUG("hello");
}

void _cbsayhello2(){
    Z_DEBUG(" just not say hello ha~");
}
typedef int (*intcb)();

int dohello(intcb cb){
    cb();
}

#define TOK_ADD 5
char *tok_ptr;
void do_line();
void cmd_add();
int get_token();

void do_line(char *ptr){
    int cmd;

    tok_ptr = ptr;
    while ((cmd == get_token()) > 0)
    {
        switch (cmd)
        {
        case TOK_ADD:
            cmd_add();
            break;
        
        default:
            break;
        }
    } 
}

void cmd_add(){
    int token;
    token = get_token();
    
}

int get_token(){
    
}


static time_t mTime;

extern char **environ; //获取环境表全局变量             

int main(int argv, char** argc){
    zLog_setLogLevel(Z_LOG_ALL);
    Z_DEBUG("hello this process ID is [%d]", getpid());
    //_fileHole("TEST.hole");
    //_stdIN2stdOUT();
    if(argv >= 2){
        _stat(argc[1]);
    }

    atexit(_cbsayhello2);//注册exit退出时需要执行得函数
    if(atexit(_cbsayhello) < 0) Z_ERROR("atexit register fail");

    for(int i = 0; environ[i] != NULL; ++i){
        Z_DEBUG("%s\n",environ[i]);
    }
   
    Z_DEBUG("enc is %s\n", getenv("LOGNAME"));//根据环境变量名返回环境变量

    pid_t pid;
    int i =0;
    if((pid = fork()) < 0){
        Z_ERROR("fork error!\n");
    }else if(0 == pid){
        i = 1;
        Z_DEBUG("this is son!\n");
    }else{
        i = 2;
        Z_DEBUG("this is father!\n");
    }

    Z_DEBUG("i is %d\n",i);
    

    return 0;
}
