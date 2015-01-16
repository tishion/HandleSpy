#ifdef _DEBUG
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define DEBUG_STRING_MAX_LENGTH	1024

int _cdecl DPrintf(CONST TCHAR *format, ...)
{
	va_list arglist;
	TCHAR tchDebugString[DEBUG_STRING_MAX_LENGTH];
	int nBufLen = -1;

	ZeroMemory(tchDebugString, DEBUG_STRING_MAX_LENGTH*sizeof(TCHAR));

	if (format == NULL)
	{
		OutputDebugString(_T("DebugPrintf() FAILED: Format is null pointer!\r\n"));
		return nBufLen;
	}

	va_start(arglist, format);
	nBufLen = _vstprintf_s((TCHAR *)tchDebugString, DEBUG_STRING_MAX_LENGTH, format, arglist);

	if (nBufLen == -1)
	{
		OutputDebugString(_T("DebugPrintf() FAILED: String is too long!\r\n"));
		return nBufLen;
	}

	//OutputDebugString(_T("DebugPrintf:"));
	OutputDebugString(tchDebugString);
	return nBufLen;
};
#else
#define DPrintf __noop
#endif