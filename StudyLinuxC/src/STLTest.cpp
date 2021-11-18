/*
 * @Author: your name
 * @Date: 2021-11-12 10:15:18
 * @LastEditTime: 2021-11-12 17:20:31
 * @LastEditors: Please set LastEditors
 * @Description: 用于描述常用C++ STL库
 * @FilePath: \StudyLinuxC\src\STLTest.cpp
 */

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define TAG ("STLDEMO")

/**
 * @description: string类是一个C++中常用的处理字符串的类，包含有多种构造函数，及很多再字符串处理逻辑中非常好用的方法，可以熟练使用
 * @param {*}
 * @return {*}
 */
static void _stringDemo(){
    
    /**
     * @description:string类型的构造函数有很多，包括常见的七种和C++11新添加的两种 
     * @param 
     * @return {*}
     */    
    string hellostr("hello world!hello world!hello world!");
    string string1 = "string1";//自动实现了拷贝构造，将const char*构造喂string类。
    cout<< string1 <<endl;

    string string2(5, 'c');//构造组成元素为五个c的字符串
    cout << string2 <<endl;

    string string3("hello world!", 5); //构造字符串的前几位
    cout << string3 <<endl;

    string string4(string3);//将string类初始化为某一个string对象
    cout << string4 <<endl;

    string string5(&hellostr[1],&hellostr[6]);
    cout <<string5 <<endl;

    /**
     * @description: string再IO中的使用
     * @param {*}
     * @return {*}
     */    
    string stuff;
    //cin >> stuff;//从cin读一个单词，以空格为分割
    //getline(cin, stuff);//读一行，以换行符分割
    getline(cin, stuff,'!');//从cin读取，但是以'！'符为分割
    cout <<stuff <<endl;

    fstream fp("./test/log");
    if(fp.is_open()){
        cerr << "open fail" <<endl;
    }else{
        while(fp){
            getline(fp, stuff, ':');
            cout << "#:" <<stuff <<endl;
        }
    }
    cout << "over~"<<endl;
    fp.close();

    
}


int zSTLDemoMain(){
    _stringDemo();
    return 0;
}