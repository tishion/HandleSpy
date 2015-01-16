#include "StdAfx.h"
#include "Detector.h"
#include "Inject.h"
#include "StackStorage.h"

#define STORAGE_BUFFER_SIZE	10*1024*1024

CDetector::CDetector()
{
	m_dwPid = 0;
	m_bIsWorking = FALSE;
	m_hProcess = NULL;
	m_hInjectedModule = NULL;

	m_hWndHolder = NULL;
}

CDetector::~CDetector(void)
{
}

BOOL CDetector::Init(DWORD pid, HWND h)
{
	if (TRUE == m_bIsWorking)
	{
		return FALSE;
	}

	if (FALSE == ::IsWindow(h))
	{
		return FALSE;
	}

	m_hWndHolder = h;

	CHAR szHSHookPath[_MAX_PATH];
	ZeroMemory(szHSHookPath, sizeof(szHSHookPath));
	DWORD dwLen = ::GetModuleFileNameA(NULL, szHSHookPath, _countof(szHSHookPath));

	if (dwLen <= 0)
	{
		return FALSE;
	}

	CHAR drive[_MAX_PATH];
	CHAR dir[_MAX_PATH];

	::ZeroMemory(drive, sizeof(drive));
	::ZeroMemory(dir, sizeof(dir));
	::_splitpath_s(szHSHookPath, drive, _countof(drive), dir, _countof(dir), NULL, 0, NULL, 0);
	::ZeroMemory(szHSHookPath, sizeof(szHSHookPath));
	::strcat_s(szHSHookPath, drive);
	::strcat_s(szHSHookPath, dir);
	::strcat_s(szHSHookPath, "HSHook.dll");

	m_strDllPath.Format(szHSHookPath);

	if (FALSE == ::PathFileExistsA(m_strDllPath))
	{
		AtlMessageBox(m_hWndHolder, _T("功能模块文件HSHook.dll丢失！"));
		return FALSE;
	}

	m_hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (NULL == m_hProcess)
	{
		AtlMessageBox(m_hWndHolder, _T("打开目标进程失败！"));
		return FALSE;
	}

	m_dwPid = pid;
	m_hInjectedModule = NULL;

	return TRUE;
}

BOOL CDetector::Start()
{
	BOOL bRet = FALSE;
	bRet = (CStackStorage::GetInstance()->InitSharedFileMapping(STORAGE_BUFFER_SIZE) &&
		CStackStorage::GetInstance()->Open());

	if (FALSE == bRet)
	{
		AtlMessageBox(m_hWndHolder, _T("设置数据存储空间失败！"));
		return FALSE;
	}

	bRet = InjectHSHook();
	if (FALSE == bRet)
	{
		CStackStorage::GetInstance()->Close();
		AtlMessageBox(m_hWndHolder, _T("功能模块注入目标进程失败！"));
	}

	m_bIsWorking = bRet;
	
	return bRet;
}

BOOL CDetector::Stop()
{
	BOOL bRet = UnInjectHSHook();
	if (TRUE == bRet)
	{
		m_bIsWorking = FALSE;
	}

	return bRet;
}

BOOL CDetector::InjectHSHook()
{
	if (TRUE == m_bIsWorking)
	{
		return FALSE;
	}

	if (FALSE == ::PathFileExistsA(m_strDllPath))
	{
		return FALSE;
	}

	if (FALSE == ::Inject(m_dwPid, m_strDllPath, (PDWORD)&m_hInjectedModule))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDetector::UnInjectHSHook()
{
	if (FALSE == m_bIsWorking)
	{
		return FALSE;
	}

	if (FALSE == ::UnLoadModule(m_dwPid, m_hInjectedModule))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CDetector::GetHandleCount(DWORD* pdwCount)
{
	if (FALSE == m_bIsWorking)
	{
		return FALSE;
	}

	if (NULL == m_hProcess)
	{
		return FALSE;
	}

	DWORD dwExitCode = 0;
	::GetExitCodeProcess(m_hProcess, &dwExitCode); 
	if (STILL_ACTIVE != dwExitCode)
	{
		//进程已经退出
		::SendMessage(m_hWndHolder, HSMSG_TARGETPROCESSEXIT, 0, 0);
		return FALSE;
	}

	return ::GetProcessHandleCount(m_hProcess, pdwCount);
}