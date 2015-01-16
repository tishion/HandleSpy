#include "StdAfx.h"
#include "Api.h"
#include "ApiIndex.h"
#include "NtApiIndex.h"

TCHAR * CApi::GetNameByIndex(DWORD dwIndex)
{
	DWORD nIndex = dwIndex & 0x0000FFFF;

	if (nIndex >= Index_ApiTable_End)
	{
		return invalidIndex;
	}
	else
	{
		return Name[nIndex];
	}
}

TCHAR *CApi::invalidIndex = _T("Invalid_Index");

#ifdef NT_LAYER_FUNCTION_HOOK

TCHAR * CApi::Name[] = 
{
	_T("Close_Begin"),
	_T("NtClose"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("CloseDesktop"),
	_T("CloseWindowStation"),
	_T("Close_End"),


	//Type operation RING3Î´±©Â¶
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Directory operation
	_T("NtCreateDirectoryObject"),
	_T("NtOpenDirectoryObject"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//SymbolicLink operation
	_T("NtCreateSymbolicLinkObject"),
	_T("NtOpenSymbolicLinkObject"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Token operation 
	_T("NtCreateToken"),
	_T("NtDuplicateToken"),
	_T("NtOpenProcessToken"),
	_T("NtOpenProcessTokenEx"),
	_T("NtOpenThreadToken"),
	_T("NtOpenThreadTokenEx"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Process operation
	_T("NtCreateProcess"),
	_T("NtCreateProcessEx"),
	_T("NtCreateUserProcess"),
	_T("NtGetNextProcess"),
	_T("NtOpenProcess"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Thread operation
	_T("NtCreateThread"),
	_T("NtCreateThreadEx"),
	_T("NtGetNextThread"),
	_T("NtOpenThread"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Job operation
	_T("NtCreateJobObject"),
	_T("NtOpenJobObject"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),



	//Event operation
	_T("NtCreateEvent"),
	_T("NtOpenEvent"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//EventPair operation
	_T("NtCreateEventPair"),
	_T("NtOpenEventPair"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Mutant operation
	_T("NtCreateMutant"),
	_T("NtOpenMutant"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//CallBack operation RING3 ºöÂÔ
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Semaphore operation
	_T("NtCreateSemaphore"),
	_T("NtOpenSemaphore"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Timer operation
	_T("NtCreateTimer"),
	_T("NtOpenTimer"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Profile operation
	_T("NtCreateProfile"),
	_T("NtCreateProfileEx"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//WindowStation operation
	_T("CreateWindowStationA"),
	_T("CreateWindowStationW"),
	_T("OpenWindowStationA"),
	_T("OpenWindowStationW"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Desktop operation
	_T("CreateDesktopA"),
	_T("CreateDesktopW"),
	_T("OpenDesktopA"),
	_T("OpenDesktopW"),
	_T("OpenInputDesktop"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Section operation
	_T("NtCreateSection"),
	_T("NtOpenSection"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Key operation
	_T("NtCreateKey"),
	_T("NtCreateKeyTransacted"),
	_T("NtOpenKey"),
	_T("NtOpenKeyEx"),
	_T("NtOpenKeyTransacted"),
	_T("NtOpenKeyTransactedEx"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Port operation
	_T("NtCreatePort"),
	_T("NtConnectPort"),
	_T("NtSecureConnectPort"),
	_T("NtAcceptConnectPort"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//WaitablePort operation
	_T("NtCreateWaitablePort"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Adapter operation RING3Î´±©Â¶
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Controller operation RING3Î´±©Â¶
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Device operation RING3Î´±©Â¶
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//Driver operation RING3Î´±©Â¶
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),



	//IoCompletion operation
	_T("NtCreateIoCompletion"),
	_T("NtOpenIoCompletion"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	//File operation
	_T("NtCreateFile"),
	_T("NtCreateMailslotFile"),
	_T("NtCreateNamedPipeFile"),
	_T("NtOpenFile"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//WmiGuid operation RING3Î´±©Â¶
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),


	_T("NtDuplicateObject")
};

#endif

#ifndef NT_LAYER_FUNCTION_HOOK

TCHAR* CApi::Name[] = 
{
	//Ö»ÓÐÒ»¸ö¾ä±ú£¬·µ»ØÖµÎªHANDLE
	_T("API_NAME_TABLE_BEGIN"),		//0x00

	//Close Àà
	_T("NtClose"),
	_T("CloseHandle"),
	_T("RegCloseKey"),
	_T("FindClose"),
	_T("CloseDesktop"),
	_T("CloseWindowStation"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	//Reg Key Àà
	_T("RegCreateKeyA"),			//0x10	
	_T("RegCreateKeyW"),			//0x11
	_T("RegCreateKeyExA"),			//0x12
	_T("RegCreateKeyExW"),			//0x13
	_T("RegCreateKeyTransactedA"),	//0x14
	_T("RegCreateKeyTransactedW"),	//0x15
	_T("RegOpenKeyA"),			
	_T("RegOpenKeyW"),			
	_T("RegOpenKeyExA"),			
	_T("RegOpenKeyExW"),
	_T("RegOpenKeyTransactedA"),
	_T("RegOpenKeyTransactedW"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),
	_T("NULL"),

	_T("CreateFileA"),		
	_T("CreateFileW"),		

	_T("OpenFile"),

	_T("CreateFileMappingA"),
	_T("CreateFileMappingW"),
	_T("OpenFileMappingA"),
	_T("OpenFileMappingW"),

	_T("CreateProcessA"),		
	_T("CreateProcessW"),	
	_T("OpenProcess"),	

	_T("CreateThread"),
	_T("CreateRemoteThread"),			
	_T("OpenThread"),			

	_T("CreateEventA"),			
	_T("CreateEventW"),			
	_T("OpenEventA"),			
	_T("OpenEventW"),		

	_T("CreateMutexA"),			
	_T("CreateMutexW"),			
	_T("OpenMutexA"),			
	_T("OpenMutexW"),		

	_T("CreateSemaphoreA"),		
	_T("CreateSemaphoreW"),		
	_T("OpenSemaphoreA"),		
	_T("OpenSemaphoreW"),		
		
	_T("CreateJobObjectA"),		
	_T("CreateJobObjectW"),
	_T("OpenJobOcjectA"),
	_T("OpenJobOcjectW"),

	_T("CreateWaitableTimerA"),	
	_T("CreateWaitableTimerW"),	
	_T("OpenWaitableTimerA"),	
	_T("OpenWaitableTimerW"),	

	_T("CreateDesktopA"),
	_T("CreateDesktopW"),
	_T("OpenDesktopA"),
	_T("OpenDesktopW"),
	_T("OpenInputDesktop"),

	_T("CreateWindowStationA"),
	_T("CreateWindowStationW"),
	_T("OpenWindowStationA"),
	_T("OpenWindowStationA"),

	_T("CreatePipe"),
	_T("CreateNamedPipeA"),		
	_T("CreateNamedPipeW"),	

	_T("CreateMailslotA"),		
	_T("CreateMailslotW"),		
	

	_T("DuplicateHandle"),		

	_T("CreateToolhelp32Snapshot"),

	_T("FindFirstFileA"),
	_T("FindFirstFileW"),
	_T("FindFirstFileExA"),
	_T("FindFirstFileExW"),

	_T("API_NAME_TABLE_END"),
};

#endif