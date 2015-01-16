#ifndef __DPRINTF_H__
#define __DPRINTF_H__
#ifdef _DEBUG
int _cdecl DPrintf(CONST TCHAR *format, ...);
#else
#define DPrintf __noop
#endif
#endif