/*
 * @Author: your name
 * @Date: 2021-04-12 14:30:23
 * @LastEditTime: 2021-04-12 14:55:43
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \PolteProject\StudyLinuxC\include\classTest.h
 */
class CBase
{
private:
    /* data */
    int name;

public:
    CBase(/* args */);
    ~CBase();
};

class CSonTest1 :virtual public CBase
{
private:
    /* data */
public:
    CSonTest1(/* args */);
    ~CSonTest1();
};

class CSonTest2 :virtual public CBase
{
private:
    /* data */
public:

};

class CSonTest3 :public CSonTest1 ,public CSonTest2
{
private:
    /* data */
public:

};


