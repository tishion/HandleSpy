/************************************************************************\
 * File		:	Detoursor.h
 * Module	:	Detours Helper
 * Created	:	shiontian @ 2013-01-01 
 * Description:
 * 用于批量Hook系统API的detours辅助类
 * 
 * Remark:
 *	非多线程安全类
 *
\************************************************************************/
#ifndef __DETOURSOR_H__
#define __DETOURSOR_H__
#pragma once

#include <map>
#include <string>
#include "detours/detours.h"


typedef std::basic_string<TCHAR> StringT;

typedef struct _ADRRESSE_PAIR
{
	PVOID* TrueAddr;
	PVOID FakeAddr;
}ADRRESSE_PAIR, *PADRRESSE_PAIR;

class CDetoursor
{
public:
	static CDetoursor* GetInstance();

	BOOL DoHook();

	BOOL UndoHook();

	BOOL AddAddressPair(LPCTSTR lpName, ADRRESSE_PAIR& sAddrPair);
	BOOL InsertFunction(LPCTSTR lpName, PVOID* pTrueAddr, PVOID FakeAddr);
	BOOL RemoveFunction(LPCTSTR lpName);
	BOOL ClearFunctionTable();

	BOOL HasHooked();

protected:
	CDetoursor(void);
	~CDetoursor(void);

private:
	//CRITICAL_SECTION m_cs;
	static BOOL s_bIsHooked;
	static std::map<StringT, ADRRESSE_PAIR> s_mapFunctionTable;
};
#endif