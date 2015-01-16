/************************************************************************\
* File		:	HandleApis.cpp
* Module	:	Detours Helper
* Created	:	shiontian @ 2013-01-01 
* Description:
* 需要Hook的目标函数的原始地址和假函数列表
* 
* 本文件中的函数几乎全部位于NTDLL层，所以不需要关注UNICODE和Ansi版本
* 
*
\************************************************************************/

#include "stdafx.h"
#ifdef NT_LAYER_FUNCTION_HOOK
#include <Windows.h>
#include <TlHelp32.h>
#include "NtApiIndex.h"
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


#define GetCallStackOnNtSuccess(x, type, h, h2)	\
	do \
	{ \
		if (NT_SUCCESS(x)) \
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


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
EXTERN_C {

//////////////////////////////////////////////////////////////////////////
//Close

typedef NTSTATUS (NTAPI * Type_NtClose)(HANDLE);
static NTSTATUS (NTAPI * True_NtClose)(
		IN HANDLE  Handle) = (Type_NtClose)DetourFindFunction("ntdll.dll", "NtClose");

NTSTATUS NTAPI Fake_NtClose(
		IN HANDLE  Handle
		)
{
	CFunCallerLogger logger(_T("NtClose"));

	NTSTATUS status = True_NtClose(Handle);

	GetCallStackOnNtSuccess(status, Index_NtClose, Handle, NULL);

	return status;
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


/*
* Functions to be hooked
* Get all the addresses of the function to be hooked
*/
//////////////////////////////////////////////////////////////////////////

//Type operation RING3未暴露

//Directory operation
//NTSTATUS NTAPI
//NtSetSecurityObject( IN  HANDLE  Handle,
//					IN   SECURITY_INFORMATION  SecurityInformation,
//					IN   PSECURITY_DESCRIPTOR  SecurityDescriptor
//					);
//
//
//NTSTATUS NTAPI
//NtOpenDirectoryObject(
//					  OUT   PHANDLE  DirectoryHandle,
//					  IN   ACCESS_MASK  DesiredAccess,
//					  IN   POBJECT_ATTRIBUTES  ObjectAttributes
//					  );


//SymbolicLink operation
//NTSTATUS NTAPI
//NtCreateSymbolicLinkObject(
//						   OUT   PHANDLE  SymbolicLinkHandle,
//						   IN   ACCESS_MASK  DesiredAccess,
//						   IN   POBJECT_ATTRIBUTES  ObjectAttributes,
//						   IN   PUNICODE_STRING  TargetName
//						   );
//
//
//NTSTATUS NTAPI
//NtOpenSymbolicLinkObject(
//						 OUT   PHANDLE  SymbolicLinkHandle,
//						 IN   ACCESS_MASK  DesiredAccess,
//						 IN   POBJECT_ATTRIBUTES  ObjectAttributes
//						 );

//Token operation 
typedef NTSTATUS (NTAPI * Type_NtCreateToken)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, TOKEN_TYPE, PLUID, PLARGE_INTEGER, PTOKEN_USER, PTOKEN_GROUPS, PTOKEN_PRIVILEGES, PTOKEN_OWNER, PTOKEN_PRIMARY_GROUP, PTOKEN_DEFAULT_DACL, PTOKEN_SOURCE);
static NTSTATUS (NTAPI * True_NtCreateToken)(
	OUT   PHANDLE  TokenHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   TOKEN_TYPE  Type,
	IN   PLUID AuthenticationId,
	IN   PLARGE_INTEGER  ExpirationTime,
	IN   PTOKEN_USER  User,
	IN   PTOKEN_GROUPS  Groups,
	IN   PTOKEN_PRIVILEGES  Privileges,
	IN   PTOKEN_OWNER  Owner,
	IN   PTOKEN_PRIMARY_GROUP  PrimaryGroup,
	IN   PTOKEN_DEFAULT_DACL  DefaultDacl,
	IN   PTOKEN_SOURCE  Source) = (Type_NtCreateToken)DetourFindFunction("ntdll.dll", "NtCreateToken");

NTSTATUS NTAPI Fake_NtCreateToken(
								  OUT   PHANDLE  TokenHandle,
								  IN   ACCESS_MASK  DesiredAccess,
								  IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								  IN   TOKEN_TYPE  Type,
								  IN   PLUID AuthenticationId,
								  IN   PLARGE_INTEGER  ExpirationTime,
								  IN   PTOKEN_USER  User,
								  IN   PTOKEN_GROUPS  Groups,
								  IN   PTOKEN_PRIVILEGES  Privileges,
								  IN   PTOKEN_OWNER  Owner,
								  IN   PTOKEN_PRIMARY_GROUP  PrimaryGroup,
								  IN   PTOKEN_DEFAULT_DACL  DefaultDacl,
								  IN   PTOKEN_SOURCE  Source
								  )
{
	CFunCallerLogger logger(_T("NtCreateToken"));

	NTSTATUS status = True_NtCreateToken(
		TokenHandle, 
		DesiredAccess, 
		ObjectAttributes, 
		Type, AuthenticationId, 
		ExpirationTime, 
		User, 
		Groups, 
		Privileges, 
		Owner, 
		PrimaryGroup, 
		DefaultDacl, 
		Source);

	GetCallStackOnNtSuccess(status, Index_NtCreateToken, *TokenHandle, NULL);

	return status;
}



typedef NTSTATUS (NTAPI * Type_NtDuplicateToken)(HANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, BOOLEAN, TOKEN_TYPE, PHANDLE);
static NTSTATUS (NTAPI * True_NtDuplicateToken)(
	IN   HANDLE  ExistingTokenHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   BOOLEAN  EffectiveOnly, 
	IN  TOKEN_TYPE  TokenType, 
	OUT  PHANDLE  NewTokenHandle) = (Type_NtDuplicateToken)DetourFindFunction("ntdll.dll", "NtDuplicateToken");

NTSTATUS NTAPI Fake_NtDuplicateToken(
									 IN   HANDLE  ExistingTokenHandle,
									 IN   ACCESS_MASK  DesiredAccess,
									 IN   POBJECT_ATTRIBUTES  ObjectAttributes,
									 IN   BOOLEAN  EffectiveOnly, 
									 IN  TOKEN_TYPE  TokenType, 
									 OUT  PHANDLE  NewTokenHandle
									 )
{
	CFunCallerLogger logger(_T("NtDuplicateToken"));

	NTSTATUS status = True_NtDuplicateToken(
		ExistingTokenHandle, 
		DesiredAccess, 
		ObjectAttributes, 
		EffectiveOnly, 
		TokenType, 
		NewTokenHandle);

	GetCallStackOnNtSuccess(status, Index_NtDuplicateToken, *NewTokenHandle, NULL);

	return status;
}


typedef NTSTATUS (NTAPI * Type_NtOpenProcessToken)(HANDLE, ACCESS_MASK, PHANDLE);
static NTSTATUS (NTAPI * True_NtOpenProcessToken)(
	IN   HANDLE  ProcessHandle,
	IN   ACCESS_MASK  DesiredAccess,
	OUT  PHANDLE  TokenHandle) = (Type_NtOpenProcessToken)DetourFindFunction("ntdll.dll", "NtOpenProcessToken");

NTSTATUS NTAPI Fake_NtOpenProcessToken(
									   IN   HANDLE  ProcessHandle,
									   IN   ACCESS_MASK  DesiredAccess,
									   OUT  PHANDLE  TokenHandle
									   )
{
	CFunCallerLogger logger(_T("NtOpenProcessToken"));

	NTSTATUS status = True_NtOpenProcessToken(
		ProcessHandle, 
		DesiredAccess, 
		TokenHandle);

	GetCallStackOnNtSuccess(status, Index_NtOpenProcessToken, *TokenHandle, NULL);

	return status;
}


typedef NTSTATUS (NTAPI * Type_NtOpenProcessTokenEx)(HANDLE, ACCESS_MASK, ULONG, PHANDLE);
static NTSTATUS (NTAPI * True_NtOpenProcessTokenEx)(
	IN   HANDLE  ProcessHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN	ULONG HandleAttributes,
	OUT  PHANDLE  TokenHandle) = (Type_NtOpenProcessTokenEx)DetourFindFunction("ntdll.dll", "NtOpenProcessTokenEx");

NTSTATUS NTAPI Fake_NtOpenProcessTokenEx(
	IN   HANDLE  ProcessHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN	ULONG HandleAttributes,
	OUT  PHANDLE  TokenHandle
	)
{
	CFunCallerLogger logger(_T("NtOpenProcessTokenEx"));

	NTSTATUS status = True_NtOpenProcessTokenEx(
		ProcessHandle,
		DesiredAccess,
		HandleAttributes,
		TokenHandle);

	GetCallStackOnNtSuccess(status, Index_NtOpenProcessTokenEx, *TokenHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenThreadToken)(HANDLE, ACCESS_MASK, BOOLEAN, PHANDLE);				   
static NTSTATUS (NTAPI * True_NtOpenThreadToken)(
	IN   HANDLE  ThreadHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   BOOLEAN  OpenAsSelf,
	OUT  PHANDLE  TokenHandle) = (Type_NtOpenThreadToken)DetourFindFunction("ntdll.dll", "NtOpenThreadToken");

NTSTATUS NTAPI Fake_NtOpenThreadToken(
									  IN   HANDLE  ThreadHandle,
									  IN   ACCESS_MASK  DesiredAccess,
									  IN   BOOLEAN  OpenAsSelf,
									  OUT  PHANDLE  TokenHandle
									  )
{
	CFunCallerLogger logger(_T("OpenThreadToken"));

	NTSTATUS status = True_NtOpenThreadToken(
		ThreadHandle,
		DesiredAccess,
		OpenAsSelf,
		TokenHandle);

	GetCallStackOnNtSuccess(status, Index_NtOpenThreadToken, *TokenHandle, NULL);

	return status;
}


typedef NTSTATUS (NTAPI * Type_NtOpenThreadTokenEx)(HANDLE, ACCESS_MASK, BOOLEAN, ULONG, PHANDLE);		  
static NTSTATUS (NTAPI * True_NtOpenThreadTokenEx)(
	IN   HANDLE  ThreadHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   BOOLEAN  OpenAsSelf,
	IN   ULONG HandleAttributes,
	OUT  PHANDLE  TokenHandle) = (Type_NtOpenThreadTokenEx)DetourFindFunction("ntdll.dll", "NtOpenThreadTokenEx");

NTSTATUS NTAPI Fake_NtOpenThreadTokenEx(
										IN   HANDLE  ThreadHandle,
										IN   ACCESS_MASK  DesiredAccess,
										IN   BOOLEAN  OpenAsSelf,
										IN   ULONG HandleAttributes,
										OUT  PHANDLE  TokenHandle
										)
{
	CFunCallerLogger logger(_T("NtOpenThreadTokenEx"));

	NTSTATUS status = True_NtOpenThreadTokenEx(
		ThreadHandle, 
		DesiredAccess, 
		OpenAsSelf, 
		HandleAttributes, 
		TokenHandle);

	GetCallStackOnNtSuccess(status, Index_NtOpenThreadTokenEx, *TokenHandle, NULL);

	return status;
}

//Process operation
typedef NTSTATUS (NTAPI * Type_NtCreateProcess)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, HANDLE, BOOLEAN, HANDLE, HANDLE, HANDLE);
static NTSTATUS (NTAPI * True_NtCreateProcess)(
	OUT  PHANDLE  ProcessHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   HANDLE  InheritFromProcessHandle,
	IN   BOOLEAN  InheritHandles,
	IN   HANDLE  SectionHandle  OPTIONAL, 
	IN  HANDLE  DebugPort OPTIONAL,
	IN   HANDLE  ExceptionPort OPTIONAL) = (Type_NtCreateProcess)DetourFindFunction("ntdll.dll", "NtCreateProcess");
NTSTATUS NTAPI Fake_NtCreateProcess(
									OUT PHANDLE  ProcessHandle,
									IN  ACCESS_MASK  DesiredAccess,
									IN  POBJECT_ATTRIBUTES  ObjectAttributes,
									IN  HANDLE  InheritFromProcessHandle,
									IN  BOOLEAN  InheritHandles,
									IN  HANDLE  SectionHandle  OPTIONAL, 
									IN  HANDLE  DebugPort OPTIONAL,
									IN  HANDLE  ExceptionPort OPTIONAL
									)
{
	CFunCallerLogger logger(_T("NtCreateProcess"));

	NTSTATUS status = True_NtCreateProcess(
		ProcessHandle, 
		DesiredAccess, 
		ObjectAttributes, 
		InheritFromProcessHandle, 
		InheritHandles, 
		SectionHandle, 
		DebugPort, 
		ExceptionPort);

	GetCallStackOnNtSuccess(status, Index_NtCreateProcess, *ProcessHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtCreateProcessEx)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, HANDLE, ULONG, HANDLE, HANDLE, HANDLE, BOOLEAN);
static NTSTATUS (NTAPI * True_NtCreateProcessEx)(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ParentProcess,
	IN ULONG Flags,
	IN HANDLE SectionHandle OPTIONAL,
	IN HANDLE DebugPort OPTIONAL,
	IN HANDLE ExceptionPort OPTIONAL,
	IN BOOLEAN InJob) = (Type_NtCreateProcessEx)DetourFindFunction("ntdll.dll", "NtCreateProcessEx");
NTSTATUS NTAPI Fake_NtCreateProcessEx(
									  OUT PHANDLE ProcessHandle,
									  IN ACCESS_MASK DesiredAccess,
									  IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
									  IN HANDLE ParentProcess,
									  IN ULONG Flags,
									  IN HANDLE SectionHandle OPTIONAL,
									  IN HANDLE DebugPort OPTIONAL,
									  IN HANDLE ExceptionPort OPTIONAL,
									  IN BOOLEAN InJob
									  )
{
	CFunCallerLogger logger(_T("NtCreateProcessEx"));

	NTSTATUS status = True_NtCreateProcessEx(
		ProcessHandle, 
		DesiredAccess, 
		ObjectAttributes, 
		ParentProcess, 
		Flags,
		SectionHandle, 
		DebugPort, 
		ExceptionPort, 
		InJob);

	GetCallStackOnNtSuccess(status, Index_NtCreateProcessEx, *ProcessHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtCreateUserProcess)(PHANDLE, PHANDLE, ACCESS_MASK, ACCESS_MASK, POBJECT_ATTRIBUTES, POBJECT_ATTRIBUTES, ULONG, ULONG, PVOID, PVOID, PVOID);
static NTSTATUS (NTAPI * True_NtCreateUserProcess)(
	OUT PHANDLE ProcessHandle,
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK ProcessDesiredAccess,
	IN ACCESS_MASK ThreadDesiredAccess,
	IN POBJECT_ATTRIBUTES ProcessObjectAttributes OPTIONAL,
	IN POBJECT_ATTRIBUTES ThreadObjectAttributes OPTIONAL,
	IN ULONG CreateProcessFlags,
	IN ULONG CreateThreadFlags,
	IN PVOID ProcessParameters,
	IN PVOID Parameter9,
	IN PVOID AttributeList) = (Type_NtCreateUserProcess)DetourFindFunction("ntdll.dll", "NtCreateUserProcess");
NTSTATUS NTAPI Fake_NtCreateUserProcess(
										OUT PHANDLE ProcessHandle,
										OUT PHANDLE ThreadHandle,
										IN ACCESS_MASK ProcessDesiredAccess,
										IN ACCESS_MASK ThreadDesiredAccess,
										IN POBJECT_ATTRIBUTES ProcessObjectAttributes OPTIONAL,
										IN POBJECT_ATTRIBUTES ThreadObjectAttributes OPTIONAL,
										IN ULONG CreateProcessFlags,
										IN ULONG CreateThreadFlags,
										IN PVOID ProcessParameters,
										IN PVOID Parameter9,
										IN PVOID AttributeList
										)
{
	CFunCallerLogger logger(_T("NtCreateUserProcess"));

	NTSTATUS status = True_NtCreateUserProcess(
		ProcessHandle, 
		ThreadHandle,
		ProcessDesiredAccess,
		ThreadDesiredAccess,
		ProcessObjectAttributes,
		ThreadObjectAttributes,
		CreateProcessFlags,
		CreateThreadFlags,
		ProcessParameters,
		Parameter9,
		AttributeList);

	GetCallStackOnNtSuccess(status, Index_NtCreateUserProcess, *ProcessHandle, *ThreadHandle);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtGetNextProcess)(HANDLE, ACCESS_MASK, ULONG, ULONG, PHANDLE);
static NTSTATUS (NTAPI * True_NtGetNextProcess)(
	IN HANDLE ProcessHandle,
	IN	ACCESS_MASK DesiredAccess,
	IN	ULONG HandleAttributes,
	IN	ULONG Flags,
	OUT	PHANDLE NewProcessHandle) = (Type_NtGetNextProcess)DetourFindFunction("ntdll.dll", "NtGetNextProcess");
NTSTATUS NTAPI Fake_NtGetNextProcess(
									 IN HANDLE ProcessHandle,
									 IN	ACCESS_MASK DesiredAccess,
									 IN	ULONG HandleAttributes,
									 IN	ULONG Flags,
									 OUT PHANDLE NewProcessHandle
									 )
{
	CFunCallerLogger logger(_T("NtGetNextProcess"));

	NTSTATUS status = True_NtGetNextProcess(
		ProcessHandle,
		DesiredAccess,
		HandleAttributes,
		Flags,
		NewProcessHandle);

	GetCallStackOnNtSuccess(status, Index_NtGetNextProcess, *NewProcessHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenProcess)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID);
static NTSTATUS (NTAPI * True_NtOpenProcess)(
	OUT   PHANDLE  ProcessHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   PVOID  ClientId OPTIONAL) = (Type_NtOpenProcess)DetourFindFunction("ntdll.dll", "NtOpenProcess");
NTSTATUS NTAPI Fake_NtOpenProcess(
								  OUT   PHANDLE  ProcessHandle,
								  IN   ACCESS_MASK  DesiredAccess,
								  IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								  IN   PVOID  ClientId OPTIONAL
								  )
{
	CFunCallerLogger logger(_T("NtOpenProcess"));

	NTSTATUS status = True_NtOpenProcess(
		ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ClientId);

	GetCallStackOnNtSuccess(status, Index_NtOpenProcess, *ProcessHandle, NULL);

	return status;
}




//Thread operation
typedef NTSTATUS (NTAPI * Type_NtCreateThread)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, HANDLE, PVOID, PCONTEXT, PVOID, BOOLEAN);
static NTSTATUS (NTAPI * True_NtCreateThread)(
	OUT   PHANDLE  ThreadHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   HANDLE  ProcessHandle,
	OUT  PVOID  ClientId,
	IN   PCONTEXT  ThreadContext, 
	IN  PVOID  UserStack, 
	IN  BOOLEAN  CreateSuspended) = (Type_NtCreateThread)DetourFindFunction("ntdll.dll", "NtCreateThread");
NTSTATUS NTAPI Fake_NtCreateThread(
								   OUT   PHANDLE  ThreadHandle,
								   IN   ACCESS_MASK  DesiredAccess,
								   IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								   IN   HANDLE  ProcessHandle,
								   OUT  PVOID  ClientId,
								   IN   PCONTEXT  ThreadContext, 
								   IN  PVOID  UserStack, 
								   IN  BOOLEAN  CreateSuspended
								   )
{
	CFunCallerLogger logger(_T("NtCreateThread"));

	NTSTATUS status = True_NtCreateThread(
		ThreadHandle, 
		DesiredAccess,
		ObjectAttributes,
		ProcessHandle,
		ClientId,
		ThreadContext,
		UserStack,
		CreateSuspended);

	GetCallStackOnNtSuccess(status, Index_NtCreateThread, *ThreadHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtCreateThreadEx)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, HANDLE, PTHREAD_START_ROUTINE, PVOID, ULONG, ULONG, ULONG, ULONG, PVOID);
static NTSTATUS (NTAPI * True_NtCreateThreadEx)(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	IN PTHREAD_START_ROUTINE StartRoutine,
	IN PVOID StartContext,
	IN ULONG CreateThreadFlags,
	IN ULONG ZeroBits OPTIONAL,
	IN ULONG StackSize OPTIONAL,
	IN ULONG MaximumStackSize OPTIONAL,
	IN PVOID AttributeList) = (Type_NtCreateThreadEx)DetourFindFunction("ntdll.dll", "NtCreateThreadEx");
NTSTATUS NTAPI Fake_NtCreateThreadEx(
									 OUT PHANDLE ThreadHandle,
									 IN ACCESS_MASK DesiredAccess,
									 IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
									 IN HANDLE ProcessHandle,
									 IN PTHREAD_START_ROUTINE StartRoutine,
									 IN PVOID StartContext,
									 IN ULONG CreateThreadFlags,
									 IN ULONG ZeroBits OPTIONAL,
									 IN ULONG StackSize OPTIONAL,
									 IN ULONG MaximumStackSize OPTIONAL,
									 IN PVOID AttributeList
									 )
{
	CFunCallerLogger logger(_T("NtCreateThreadEx"));

	NTSTATUS status = True_NtCreateThreadEx(
		ThreadHandle,
		DesiredAccess,
		ObjectAttributes,
		ProcessHandle,
		StartRoutine,
		StartContext,
		CreateThreadFlags,
		ZeroBits,
		StackSize,
		MaximumStackSize,
		AttributeList);

	GetCallStackOnNtSuccess(status, Index_NtCreateThreadEx, *ThreadHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtGetNextThread)(HANDLE, HANDLE, ACCESS_MASK, ULONG, ULONG, PHANDLE);
static NTSTATUS (NTAPI * True_NtGetNextThread)(
	IN HANDLE ProcessHandle,
	IN HANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN ULONG HandleAttributes,
	IN ULONG Flags,
	OUT PHANDLE NewThreadHandle) = (Type_NtGetNextThread)DetourFindFunction("ntdll.dll", "NtGetNextThread");
NTSTATUS NTAPI Fake_NtGetNextThread(
									IN HANDLE ProcessHandle,
									IN HANDLE ThreadHandle,
									IN ACCESS_MASK DesiredAccess,
									IN ULONG HandleAttributes,
									IN ULONG Flags,
									OUT PHANDLE NewThreadHandle
									)
{
	CFunCallerLogger logger(_T("NtGetNextThread"));

	NTSTATUS status = True_NtGetNextThread(
		ProcessHandle,
		ThreadHandle,
		DesiredAccess,
		HandleAttributes,
		Flags,
		NewThreadHandle);

	GetCallStackOnNtSuccess(status, Index_NtGetNextThread, *NewThreadHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenThread)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID);
static NTSTATUS (NTAPI * True_NtOpenThread)(
	OUT   PHANDLE  ThreadHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   PVOID  ClientId) = (Type_NtOpenThread)DetourFindFunction("ntdll.dll", "NtOpenThread");
NTSTATUS NTAPI Fake_NtOpenThread(
								 OUT   PHANDLE  ThreadHandle,
								 IN   ACCESS_MASK  DesiredAccess,
								 IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								 IN   PVOID  ClientId
								 )
{
	CFunCallerLogger logger(_T("NtOpenThread"));

	NTSTATUS status = True_NtOpenThread(
		ThreadHandle,
		DesiredAccess,
		ObjectAttributes,
		ClientId);

	GetCallStackOnNtSuccess(status, Index_NtOpenThread, *ThreadHandle, NULL);

	return status;
}


//Job operation
typedef NTSTATUS (NTAPI * Type_NtCreateJobObject)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtCreateJobObject)(
	OUT   PHANDLE  JobHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtCreateJobObject)DetourFindFunction("ntdll.dll", "NtCreateJobObject");
NTSTATUS NTAPI Fake_NtCreateJobObject(
									  OUT   PHANDLE  JobHandle,
									  IN   ACCESS_MASK  DesiredAccess,
									  IN   POBJECT_ATTRIBUTES  ObjectAttributes
									  )
{
	CFunCallerLogger logger(_T("NtCreateJobObject"));

	NTSTATUS status = True_NtCreateJobObject(
		JobHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtCreateJobObject, *JobHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenJobObject)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenJobObject)(
	OUT   PHANDLE  JobHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtCreateJobObject)DetourFindFunction("ntdll.dll", "NtOpenJobObject");
NTSTATUS NTAPI Fake_NtOpenJobObject(
									  OUT   PHANDLE  JobHandle,
									  IN   ACCESS_MASK  DesiredAccess,
									  IN   POBJECT_ATTRIBUTES  ObjectAttributes
									  )
{
	CFunCallerLogger logger(_T("NtOpenJobObject"));

	NTSTATUS status = True_NtOpenJobObject(
		JobHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenJobObject, *JobHandle, NULL);

	return status;
}

//Event operation
typedef NTSTATUS (NTAPI * Type_NtCreateEvent)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, ULONG, BOOLEAN);
static NTSTATUS (NTAPI * True_NtCreateEvent)(
	OUT   PHANDLE  EventHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   ULONG  EventType,
	IN   BOOLEAN  InitialState) = (Type_NtCreateEvent)DetourFindFunction("ntdll.dll", "NtCreateEvent");
NTSTATUS NTAPI Fake_NtCreateEvent(
								  OUT   PHANDLE  EventHandle,
								  IN   ACCESS_MASK  DesiredAccess,
								  IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								  IN   ULONG  EventType,
								  IN   BOOLEAN  InitialState
								  )
{
	CFunCallerLogger logger(_T("NtCreateEvent"));

	NTSTATUS status = True_NtCreateEvent(
		EventHandle, 
		DesiredAccess, 
		ObjectAttributes,
		EventType,
		InitialState);

	GetCallStackOnNtSuccess(status, Index_NtCreateEvent, *EventHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenEvent)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenEvent)(
	OUT   PHANDLE  EventHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtOpenEvent)DetourFindFunction("ntdll.dll", "NtOpenEvent");
NTSTATUS NTAPI Fake_NtOpenEvent(
								OUT   PHANDLE  EventHandle,
								IN   ACCESS_MASK  DesiredAccess,
								IN   POBJECT_ATTRIBUTES  ObjectAttributes
								)
{
	CFunCallerLogger logger(_T("NtOpenEvent"));

	NTSTATUS status = True_NtOpenEvent(
		EventHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenEvent, *EventHandle, NULL);

	return status;
}

// 不常用，所以无需关注
//EventPair operation
//NTSTATUS NTAPI
//NtCreateEventPair(
//				  OUT   PHANDLE  EventPairHandle,
//				  IN   ACCESS_MASK  DesiredAccess,
//				  IN   POBJECT_ATTRIBUTES  ObjectAttributes
//				  );
//
//NTSTATUS NTAPI
//NtOpenEventPair(
//				OUT   PHANDLE  EventPairHandle,
//				IN   ACCESS_MASK  DesiredAccess,
//				IN   POBJECT_ATTRIBUTES  ObjectAttributes
//				);


//Mutant operation
typedef NTSTATUS (NTAPI * Type_NtCreateMutant)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, BOOLEAN);
static NTSTATUS (NTAPI * True_NtCreateMutant)(
	OUT   PHANDLE  MutantHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   BOOLEAN  InitialOwner) = (Type_NtCreateMutant)DetourFindFunction("ntdll.dll", "NtCreateMutant");
