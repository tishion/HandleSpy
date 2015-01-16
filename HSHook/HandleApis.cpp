/************************************************************************\
* File		:	HandleApis.cpp
* Module	:	Detours Helper
* Created	:	shiontian @ 2013-01-01 
* Description:
* 需要Hook的目标函数的原始地址和假函数列表
* 
* Fuck！！！写这个文件就是噩梦啊！！！2013/01/16 shiontian
* 
*
\************************************************************************/

#include "stdafx.h"

#ifndef NT_LAYER_FUNCTION_HOOK
#include <Windows.h>
#include <TlHelp32.h>
#include "ApiIndex.h"
#include "detours/detours.h"
#include "dprintf/DPrintf.h"
#include "DetoursorHelper.h"
#include "callstack/CallStack.h"
#include "StackStorage.h"

#define MAX_FRAME_COUNT		128

class CFunCallerLogger
{
public:
	CFunCallerLogger(LPCTSTR name)
	{
		if (NULL == name)
		{
			m_strName = _T("Unknow_Function_Name");
		}
		else
		{
			m_strName = name;
		}

		DPrintf(_T("+++:%s\r\n"), m_strName.c_str());
	}

	~CFunCallerLogger()
	{
		DPrintf(_T("---:%s\r\n"), m_strName.c_str());
	}

private:
	StringT m_strName;
};
#ifndef _DEBUG
#define CFunCallerLogger {}
#define logger	__noop
#endif

extern CWitlessCriticalSection g_csForStackDataBuffer;

#define GetCallStackOnNotNull(x, type)	\
	do \
	{ \
		if (NULL != x && INVALID_HANDLE_VALUE != x) \
		{ \
			EnterCriticalSection(g_csForStackDataBuffer); \
			PCALL_STACK pCallStack = (PCALL_STACK)(CStackStorage::GetInstance()->GetStackDataBufferTail()); \
			DWORD dwBytesWritten = CCallStack::GetCurrentCallStack(pCallStack, MAX_FRAME_COUNT); \
			pCallStack->Type = type; \
			pCallStack->Handle = x; \
			pCallStack->Handle2 = NULL; \
			CStackStorage::GetInstance()->IncreaseStackDataLen(dwBytesWritten); \
			LeaveCriticalSection(g_csForStackDataBuffer); \
		} \
	} while (0)


#define GetCallStackOnErrorSuccess(x, type, h)	\
	do \
	{ \
		if (ERROR_SUCCESS == x) \
		{ \
			EnterCriticalSection(g_csForStackDataBuffer); \
			PCALL_STACK pCallStack = (PCALL_STACK)(CStackStorage::GetInstance()->GetStackDataBufferTail()); \
			DWORD dwBytesWritten = CCallStack::GetCurrentCallStack(pCallStack, MAX_FRAME_COUNT); \
			pCallStack->Type = type; \
			pCallStack->Handle = h; \
			pCallStack->Handle2 = NULL; \
			CStackStorage::GetInstance()->IncreaseStackDataLen(dwBytesWritten); \
			LeaveCriticalSection(g_csForStackDataBuffer); \
		} \
	} while (0)

#define GetCallStackOnTrue(x, type, h, h2)	\
	do \
	{ \
		if (TRUE == x) \
		{ \
			EnterCriticalSection(g_csForStackDataBuffer); \
			PCALL_STACK pCallStack = (PCALL_STACK)(CStackStorage::GetInstance()->GetStackDataBufferTail()); \
			DWORD dwBytesWritten = CCallStack::GetCurrentCallStack(pCallStack, MAX_FRAME_COUNT); \
			pCallStack->Type = type; \
			pCallStack->Handle = h; \
			pCallStack->Handle2 = h2; \
			CStackStorage::GetInstance()->IncreaseStackDataLen(dwBytesWritten); \
			LeaveCriticalSection(g_csForStackDataBuffer); \
		} \
	} while (0)


#define GetCallStackOnNtSuccess(x, type, h)	\
	do \
	{ \
		if (NT_SUCCESS(x)) \
		{ \
			EnterCriticalSection(g_csForStackDataBuffer); \
			PCALL_STACK pCallStack = (PCALL_STACK)(CStackStorage::GetInstance()->GetStackDataBufferTail()); \
			DWORD dwBytesWritten = CCallStack::GetCurrentCallStack(pCallStack, MAX_FRAME_COUNT); \
			pCallStack->Type = type; \
			pCallStack->Handle = h; \
			pCallStack->Handle2 = NULL; \
			CStackStorage::GetInstance()->IncreaseStackDataLen(dwBytesWritten); \
			LeaveCriticalSection(g_csForStackDataBuffer); \
		} \
	} while (0)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
