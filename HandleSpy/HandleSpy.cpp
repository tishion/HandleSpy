// HandleSpy.cpp : main source file for HandleSpy.exe
//

#include "stdafx.h"

#include "DetectDlg.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "Inject.h"

#include "SymbolHandler.h"

TCHAR* g_SingleInstacneMutexName = _T("HandleSpy_{2F1B687C-A7B8-45ba-AFC4-32900FA16559}_SingleInstanceMutexName");

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	// 创建互斥量
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, g_SingleInstacneMutexName);
	// 检查错误代码
	if (::GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		AtlMessageBox(
			NULL, 
			_T("只允许一个HandleSpy实例运行！"), 
			_T("HandleSpy"));
		::CloseHandle(hMutex);
		 hMutex = NULL;
		 return FALSE;
	}

	if (FALSE == EnableDebugPrivilege())
	{
		AtlMessageBox(
			NULL, 
			_T("调试权限获取失败，少数进程可能无法正常使用该工具！"), 
			_T("提示"));
	}

	//初始化GDI+
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	//初始化符号管理器
	CONST TCHAR* lpConfigFileName = _T("Config.ini");
	TCHAR szExePath[_MAX_PATH];
	::ZeroMemory(szExePath, sizeof(szExePath));
	::GetModuleFileName(NULL, szExePath, _countof(szExePath));
	LPTSTR lpName = ::PathFindFileName(szExePath);
	_tcscpy_s(lpName, _tcslen(lpConfigFileName)+1, lpConfigFileName);
	CSymbolHandler::GetInstance()->Init(szExePath);
	
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	GdiplusShutdown(gdiplusToken);

	return nRet;
}
