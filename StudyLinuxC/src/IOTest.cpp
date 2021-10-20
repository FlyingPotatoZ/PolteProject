/*
 * @Author: your name
 * @Date: 2021-09-17 10:49:36
 * @LastEditTime: 2021-10-20 16:00:40
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \StudyLinuxC\src\IOTest.cpp
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>

#include "z_log.h"
#include "z_define.h"

#define TAG ("IODEMO")

//注意！！！！：C语言中使用最多的两种IO方式，需要明确系统调用和标准的IO的区别，不能弄混淆。一般情况下使用标准IO就可以了，极致追求效率时才使用系统调用（需要仔细设计缓冲区大小）


/**
 * @description:系统调用中得IO函数：open read close write lseek create（完全被open取代，不需要了） 
 *              以文件描述符作为基础进行IO操作，直接操作字节数据。
 *              其中0、1、2分别表示标准输入，标准输出，标准出错
 * @param {*}
 * @return {*}
 */  
extern "C"
{

    static const char *path ="./test.log";

    static void sysCallsIO(){
        /**
         * @description: open函数
         * 打开一个文件描述符，第三个参数得选项由或运算符连接，有个必选项分别是：O_RDONLY 只读、O_WRONLY 只写、O_RDWR 读写 三者之一
                        其余可选项为：
                        O_APPEND：以后每次写文件时都会先将当前文件偏移量设置到文件末尾，但是读文件时时不影响的
                        O_CREAT：如果文件不存在则创建
                        O_EXCL：要打开的文件如果存在则出错，必须要和O_CREAT参数一起使用
                        O_TRUNC：打开文件的同时将文件中的内容清除
                        O_NOCTTY：如果打开的文件是终端设备，则不将此设备设置为进程的控制终端
                        O_NONBLOCK：如果打开的文件是一个管道、一个块设备文件或一个字符设备文件，则后续的I/O操作均设置为非阻塞方式
                        O_SYNC：使每次write都等到物理I/O操作完成，包括由该write操作引起的文件属性更新所需的I/O
            */        
        int fp = open(path, O_RDWR);
        fp = open(path, O_RDONLY|O_TRUNC|O_SYNC);


        /**
         * @description: close函数
         *              关闭一个被打开得文件描述符，释放系统资源。进程终止时内核也会自动关闭打开得文件，所以有时可以不用。
         */        
        close(fp);


        /**
         * @description: lseek函数
         * 文件被打开时，都会有一个相关联得文件偏移量，通常读写操作都是从这个偏移点开始的。并使得偏移量增加所读写的字节数。
         *              通常一个文件被打开时，在没有指定O_APPEND时，偏移量都默认设置为0。
         * @param ：第二个参数的含义取决于第三个参数，第三个参数的可选值有：
         *              SEEK_SET 参数offset 即为新的读写位置.
                        SEEK_CUR 以目前的读写位置往后增加offset 个位移量.
                        SEEK_END 将读写位置指向文件尾后再增加offset 个位移量. 当whence 值为SEEK_CUR 或SEEK_END 时, 参数offet 允许负值的出现
         *   @return ： 成功时返回当前偏移量，失败时返回-1
         */   
        off_t pos = lseek(fp, 10, SEEK_CUR);



        /**
         * @description: read函数
         * 从被打开的文件中读取数据，将数据按字节数读取到一块buf上面。
         * @param {*} 第二个参数表示读取数据存储区，第三个参数表示期望读取的字节数。
         * @return {*}成功则返回读取的字节数，失败则返回-1，到达文件结尾则返回0
         */   

        char buf[1024];
        ssize_t rsize = read(fp, buf, 256);


        /**
         * @description: write函数
         * 向打开的文件中写入数据，写入开始的节点是文件当前偏移量所在点。若指定了O_APPEND，每次写入的节点会在文件末尾。
         * @param {*}第二个参数表示带写入数据，第三个参数表示期望写入的字节数。
         * @return {*}正常返回写入的字节数，失败返回-1
         */            
        const char *str = "write data!";
        ssize_t wsize = write(fp, str, strlen(str));
    }

    static void sysCallIODemo(){
        int fp = open(path, O_RDWR|O_CREAT);
        if(fp != -1){
            const char *str = "write data!";
            //ssize_t wsize = write(fp, str, strlen(str));
        }

        char buf[1024];
        //lseek(fp, 0, SEEK_SET);
        ssize_t rsize = read(fp, (void *)buf, 80);
        
        printf("rsize is : %d\n",rsize);
        printf("bufsize is : %d\n",strlen(buf));
        printf("data is : %s\n",buf);

        close(fp);

    }


}