EXTERN_C {

typedef  LONG (WINAPI * Type_RegCreateKeyA)(HKEY,LPCSTR,PHKEY);
static LONG (WINAPI * True_RegCreateKeyA)(
	__in HKEY hKey,
	__in_opt LPCSTR lpSubKey,
	__out PHKEY phkResult) = (Type_RegCreateKeyA)DetourFindFunction("Advapi32.dll", "RegCreateKeyA");

LONG WINAPI Fake_RegCreateKeyA(
							   __in HKEY hKey,
							   __in_opt LPCSTR lpSubKey,
							   __out PHKEY phkResult)
{
	CFunCallerLogger logger(_T("RegCreateKeyA"));

	LONG lRet = True_RegCreateKeyA(
		hKey, 
		lpSubKey, 
		phkResult);

	GetCallStackOnErrorSuccess(lRet, Index_RegCreateKeyA, *phkResult);

	return lRet;
}

typedef  LONG (WINAPI * Type_RegCreateKeyW)(HKEY,LPCWSTR,PHKEY);
static LONG (WINAPI * True_RegCreateKeyW)(
	__in HKEY hKey,
	__in_opt LPCWSTR lpSubKey,
	__out PHKEY phkResult) = (Type_RegCreateKeyW)DetourFindFunction("Advapi32.dll", "RegCreateKeyW");

LONG WINAPI Fake_RegCreateKeyW(
							   __in HKEY hKey,
							   __in_opt LPCWSTR lpSubKey,
							   __out PHKEY phkResult)
{
	CFunCallerLogger logger(_T("RegCreateKeyW"));

	LONG lRet = True_RegCreateKeyW(
		hKey, 
		lpSubKey, 
		phkResult);

	GetCallStackOnErrorSuccess(lRet, Index_RegCreateKeyW, *phkResult);

	return lRet;
}

typedef LONG (WINAPI * Type_RegCreateKeyExA)(HKEY,LPCSTR,DWORD,LPSTR,DWORD,REGSAM,LPSECURITY_ATTRIBUTES,PHKEY,LPDWORD);
static LONG (WINAPI * True_RegCreateKeyExA)(
	__in HKEY hKey,
	__in LPCSTR lpSubKey,
	__reserved DWORD Reserved,
	__in_opt LPSTR lpClass,
	__in DWORD dwOptions,
	__in REGSAM samDesired,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__out PHKEY phkResult,
	__out_opt LPDWORD lpdwDisposition) = (Type_RegCreateKeyExA)DetourFindFunction("Advapi32.dll", "RegCreateKeyExA");

LONG WINAPI Fake_RegCreateKeyExA(
								 __in HKEY hKey,
								 __in LPCSTR lpSubKey,
								 __reserved DWORD Reserved,
								 __in_opt LPSTR lpClass,
								 __in DWORD dwOptions,
								 __in REGSAM samDesired,
								 __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								 __out PHKEY phkResult,
								 __out_opt LPDWORD lpdwDisposition)
{
	CFunCallerLogger logger(_T("RegCreateKeyExA"));

	LONG lRet = True_RegCreateKeyExA(
		hKey, 
		lpSubKey, 
		Reserved,
		lpClass,
		dwOptions,
		samDesired,
		lpSecurityAttributes,
		phkResult,
		lpdwDisposition);

	GetCallStackOnErrorSuccess(lRet, Index_RegCreateKeyExA, *phkResult);

	return lRet;
}

typedef LONG (WINAPI * Type_RegCreateKeyExW)(HKEY,LPCWSTR,DWORD,LPWSTR,DWORD,REGSAM,LPSECURITY_ATTRIBUTES,PHKEY,LPDWORD);
static LONG (WINAPI * True_RegCreateKeyExW)(
	__in HKEY hKey,
	__in LPCWSTR lpSubKey,
	__reserved DWORD Reserved,
	__in_opt LPWSTR lpClass,
	__in DWORD dwOptions,
	__in REGSAM samDesired,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__out PHKEY phkResult,
	__out_opt LPDWORD lpdwDisposition) = (Type_RegCreateKeyExW)DetourFindFunction("Advapi32.dll", "RegCreateKeyExW");

LONG WINAPI Fake_RegCreateKeyExW(
								 __in HKEY hKey,
								 __in LPCWSTR lpSubKey,
								 __reserved DWORD Reserved,
								 __in_opt LPWSTR lpClass,
								 __in DWORD dwOptions,
								 __in REGSAM samDesired,
								 __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								 __out PHKEY phkResult,
								 __out_opt LPDWORD lpdwDisposition)
{
	CFunCallerLogger logger(_T("RegCreateKeyExW"));

	LONG lRet = True_RegCreateKeyExW(
		hKey, 
		lpSubKey, 
		Reserved,
		lpClass,
		dwOptions,
		samDesired,
		lpSecurityAttributes,
		phkResult,
		lpdwDisposition);

	GetCallStackOnErrorSuccess(lRet, Index_RegCreateKeyExW, *phkResult);

	return lRet;
}

typedef LONG (WINAPI * Type_RegOpenKeyA)(HKEY,LPCSTR,PHKEY);
static LONG (WINAPI * True_RegOpenKeyA)(
										__in HKEY hKey,
										__in_opt LPCSTR lpSubKey,
										__out PHKEY phkResult) = (Type_RegOpenKeyA)DetourFindFunction("Advapi32.dll", "RegOpenKeyA");

LONG WINAPI Fake_RegOpenKeyA(
							 __in HKEY hKey,
							 __in_opt LPCSTR lpSubKey,
							 __out PHKEY phkResult)
{
	CFunCallerLogger logger(_T("RegOpenKeyA"));

	LONG lRet = True_RegOpenKeyA(
		hKey, 
		lpSubKey, 
		phkResult);

	GetCallStackOnErrorSuccess(lRet, Index_RegOpenKeyA, *phkResult);

	return lRet;
}

typedef LONG (WINAPI * Type_RegOpenKeyW)(HKEY,LPCWSTR,PHKEY);
static LONG (WINAPI * True_RegOpenKeyW)(
										__in HKEY hKey,
										__in_opt LPCWSTR lpSubKey,
										__out PHKEY phkResult) = (Type_RegOpenKeyW)DetourFindFunction("Advapi32.dll", "RegOpenKeyW");

LONG WINAPI Fake_RegOpenKeyW(
							 __in HKEY hKey,
							 __in_opt LPCWSTR lpSubKey,
							 __out PHKEY phkResult)
{
	CFunCallerLogger logger(_T("RegOpenKeyW"));

	LONG lRet = True_RegOpenKeyW(
		hKey, 
		lpSubKey, 
		phkResult);

	GetCallStackOnErrorSuccess(lRet, Index_RegOpenKeyW, *phkResult);

	return lRet;
}


typedef LONG (WINAPI * Type_RegOpenKeyExA)(HKEY,LPCSTR,DWORD,REGSAM,PHKEY);
static LONG (WINAPI * True_RegOpenKeyExA)(
	__in HKEY hKey,
	__in_opt LPCSTR lpSubKey,
	__reserved DWORD ulOptions,
	__in REGSAM samDesired,
	__out PHKEY phkResult) = (Type_RegOpenKeyExA)DetourFindFunction("Advapi32.dll", "RegOpenKeyExA");

LONG WINAPI Fake_RegOpenKeyExA(
							   __in HKEY hKey,
							   __in_opt LPCSTR lpSubKey,
							   __reserved DWORD ulOptions,
							   __in REGSAM samDesired,
							   __out PHKEY phkResult)
{
	CFunCallerLogger logger(_T("RegOpenKeyExA"));

	LONG lRet = True_RegOpenKeyExA(
		hKey, 
		lpSubKey, 
		ulOptions,
		samDesired,
		phkResult);

	GetCallStackOnErrorSuccess(lRet, Index_RegOpenKeyExA, *phkResult);

	return lRet;
}

typedef LONG (WINAPI * Type_RegOpenKeyExW)(HKEY,LPCWSTR,DWORD,REGSAM,PHKEY);
static LONG (WINAPI * True_RegOpenKeyExW)(
	__in HKEY hKey,
	__in_opt LPCWSTR lpSubKey,
	__reserved DWORD ulOptions,
	__in REGSAM samDesired,
	__out PHKEY phkResult) = (Type_RegOpenKeyExW)DetourFindFunction("Advapi32.dll", "RegOpenKeyExW");

LONG WINAPI Fake_RegOpenKeyExW(
							   __in HKEY hKey,
							   __in_opt LPCWSTR lpSubKey,
							   __reserved DWORD ulOptions,
							   __in REGSAM samDesired,
							   __out PHKEY phkResult)
{
	CFunCallerLogger logger(_T("RegOpenKeyExW"));

	LONG lRet = True_RegOpenKeyExW(
		hKey, 
		lpSubKey, 
		ulOptions,
		samDesired,
		phkResult);

	GetCallStackOnErrorSuccess(lRet, Index_RegOpenKeyExW, *phkResult);

	return lRet;
}

typedef LONG (APIENTRY * Type_RegCloseKey)(HKEY);
static LONG (APIENTRY * True_RegCloseKey)(
	__in HKEY hKey) = (Type_RegCloseKey)DetourFindFunction("Advapi32.dll", "RegCloseKey");

LONG APIENTRY Fake_RegCloseKey(
							   __in HKEY hKey
							   )
{
	CFunCallerLogger logger(_T("RegCloseKey"));

	LONG lRet = True_RegCloseKey(
		hKey);

	GetCallStackOnErrorSuccess(lRet, Index_RegCloseKey, hKey);

	return lRet;
}
//key-end

typedef NTSTATUS (NTAPI * Type_NtClose)(HANDLE);
static NTSTATUS (NTAPI * True_NtClose)(
		IN HANDLE  Handle) = (Type_NtClose)DetourFindFunction("ntdll.dll", "NtClose");

NTSTATUS NTAPI Fake_NtClose(
		IN HANDLE  Handle
		)
{
	CFunCallerLogger logger(_T("NtClose"));

	NTSTATUS status = True_NtClose(Handle);

	GetCallStackOnNtSuccess(status, Index_NtClose, Handle);

	return status;
}


//CloseHandle
typedef BOOL (WINAPI * Type_CloseHandle)(HANDLE);
static BOOL (WINAPI * True_CloseHandle)(
										__in HANDLE hObject) = (Type_CloseHandle)DetourFindFunction("Kernel32.dll", "CloseHandle");

BOOL WINAPI Fake_CloseHandle(
							 __in HANDLE hObject)
{
	CFunCallerLogger logger(_T("CloseHandle"));

	BOOL bRet = True_CloseHandle(
		hObject);

	GetCallStackOnTrue(bRet, Index_CloseHandle, hObject, NULL);

	return bRet;
}

//CloseDesktop
typedef BOOL (WINAPI * Type_CloseDesktop)(HDESK);
static BOOL (WINAPI * True_CloseDesktop)(
	__in HDESK hDesktop) = (Type_CloseDesktop)DetourFindFunction("User32.dll", "CloseDesktop");

BOOL WINAPI Fake_CloseDesktop(
							  __in HDESK hDesktop)
{
	CFunCallerLogger logger(_T("CloseDesktop"));

	BOOL bRet = True_CloseDesktop(
		hDesktop);

	GetCallStackOnTrue(bRet, Index_CloseDesktop, hDesktop, NULL);

	return bRet;
}

//CloseWindowStation
typedef BOOL (WINAPI * Type_CloseWindowstation)(HWINSTA);
static BOOL (WINAPI * True_CloseWindowStation)(
	__in HWINSTA hWinSta) = (Type_CloseWindowstation)DetourFindFunction("User32.dll", "CloseWindowStation");

BOOL WINAPI Fake_CloseWindowStation(
									__in HWINSTA hWinSta)
{
	CFunCallerLogger logger(_T("CloseWindowStation"));

	BOOL bRet = True_CloseWindowStation(
		hWinSta);

	GetCallStackOnTrue(bRet, Index_CloseWindowStation, hWinSta, NULL);

	return bRet;
}

//FindClose;
typedef BOOL (WINAPI * Type_FindClose)(HANDLE);
static BOOL (WINAPI * True_FindClose)(
									  __inout HANDLE hFindFile) = (Type_FindClose)DetourFindFunction("Kernel32.dll", "FindClose");

BOOL WINAPI Fake_FindClose(
						   __inout HANDLE hFindFile)
{
	CFunCallerLogger logger(_T("FindClose"));

	BOOL bRet = True_FindClose(
		hFindFile);

	GetCallStackOnTrue(bRet, Index_FindClose, hFindFile, NULL);

	return bRet;
}


/*
* Functions to be hooked
* Get all the addresses of the function to be hooked
*/
//////////////////////////////////////////////////////////////////////////
//File
/* CreateFileA */
typedef HANDLE (WINAPI * Type_CreateFileA)(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
static __out HANDLE (WINAPI * True_CreateFileA)(
	__in     LPCSTR lpFileName,
	__in     DWORD dwDesiredAccess,
	__in     DWORD dwShareMode,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__in     DWORD dwCreationDisposition,
	__in     DWORD dwFlagsAndAttributes,
	__in_opt HANDLE hTemplateFile) = (Type_CreateFileA)DetourFindFunction("Kernel32.dll", "CreateFileA");

__out HANDLE WINAPI Fake_CreateFileA(
									 __in     LPCSTR lpFileName,
									 __in     DWORD dwDesiredAccess,
									 __in     DWORD dwShareMode,
									 __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
									 __in     DWORD dwCreationDisposition,
									 __in     DWORD dwFlagsAndAttributes,
									 __in_opt HANDLE hTemplateFile)
{
	CFunCallerLogger logger(_T("CreateFileA"));

	HANDLE hRet = True_CreateFileA(
		lpFileName, 
		dwDesiredAccess,
		dwShareMode, 
		lpSecurityAttributes, 
		dwCreationDisposition, 
		dwFlagsAndAttributes, 
		hTemplateFile);

	GetCallStackOnNotNull(hRet, Index_CreateFileA);

	return hRet;
}

typedef HFILE (WINAPI * Type_OpenFile)(LPCSTR, LPOFSTRUCT, UINT);
static __out HFILE (WINAPI * True_OpenFile)(
			   LPCSTR lpFileName,
			   LPOFSTRUCT lpReOpenBuff,
			   UINT uStyle) = (Type_OpenFile)DetourFindFunction("Kernel32.dll", "OpenFile");
__out HFILE WINAPI Fake_OpenFile(
								 LPCSTR lpFileName,
								 LPOFSTRUCT lpReOpenBuff,
								 UINT uStyle)
{
	CFunCallerLogger logger(_T("OpenFile"));

	HFILE hRet = True_OpenFile(
		lpFileName,
		lpReOpenBuff,
		uStyle);

	GetCallStackOnNotNull((HANDLE)hRet, Index_OpenFile);
	
	return hRet;
}



/* CreateFileW */
typedef HANDLE (WINAPI * Type_CreateFileW)(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
static __out HANDLE (WINAPI * True_CreateFileW)(
	__in     LPCWSTR lpFileName,
	__in     DWORD dwDesiredAccess,
	__in     DWORD dwShareMode,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__in     DWORD dwCreationDisposition,
	__in     DWORD dwFlagsAndAttributes,
	__in_opt HANDLE hTemplateFile) = (Type_CreateFileW)DetourFindFunction("Kernel32.dll", "CreateFileW");

__out HANDLE WINAPI Fake_CreateFileW(
									 __in     LPCWSTR lpFileName,
									 __in     DWORD dwDesiredAccess,
									 __in     DWORD dwShareMode,
									 __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
									 __in     DWORD dwCreationDisposition,
									 __in     DWORD dwFlagsAndAttributes,
									 __in_opt HANDLE hTemplateFile)
{
	CFunCallerLogger logger(_T("CreateFileW"));

	HANDLE hRet = True_CreateFileW(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);

	GetCallStackOnNotNull(hRet, Index_CreateFileW);

	return hRet;
}


//////////////////////////////////////////////////////////////////////////
//FileMapping
typedef HANDLE (WINAPI * Type_CreateFileMappingA)(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCSTR);
static __out HANDLE (WINAPI * True_CreateFileMappingA)(
	__in     HANDLE hFile,
	__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	__in     DWORD flProtect,
	__in     DWORD dwMaximumSizeHigh,
	__in     DWORD dwMaximumSizeLow,
	__in_opt LPCSTR lpName) = (Type_CreateFileMappingA)DetourFindFunction("Kernel32.dll", "CreateFileMappingA");

__out HANDLE WINAPI Fake_CreateFileMappingA(
	__in     HANDLE hFile,
	__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	__in     DWORD flProtect,
	__in     DWORD dwMaximumSizeHigh,
	__in     DWORD dwMaximumSizeLow,
	__in_opt LPCSTR lpName)
{
	CFunCallerLogger logger(_T("CreateFileMappingA"));

	HANDLE hRet = True_CreateFileMappingA(
		hFile,
		lpFileMappingAttributes,
		flProtect,
		dwMaximumSizeHigh,
		dwMaximumSizeLow,
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateFileMappingA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_CreateFileMappingW)(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCWSTR);
static __out HANDLE (WINAPI * True_CreateFileMappingW)(    
	__in     HANDLE hFile,
	__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	__in     DWORD flProtect,
	__in     DWORD dwMaximumSizeHigh,
	__in     DWORD dwMaximumSizeLow,
	__in_opt LPCWSTR lpName) = (Type_CreateFileMappingW)DetourFindFunction("Kernel32.dll", "CreateFileMappingW");

__out HANDLE WINAPI Fake_CreateFileMappingW(
	__in     HANDLE hFile,
	__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	__in     DWORD flProtect,
	__in     DWORD dwMaximumSizeHigh,
	__in     DWORD dwMaximumSizeLow,
	__in_opt LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("CreateFileMappingW"));

	HANDLE hRet = True_CreateFileMappingW(
		hFile,
		lpFileMappingAttributes,
		flProtect,
		dwMaximumSizeHigh,
		dwMaximumSizeLow,
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateFileMappingW);

	return hRet;
}

typedef HANDLE (WINAPI * Type_OpenFileMappingA)(DWORD,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_OpenFileMappingA)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpName) = (Type_OpenFileMappingA)DetourFindFunction("Kernel32.dll", "OpenFileMappingA");

__out HANDLE WINAPI Fake_OpenFileMappingA(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpName)
{
	CFunCallerLogger logger(_T("OpenFileMappingA"));

	HANDLE hRet = True_OpenFileMappingA(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenFileMappingA);

	return hRet;
}


typedef HANDLE (WINAPI * Type_OpenFileMappingW)(DWORD,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_OpenFileMappingW)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpName) = (Type_OpenFileMappingW)DetourFindFunction("Kernel32.dll", "OpenFileMappingW");

__out HANDLE WINAPI Fake_OpenFileMappingW(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("OpenFileMappingW"));

	HANDLE hRet = True_OpenFileMappingW(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenFileMappingW);

	return hRet;
}