NTSTATUS NTAPI Fake_NtCreateMutant(
								   OUT   PHANDLE  MutantHandle,
								   IN   ACCESS_MASK  DesiredAccess,
								   IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								   IN   BOOLEAN  InitialOwner
								   )
{
	CFunCallerLogger logger(_T("NtCreateMutant"));

	NTSTATUS status = True_NtCreateMutant(
		MutantHandle,
		DesiredAccess,
		ObjectAttributes,
		InitialOwner);

	GetCallStackOnNtSuccess(status, Index_NtCreateMutant, *MutantHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenMutant)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenMutant)(
	OUT   PHANDLE  MutantHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtOpenMutant)DetourFindFunction("ntdll.dll", "NtOpenMutant");
NTSTATUS NTAPI Fake_NtOpenMutant(
								 OUT   PHANDLE  MutantHandle,
								 IN   ACCESS_MASK  DesiredAccess,
								 IN   POBJECT_ATTRIBUTES  ObjectAttributes
								 )
{
	CFunCallerLogger logger(_T("NtOpenMutant"));

	NTSTATUS status = True_NtOpenMutant(
		MutantHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenMutant, *MutantHandle, NULL);

	return status;
}

//CallBack operation RING3 忽略

//Semaphore operation
typedef NTSTATUS (NTAPI * Type_NtCreateSemaphore)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, LONG, LONG);
static NTSTATUS (NTAPI * True_NtCreateSemaphore)(
	OUT   PHANDLE  SemaphoreHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   LONG  InitialCount,
	IN   LONG  MaximumCount) = (Type_NtCreateSemaphore)DetourFindFunction("ntdll.dll", "NtCreateSemaphore");
