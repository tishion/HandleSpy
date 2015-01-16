/************************************************************************\
 * File		:	CallStack.h
 * Module	:	CallStack
 * Created	:	shiontian @ 2013-01-01 
 * Description:
 * 用于获取当前函数的堆栈信息的类
 * 
 *
\************************************************************************/
#ifndef __CALLSTATK_H__
#define __CALLSTATK_H__

#include <Windows.h>
#include <tchar.h>
#include "..\SysTypedef.h"

#include <vector>
#pragma comment (lib, "winmm.lib")

#include "CallStackTypeDefine.h"

typedef struct __RAW_MOD_INFO
{
	DWORD dwModuleBase;
	DWORD dwImageSize;
	DWORD dwTimeStamp;
	PWSTR pModPath;

	__RAW_MOD_INFO()
	{
		ZeroMemory(this, sizeof(__RAW_MOD_INFO));
	}
}RAW_MOD_INFO, *PRAW_MOD_INFO;

#define MAX_MODULE_COUNT	256

class CCallStack
{
public:
	//CCallStack(void);
	//~CCallStack(void);
	//static CCallStack* GetInstance();
	// 
	static DWORD GetCurrentCallStack(const PCALL_STACK pcs, DWORD dwMaxFrameCount);
	static BOOL GetModuleInfoFromAddr(DWORD addr, RAW_MOD_INFO& modinfo);
	static void ClearModInfo();

	static  std::vector<MOD_INFO>& GetModInfoVector();

protected:
	static DWORD GetTimeStamp();

private:
	static std::vector<MOD_INFO>	s_vecModInfo;
};

#endif