//////////////////////////////////////////////////////////////////////////
//Process only
typedef BOOL (WINAPI * Type_CreateProcessA)(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
static BOOL (WINAPI * True_CreateProcessA)(
	__in_opt    LPCSTR lpApplicationName,
	__inout_opt LPSTR lpCommandLine,
	__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in        BOOL bInheritHandles,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOA lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation) = (Type_CreateProcessA)DetourFindFunction("Kernel32.dll", "CreateProcessA");

BOOL WINAPI Fake_CreateProcessA(
								__in_opt    LPCSTR lpApplicationName,
								__inout_opt LPSTR lpCommandLine,
								__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
								__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
								__in        BOOL bInheritHandles,
								__in        DWORD dwCreationFlags,
								__in_opt    LPVOID lpEnvironment,
								__in_opt    LPCSTR lpCurrentDirectory,
								__in        LPSTARTUPINFOA lpStartupInfo,
								__out       LPPROCESS_INFORMATION lpProcessInformation)
{
	CFunCallerLogger logger(_T("CreateProcessA"));

	BOOL bRet = True_CreateProcessA(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation);

	GetCallStackOnTrue(bRet, Index_CreateProcessA, lpProcessInformation->hProcess, lpProcessInformation->hThread);

	return bRet;
}

typedef BOOL (WINAPI * Type_CreateProcessW)(LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);
static BOOL (WINAPI * True_CreateProcessW)(
	__in_opt    LPCWSTR lpApplicationName,
	__inout_opt LPWSTR lpCommandLine,
	__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in        BOOL bInheritHandles,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCWSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOW lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation) = (Type_CreateProcessW)DetourFindFunction("Kernel32.dll", "CreateProcessW");

BOOL WINAPI Fake_CreateProcessW(
								__in_opt    LPCWSTR lpApplicationName,
								__inout_opt LPWSTR lpCommandLine,
								__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
								__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
								__in        BOOL bInheritHandles,
								__in        DWORD dwCreationFlags,
								__in_opt    LPVOID lpEnvironment,
								__in_opt    LPCWSTR lpCurrentDirectory,
								__in        LPSTARTUPINFOW lpStartupInfo,
								__out       LPPROCESS_INFORMATION lpProcessInformation)
{
	CFunCallerLogger logger(_T("CreateProcessW"));

	BOOL bRet = True_CreateProcessW(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation);

	GetCallStackOnTrue(bRet, Index_CreateProcessW, lpProcessInformation->hProcess, lpProcessInformation->hThread);

	return bRet;
}

typedef HANDLE (WINAPI * Type_OpenProcess)(DWORD,BOOL,DWORD);
static __out HANDLE (WINAPI * True_OpenProcess)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in DWORD dwProcessId) = (Type_OpenProcess)DetourFindFunction("Kernel32.dll", "OpenProcess");

__out HANDLE WINAPI Fake_OpenProcess(
									 __in DWORD dwDesiredAccess, 
									 __in BOOL bInheritHandle, 
									 __in DWORD dwProcessId)
{
	CFunCallerLogger logger(_T("OpenProcess"));

	HANDLE hRet = True_OpenProcess(
		dwDesiredAccess, 
		bInheritHandle, 
		dwProcessId);

	GetCallStackOnNotNull(hRet, Index_OpenProcess);

	return hRet;
}


