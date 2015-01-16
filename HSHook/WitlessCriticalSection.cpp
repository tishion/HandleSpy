#include "StdAfx.h"
#include <assert.h>
#include "WitlessCriticalSection.h"

CWitlessCriticalSection::CWitlessCriticalSection(void):
m_lLockCount(0),
m_lOwninThread(0),
m_lRecursionCount(0)
{
}

CWitlessCriticalSection::~CWitlessCriticalSection(void)
{
}

void CWitlessCriticalSection::Enter()
{
	if (::GetCurrentThreadId() == m_lOwninThread)
	{
		m_lRecursionCount++;
	} 
	else
	{
		unsigned long lLoopCount = 0;
		while (0 != ::InterlockedCompareExchange(&m_lLockCount, 1, 0))
		{
			if (lLoopCount++ == 10)
			{
				lLoopCount = 0;
				::SwitchToThread();
			}
		}

		m_lOwninThread = ::GetCurrentThreadId();

		m_lRecursionCount++;
	}
}

void CWitlessCriticalSection::Leave()
{
	assert(m_lLockCount == 1);
	assert(m_lOwninThread != 0);
	assert(m_lRecursionCount > 0);

	if (::GetCurrentThreadId() == m_lOwninThread)
	{
		m_lRecursionCount--;

		if (0 == m_lRecursionCount)
		{
			m_lOwninThread = 0;
			::InterlockedExchange(&m_lLockCount, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 
// inline asm
//void CWitlessCriticalSection::Enter()
//{
//	if (m_lOwninThread == ::GetCurrentThreadId())
//	{
//		m_lRecursionCount++;
//	}
//	else
//	{
//		__asm
//		{
//			mov ecx, this;
//			mov edx, 01h;
//			xor ebx, ebx;
//		_Acquire:
//			xor eax, eax;
//			lock cmpxchg [ecx], edx;
//			je _AcquiredOk;				// m_lTakeCount = 0 ?
//			inc ebx;
//			test ebx, 8000h;
//			jz _Acquire;
//			call SwitchToThread;
//			xor ebx, ebx;
//			jmp _Acquire;
//		_AcquiredOk:
//		}
//
//		m_lOwninThread = ::GetCurrentThreadId();
//		m_lRecursionCount++;
//	}
//
//	return ;
//}
//
//void CWitlessCriticalSection::Leave()
//{
//	if (m_lOwninThread == ::GetCurrentThreadId())
//	{
//		// 当前线程拥有该CS
//		m_lRecursionCount--;
//		if (0 == m_lRecursionCount)
//		{
//			__asm
//			{
//				mov ecx, this
//				lock mov [ecx], 00h;
//			}
//			m_lOwninThread = 0;
//		}
//	}
//}
