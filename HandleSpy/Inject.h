/************************************************************************\
 * File		:	Inject.h
 * Module	:	Inject
 * Created	:	shiontian @ 2013-01-05
 * Description:
 * 用于远程线程注入和反注入的函数
 * 
 *
\************************************************************************/

#include "stdafx.h"
#define I_ERROR_OPENPROCESS			0X20001000
#define I_ERROR_ALLOCMEMORY			0x20001001
#define I_ERROR_WRITEMEMORY			0x20001002
#define I_ERROR_GETLOADLIBRARYFUNC	0x20001003
#define I_ERROR_GETFREELIBRARYFUNC	0x20001004
#define I_ERROR_CREATEREMOTEPROCESS	0x20001005

BOOL EnableDebugPrivilege();
BOOL Inject(DWORD dwProcessID, LPCSTR lpDllPath, PDWORD phModule);
BOOL UnLoadModule(DWORD dwProcessID, HMODULE hModule);