//////////////////////////////////////////////////////////////////////////
//Thread only
typedef HANDLE (WINAPI * Type_CreateThread)(LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
static __out HANDLE (WINAPI * True_CreateThread)(
	__in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in      SIZE_T dwStackSize,
	__in      LPTHREAD_START_ROUTINE lpStartAddress,
	__in_opt  LPVOID lpParameter,
	__in      DWORD dwCreationFlags,
	__out_opt LPDWORD lpThreadId) = (Type_CreateThread)DetourFindFunction("Kernel32.dll", "CreateThread");

__out HANDLE WINAPI Fake_CreateThread(
									  __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
									  __in      SIZE_T dwStackSize,
									  __in      LPTHREAD_START_ROUTINE lpStartAddress,
									  __in_opt  LPVOID lpParameter,
									  __in      DWORD dwCreationFlags,
									  __out_opt LPDWORD lpThreadId)
{
	CFunCallerLogger logger(_T("CreateThread"));

	HANDLE hRet = True_CreateThread(
		lpThreadAttributes, 
		dwStackSize,
		lpStartAddress,
		lpParameter,
		dwCreationFlags,
		lpThreadId);

	GetCallStackOnNotNull(hRet, Index_CreateThread);

	return hRet;
}


typedef HANDLE (WINAPI * Type_CreateRemoteThread)(HANDLE,LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
static __out HANDLE (WINAPI * True_CreateRemoteThread)(
	__in      HANDLE hProcess,
	__in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in      SIZE_T dwStackSize,
	__in      LPTHREAD_START_ROUTINE lpStartAddress,
	__in_opt  LPVOID lpParameter,
	__in      DWORD dwCreationFlags,
	__out_opt LPDWORD lpThreadId) = (Type_CreateRemoteThread)DetourFindFunction("Kernel32.dll", "CreateRemoteThread");

__out HANDLE WINAPI Fake_CreateRemoteThread(
	__in      HANDLE hProcess,
	__in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in      SIZE_T dwStackSize,
	__in      LPTHREAD_START_ROUTINE lpStartAddress,
	__in_opt  LPVOID lpParameter,
	__in      DWORD dwCreationFlags,
	__out_opt LPDWORD lpThreadId)
{
	CFunCallerLogger logger(_T("CreateRemoteThread"));

	HANDLE hRet = True_CreateRemoteThread(
		hProcess,
		lpThreadAttributes,
		dwStackSize,
		lpStartAddress,
		lpParameter,
		dwCreationFlags,
		lpThreadId);

	GetCallStackOnNotNull(hRet, Index_CreateRemoteThread);

	return hRet;
}

typedef HANDLE (WINAPI * Type_OpenThread)(DWORD,BOOL,DWORD);
static __out HANDLE (WINAPI * True_OpenThread)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in DWORD dwThreadId) = (Type_OpenThread)DetourFindFunction("Kernel32.dll", "OpenThread");

__out HANDLE WINAPI Fake_OpenThread(
									__in DWORD dwDesiredAccess,
									__in BOOL bInheritHandle,
									__in DWORD dwThreadId)
{
	CFunCallerLogger logger(_T("OpenThread"));

	HANDLE hRet = True_OpenThread(
		dwDesiredAccess,
		bInheritHandle, 
		dwThreadId);

	GetCallStackOnNotNull(hRet, Index_OpenThread);

	return hRet;
}

//////////////////////////////////////////////////////////////////////////
//Event
typedef HANDLE (WINAPI * Type_CreateEventA)(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_CreateEventA)(
	__in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
	__in     BOOL bManualReset,
	__in     BOOL bInitialState,
	__in_opt LPCSTR lpName) = (Type_CreateEventA)DetourFindFunction("Kernel32.dll", "CreateEventA");

__out HANDLE WINAPI Fake_CreateEventA(
									  __in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
									  __in     BOOL bManualReset,
									  __in     BOOL bInitialState,
									  __in_opt LPCSTR lpName)
{
	CFunCallerLogger logger(_T("CreateEventA"));

	HANDLE hRet = True_CreateEventA(
		lpEventAttributes, 
		bManualReset, 
		bInitialState, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateEventA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_CreateEventW)(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_CreateEventW)(
	__in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
	__in     BOOL bManualReset,
	__in     BOOL bInitialState,
	__in_opt LPCWSTR lpName) = (Type_CreateEventW)DetourFindFunction("Kernel32.dll", "CreateEventW");

__out HANDLE WINAPI Fake_CreateEventW(
									  __in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
									  __in     BOOL bManualReset,
									  __in     BOOL bInitialState,
									  __in_opt LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("CreateEventW"));

	HANDLE hRet = True_CreateEventW(
		lpEventAttributes, 
		bManualReset, 
		bInitialState, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateEventW);

	return hRet;
}


typedef HANDLE (WINAPI * Type_OpenEventA)(DWORD,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_OpenEventA)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpName) = (Type_OpenEventA)DetourFindFunction("Kernel32.dll", "OpenEventA");

__out HANDLE WINAPI Fake_OpenEventA(
									__in DWORD dwDesiredAccess,
									__in BOOL bInheritHandle,
									__in LPCSTR lpName)
{
	CFunCallerLogger logger(_T("OpenEventA"));

	HANDLE hRet = True_OpenEventA(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenEventA);

	return hRet;
}


typedef HANDLE (WINAPI * Type_OpenEventW)(DWORD,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_OpenEventW)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpName) = (Type_OpenEventW)DetourFindFunction("Kernel32.dll", "OpenEventW");

__out HANDLE WINAPI Fake_OpenEventW(
									__in DWORD dwDesiredAccess,
									__in BOOL bInheritHandle,
									__in LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("OpenEventW"));

	HANDLE hRet = True_OpenEventW(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenEventW);

	return hRet;
}

//////////////////////////////////////////////////////////////////////////
//Mutex
typedef HANDLE (WINAPI * Type_CreateMutexA)(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_CreateMutexA)(
	__in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
	__in     BOOL bInitialOwner,
	__in_opt LPCSTR lpName) = (Type_CreateMutexA)DetourFindFunction("Kernel32.dll", "CreateMutexA");

__out HANDLE WINAPI Fake_CreateMutexA(
									  __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
									  __in     BOOL bInitialOwner,
									  __in_opt LPCSTR lpName)
{
	CFunCallerLogger logger(_T("CreateMutexA"));

	HANDLE hRet = True_CreateMutexA(
		lpMutexAttributes, 
		bInitialOwner, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateMutexA);

	return hRet;
};

typedef HANDLE (WINAPI * Type_CreateMutexW)(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_CreateMutexW)(
	__in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
	__in     BOOL bInitialOwner,
	__in_opt LPCWSTR lpName) = (Type_CreateMutexW)DetourFindFunction("Kernel32.dll", "CreateMutexW");

__out HANDLE WINAPI Fake_CreateMutexW(
									  __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
									  __in     BOOL bInitialOwner,
									  __in_opt LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("CreateMutexW"));

	HANDLE hRet = True_CreateMutexW(
		lpMutexAttributes, 
		bInitialOwner, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateMutexW);

	return hRet;
};


typedef HANDLE (WINAPI * Type_OpenMutexA)(DWORD,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_OpenMutexA)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpName) = (Type_OpenMutexA)DetourFindFunction("Kernel32.dll", "OpenMutexA");

__out HANDLE WINAPI Fake_OpenMutexA(
									__in DWORD dwDesiredAccess,
									__in BOOL bInheritHandle,
									__in LPCSTR lpName)
{
	CFunCallerLogger logger(_T("OpenMutexA"));

	HANDLE hRet = True_OpenMutexA(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenMutexA);

	return hRet;
};

typedef HANDLE (WINAPI * Type_OpenMutexW)(DWORD,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_OpenMutexW)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpName) = (Type_OpenMutexW)DetourFindFunction("Kernel32.dll", "OpenMutexW");

__out HANDLE WINAPI Fake_OpenMutexW(
									__in DWORD dwDesiredAccess,
									__in BOOL bInheritHandle,
									__in LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("OpenMutexW"));

	HANDLE hRet = True_OpenMutexW(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenMutexW);

	return hRet;
};

//////////////////////////////////////////////////////////////////////////
//Semaphore
typedef HANDLE (WINAPI * Type_CreateSemaphoreA)(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCSTR);
static __out HANDLE (WINAPI * True_CreateSemaphoreA)(
	__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
	__in     LONG lInitialCount,
	__in     LONG lMaximumCount,
	__in_opt LPCSTR lpName) = (Type_CreateSemaphoreA)DetourFindFunction("Kernel32.dll", "CreateSemaphoreA");

__out HANDLE WINAPI Fake_CreateSemaphoreA(
	__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
	__in     LONG lInitialCount,
	__in     LONG lMaximumCount,
	__in_opt LPCSTR lpName)
{
	CFunCallerLogger logger(_T("CreateSemaphoreA"));

	HANDLE hRet = True_CreateSemaphoreA(
		lpSemaphoreAttributes, 
		lInitialCount, 
		lMaximumCount,
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateSemaphoreA);

	return hRet;
};

typedef HANDLE (WINAPI * Type_CreateSemaphoreW)(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCWSTR);
static __out HANDLE (WINAPI * True_CreateSemaphoreW)(
	__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
	__in     LONG lInitialCount,
	__in     LONG lMaximumCount,
	__in_opt LPCWSTR lpName) = (Type_CreateSemaphoreW)DetourFindFunction("Kernel32.dll", "CreateSemaphoreW");

__out HANDLE WINAPI Fake_CreateSemaphoreW(
	__in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
	__in     LONG lInitialCount,
	__in     LONG lMaximumCount,
	__in_opt LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("CreateSemaphoreW"));

	HANDLE hRet = True_CreateSemaphoreW(
		lpSemaphoreAttributes, 
		lInitialCount, 
		lMaximumCount,
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateSemaphoreW);

	return hRet;
};


typedef HANDLE (WINAPI * Type_OpenSemaphoreA)(DWORD,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_OpenSemaphoreA)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpName) = (Type_OpenSemaphoreA)DetourFindFunction("Kernel32.dll", "OpenSemaphoreA");

__out HANDLE WINAPI Fake_OpenSemaphoreA(
										__in DWORD dwDesiredAccess,
										__in BOOL bInheritHandle,
										__in LPCSTR lpName)
{
	CFunCallerLogger logger(_T("OpenSemaphoreA"));

	HANDLE hRet = True_OpenSemaphoreA(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenSemaphoreA);

	return hRet;
};

typedef HANDLE (WINAPI * Type_OpenSemaphoreW)(DWORD,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_OpenSemaphoreW)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpName) = (Type_OpenSemaphoreW)DetourFindFunction("Kernel32.dll", "OpenSemaphoreW");

__out HANDLE WINAPI Fake_OpenSemaphoreW(
										__in DWORD dwDesiredAccess,
										__in BOOL bInheritHandle,
										__in LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("OpenSemaphoreW"));

	HANDLE hRet = True_OpenSemaphoreW(
		dwDesiredAccess, 
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenSemaphoreW);

	return hRet;
};



//////////////////////////////////////////////////////////////////////////
//Job
typedef HANDLE (WINAPI * Type_CreateJobObjectA)(LPSECURITY_ATTRIBUTES,LPCSTR);
static __out HANDLE (WINAPI * True_CreateJobObjectA)(
	__in_opt LPSECURITY_ATTRIBUTES lpJobAttributes,
	__in_opt LPCSTR lpName) = (Type_CreateJobObjectA)DetourFindFunction("Kernel32.dll", "CreateJobObjectA");

