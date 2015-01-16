// HSHook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "detours/detours.h"
#include "DetoursorHelper.h"
#include "callstack/CallStack.h"
#include "StackStorage.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	BOOL bRet = FALSE;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);

		bRet = (CStackStorage::GetInstance()->InitSharedFileMapping(0) 
			&& CStackStorage::GetInstance()->Open());

		if (TRUE == bRet)
		{
			bRet = DetoursorHelper::AddAllFunctionsToDetoursor(CDetoursor::GetInstance());		
			if (TRUE == bRet)
			{
				bRet = CDetoursor::GetInstance()->DoHook();
			}
		}
		break;
	case DLL_PROCESS_DETACH:
		bRet = CDetoursor::GetInstance()->UndoHook();
		if (TRUE == bRet)
		{			
			WCHAR wcsExeFilePathName[_MAX_PATH];
			ZeroMemory(wcsExeFilePathName, sizeof(_MAX_PATH));
			GetModuleFileNameW(NULL, wcsExeFilePathName, _countof(wcsExeFilePathName));
			
			CDetoursor::GetInstance()->ClearFunctionTable();
			
			CStackStorage::GetInstance()->SetModInfoData(CCallStack::GetModInfoVector());
			CStackStorage::GetInstance()->SetHeaderData(wcsExeFilePathName);
			CStackStorage::GetInstance()->Close();
		}
		break;
	}

    return bRet;
}

/* 
 * 模块自我卸载
 * 没有真正使用，只是一个预留功能
 */
DWORD CALLBACK ModuleSelfUnloadThreadProc(LPVOID lpVoid)
{
	HMODULE hModule = NULL;
	DWORD dwThreadId = 0;
	BOOL bRet = GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)ModuleSelfUnloadThreadProc,
		&hModule);

	if (TRUE == bRet)
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, hModule, NULL, &dwThreadId);
	}

	return 0;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

