#ifndef DATALISTTHREAD_H
#define DATALISTTHREAD_H

#include <thread>
#include <list>
#include <synchapi.h>
#include <sys/time.h>
#include <iostream>
#include "Cond.h"


/**
 * @brief The DataListThread class
 */

template<class T>
class DataListThread
{
public:
    DataListThread();
    virtual ~DataListThread();

public:
    void start();

    void stop(bool isWait = false);

    int inputData(const T &node);

    void clearData();
    int64_t getTimeStamp_MilliSecond1();

protected:
    bool mIsThreadRunning;
    bool mIsStop;
    void run();

private:
    Cond *mCondition;

    std::list<T> mList;

protected:
    virtual void threadStart();
    virtual void threadStop();
    virtual void dealwithDataNode(const T &node) = 0;

};


template<typename T>
DataListThread<T>::DataListThread()
{
    mIsStop = true;
    mIsThreadRunning = false;
    mCondition = new Cond;
}

template<typename T>
DataListThread<T>::~DataListThread()
{

}

template<typename T>
void DataListThread<T>::start()
{
    mIsStop = false;

//    启动新的线程
    std::thread([&](DataListThread *pointer)
    {
        pointer->run();

    }, this).detach();
}

template<typename T>
void DataListThread<T>::stop(bool isWait)
{
    mIsStop = true;

    if (isWait)
    {
        while(mIsThreadRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

template<typename T>
int DataListThread<T>::inputData(const T &node)
{
    int size = 0;

    mCondition->Lock();
    mList.push_back(node);
    size = mList.size();
    mCondition->Unlock();

    return size;
}

template<typename T>
void DataListThread<T>::clearData()
{
    mCondition->Lock();
    mList.clear();
    mCondition->Unlock();
}
template<typename T>
int64_t DataListThread<T>::getTimeStamp_MilliSecond1()
{
    int mSecond = 0; //当前毫秒数

//#if defined(WIN32)

//    SYSTEMTIME sys;
//    GetLocalTime(&sys);
//    mSecond = sys.wMilliseconds;
//#else

    struct timeval    tv;
    struct timezone tz;

    struct tm         *p;

    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);

    mSecond = tv.tv_usec / 1000;


//#endif
    int64_t timeStamp = ((int64_t)time(NULL)) * 1000 + mSecond;

    return timeStamp;

}

template<typename T>
void DataListThread<T>::run()
{
    mIsThreadRunning = true;
    threadStart();

    while(!mIsStop)
    {
//        int64_t start = getTimeStamp_MilliSecond1();
        mCondition->Lock();

        if (mList.size() <= 0)
        {
            mCondition->Unlock();
//            //要睡。。
//            Sleep(100);
            continue;
        }
        else {
                T node = mList.front();
                mList.pop_front();
                mCondition->Unlock();
                dealwithDataNode(node);
               // int64_t end = getTimeStamp_MilliSecond1();
               // std::cout << end % 1000 << " dealed" << std::endl;
        }
    }

    mList.clear(); //清空队列

    threadStop();

    mIsThreadRunning = false;
}

template<typename T>
void DataListThread<T>::threadStart()
{

}

template<typename T>
void DataListThread<T>::threadStop()
{

}

#endif // DATALISTTHREAD_H