NTSTATUS NTAPI Fake_NtCreateSemaphore(
									  OUT   PHANDLE  SemaphoreHandle,
									  IN   ACCESS_MASK  DesiredAccess,
									  IN   POBJECT_ATTRIBUTES  ObjectAttributes,
									  IN   LONG  InitialCount,
									  IN   LONG  MaximumCount
									  )
{
	CFunCallerLogger logger(_T("NtCreateSemaphore"));

	NTSTATUS status = True_NtCreateSemaphore(
		SemaphoreHandle,
		DesiredAccess,
		ObjectAttributes,
		InitialCount,
		MaximumCount);

	GetCallStackOnNtSuccess(status, Index_NtCreateSemaphore, *SemaphoreHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenSemaphore)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenSemaphore)(
	OUT   PHANDLE  SemaphoreHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtOpenSemaphore)DetourFindFunction("ntdll.dll", "NtOpenSemaphore");
NTSTATUS NTAPI Fake_NtOpenSemaphore(
									OUT   PHANDLE  SemaphoreHandle,
									IN   ACCESS_MASK  DesiredAccess,
									IN   POBJECT_ATTRIBUTES  ObjectAttributes
									)
{
	CFunCallerLogger logger(_T("NtOpenSemaphore"));

	NTSTATUS status = True_NtOpenSemaphore(
		SemaphoreHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenSemaphore, *SemaphoreHandle, NULL);

	return status;
}


