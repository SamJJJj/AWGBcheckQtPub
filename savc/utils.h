#ifndef MYUTILS_H
#define MYUTILS_H

#include <windows.h>


#define PRINT(x, ...)	{char temp[512];sprintf_s(temp,512,"[%s, %d]: "##x, __FILE__, __LINE__, ##__VA_ARGS__);OutputDebugStringA(temp);}

class CritSec
{
public:
	CritSec() { InitializeCriticalSection(&m_CritSec); };
	~CritSec() { DeleteCriticalSection(&m_CritSec); };
	void Lock() { EnterCriticalSection(&m_CritSec); };
	void UnLock() { LeaveCriticalSection(&m_CritSec); };

private:
	CRITICAL_SECTION m_CritSec;
};

class AutoLock
{
public:
	AutoLock(CritSec * plock) {
		m_pLock = plock;
		m_pLock->Lock();
	};

	~AutoLock() {
		m_pLock->UnLock();
	};

private:
	CritSec * m_pLock;
};

#endif // MYUTILS_H
#pragma once