__out HANDLE WINAPI Fake_CreateJobObjectA(
	__in_opt LPSECURITY_ATTRIBUTES lpJobAttributes,
	__in_opt LPCSTR lpName)
{
	CFunCallerLogger logger(_T("CreateJobObjectA"));

	HANDLE hRet = True_CreateJobObjectA(
		lpJobAttributes, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateJobObjectA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_CreateJobObjectW)(LPSECURITY_ATTRIBUTES,LPCWSTR);
static __out HANDLE (WINAPI * True_CreateJobObjectW)(
	__in_opt LPSECURITY_ATTRIBUTES lpJobAttributes,
	__in_opt LPCWSTR lpName) = (Type_CreateJobObjectW)DetourFindFunction("Kernel32.dll", "CreateJobObjectW");

__out HANDLE WINAPI Fake_CreateJobObjectW(
	__in_opt LPSECURITY_ATTRIBUTES lpJobAttributes,
	__in_opt LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("CreateJobObjectW"));

	HANDLE hRet = True_CreateJobObjectW(
		lpJobAttributes, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_CreateJobObjectW);

	return hRet;
}

typedef HANDLE (WINAPI * Type_OpenJobObjectA)(DWORD,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_OpenJobObjectA)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpName) = (Type_OpenJobObjectA)DetourFindFunction("Kernel32.dll", "OpenJobObjectA");

__out HANDLE WINAPI Fake_OpenJobObjectA(
										__in DWORD dwDesiredAccess,
										__in BOOL bInheritHandle,
										__in LPCSTR lpName)
{
	CFunCallerLogger logger(_T("OpenJobObjectA"));

	HANDLE hRet = True_OpenJobObjectA(
		dwDesiredAccess,
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenJobObjectA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_OpenJobObjectW)(DWORD,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_OpenJobObjectW)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpName) = (Type_OpenJobObjectW)DetourFindFunction("Kernel32.dll", "OpenJobObjectW");

__out HANDLE WINAPI Fake_OpenJobObjectW(
										__in DWORD dwDesiredAccess,
										__in BOOL bInheritHandle,
										__in LPCWSTR lpName)
{
	CFunCallerLogger logger(_T("OpenJobObjectW"));

	HANDLE hRet = True_OpenJobObjectW(
		dwDesiredAccess,
		bInheritHandle, 
		lpName);

	GetCallStackOnNotNull(hRet, Index_OpenJobObjectW);

	return hRet;
}

//////////////////////////////////////////////////////////////////////////
//Timer
typedef HANDLE (WINAPI * Type_CreateWaitableTimerA)(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_CreateWaitableTimerA)(
	__in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
	__in     BOOL bManualReset,
	__in_opt LPCSTR lpTimerName) = (Type_CreateWaitableTimerA)DetourFindFunction("Kernel32.dll", "CreateWaitableTimerA");

__out HANDLE WINAPI Fake_CreateWaitableTimerA(
	__in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
	__in     BOOL bManualReset,
	__in_opt LPCSTR lpTimerName)
{
	CFunCallerLogger logger(_T("CreateWaitableTimerA"));

	HANDLE hRet = True_CreateWaitableTimerA(
		lpTimerAttributes,
		bManualReset,
		lpTimerName);

	GetCallStackOnNotNull(hRet, Index_CreateWaitableTimerA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_CreateWaitableTimerW)(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_CreateWaitableTimerW)(
	__in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
	__in     BOOL bManualReset,
	__in_opt LPCWSTR lpTimerName) = (Type_CreateWaitableTimerW)DetourFindFunction("Kernel32.dll", "CreateWaitableTimerW");

__out HANDLE WINAPI Fake_CreateWaitableTimerW(
	__in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
	__in     BOOL bManualReset,
	__in_opt LPCWSTR lpTimerName)
{
	CFunCallerLogger logger(_T("CreateWaitableTimerW"));

	HANDLE hRet = True_CreateWaitableTimerW(
		lpTimerAttributes,
		bManualReset,
		lpTimerName);

	GetCallStackOnNotNull(hRet, Index_CreateWaitableTimerW);

	return hRet;
}

typedef HANDLE (WINAPI * Type_OpenWaitableTimerA)(DWORD,BOOL,LPCSTR);
static __out HANDLE (WINAPI * True_OpenWaitableTimerA)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpTimerName) = (Type_OpenWaitableTimerA)DetourFindFunction("Kernel32.dll", "OpenWaitableTimerA");