//Timer operation
typedef NTSTATUS (NTAPI * Type_NtCreateTimer)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, ULONG);
static NTSTATUS (NTAPI * True_NtCreateTimer)(
	OUT   PHANDLE  TimerHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   ULONG  TimerType) = (Type_NtCreateTimer)DetourFindFunction("ntdll.dll", "NtCreateTimer");
NTSTATUS NTAPI Fake_NtCreateTimer(
								  OUT   PHANDLE  TimerHandle,
								  IN   ACCESS_MASK  DesiredAccess,
								  IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								  IN   ULONG  TimerType
								  )
{
	CFunCallerLogger logger(_T("NtCreateTimer"));

	NTSTATUS status = True_NtCreateTimer(
		TimerHandle,
		DesiredAccess,
		ObjectAttributes,
		TimerType);

	GetCallStackOnNtSuccess(status, Index_NtCreateTimer, *TimerHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenTimer)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenTimer)(
	OUT   PHANDLE  TimerHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtOpenTimer)DetourFindFunction("ntdll.dll", "NtOpenTimer");
NTSTATUS NTAPI Fake_NtOpenTimer(
								OUT   PHANDLE  TimerHandle,
								IN   ACCESS_MASK  DesiredAccess,
								IN   POBJECT_ATTRIBUTES  ObjectAttributes
								)
{
	CFunCallerLogger logger(_T("NtOpenTimer"));

	NTSTATUS status = True_NtOpenTimer(
		TimerHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenTimer, *TimerHandle, NULL);

	return status;
}

