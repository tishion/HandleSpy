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
#include <Tlhelp32.h>
#include "Inject.h"

BOOL EnableDebugPrivilege()   
{   
	HANDLE hToken;   
	BOOL bRet = FALSE;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))   
	{   
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;   
		if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			return bRet;
		}

		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if(!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
		{
			return bRet;
		}

		bRet = (GetLastError() == ERROR_SUCCESS);   
		CloseHandle(hToken);
	}   
	return bRet;   
}

BOOL Inject(DWORD dwProcessID, LPCSTR lpDllPath, PDWORD phModule)
{
		LPVOID lpBuf = NULL;
		HANDLE hThread = NULL;	
		HANDLE hProcess = NULL;
		ULONG dwSize = 0;
		ULONG dwWritten = 0;
		DWORD  dwID = 0;
		PTHREAD_START_ROUTINE pFunc = NULL;

		*phModule = 0;
		hProcess = OpenProcess(
			PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE |PROCESS_VM_READ,
			0, dwProcessID);
		if (hProcess == NULL)
		{
			SetLastError(I_ERROR_OPENPROCESS);
			return FALSE;
		}

		dwSize = strlen(lpDllPath) + 1;

		lpBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);

		if (lpBuf == NULL)    
		{   
			CloseHandle(hProcess);
			SetLastError(I_ERROR_ALLOCMEMORY);
			return FALSE;
		}    

		if (!WriteProcessMemory(hProcess, lpBuf, (LPCVOID)lpDllPath, dwSize, &dwWritten))
		{
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			CloseHandle(hProcess);
			SetLastError(I_ERROR_WRITEMEMORY);
			return FALSE;
		}
		pFunc = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
		if (pFunc == NULL )
		{
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			CloseHandle(hProcess);
			SetLastError(I_ERROR_GETLOADLIBRARYFUNC);
			return FALSE;
		}		
		hThread = CreateRemoteThread(hProcess, NULL, 0, pFunc, lpBuf, 0, &dwID);
		if (hThread == NULL)
		{
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			CloseHandle(hProcess);
			 SetLastError(I_ERROR_CREATEREMOTEPROCESS);
			 return FALSE;
		}
		else
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(hThread, INFINITE))
			{
				GetExitCodeThread(hThread, phModule);
			}
			
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			CloseHandle(hProcess);
			return TRUE;
		}
}

BOOL UnLoadModule(DWORD dwProcessID, HMODULE hModule)
{
	HANDLE	hThread = NULL;	
	HANDLE	hProcess = NULL;
	ULONG	dwSize = 0;
	ULONG	dwWritten = 0;
	DWORD	dwID = 0;
	BOOL	bRet = FALSE;
	PTHREAD_START_ROUTINE pFunc = NULL;

	hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE |PROCESS_VM_READ,
		0, dwProcessID);
	if (hProcess == NULL)
	{
		SetLastError(I_ERROR_OPENPROCESS);
		return FALSE;
	}

	pFunc = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "FreeLibrary");
	if (pFunc == NULL )
	{
		CloseHandle(hProcess);
		SetLastError(I_ERROR_GETFREELIBRARYFUNC);
		return FALSE;
	}		
	hThread = CreateRemoteThread(hProcess, NULL, 0, pFunc, hModule, 0, &dwID);
	if (hThread == NULL)
	{
		CloseHandle(hProcess);
		SetLastError(I_ERROR_CREATEREMOTEPROCESS);
		return FALSE;
	}
	else
	{			
		if (WAIT_OBJECT_0 == WaitForSingleObject(hThread, INFINITE))
		{
			GetExitCodeThread(hThread, (LPDWORD)&bRet);
		}

		CloseHandle(hProcess);
		return bRet;
	}
}