/**
 * @description: 标准IO库中的函数，以文件指针FILE* 作为标识进行识别，以流的形式进行IO，主要缓冲方式包括全缓冲、行缓冲、不缓冲三种。
 * 主要包括：open系列函数（fopen、freopen、fdopen）；fclose；
 *          异常判断函数（ferror、feof、clearerr）；
 *          读系列函数（getc、fgetc、getchar|gets、fgets|fread）；
 *          写系列函数（putc、fputc、putchar|puts、fputs|fwrite）；
 *          定位偏移系列函数（ftell、fseek|ftello、fseeko|fgetpos、fsetpos）；
 *          格式化输出系列函数（printf、fprintf、sprintf、snprintf）；
 *          格式话输入系列函数（scanf、fscanf、sscanf）
 *          
 * 
 * @param {*}
 * @return {*}
 */
extern "C"
{
    static void StandardIO(){
        /**
         * @description: open系列函数：
         *      FILE *fopen(const char *path, const char *mode);
                FILE *fdopen(int fd, const char *mode);
                FILE *freopen(const char *path, const char *mode, FILE *stream);
                fopen打开指定的文件，返回一个文件指针
                freopen在指定的流上打开特定的文件，并且该stream如果先前打开了就先关闭它：经常用于打开指定的文件作为标准输入输出或者错误流
                fdopen通过fd（文件描述符）打开流，通常用于pipe、socket等
         * @param {*}第二个参数为打开模式，包括：
                    “r”	以读模式打开文件
                    “w”	以写模式打开文件，把现有文件的长度截为0，如果文件不存在，则创建一个新文件
                    “a”	以写模式打开文件，在现有文件末尾添加内容，如果文件不存在，则创建一个新文件
                    “r+”	读写文件（覆盖）
                    “w+”	读写文件，如果文件存在，则将其长度截为0；如果文件不存在，则创建一个新文件（清除重写）
                    “a+”	读写文件，在现有文件的末尾添加内容，如果文件不存在则创建一个新文件；可以读整个文件，但是只能从末尾添加内容（尾部续写）

         * 
         * @return {*}
         */ 
        FILE *Fp = fopen(path, "r");
        Fp = freopen(path, "r+", stdout);//将标准输出打开为流，并返回流指针
        Fp = fdopen(0,"a+");//将标准输入与一个流相关联，然后该流指针

        


        /**
         * @description: putc、fputc、putchar函数：一次给流输入一个字符
         *  int fputc(int c, FILE *stream);
            int putc(int c, FILE *stream);
            int putchar(int c);
            三者区别在于:
            putc()功能和fputc()差不多，一个是宏，一个是函数
            putc（int ch，FILE *fp），即将字符ch输出到fp所指的文件中
            putchar（char ch)，即将字符ch输出到标准输出设备上,相当于putc(char ch, stdout);
         * @param {*将参数c，输入stream中}
         * @return {*成功返回c，失败返回EOF}
         */  

        if((putc('a',Fp)) != EOF){
            
        }
        putchar('!');

        /**
         * @description: gutc、fgutc、gutchar函数：一次从流中读入一个字符
         *  int fgetc(FILE *stream);
            int getc(FILE *stream);
            int getchar(void);
            int ungetc(int c, FILE *stream);

            对put相对应的三者区别在于:
            gutc()功能和fgutc()差不多，一个是宏，一个是函数
            gutc（FILE *fp），即从fp所指的文件中读出一个字符，
            gutchar（)，从标准输入读入一个字符,相当于gutc(stdin);
         * @param {}
         * @return {*成功返回读取的字符（为什么是int而不是char，是因为需要兼容错误码，带符号的），失败返回EOF}
         */       
        getchar();
        int cc;
        if((cc = getc(Fp)) != EOF){
            std::cout << (char)cc;
        }


        /**
         * @description: fputs、puts函数：一次给流输入一行字符串
         *              int fputs(const char *s, FILE *stream);
         *              int puts(const char *s);（尽管没有gets那样不安全，也不推荐使用）
         * 
         * 两者区别：
         * fputs将以null符终止的字符串写道指定的流，尾端的终止符null不写出，并不要求null之前一定是一个换行符，通常是，但可以不是。
         * puts将一个以null符终止的字符串写道标准输出，终止符不写出，然后再继续写一个换行符。也就是说如果我们自己写入一个换行符，标准输出将打印两个换行符。
         * 通俗来讲就是puts会自动添加换行符，fputs则不会，再使用fputs时需要自行处理换行。
         * @param {*}
         * @return {*}成功返回非负值，失败返回EOF
         */    

        const char *str="puts hello\n";
        if((fputs(str, Fp)) != EOF){
            
        }


        /**
         * @description: fguts、guts函数：一次从流读出一行字符串
         * char *fgets(char *s, int size, FILE *stream);
         * char *gets(char *s);(注意！！！不安全，不使用，曾经引发蠕虫病毒)
         * 
         * 两者区别：
         * gets不指定缓冲区长度，从标准输入一直读取字符串，直到换行符。当一行字符串长度大于缓冲区时，就会产生不可预知的结果。
         * fgets通过参数指定缓冲区大小
         * @param {*}
         * @return {*}成功返回buf，失败或到达文件结尾返回NULL
         */        
        char buf[1024];
        if((fgets(buf, 1024, Fp)) != NULL){
            
        }


        /**
         * @description:fread函数，从流中读取指定大小的数据 
         *              size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
         * @param {*ptr缓冲区指针，size每一个对象的大小，nmemb需要读取对象的数目，}
         * @return {*}返回读取到的对象数目
         */
        size_t len =0;
        if((len = fread(buf, 1, 1024, Fp)) > 0){
            
        }


        /**
         * @description: fwrite函数，给指定流写入数据
         *             size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
         * @param {*ptr缓冲区指针，size每一个对象的大小，nmemb需要写入对象的数目，}
         * @return {*}返回写入的对象数目
         */        

        float data[64];
        if((len = fwrite(data, sizeof(float), 64, Fp)) != 64){
            printf("write error ,size is %d\n", len);
        }


        /**
         * @description: fclose函数
         *              关闭一个被打开得流，释放系统资源。进程终止时内核也会自动关闭打开得文件，所以有时可以不用。
         */        
        fclose(Fp);


        /**
         * @description:printf、fprintf、sprintf、snprintf格式化输出函数
        *       int printf(const char *format, ...);
                int fprintf(FILE *stream, const char *format, ...);
                int sprintf(char *str, const char *format, ...);
                int snprintf(char *str, size_t size, const char *format, ...);(推荐使用)
        
        区别：
        printf为标准输出函数，将格式化字符format打印到标准输出
        fprintf与printf类似，将格式化字符串写入指定流，printf相当于fprintf(stdout, format, ...)
        两者的返回值一样，成功返回写入流的字符数，失败返回负值。

        sprintf将指定格式化字符串写入指定buf。sprintf会在写入buf的尾端自动加一个null。可能导致缓冲区溢出，需要程序员自行控制。
        snprintf将指定格式化字符串写入指定buf。不同的是可以指定缓冲区大小，防止缓冲区溢出
        两者返回值一样，成功返回写入的字符数，失败返回负值。
         * @param {*}
         * @return {*}
         */        
        printf("hello world! %d times", 10086);
        fprintf(Fp, "this day is a good day\n");
        
        sprintf(buf, "hello world!\n");
        snprintf(buf, sizeof(buf), "paly %d times!\n", 10);


        /**
         * @description: scanf、fscanf、sscanf用于格式化分析输入的字符串，支持正则化表达式，可以实现字符串分割，功能十分强大。
         *          int scanf(const char *format, ...);
                    int fscanf(FILE *stream, const char *format, ...);
                    int sscanf(const char *str, const char *format, ...);
        说明：
            scanf函数称为格式输入函数，即按用户指定的格式从键盘上把数据输入到指定的变量之中
            sscanf函数，将指定的字符串按格式进行分析，format支持正则表达式，经常用于字符串的分割
            fscanf函数，将文件内容按指定的格式进行分析，format支持正则表达式，通过用于日志文件等有规律格式文件的分析

        format可以使用的标签包括：
            1、类型：
                d	输入十进制整数
                o	输入八进制整数
                x	输入十六进制整数
                u	输入无符号十进制整数
                f或e	输入实型数(用小数形式或指数形式)
                c	输入单个字符
                s	输入字符串
            
            2、*号
                *表示有该输入项，但是不给相应位置的地址赋值，即跳过该项。例如：scanf("%d %*d %d",&a,&b);当输入为：1 2 3时，把1赋予a，2被跳过，3赋予b。
            
            3、宽度
                用十进制整数指定输入的宽度（即字符数，例如：scanf("%4d%4d",&a,&b); 输入12345678将把1234赋予a，而把5678赋予b
            
            4、类似正则表达式
                format可以是一个或多个 {%[*] [width] [{h | l | I64 | L}]type | ' ' | '\t' | '\n' | 非%符号}
                注：
                    1、 * 亦可用于格式中, (即 %*d 和 %*s) 加了星号 (*) 表示跳过此数据不读入. (也就是不把此数据读入参数中)
                　　2、{a|b|c}表示a,b,c中选一，[d],表示可以有d也可以没有d。
                　　3、width表示读取宽度。
                　　4、{h | l | I64 | L}:参数的size,通常h表示单字节size，I表示2字节 size,L表示4字节size(double例外),l64表示8字节size。
                　　5、type :这就很多了，就是%s,%d之类。
                　　6、特别的：%*[width] [{h | l | I64 | L}]type 表示满足该条件的被过滤掉，不会向目标参数中写入值
                　　支持集合操作：
                　　%[a-z] 表示匹配a到z中任意字符，贪婪性(尽可能多的匹配)
                　　%[aB'] 匹配a、B、'中一员，贪婪性
                　　%[^a] 匹配非a的任意字符，贪婪性
                例子：
                     取到指定字符为止的字符串 ：sscanf("123456 abcdedf", "%[^ ]", buf); 取遇到空格为止字符串，结果为：123456
                     取仅包含指定字符集的字符串：sscanf("123456abcdedfBCDEF", "%[1-9a-z]", buf); 取仅包含1到9和小写字母的字符串，结果为：123456abcdedf
                     给定一个字符串iios/12DDWDFF@122，获取 / 和 @ 之间的字符串：sscanf("iios/12DDWDFF@122", "%*[^/]/%[^@]", buf); 结果为：12DDWDFF
                     字符串分割：sscanf("2006:03:18 - 2006:04:18", "%[0-9,:] - %[0-9,:]", sztime1, sztime2);


         * @param {*}
         * @return {*}
         */ 
        int a;
        char c;
        scanf("%d-%c", &a, &c);//终端输入10-s，a=10 c=s

        int year,mouth,day,hour,min,second;
        sscanf("2019.10.10-20:35:56","%d.%d.%d-%d:%d:%d",&year,&mouth,&day,&hour,&min,&second);   


        long dev;
        long offset;
        long length;
        char ch;
        double ts=0.000000;
        fscanf(Fp,"%ld,%ld,%ld,%c,%lf\n",&dev,&offset,&length,&ch,&ts); //Fp中的文件都是类似于2,50,41,w,20.585828
        while(5==fscanf(Fp,"%ld,%ld,%ld,%c,%lf\n",&dev,&offset,&length,&ch,&ts)){
            
        }
    }
}

extern "C++"
{
    static void ios(){

    }
}




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
    //_multiIO();
    //sysCallIODemo();
    //StandardIO();
    return 0;
}