// 不常用
//Profile operation
//NTSTATUS NTAPI
//NtCreateProfile(
//				OUT   PHANDLE  ProfileHandle,
//				IN   HANDLE  ProcessHandle,
//				IN   PVOID Base,
//				IN   ULONG  Size,
//				IN   ULONG  BucketShift,
//				IN   PULONG  Buffer,
//				IN   ULONG  BufferLength,
//				IN   KPROFILE_SOURCE  Source,
//				IN   ULONG  ProcessorMask
//				);
//
//NTSTATUS NTAPI
//NtCreateProfile(
//				OUT   PHANDLE  ProfileHandle,
//				IN   HANDLE  ProcessHandle,
//				IN   PVOID arg_3,
//				IN   PVOID arg_4,
//				IN   PVOID arg_5,
//				IN   PVOID arg_6,
//				IN   PVOID arg_7,
//				IN   PVOID arg_8,
//				IN   PVOID arg_9,
//				IN	 PVOID arg_10
//				);


//WindowStation operation
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


//Desktop operation
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

//Section operation
typedef NTSTATUS (NTAPI * Type_NtCreateSection)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PLARGE_INTEGER, ULONG, ULONG, HANDLE);
static NTSTATUS (NTAPI * True_NtCreateSection)(
	OUT PHANDLE  SectionHandle,
	IN ACCESS_MASK  DesiredAccess,
	IN POBJECT_ATTRIBUTES  ObjectAttributes, 
	IN  PLARGE_INTEGER  SectionSize OPTIONAL, 
	IN  ULONG  Protect,
	IN ULONG  Attributes,
	IN HANDLE  FileHandle) = (Type_NtCreateSection)DetourFindFunction("ntdll.dll", "NtCreateSection");
NTSTATUS NTAPI Fake_NtCreateSection(
									OUT PHANDLE  SectionHandle,
									IN ACCESS_MASK  DesiredAccess,
									IN POBJECT_ATTRIBUTES  ObjectAttributes, 
									IN  PLARGE_INTEGER  SectionSize OPTIONAL, 
									IN  ULONG  Protect,
									IN ULONG  Attributes,
									IN HANDLE  FileHandle
									)
{
	CFunCallerLogger logger(_T("NtCreateSection"));

	NTSTATUS status = True_NtCreateSection(
		SectionHandle,
		DesiredAccess,
		ObjectAttributes,
		SectionSize,
		Protect,
		Attributes,
		FileHandle);

	GetCallStackOnNtSuccess(status, Index_NtCreateSection, *SectionHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenSection)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenSection)(
	OUT   PHANDLE  SectionHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtOpenSection)DetourFindFunction("ntdll.dll", "NtOpenSection");
NTSTATUS NTAPI Fake_NtOpenSection(
								  OUT   PHANDLE  SectionHandle,
								  IN   ACCESS_MASK  DesiredAccess,
								  IN   POBJECT_ATTRIBUTES  ObjectAttributes
								  )
{
	CFunCallerLogger logger(_T("NtOpenSection"));

	NTSTATUS status = True_NtOpenSection(
		SectionHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenSection, *SectionHandle, NULL);

	return status;
}



//Key operation
typedef NTSTATUS (NTAPI * Type_NtCreateKey)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, ULONG, PUNICODE_STRING, ULONG, PULONG);
static NTSTATUS (NTAPI * True_NtCreateKey)(
	OUT   PHANDLE  KeyHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   ULONG  TitleIndex,
	IN   PUNICODE_STRING  Class OPTIONAL,
	IN   ULONG  CreateOptions,
	OUT    PULONG  Disposition OPTIONAL) = (Type_NtCreateKey)DetourFindFunction("ntdll.dll", "NtCreateKey");
NTSTATUS NTAPI Fake_NtCreateKey(
								OUT   PHANDLE  KeyHandle,
								IN   ACCESS_MASK  DesiredAccess,
								IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								IN   ULONG  TitleIndex,
								IN   PUNICODE_STRING  Class OPTIONAL,
								IN   ULONG  CreateOptions,
								OUT    PULONG  Disposition OPTIONAL
								)
{
	CFunCallerLogger logger(_T("NtCreateKey"));

	NTSTATUS status = True_NtCreateKey(
		KeyHandle,
		DesiredAccess,
		ObjectAttributes,
		TitleIndex,
		Class,
		CreateOptions,
		Disposition);

	GetCallStackOnNtSuccess(status, Index_NtCreateKey, *KeyHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenKey)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenKey)(
	OUT   PHANDLE  KeyHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtOpenKey)DetourFindFunction("ntdll.dll", "NtOpenKey");
