#include "Mutex.h"

Mutex::Mutex()
{
#if defined(WIN32) && !defined(MINGW)
    m_mutex = ::CreateMutex(NULL, FALSE, NULL);
#else
    pthread_mutex_init(&mutex, NULL);
#endif

}

Mutex::~Mutex()
{
#if defined(WIN32) && !defined(MINGW)
    ::CloseHandle(m_mutex);
#else
     pthread_mutex_destroy(&mutex);
#endif

}

int Mutex::Lock() const
{
#if defined(WIN32) && !defined(MINGW)
    DWORD ret = WaitForSingleObject(m_mutex, INFINITE);
#else
    int ret = pthread_mutex_lock((pthread_mutex_t*)&mutex);
#endif

    return ret;

}

int Mutex::Unlock() const
{
#if defined(WIN32) && !defined(MINGW)
    bool ret = ::ReleaseMutex(m_mutex);
#else
    int ret = pthread_mutex_unlock((pthread_mutex_t*)&mutex);
#endif
    return ret;
}