__out HANDLE WINAPI Fake_OpenWaitableTimerA(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCSTR lpTimerName)
{
	CFunCallerLogger logger(_T("OpenWaitableTimerA"));

	HANDLE hRet = True_OpenWaitableTimerA(
		dwDesiredAccess,
		bInheritHandle,
		lpTimerName);

	GetCallStackOnNotNull(hRet, Index_OpenWaitableTimerA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_OpenWaitableTimerW)(DWORD,BOOL,LPCWSTR);
static __out HANDLE (WINAPI * True_OpenWaitableTimerW)(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpTimerName) = (Type_OpenWaitableTimerW)DetourFindFunction("Kernel32.dll", "OpenWaitableTimerW");

__out HANDLE WINAPI Fake_OpenWaitableTimerW(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in LPCWSTR lpTimerName)
{
	CFunCallerLogger logger(_T("OpenWaitableTimerW"));

	HANDLE hRet = True_OpenWaitableTimerW(
		dwDesiredAccess,
		bInheritHandle,
		lpTimerName);

	GetCallStackOnNotNull(hRet, Index_OpenWaitableTimerW);

	return hRet;
}



//////////////////////////////////////////////////////////////////////////
//Desktop
typedef HDESK (WINAPI * Type_CreateDesktopA)(LPCSTR,LPCSTR,LPDEVMODEA,DWORD,ACCESS_MASK,LPSECURITY_ATTRIBUTES);
static HDESK (WINAPI * True_CreateDesktopA)(
	__in LPCSTR lpszDesktop,
	__reserved LPCSTR lpszDevice,
	__reserved LPDEVMODEA pDevmode,
	__in DWORD dwFlags,
	__in ACCESS_MASK dwDesiredAccess,
	__in_opt LPSECURITY_ATTRIBUTES lpsa) = (Type_CreateDesktopA)DetourFindFunction("User32.dll", "CreateDesktopA");

HDESK WINAPI Fake_CreateDesktopA(
								 __in LPCSTR lpszDesktop,
								 __reserved LPCSTR lpszDevice,
								 __reserved LPDEVMODEA pDevmode,
								 __in DWORD dwFlags,
								 __in ACCESS_MASK dwDesiredAccess,
								 __in_opt LPSECURITY_ATTRIBUTES lpsa)
{
	CFunCallerLogger logger(_T("CreateDesktopA"));

	HDESK hRet = True_CreateDesktopA(
		lpszDesktop, 
		lpszDevice,
		pDevmode,
		dwFlags, 
		dwDesiredAccess, 
		lpsa);

	GetCallStackOnNotNull(hRet, Index_CreateDesktopA);

	return hRet;
}

typedef HDESK (WINAPI * Type_CreateDesktopW)(LPCWSTR,LPCWSTR,LPDEVMODEW,DWORD,ACCESS_MASK,LPSECURITY_ATTRIBUTES);
static HDESK (WINAPI * True_CreateDesktopW)(
	__in LPCWSTR lpszDesktop,
	__reserved LPCWSTR lpszDevice,
	__reserved LPDEVMODEW pDevmode,
	__in DWORD dwFlags,
	__in ACCESS_MASK dwDesiredAccess,
	__in_opt LPSECURITY_ATTRIBUTES lpsa) = (Type_CreateDesktopW)DetourFindFunction("User32.dll", "CreateDesktopW");

HDESK WINAPI Fake_CreateDesktopW(
								 __in LPCWSTR lpszDesktop,
								 __reserved LPCWSTR lpszDevice,
								 __reserved LPDEVMODEW pDevmode,
								 __in DWORD dwFlags,
								 __in ACCESS_MASK dwDesiredAccess,
								 __in_opt LPSECURITY_ATTRIBUTES lpsa)
{
	CFunCallerLogger logger(_T("CreateDesktopW"));

	HDESK hRet = True_CreateDesktopW(
		lpszDesktop, 
		lpszDevice,
		pDevmode,
		dwFlags, 
		dwDesiredAccess, 
		lpsa);

	GetCallStackOnNotNull(hRet, Index_CreateDesktopW);

	return hRet;
}

typedef HDESK (WINAPI * Type_OpenDesktopA)(LPCSTR,DWORD,BOOL,ACCESS_MASK);
static HDESK (WINAPI * True_OpenDesktopA)(
	__in LPCSTR lpszDesktop,
	__in DWORD dwFlags,
	__in BOOL fInherit,
	__in ACCESS_MASK dwDesiredAccess) = (Type_OpenDesktopA)DetourFindFunction("User32.dll", "OpenDesktopA");

HDESK WINAPI Fake_OpenDesktopA(
							   __in LPCSTR lpszDesktop,
							   __in DWORD dwFlags,
							   __in BOOL fInherit,
							   __in ACCESS_MASK dwDesiredAccess)
{
	CFunCallerLogger logger(_T("OpenDesktopA"));

	HDESK hRet = True_OpenDesktopA(
		lpszDesktop, 
		dwFlags,
		fInherit, 
		dwDesiredAccess);

	GetCallStackOnNotNull(hRet, Index_OpenDesktopA);

	return hRet;
}

typedef HDESK (WINAPI * Type_OpenDesktopW)(LPCWSTR,DWORD,BOOL,ACCESS_MASK);
static HDESK (WINAPI * True_OpenDesktopW)(
	__in LPCWSTR lpszDesktop,
	__in DWORD dwFlags,
	__in BOOL fInherit,
	__in ACCESS_MASK dwDesiredAccess) = (Type_OpenDesktopW)DetourFindFunction("User32.dll", "OpenDesktopW");

HDESK WINAPI Fake_OpenDesktopW(
							   __in LPCWSTR lpszDesktop,
							   __in DWORD dwFlags,
							   __in BOOL fInherit,
							   __in ACCESS_MASK dwDesiredAccess)
{
	CFunCallerLogger logger(_T("OpenDesktopW"));

	HDESK hRet = True_OpenDesktopW(
		lpszDesktop, 
		dwFlags,
		fInherit, 
		dwDesiredAccess);

	GetCallStackOnNotNull(hRet, Index_OpenDesktopW);

	return hRet;
}

typedef HDESK (WINAPI * Type_OpenInputDesktop)(DWORD,BOOL,ACCESS_MASK);
static HDESK (WINAPI * True_OpenInputDesktop)(
	__in DWORD dwFlags,
	__in BOOL fInherit,
	__in ACCESS_MASK dwDesiredAccess) = (Type_OpenInputDesktop)DetourFindFunction("User32.dll", "OpenInputDesktop");

HDESK WINAPI Fake_OpenInputDesktop(
								   __in DWORD dwFlags,
								   __in BOOL fInherit,
								   __in ACCESS_MASK dwDesiredAccess)
{
	CFunCallerLogger logger(_T("OpenInputDesktop"));

	HDESK hRet = True_OpenInputDesktop(
		dwFlags,
		fInherit, 
		dwDesiredAccess);

	GetCallStackOnNotNull(hRet, Index_OpenInputDesktop);

	return hRet;
}


//////////////////////////////////////////////////////////////////////////
//Window station
typedef HWINSTA (WINAPI * Type_CreateWindowStationA)(LPCSTR,DWORD,ACCESS_MASK,LPSECURITY_ATTRIBUTES);
static HWINSTA (WINAPI * True_CreateWindowStationA)(
	__in_opt LPCSTR lpwinsta,
	__in DWORD dwFlags,
	__in ACCESS_MASK dwDesiredAccess,
	__in_opt LPSECURITY_ATTRIBUTES lpsa) = (Type_CreateWindowStationA)DetourFindFunction("User32.dll", "CreateWindowStationA");


HWINSTA WINAPI Fake_CreateWindowStationA(
	__in_opt LPCSTR lpwinsta,
	__in DWORD dwFlags,
	__in ACCESS_MASK dwDesiredAccess,
	__in_opt LPSECURITY_ATTRIBUTES lpsa)
{
	CFunCallerLogger logger(_T("CreateWindowStationA"));

	HWINSTA hRet = True_CreateWindowStationA(
		lpwinsta,
		dwFlags,
		dwDesiredAccess,
		lpsa);

	GetCallStackOnNotNull(hRet, Index_CreateWindowStationA);

	return hRet;
}

typedef HWINSTA (WINAPI * Type_CreateWindowStationW)(LPCWSTR,DWORD,ACCESS_MASK,LPSECURITY_ATTRIBUTES);
static HWINSTA (WINAPI * True_CreateWindowStationW)(
	__in_opt LPCWSTR lpwinsta,
	__in DWORD dwFlags,
	__in ACCESS_MASK dwDesiredAccess,
	__in_opt LPSECURITY_ATTRIBUTES lpsa) = (Type_CreateWindowStationW)DetourFindFunction("User32.dll", "CreateWindowStationW");


HWINSTA WINAPI Fake_CreateWindowStationW(
	__in_opt LPCWSTR lpwinsta,
	__in DWORD dwFlags,
	__in ACCESS_MASK dwDesiredAccess,
	__in_opt LPSECURITY_ATTRIBUTES lpsa)
{
	CFunCallerLogger logger(_T("CreateWindowStationW"));

	HWINSTA hRet = True_CreateWindowStationW(
		lpwinsta,
		dwFlags,
		dwDesiredAccess,
		lpsa);

	GetCallStackOnNotNull(hRet, Index_CreateWindowStationW);

	return hRet;
}

typedef HWINSTA (WINAPI * Type_OpenWindowStationA)(LPCSTR,BOOL,ACCESS_MASK);
static HWINSTA (WINAPI * True_OpenWindowStationA)(
	__in LPCSTR lpszWinSta,
	__in BOOL fInherit,
	__in ACCESS_MASK dwDesiredAccess) = (Type_OpenWindowStationA)DetourFindFunction("User32.dll", "OpenWindowStationA");


HWINSTA WINAPI Fake_OpenWindowStationA(
									   __in LPCSTR lpszWinSta,
									   __in BOOL fInherit,
									   __in ACCESS_MASK dwDesiredAccess)
{
	CFunCallerLogger logger(_T("OpenWindowStationA"));

	HWINSTA hRet = True_OpenWindowStationA(
		lpszWinSta,
		fInherit,
		dwDesiredAccess);

	GetCallStackOnNotNull(hRet, Index_OpenWindowStationA);

	return hRet;
}

typedef HWINSTA (WINAPI * Type_OpenWindowStationW)(LPCWSTR,BOOL,ACCESS_MASK);
static HWINSTA (WINAPI * True_OpenWindowStationW)(
	__in LPCWSTR lpszWinSta,
	__in BOOL fInherit,
	__in ACCESS_MASK dwDesiredAccess) = (Type_OpenWindowStationW)DetourFindFunction("User32.dll", "OpenWindowStationW");


HWINSTA WINAPI Fake_OpenWindowStationW(
									   __in LPCWSTR lpszWinSta,
									   __in BOOL fInherit,
									   __in ACCESS_MASK dwDesiredAccess)
{
	CFunCallerLogger logger(_T("OpenWindowStationW"));

	HWINSTA hRet = True_OpenWindowStationW(
		lpszWinSta,
		fInherit,
		dwDesiredAccess);

	GetCallStackOnNotNull(hRet, Index_OpenWindowStationW);

	return hRet;
}


//////////////////////////////////////////////////////////////////////////
//Pipe 返回两个句柄
typedef BOOL (WINAPI * Type_CreatePipe)(PHANDLE,PHANDLE,LPSECURITY_ATTRIBUTES,DWORD);
static BOOL (WINAPI * True_CreatePipe)(
									   __out_ecount_full(1) PHANDLE hReadPipe,
									   __out_ecount_full(1) PHANDLE hWritePipe,
									   __in_opt LPSECURITY_ATTRIBUTES lpPipeAttributes,
									   __in     DWORD nSize) = (Type_CreatePipe)DetourFindFunction("Kernel32.dll", "CreatePipe");

BOOL WINAPI Fake_CreatePipe(
							__out_ecount_full(1) PHANDLE hReadPipe,
							__out_ecount_full(1) PHANDLE hWritePipe,
							__in_opt LPSECURITY_ATTRIBUTES lpPipeAttributes,
							__in     DWORD nSize)
{
	CFunCallerLogger logger(_T("CreatePipe"));

	BOOL bRet = True_CreatePipe(
		hReadPipe, 
		hWritePipe,
		lpPipeAttributes,
		nSize);

	GetCallStackOnTrue(bRet, Index_CreateProcessA, *hReadPipe, *hWritePipe);

	return bRet;
}

typedef HANDLE (WINAPI * Type_CreateNamedPipeA)(LPCSTR,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
static __out HANDLE (WINAPI * True_CreateNamedPipeA)(
	__in     LPCSTR lpName,
	__in     DWORD dwOpenMode,
	__in     DWORD dwPipeMode,
	__in     DWORD nMaxInstances,
	__in     DWORD nOutBufferSize,
	__in     DWORD nInBufferSize,
	__in     DWORD nDefaultTimeOut,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes) = (Type_CreateNamedPipeA)DetourFindFunction("Kernel32.dll", "CreateNamedPipeA");

__out HANDLE WINAPI Fake_CreateNamedPipeA(
	__in     LPCSTR lpName,
	__in     DWORD dwOpenMode,
	__in     DWORD dwPipeMode,
	__in     DWORD nMaxInstances,
	__in     DWORD nOutBufferSize,
	__in     DWORD nInBufferSize,
	__in     DWORD nDefaultTimeOut,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	CFunCallerLogger logger(_T("CreateNamedPipeA"));

	HANDLE hRet = True_CreateNamedPipeA(
		lpName,
		dwOpenMode,
		dwPipeMode,
		nMaxInstances,
		nOutBufferSize,
		nInBufferSize,
		nDefaultTimeOut,
		lpSecurityAttributes);

	GetCallStackOnNotNull(hRet, Index_CreateNamedPipeA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_CreateNamedPipeW)(LPCWSTR,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
static __out HANDLE (WINAPI * True_CreateNamedPipeW)(
	__in     LPCWSTR lpName,
	__in     DWORD dwOpenMode,
	__in     DWORD dwPipeMode,
	__in     DWORD nMaxInstances,
	__in     DWORD nOutBufferSize,
	__in     DWORD nInBufferSize,
	__in     DWORD nDefaultTimeOut,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes) = (Type_CreateNamedPipeW)DetourFindFunction("Kernel32.dll", "CreateNamedPipeW");

__out HANDLE WINAPI Fake_CreateNamedPipeW(
	__in     LPCWSTR lpName,
	__in     DWORD dwOpenMode,
	__in     DWORD dwPipeMode,
	__in     DWORD nMaxInstances,
	__in     DWORD nOutBufferSize,
	__in     DWORD nInBufferSize,
	__in     DWORD nDefaultTimeOut,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	CFunCallerLogger logger(_T("CreateNamedPipeW"));

	HANDLE hRet = True_CreateNamedPipeW(
		lpName,
		dwOpenMode,
		dwPipeMode,
		nMaxInstances,
		nOutBufferSize,
		nInBufferSize,
		nDefaultTimeOut,
		lpSecurityAttributes);

	GetCallStackOnNotNull(hRet, Index_CreateNamedPipeW);

	return hRet;
}


//////////////////////////////////////////////////////////////////////////
//Mailslot
typedef HANDLE (WINAPI * Type_CreateMailslotA)(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
static __out HANDLE (WINAPI * True_CreateMailslotA)(
	__in     LPCSTR lpName,
	__in     DWORD nMaxMessageSize,
	__in     DWORD lReadTimeout,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes) = (Type_CreateMailslotA)DetourFindFunction("Kernel32.dll", "CreateMailslotA");


__out HANDLE WINAPI Fake_CreateMailslotA(
	__in     LPCSTR lpName,
	__in     DWORD nMaxMessageSize,
	__in     DWORD lReadTimeout,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	CFunCallerLogger logger(_T("CreateMailslotA"));

	HANDLE hRet = True_CreateMailslotA(
		lpName, 
		nMaxMessageSize, 
		lReadTimeout, 
		lpSecurityAttributes);

	GetCallStackOnNotNull(hRet, Index_CreateMailslotA);

	return hRet;
}

typedef HANDLE (WINAPI * Type_CreateMailslotW)(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
static __out HANDLE (WINAPI * True_CreateMailslotW)(
	__in     LPCWSTR lpName,
	__in     DWORD nMaxMessageSize,
	__in     DWORD lReadTimeout,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes) = (Type_CreateMailslotW)DetourFindFunction("Kernel32.dll", "CreateMailslotW");

__out HANDLE WINAPI Fake_CreateMailslotW(
	__in     LPCWSTR lpName,
	__in     DWORD nMaxMessageSize,
	__in     DWORD lReadTimeout,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	CFunCallerLogger logger(_T("CreateMailslotW"));

	HANDLE hRet = True_CreateMailslotW(
		lpName, 
		nMaxMessageSize, 
		lReadTimeout, 
		lpSecurityAttributes);

	GetCallStackOnNotNull(hRet, Index_CreateMailslotW);

	return hRet;
}


//////////////////////////////////////////////////////////////////////////
//DuplicateHandle
typedef BOOL (WINAPI * Type_DuplicateHandle)(HANDLE,HANDLE,HANDLE,LPHANDLE,DWORD,BOOL,DWORD);
static BOOL (WINAPI * True_DuplicateHandle)(
	__in        HANDLE hSourceProcessHandle,
	__in        HANDLE hSourceHandle,
	__in        HANDLE hTargetProcessHandle,
	__deref_out LPHANDLE lpTargetHandle,
	__in        DWORD dwDesiredAccess,
	__in        BOOL bInheritHandle,
	__in        DWORD dwOptions) = (Type_DuplicateHandle)DetourFindFunction("Kernel32.dll", "DuplicateHandle");

BOOL WINAPI Fake_DuplicateHandle(
								 __in        HANDLE hSourceProcessHandle,
								 __in        HANDLE hSourceHandle,
								 __in        HANDLE hTargetProcessHandle,
								 __deref_out LPHANDLE lpTargetHandle,
								 __in        DWORD dwDesiredAccess,
								 __in        BOOL bInheritHandle,
								 __in        DWORD dwOptions)
{
	CFunCallerLogger logger(_T("DuplicateHandle"));

	BOOL bRet = True_DuplicateHandle(
		hSourceProcessHandle,
		hSourceHandle,
		hTargetProcessHandle,
		lpTargetHandle,
		dwDesiredAccess,
		bInheritHandle,
		dwOptions);

	if (hTargetProcessHandle != GetCurrentProcess())
	{// 如果不是复制到自身进程，过滤
		return bRet;
	}

	GetCallStackOnTrue(bRet, Index_DuplicateHandle, *lpTargetHandle, NULL);

	return bRet;
}

typedef HANDLE (WINAPI * Type_CreateToolhelp32Snapshot)(DWORD,DWORD);
static __out HANDLE (WINAPI * True_CreateToolhelp32Snapshot)(
	DWORD dwFlags,
	DWORD th32ProcessID) = (Type_CreateToolhelp32Snapshot)DetourFindFunction("Kernel32.dll", "CreateToolhelp32Snapshot");

__out HANDLE WINAPI Fake_CreateToolhelp32Snapshot(
	DWORD dwFlags,
	DWORD th32ProcessID)
{
	CFunCallerLogger logger(_T("CreateToolhelp32Snapshot"));

	HANDLE hRet = True_CreateToolhelp32Snapshot(
		dwFlags, 
		th32ProcessID);

	GetCallStackOnNotNull(hRet, Index_CreateToolhelp32Snapshot);

	return hRet;
}

//FindFirstFileA
typedef HANDLE (WINAPI * Type_FindFirstFileA)(LPCSTR,LPWIN32_FIND_DATAA);
static __out HANDLE (WINAPI * True_FindFirstFileA)(
	__in  LPCSTR lpFileName,
	__out LPWIN32_FIND_DATAA lpFindFileData) = (Type_FindFirstFileA)DetourFindFunction("Kernel32.dll", "FindFirstFileA");

__out HANDLE WINAPI Fake_FindFirstFileA(
										__in  LPCSTR lpFileName,
										__out LPWIN32_FIND_DATAA lpFindFileData)
{
	CFunCallerLogger logger(_T("FindFirstFileA"));

	HANDLE hRet = True_FindFirstFileA(
		lpFileName, 
		lpFindFileData);

	GetCallStackOnNotNull(hRet, Index_FindFirstFileA);

	return hRet;
}

//FindFirstFileW
typedef HANDLE (WINAPI * Type_FindFirstFileW)(LPCWSTR,LPWIN32_FIND_DATAW);
static __out HANDLE (WINAPI * True_FindFirstFileW)(
	__in  LPCWSTR lpFileName,
	__out LPWIN32_FIND_DATAW lpFindFileData) = (Type_FindFirstFileW)DetourFindFunction("Kernel32.dll", "FindFirstFileW");

__out HANDLE WINAPI Fake_FindFirstFileW(
										__in  LPCWSTR lpFileName,
										__out LPWIN32_FIND_DATAW lpFindFileData)
{
	CFunCallerLogger logger(_T("FindFirstFileW"));

	HANDLE hRet = True_FindFirstFileW(
		lpFileName, 
		lpFindFileData);

	GetCallStackOnNotNull(hRet, Index_FindFirstFileW);

	return hRet;
}

//FindFirstFileExA
typedef HANDLE (WINAPI * Type_FindFirstFileExA)(LPCSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
static __out HANDLE (WINAPI * True_FindFirstFileExA)(
	__in       LPCSTR lpFileName,
	__in       FINDEX_INFO_LEVELS fInfoLevelId,
	__out      LPVOID lpFindFileData,
	__in       FINDEX_SEARCH_OPS fSearchOp,
	__reserved LPVOID lpSearchFilter,
	__in       DWORD dwAdditionalFlags) = (Type_FindFirstFileExA)DetourFindFunction("Kernel32.dll", "FindFirstFileExA");

__out HANDLE WINAPI Fake_FindFirstFileExA(
	__in       LPCSTR lpFileName,
	__in       FINDEX_INFO_LEVELS fInfoLevelId,
	__out      LPVOID lpFindFileData,
	__in       FINDEX_SEARCH_OPS fSearchOp,
	__reserved LPVOID lpSearchFilter,
	__in       DWORD dwAdditionalFlags)
{
	CFunCallerLogger logger(_T("FindFirstFileExA"));

	HANDLE hRet = True_FindFirstFileExA(
		lpFileName, 
		fInfoLevelId, 
		lpFindFileData, 
		fSearchOp, 
		lpSearchFilter, 
		dwAdditionalFlags);

	GetCallStackOnNotNull(hRet, Index_FindFirstFileExA);

	return hRet;
}

//FindFirstFileExW
typedef HANDLE (WINAPI * Type_FindFirstFileExW)(LPCWSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
static __out HANDLE (WINAPI * True_FindFirstFileExW)(
	__in       LPCWSTR lpFileName,
	__in       FINDEX_INFO_LEVELS fInfoLevelId,
	__out      LPVOID lpFindFileData,
	__in       FINDEX_SEARCH_OPS fSearchOp,
	__reserved LPVOID lpSearchFilter,
	__in       DWORD dwAdditionalFlags) = (Type_FindFirstFileExW)DetourFindFunction("Kernel32.dll", "FindFirstFileExW");

__out HANDLE WINAPI Fake_FindFirstFileExW(
	__in       LPCWSTR lpFileName,
	__in       FINDEX_INFO_LEVELS fInfoLevelId,
	__out      LPVOID lpFindFileData,
	__in       FINDEX_SEARCH_OPS fSearchOp,
	__reserved LPVOID lpSearchFilter,
	__in       DWORD dwAdditionalFlags
	)
{
	CFunCallerLogger logger(_T("FindFirstFileExW"));

	HANDLE hRet = True_FindFirstFileExW(
		lpFileName, 
		fInfoLevelId, 
		lpFindFileData, 
		fSearchOp, 
		lpSearchFilter, 
		dwAdditionalFlags);

	GetCallStackOnNotNull(hRet, Index_FindFirstFileExW);

	return hRet;
}

}; //EXTERN_C
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

/*
* Macro to construct a statement to call the member method InsertFunction of class CDetoursor.
* 
* @example: if you use the macro as follows:
* 
*		InvokeInsertFuntion(pDetoursor, CreateFileA, bRet);
* 
* it is equal to the statement as follows
* 
*		bRet = pDetoursor->InsertFunction(_T("CreateFileA"), (PVOID*)(True_CreateFileA), (PVOID)Fake_CreateFileA);
*		
* shiontian 2013-01-01
*/
#define InvokeInsertFuntion(p, name, ret) \
	do \
{ \
	ret = p->InsertFunction( \
	_T(#name), \
	(PVOID*)(&True_##name), \
	(PVOID)Fake_##name); \
} while(0)

//MEMORY_INFORMATION_CLASS定义
typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation,					//内存基本信息
	MemoryWorkingSetInformation,			//工作集信息
	MemoryMappedFilenameInformation			//内存映射文件名信息
} MEMORY_INFORMATION_CLASS;

typedef struct _MEMORY_WORKING_SET_INFORMATION {
	ULONG		SizeOfWorkingSet;
	DWORD		WsEntries[ANYSIZE_ARRAY];
} MEMORY_WORKING_SET_INFORMATION, *PMEMORY_WORKING_SET_INFORMATION;

#define _MAX_OBJECT_NAME	1024/sizeof(WCHAR)
typedef struct _MEMORY_MAPPED_FILE_NAME_INFORMATION {
	UNICODE_STRING Name;
	WCHAR		   Buffer[_MAX_OBJECT_NAME];
} MEMORY_MAPPED_FILE_NAME_INFORMATION, *PMEMORY_MAPPED_FILE_NAME_INFORMATION;

typedef NTSTATUS (NTAPI * Type_NtQueryVirtualMemory)(HANDLE,PVOID,MEMORY_INFORMATION_CLASS,PVOID,ULONG,PULONG);
static NTSTATUS (NTAPI * NtQueryVirtualMemory)(
							   IN HANDLE               ProcessHandle,					//目标进程句柄
							   IN PVOID                BaseAddress,						//目标内存地址
							   IN MEMORY_INFORMATION_CLASS MemoryInformationClass,		//查询内存信息的类别
							   OUT PVOID               Buffer,							//用于存储获取到的内存信息的结构地址
							   IN ULONG                Length,							//Buffer的最大长度
							   OUT PULONG              ResultLength OPTIONAL) = NULL;	//存储该函数处理返回的信息的长度的ULONG的地址


/*
 * 通过查找内存Section对象匹配apisetschema.dll，看是否存在这个dll
 * 如果存在一定可以判断至少是windows 7
 */
BOOL IsWindows7()
{
	NtQueryVirtualMemory = (Type_NtQueryVirtualMemory)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwQueryVirtualMemory");

	if (NULL == NtQueryVirtualMemory)
	{
		return FALSE;
	}

	PPEB pPeb = NULL;

	/* 获取PEB地址*/
	__asm
	{
		push eax
		mov eax, fs:[0x30]
		mov pPeb, eax
		pop eax
	} 

	/* 从PEB中获取ApiSetMap结构地址 */
	PVOID pApiSetMap = pPeb->ApiSetMap;

	MEMORY_BASIC_INFORMATION MemBasciInfo;
	ULONG ulRetLen = 0;

	NTSTATUS status = NtQueryVirtualMemory(
		GetCurrentProcess(), 
		pApiSetMap, 
		MemoryBasicInformation, 
		&MemBasciInfo, 
		sizeof(MEMORY_BASIC_INFORMATION), 
		&ulRetLen);

	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}


	if (MEM_IMAGE != MemBasciInfo.Type)
	{
		return FALSE;
	}

	//注意这两个变量一定要邻接，并且buffer紧随UstrName之后
	MEMORY_MAPPED_FILE_NAME_INFORMATION MappedFileName;
	ZeroMemory(&MappedFileName, sizeof(MappedFileName));
	MappedFileName.Name.Buffer = MappedFileName.Buffer;
	MappedFileName.Name.Length = 0;
	MappedFileName.Name.MaximumLength = sizeof(MappedFileName.Buffer);

	status = NtQueryVirtualMemory(
		GetCurrentProcess(), 
		MemBasciInfo.AllocationBase, 
		MemoryMappedFilenameInformation, 
		&MappedFileName, 
		sizeof(MappedFileName), 
		&ulRetLen);

	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}

	if (0 == MappedFileName.Name.Length || NULL == MappedFileName.Name.Buffer )
	{
		return FALSE;
	}

	_wcslwr_s(MappedFileName.Buffer);
	
	if (NULL == wcsstr(MappedFileName.Buffer, L"apisetschema.dll"))
	{
		return FALSE;
	}

	return TRUE;
}


/*
 * 首先判断是否Windows7
 * 通过查看进程内是否有ApisetScheme.dll模块来判断
 * 有：Windows7
 * 无：Windows7之前的版本
 */

namespace DetoursorHelper
{
	/*
	* Helper function of CDetoursor
	* Add many functions to CDetoursor
	*/
	BOOL AddAllFunctionsToDetoursor(CDetoursor *pDetoursor)
	{
		if (NULL == pDetoursor)
		{
			return FALSE;
		}

		if (TRUE == pDetoursor->HasHooked())
		{
			return FALSE;
		}

		BOOL bAddAllOK = FALSE;

		if (TRUE == IsWindows7())
		{// Windows7 Unicode和Ansi函数都hook
			InvokeInsertFuntion(pDetoursor, RegCreateKeyA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegCreateKeyW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegCreateKeyExA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegCreateKeyExW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegOpenKeyA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegOpenKeyW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegOpenKeyExA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegOpenKeyExW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, RegCloseKey, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, NtClose, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, CloseHandle, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, FindClose, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CloseDesktop, bAddAllOK);			//排除
			InvokeInsertFuntion(pDetoursor, CloseWindowStation, bAddAllOK);		//排除
			

			InvokeInsertFuntion(pDetoursor, CreateFileA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateFileW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, OpenFile, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateFileMappingA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateFileMappingW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenFileMappingA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenFileMappingW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateProcessA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateProcessW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenProcess, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateThread, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateRemoteThread, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenThread, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateEventA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateEventW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenEventA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenEventW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateMutexA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateMutexW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenMutexA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenMutexW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateSemaphoreA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateSemaphoreW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenSemaphoreA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenSemaphoreW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateJobObjectA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateJobObjectW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenJobObjectA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenJobObjectW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateWaitableTimerA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateWaitableTimerW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenWaitableTimerA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenWaitableTimerW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateDesktopA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateDesktopW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenDesktopA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenDesktopW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenInputDesktop, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, OpenWindowStationA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenWindowStationW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateWindowStationA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateWindowStationW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateMailslotA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateMailslotW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreatePipe, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateNamedPipeA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateNamedPipeW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, DuplicateHandle, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateToolhelp32Snapshot, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, FindFirstFileA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, FindFirstFileW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, FindFirstFileExA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, FindFirstFileExW, bAddAllOK);
		}
		else
		{// windows7 之前的版本 只Hook Unicode函数(Ansi函数会调到Unicode函数)
			//InvokeInsertFuntion(pDetoursor, RegCreateKeyA, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, RegCreateKeyW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegCreateKeyExA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegCreateKeyExW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, RegOpenKeyA, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, RegOpenKeyW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegOpenKeyExA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, RegOpenKeyExW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, RegCloseKey, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, NtClose, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, CloseHandle, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, FindClose, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CloseDesktop, bAddAllOK);			//排除
			InvokeInsertFuntion(pDetoursor, CloseWindowStation, bAddAllOK);		//排除
			

			//InvokeInsertFuntion(pDetoursor, CreateFileA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateFileW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, OpenFile, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateFileMappingA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateFileMappingW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, OpenFileMappingA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenFileMappingW, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateProcessA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateProcessW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenProcess, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateThread, bAddAllOK);			XP: CreateThread---->CreateRemoteThread
			InvokeInsertFuntion(pDetoursor, CreateRemoteThread, bAddAllOK);	
			InvokeInsertFuntion(pDetoursor, OpenThread, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateEventA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateEventW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, OpenEventA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenEventW, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateMutexA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateMutexW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, OpenMutexA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenMutexW, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateSemaphoreA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateSemaphoreW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, OpenSemaphoreA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenSemaphoreW, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateJobObjectA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateJobObjectW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, OpenJobObjectA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenJobObjectW, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateWaitableTimerA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateWaitableTimerW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, OpenWaitableTimerA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenWaitableTimerW, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateDesktopA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateDesktopW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, OpenDesktopA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenDesktopW, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenInputDesktop, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, OpenWindowStationA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, OpenWindowStationW, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, CreateWindowStationA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateWindowStationW, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, CreateMailslotA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateMailslotW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreatePipe, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, CreateNamedPipeA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, CreateNamedPipeW, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, DuplicateHandle, bAddAllOK);

			InvokeInsertFuntion(pDetoursor, CreateToolhelp32Snapshot, bAddAllOK);

			//InvokeInsertFuntion(pDetoursor, FindFirstFileA, bAddAllOK);
			//InvokeInsertFuntion(pDetoursor, FindFirstFileW, bAddAllOK);	XP: xxxxxW--->xxxxxExW
			//InvokeInsertFuntion(pDetoursor, FindFirstFileExA, bAddAllOK);
			InvokeInsertFuntion(pDetoursor, FindFirstFileExW, bAddAllOK);
		}
				
		if (FALSE == bAddAllOK)
		{
			pDetoursor->ClearFunctionTable();
			return FALSE;
		}

		return TRUE;
	}
}
#endif