NTSTATUS NTAPI Fake_NtOpenKey(
							  OUT   PHANDLE  KeyHandle,
							  IN   ACCESS_MASK  DesiredAccess,
							  IN   POBJECT_ATTRIBUTES  ObjectAttributes
							  )
{
	CFunCallerLogger logger(_T("NtOpenKey"));

	NTSTATUS status = True_NtOpenKey(
		KeyHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenKey, *KeyHandle, NULL);

	return status;
}


//_T("NtCreateKeyTransacted") Vista之后才有
//_T("NtOpenKeyEx"), Vista之后才有
//_T("NtOpenKeyTransacted"), Vista之后才有
//_T("NtOpenKeyTransactedEx"), Vista之后才有

//Port operation
//typedef  NTSTATUS (NTAPI * Type_NtCreatePort)(PHANDLE, POBJECT_ATTRIBUTES, ULONG, ULONG, ULONG);
//static NTSTATUS (NTAPI * True_NtCreatePort)(
//			 OUT   PHANDLE  PortHandle,
//			 IN   POBJECT_ATTRIBUTES  ObjectAttributes,
//			 IN   ULONG  MaxDataSize,
//			 IN   ULONG  MaxMessageSize,
//			 IN   ULONG  Reserved
//			 ) = (Type_NtCreatePort)DetourFindFunction("ntdll.dll", "NtCreatePort");
//
//NTSTATUS NTAPI Fake_NtCreatePort(
//	OUT   PHANDLE  PortHandle,
//	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
//	IN   ULONG  MaxDataSize,
//	IN   ULONG  MaxMessageSize,
//	IN   ULONG  Reserved
//	)
//{
//	CFunCallerLogger logger(_T("NtCreatePort"));
//
//	NTSTATUS status = True_NtCreatePort(
//		PortHandle, 
//		ObjectAttributes, 
//		MaxDataSize, 
//		MaxMessageSize, 
//		Reserved);
//
//	GetCallStackOnNtSuccess(status, Index_NtCreatePort, *PortHandle, NULL);
//
//	return status;
//}
//
//
//typedef  NTSTATUS (NTAPI * Type_NtConnectPort)(PHANDLE, PUNICODE_STRING, PSECURITY_QUALITY_OF_SERVICE, PVOID, PVOID, PULONG, PVOID, PULONG);
//static NTSTATUS (NTAPI * True_NtConnectPort)(
//	OUT   PHANDLE  PortHandle,
//	IN   PUNICODE_STRING  PortName,
//	IN   PSECURITY_QUALITY_OF_SERVICE  SecurityQos,
//	IN  OUT  PVOID  WriteSection OPTIONAL, 
//	IN  OUT  PVOID  ReadSection  OPTIONAL,  
//	OUT PULONG  MaxMessageSize OPTIONAL,
//	IN   OUT  PVOID ConnectData OPTIONAL,
//	IN   OUT  PULONG  ConnectDataLength OPTIONAL
//	) = (Type_NtConnectPort)DetourFindFunction("ntdll.dll", "NtConnectPort");
//
//NTSTATUS NTAPI Fake_NtConnectPort(
//								   OUT   PHANDLE  PortHandle,
//								   IN   PUNICODE_STRING  PortName,
//								   IN   PSECURITY_QUALITY_OF_SERVICE  SecurityQos,
//								   IN  OUT  PVOID  WriteSection OPTIONAL, 
//								   IN  OUT  PVOID  ReadSection  OPTIONAL,  
//								   OUT PULONG  MaxMessageSize OPTIONAL,
//								   IN   OUT  PVOID ConnectData OPTIONAL,
//								   IN   OUT  PULONG  ConnectDataLength OPTIONAL
//								   )
//{
//	CFunCallerLogger logger(_T("NtConnectPort"));
//
//	NTSTATUS status = True_NtConnectPort(
//		PortHandle, 
//		PortName, 
//		SecurityQos, 
//		WriteSection, 
//		ReadSection, 
//		MaxMessageSize, 
//		ConnectData,
//		ConnectDataLength);
//
//	GetCallStackOnNtSuccess(status, Index_NtConnectPort, *PortHandle, NULL);
//
//	return status;
//}
//
//typedef  NTSTATUS (NTAPI * Type_NtSecureConnectPort)(PHANDLE, PUNICODE_STRING, PSECURITY_QUALITY_OF_SERVICE, PVOID, PSID, PVOID, PULONG, PVOID, PULONG);
//static NTSTATUS (NTAPI * True_NtSecureConnectPort)(
//	OUT  PHANDLE  PortHandle,
//	IN   PUNICODE_STRING  PortName,
//	IN   PSECURITY_QUALITY_OF_SERVICE  SecurityQos,
//	IN   OUT PVOID   WriteSection OPTIONAL,
//	IN   PSID  ServerSid OPTIONAL,
//	IN   OUT PVOID   ReadSection OPTIONAL,
//	OUT  PULONG  MaxMessageSize OPTIONAL,
//	IN   OUT  PVOID ConnectData OPTIONAL,
//	IN   OUT  PULONG  ConnectDataLength OPTIONAL
//	) = (Type_NtSecureConnectPort)DetourFindFunction("ntdll.dll", "NtSecureConnectPort");
//
//NTSTATUS NTAPI Fake_NtSecureConnectPort(
//								   OUT  PHANDLE  PortHandle,
//								   IN   PUNICODE_STRING  PortName,
//								   IN   PSECURITY_QUALITY_OF_SERVICE  SecurityQos,
//								   IN   OUT PVOID   WriteSection OPTIONAL,
//								   IN   PSID  ServerSid OPTIONAL,
//								   IN   OUT PVOID   ReadSection OPTIONAL,
//								   OUT  PULONG  MaxMessageSize OPTIONAL,
//								   IN   OUT  PVOID ConnectData OPTIONAL,
//								   IN   OUT  PULONG  ConnectDataLength OPTIONAL
//								   )
//{
//	CFunCallerLogger logger(_T("NtSecureConnectPort"));
//
//	NTSTATUS status = True_NtSecureConnectPort(
//		PortHandle, 
//		PortName, 
//		SecurityQos, 
//		WriteSection, 
//		ServerSid,
//		ReadSection, 
//		MaxMessageSize, 
//		ConnectData,
//		ConnectDataLength);
//
//	GetCallStackOnNtSuccess(status, Index_NtSecureConnectPort, *PortHandle, NULL);
//
//	return status;
//}
//
//typedef  NTSTATUS (NTAPI * Type_NtAcceptConnectPort)(PHANDLE, ULONG, PVOID, BOOLEAN, PVOID, PVOID);
//static NTSTATUS (NTAPI * True_NtAcceptConnectPort)(
//	OUT PHANDLE  PortHandle, 
//	IN ULONG  PortIdentifier, 
//	IN PVOID  Message, 
//	IN BOOLEAN  Accept,
//	IN OUT PVOID   WriteSection OPTIONAL,
//	IN OUT PVOID   ReadSection OPTIONAL
//	) = (Type_NtAcceptConnectPort)DetourFindFunction("ntdll.dll", "NtAcceptConnectPort");
//
//NTSTATUS NTAPI Fake_NtAcceptConnectPort(
//								   OUT PHANDLE  PortHandle, 
//								   IN ULONG  PortIdentifier, 
//								   IN PVOID  Message, 
//								   IN BOOLEAN  Accept,
//								   IN OUT PVOID   WriteSection OPTIONAL,
//								   IN OUT PVOID   ReadSection OPTIONAL
//								   )
//{
//	CFunCallerLogger logger(_T("NtAcceptConnectPort"));
//
//	NTSTATUS status = True_NtAcceptConnectPort(
//		PortHandle, 
//		PortIdentifier, 
//		Message, 
//		Accept, 
//		WriteSection, 
//		ReadSection);
//
//	GetCallStackOnNtSuccess(status, Index_NtAcceptConnectPort, *PortHandle, NULL);
//
//	return status;
//}

