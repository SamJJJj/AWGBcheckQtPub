#ifndef MUTEX_H
#define MUTEX_H
#ifndef _MSC_VER
#define MINGW
#endif

#if defined(WIN32) && !defined(MINGW)
    #include <winsock2.h>
    #include <windows.h>
//#elif defined(Q_OS_LINUX)
#else
    #include <pthread.h>
#endif

class Mutex
{
public:
    Mutex();
    ~Mutex();

    //确保拥有互斥对象的线程对被保护资源的独自访问
    int Lock() const;

    //释放当前线程拥有的互斥对象，以使其它线程可以拥有互斥对象，对被保护资源进行访问
    int Unlock() const;

private:

#if defined(WIN32) && !defined(MINGW)
     HANDLE m_mutex;
#else
    pthread_mutex_t mutex;
#endif

};

#endif // MUTEX_H
