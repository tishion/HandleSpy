#pragma once


#define HSMSG_TARGETPROCESSEXIT WM_USER+0x100

class CDetector
{
public:

	CDetector();
	~CDetector(void);

	BOOL Init(DWORD pid, HWND h);

	BOOL Start();
	BOOL Stop();

	BOOL InjectHSHook();
	BOOL UnInjectHSHook();

	BOOL GetHandleCount(DWORD* pdwCount);

private:
	HWND		m_hWndHolder;
	DWORD		m_dwPid;
	BOOL		m_bIsWorking;
	HANDLE		m_hProcess;
	HMODULE		m_hInjectedModule;
	CStringA	m_strDllPath;
};