//WaitablePort operation
//typedef  NTSTATUS (NTAPI * Type_NtCreateWaitablePort)(PHANDLE, POBJECT_ATTRIBUTES, ULONG, ULONG, ULONG);
//static NTSTATUS (NTAPI * True_NtCreateWaitablePort)(
//	OUT   PHANDLE  PortHandle,
//	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
//	IN   ULONG  MaxDataSize,
//	IN   ULONG  MaxMessageSize,
//	IN   ULONG  Reserved
//	) = (Type_NtCreateWaitablePort)DetourFindFunction("ntdll.dll", "NtCreateWaitablePort");
//
//NTSTATUS NTAPI Fake_NtCreateWaitablePort(
//								  OUT   PHANDLE  PortHandle,
//								  IN   POBJECT_ATTRIBUTES  ObjectAttributes,
//								  IN   ULONG  MaxDataSize,
//								  IN   ULONG  MaxMessageSize,
//								  IN   ULONG  Reserved
//								  )
//{
//	CFunCallerLogger logger(_T("NtCreateWaitablePort"));
//
//	NTSTATUS status = True_NtCreateWaitablePort(
//		PortHandle, 
//		ObjectAttributes, 
//		MaxDataSize, 
//		MaxMessageSize, 
//		Reserved);
//
//	GetCallStackOnNtSuccess(status, Index_NtCreateWaitablePort, *PortHandle, NULL);
//
//	return status;
//}

//Adapter operation RING3未暴露

//Controller operation RING3未暴露

//Device operation RING3未暴露

//Driver operation RING3未暴露

//IoCompletion operation
typedef NTSTATUS (NTAPI * Type_NtCreateIoCompletion)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, ULONG);
static NTSTATUS (NTAPI * True_NtCreateIoCompletion)(
	OUT   PHANDLE  IoCompletionHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   ULONG  NumberOfConcurrentThreads) = (Type_NtCreateIoCompletion)DetourFindFunction("ntdll.dll", "NtCreateIoCompletion");
NTSTATUS NTAPI Fake_NtCreateIoCompletion(
	OUT   PHANDLE  IoCompletionHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	IN   ULONG  NumberOfConcurrentThreads
	)
{
	CFunCallerLogger logger(_T("NtCreateIoCompletion"));

	NTSTATUS status = True_NtCreateIoCompletion(
		IoCompletionHandle,
		DesiredAccess,
		ObjectAttributes,
		NumberOfConcurrentThreads);

	GetCallStackOnNtSuccess(status, Index_NtCreateIoCompletion, *IoCompletionHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenIoCompletion)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NTSTATUS (NTAPI * True_NtOpenIoCompletion)(
	OUT   PHANDLE  IoCompletionHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes) = (Type_NtOpenIoCompletion)DetourFindFunction("ntdll.dll", "NtOpenIoCompletion");
NTSTATUS NTAPI Fake_NtOpenIoCompletion(
									   OUT   PHANDLE  IoCompletionHandle,
									   IN   ACCESS_MASK  DesiredAccess,
									   IN   POBJECT_ATTRIBUTES  ObjectAttributes
									   )
{
	CFunCallerLogger logger(_T("NtOpenIoCompletion"));

	NTSTATUS status = True_NtOpenIoCompletion(
		IoCompletionHandle,
		DesiredAccess,
		ObjectAttributes);

	GetCallStackOnNtSuccess(status, Index_NtOpenIoCompletion, *IoCompletionHandle, NULL);

	return status;
}


//File operation
typedef NTSTATUS (NTAPI * Type_NtCreateFile)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID, PLARGE_INTEGER, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG);
static NTSTATUS (NTAPI * True_NtCreateFile)(
	OUT   PHANDLE  FileHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT   PVOID   IoStatusBlock,
	IN   PLARGE_INTEGER  AllocationSize OPTIONAL,
	IN   ULONG  FileAttributes,
	IN   ULONG  ShareAccess,
	IN   ULONG  CreateDisposition,
	IN   ULONG  CreateOptions,
	IN   PVOID EaBuffer OPTIONAL,
	IN   ULONG  EaLength) = (Type_NtCreateFile)DetourFindFunction("ntdll.dll", "NtCreateFile");
NTSTATUS NTAPI Fake_NtCreateFile(
								 OUT   PHANDLE  FileHandle,
								 IN   ACCESS_MASK  DesiredAccess,
								 IN   POBJECT_ATTRIBUTES  ObjectAttributes,
								 OUT   PVOID   IoStatusBlock,
								 IN   PLARGE_INTEGER  AllocationSize OPTIONAL,
								 IN   ULONG  FileAttributes,
								 IN   ULONG  ShareAccess,
								 IN   ULONG  CreateDisposition,
								 IN   ULONG  CreateOptions,
								 IN   PVOID EaBuffer OPTIONAL,
								 IN   ULONG  EaLength
								 )
{
	CFunCallerLogger logger(_T("NtCreateFile"));

	NTSTATUS status = True_NtCreateFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength);

	GetCallStackOnNtSuccess(status, Index_NtCreateFile, *FileHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtCreateMailslotFile)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID, ULONG, ULONG, ULONG, PLARGE_INTEGER);
static NTSTATUS (NTAPI * True_NtCreateMailslotFile)(
	OUT   PHANDLE  FileHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT   PVOID   IoStatusBlock,
	IN   ULONG  CreateOptions,
	IN   ULONG  InBufferSize,
	IN   ULONG  MaxMessageSize,
	IN   PLARGE_INTEGER  ReadTimeout OPTIONAL) = (Type_NtCreateMailslotFile)DetourFindFunction("ntdll.dll", "NtCreateMailslotFile");
NTSTATUS NTAPI Fake_NtCreateMailslotFile(
	OUT   PHANDLE  FileHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT   PVOID   IoStatusBlock,
	IN   ULONG  CreateOptions,
	IN   ULONG  InBufferSize,
	IN   ULONG  MaxMessageSize,
	IN   PLARGE_INTEGER  ReadTimeout OPTIONAL
	)
{
	CFunCallerLogger logger(_T("NtCreateMailslotFile"));

	NTSTATUS status = True_NtCreateMailslotFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		CreateOptions,
		InBufferSize,
		MaxMessageSize,
		ReadTimeout);

	GetCallStackOnNtSuccess(status, Index_NtCreateMailslotFile, *FileHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtCreateNamedPipeFile)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID, ULONG, ULONG, ULONG, BOOLEAN, BOOLEAN, BOOLEAN, ULONG, ULONG, ULONG, PLARGE_INTEGER);
static NTSTATUS (NTAPI * True_NtCreateNamedPipeFile)(
	OUT   PHANDLE  FileHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT   PVOID   IoStatusBlock,
	IN   ULONG  ShareAccess,
	IN   ULONG  CreateDisposition, 
	IN  ULONG  CreateOptions,
	IN   BOOLEAN  TypeMessage,
	IN   BOOLEAN  ReadmodeMessage,
	IN   BOOLEAN  Nonblocking, 
	IN  ULONG  MaxInstances, 
	IN  ULONG  InBufferSize, 
	IN  ULONG  OutBufferSize,
	IN   PLARGE_INTEGER  DefaultTimeout OPTIONAL) = (Type_NtCreateNamedPipeFile)DetourFindFunction("ntdll.dll", "NtCreateNamedPipeFile");
