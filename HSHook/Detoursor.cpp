/************************************************************************\
 * File		:	Detoursor.cpp
 * Module	:	Detours Helper
 * Created	:	shiontian @ 2013-01-01 
 * Description:
 * 用于批量Hook系统API的detours辅助类
 * 
 * Remark:
 *	非多线程安全类
 *
\************************************************************************/

#include "StdAfx.h"
#include "Detoursor.h"
#include <assert.h>

BOOL CDetoursor::s_bIsHooked = FALSE;
std::map<StringT, ADRRESSE_PAIR> CDetoursor::s_mapFunctionTable;

CDetoursor* CDetoursor::GetInstance()
{
	static CDetoursor sDetoursor;
	return &sDetoursor;
}

CDetoursor::CDetoursor(void)
{
	//::InitializeCriticalSection(&m_cs);
}

CDetoursor::~CDetoursor(void)
{
	//::DeleteCriticalSection(&m_cs);
}

BOOL CDetoursor::DoHook()
{
	if (TRUE == s_bIsHooked)
	{
		return FALSE;
	}

	std::map<StringT, ADRRESSE_PAIR>::const_iterator it;

	if (NO_ERROR != DetourTransactionBegin())
	{
		return FALSE;
	}

	if (NO_ERROR != DetourUpdateThread(GetCurrentThread()))
	{
		DetourTransactionAbort();
		return FALSE;
	}

	for (it = s_mapFunctionTable.begin(); it != s_mapFunctionTable.end(); it++)
	{
		if (NO_ERROR != DetourAttach(it->second.TrueAddr, it->second.FakeAddr))
		{
			DetourTransactionAbort();
			return FALSE;
		}
	}

	if (NO_ERROR != DetourTransactionCommit())
	{
		DetourTransactionAbort();
		return FALSE;
	}

	s_bIsHooked = TRUE;
	return TRUE;
}

BOOL CDetoursor::UndoHook()
{
	if (FALSE == s_bIsHooked)
	{
		return FALSE;
	}

	std::map<StringT, ADRRESSE_PAIR>::const_iterator it;

	if (NO_ERROR != DetourTransactionBegin())
	{
		return FALSE;
	}

	if (NO_ERROR != DetourUpdateThread(GetCurrentThread()))
	{
		DetourTransactionAbort();
		return FALSE;
	}

	for (it = s_mapFunctionTable.begin(); it != s_mapFunctionTable.end(); it++)
	{
		if (NO_ERROR != DetourDetach(it->second.TrueAddr, it->second.FakeAddr))
		{
			DetourTransactionAbort();
			return FALSE;
		}
	}

	if (NO_ERROR != DetourTransactionCommit())
	{
		DetourTransactionAbort();
		return FALSE;
	}

	s_bIsHooked = FALSE;
	return TRUE;
}

BOOL CDetoursor::AddAddressPair(LPCTSTR lpName, ADRRESSE_PAIR& sAddrPair)
{
	if (TRUE == s_bIsHooked)
	{
		return FALSE;
	}

	StringT strName = lpName;
	std::map<StringT, ADRRESSE_PAIR>::iterator it;
	it = s_mapFunctionTable.find(strName);
	if (it == s_mapFunctionTable.end())
	{
		s_mapFunctionTable[strName] = sAddrPair;
	}
	else
	{
		it->second.TrueAddr = sAddrPair.TrueAddr;
		it->second.FakeAddr = sAddrPair.FakeAddr;
	}
	
	return TRUE;
}

BOOL CDetoursor::InsertFunction(LPCTSTR lpName, PVOID* pTrueAddr, PVOID FakeAddr)
{
	if (NULL == lpName || NULL == *pTrueAddr || NULL == FakeAddr)
	{
		assert(false);
		return FALSE;
	}

	ADRRESSE_PAIR sAddrPair;
	sAddrPair.TrueAddr = pTrueAddr;
	sAddrPair.FakeAddr = FakeAddr;

	return AddAddressPair(lpName, sAddrPair);
}

BOOL CDetoursor::RemoveFunction(LPCTSTR lpName)
{
	if (TRUE == s_bIsHooked)
	{
		return FALSE;
	}

	StringT strName = lpName;
	std::map<StringT, ADRRESSE_PAIR>::iterator it;
	it = s_mapFunctionTable.find(strName);
	if (it == s_mapFunctionTable.end())
	{
		return TRUE;
	}
	else
	{
		s_mapFunctionTable.erase(it);
	}

	return TRUE;
}

BOOL CDetoursor::ClearFunctionTable()
{
	if (TRUE == s_bIsHooked)
	{
		return FALSE;
	}
	s_mapFunctionTable.clear();
	return TRUE;
}

BOOL CDetoursor::HasHooked()
{
	return s_bIsHooked;
}