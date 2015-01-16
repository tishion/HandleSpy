#ifndef __CALLSTACKTYPEDEF_H__
#define __CALLSTACKTYPEDEF_H__

//////////////////////////////////////////////////////////////////////////
typedef struct __MOD_INFO
{
	DWORD dwModuleBase;
	DWORD dwImageSize;
	DWORD dwTimeStamp;
	WCHAR wcszModPath[MAX_PATH];
	WCHAR wcszPdbSig[64];

	__MOD_INFO(void)
	{
		ZeroMemory(this, sizeof(__MOD_INFO));
	}
}MOD_INFO, *PMOD_INFO;

typedef struct __STACK_FRAME 
{
	DWORD dwAddr;
	SIZE_T iIndex;

	__STACK_FRAME(void)
	{
		ZeroMemory(this, sizeof(__STACK_FRAME));
	}
}STACK_FRAME, *PSTACK_FRAME;


typedef struct __CALL_STACK
{
	DWORD Type;
	DWORD dwTimeStamp;		
	
	HANDLE Handle;
	HANDLE Handle2;
	
	DWORD nFrameCount;
	DWORD dwReserve;
	//////////////////////
	STACK_FRAME frame[ANYSIZE_ARRAY];
}CALL_STACK, *PCALL_STACK;

#endif