NTSTATUS NTAPI Fake_NtCreateNamedPipeFile(
	OUT   PHANDLE  FileHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT   PVOID   IoStatusBlock,
	IN   ULONG  ShareAccess,
	IN   ULONG  CreateDisposition, 
	IN  ULONG  CreateOptions,
	IN   BOOLEAN  TypeMessage,
	IN   BOOLEAN  ReadmodeMessage,
	IN   BOOLEAN  Nonblocking, 
	IN  ULONG  MaxInstances, 
	IN  ULONG  InBufferSize, 
	IN  ULONG  OutBufferSize,
	IN   PLARGE_INTEGER  DefaultTimeout OPTIONAL
	)
{
	CFunCallerLogger logger(_T("NtCreateNamedPipeFile"));

	NTSTATUS status = True_NtCreateNamedPipeFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		TypeMessage,
		ReadmodeMessage,
		Nonblocking,
		MaxInstances,
		InBufferSize,
		OutBufferSize,
		DefaultTimeout);

	GetCallStackOnNtSuccess(status, Index_NtCreateNamedPipeFile, *FileHandle, NULL);

	return status;
}

typedef NTSTATUS (NTAPI * Type_NtOpenFile)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID, ULONG, ULONG);
static NTSTATUS (NTAPI * True_NtOpenFile)(
	OUT   PHANDLE  FileHandle,
	IN   ACCESS_MASK  DesiredAccess,
	IN   POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT   PVOID   IoStatusBlock,
	IN   ULONG  ShareAccess,
	IN   ULONG  OpenOptions) = (Type_NtOpenFile)DetourFindFunction("ntdll.dll", "NtOpenFile");
NTSTATUS NTAPI Fake_NtOpenFile(
							   OUT   PHANDLE  FileHandle,
							   IN   ACCESS_MASK  DesiredAccess,
							   IN   POBJECT_ATTRIBUTES  ObjectAttributes,
							   OUT   PVOID   IoStatusBlock,
							   IN   ULONG  ShareAccess,
							   IN   ULONG  OpenOptions
							   )
{
	CFunCallerLogger logger(_T("NtOpenFile"));

	NTSTATUS status = True_NtOpenFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		ShareAccess,
		OpenOptions);

	GetCallStackOnNtSuccess(status, Index_NtOpenFile, *FileHandle, NULL);

	return status;
}



//WmiGuid operation RING3未暴露

typedef NTSTATUS (NTAPI * Type_NtDuplicateObject)(HANDLE, HANDLE, HANDLE, PHANDLE, ACCESS_MASK, ULONG, ULONG);
static NTSTATUS (NTAPI * True_NtDuplicateObject)(
	IN   HANDLE  SourceProcessHandle,
	IN   HANDLE  SourceHandle,
	IN   HANDLE  TargetProcessHandle,
	OUT   PHANDLE  TargetHandle OPTIONAL,
	IN   ACCESS_MASK  DesiredAccess,
	IN   ULONG  Attributes,
	IN   ULONG  Options) = (Type_NtDuplicateObject)DetourFindFunction("ntdll.dll", "NtDuplicateObject");
NTSTATUS NTAPI Fake_NtDuplicateObject(
									  IN   HANDLE  SourceProcessHandle,
									  IN   HANDLE  SourceHandle,
									  IN   HANDLE  TargetProcessHandle,
									  OUT   PHANDLE  TargetHandle OPTIONAL,
									  IN   ACCESS_MASK  DesiredAccess,
									  IN   ULONG  Attributes,
									  IN   ULONG  Options
									  )
{
	CFunCallerLogger logger(_T("NtDuplicateObject"));

	NTSTATUS status = True_NtDuplicateObject(
		SourceProcessHandle,
		SourceHandle,
		TargetProcessHandle,
		TargetHandle, 
		DesiredAccess,
		Attributes,
		Options);

	if ((HANDLE)(-1) == TargetProcessHandle)
	{
		return status;
	}

	GetCallStackOnNtSuccess(status, Index_NtDuplicateObject, *TargetHandle, NULL);

	return status;
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
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQueryVirtualMemory");

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

		//BOOL bIsWindow7 = IsWindows7();

		BOOL bAddAllOK = FALSE;

		InvokeInsertFuntion(pDetoursor, NtClose, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, CloseDesktop, bAddAllOK);			
		InvokeInsertFuntion(pDetoursor, CloseWindowStation, bAddAllOK);	

		InvokeInsertFuntion(pDetoursor, NtCreateToken, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtDuplicateToken, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenProcessToken, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenProcessTokenEx, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenThreadToken, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenThreadTokenEx, bAddAllOK);	


		InvokeInsertFuntion(pDetoursor, NtCreateProcess, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtCreateProcessEx, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtCreateUserProcess, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtGetNextProcess, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenProcess, bAddAllOK);		

		InvokeInsertFuntion(pDetoursor, NtCreateThread, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtCreateThreadEx, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtGetNextThread, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenThread, bAddAllOK);	

		InvokeInsertFuntion(pDetoursor, NtCreateJobObject, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenJobObject, bAddAllOK);		

		InvokeInsertFuntion(pDetoursor, NtCreateEvent, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenEvent, bAddAllOK);	

		InvokeInsertFuntion(pDetoursor, NtCreateMutant, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenMutant, bAddAllOK);

		InvokeInsertFuntion(pDetoursor, NtCreateSemaphore, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenSemaphore, bAddAllOK);	

		InvokeInsertFuntion(pDetoursor, NtCreateTimer, bAddAllOK);		
		InvokeInsertFuntion(pDetoursor, NtOpenTimer, bAddAllOK);		

		InvokeInsertFuntion(pDetoursor, CreateDesktopA, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, CreateDesktopW, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, OpenDesktopA, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, OpenDesktopW, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, OpenInputDesktop, bAddAllOK);

		InvokeInsertFuntion(pDetoursor, OpenWindowStationA, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, OpenWindowStationW, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, CreateWindowStationA, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, CreateWindowStationW, bAddAllOK);

		InvokeInsertFuntion(pDetoursor, NtCreateSection, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, NtOpenSection, bAddAllOK);

		InvokeInsertFuntion(pDetoursor, NtCreateKey, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, NtOpenKey, bAddAllOK);

		InvokeInsertFuntion(pDetoursor, NtCreateIoCompletion, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, NtOpenIoCompletion, bAddAllOK);

		InvokeInsertFuntion(pDetoursor, NtCreateFile, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, NtCreateMailslotFile, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, NtCreateNamedPipeFile, bAddAllOK);
		InvokeInsertFuntion(pDetoursor, NtOpenFile, bAddAllOK);

		InvokeInsertFuntion(pDetoursor, NtDuplicateObject, bAddAllOK);

		if (FALSE == bAddAllOK)
		{
			pDetoursor->ClearFunctionTable();
			return FALSE;
		}

		return TRUE;
	}


	VOID InitResourceForFakeFunction()
	{
		//InitializeCriticalSection(&g_csForStackDataBuffer);
	}

	VOID CleanResourceForFakeFunction()
	{
		//DeleteCriticalSection(&g_csForStackDataBuffer);
	}
}
#endif