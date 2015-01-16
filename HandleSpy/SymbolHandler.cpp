#include "StdAfx.h"
#include <time.h>
#include "dbghelp.h"
#pragma comment(lib, ".\\lib\\dbghelp.lib")
#include "SymbolHandler.h"


CSymbolHandler* CSymbolHandler::GetInstance()
{
	static CSymbolHandler sInstance;
	return &sInstance;
}

CSymbolHandler::CSymbolHandler(void)
{
	m_strSymboPaths = _T("");
	m_vecSymbolPaths.clear();
	m_hProcess = NULL;
}

CSymbolHandler::~CSymbolHandler(void)
{
	if (NULL != m_hProcess)
	{
		::SymCleanup(m_hProcess);
		m_hProcess = NULL;
	}
}

BOOL CSymbolHandler::Init(LPCTSTR lpConfigFile)
{
	TCHAR szSymbolPaths[1024];
	ZeroMemory(szSymbolPaths, sizeof(szSymbolPaths));

	::GetPrivateProfileString(
		_T("Config"), 
		_T("sympath"), 
		_T(""), 
		szSymbolPaths, 
		_countof(szSymbolPaths), 
		lpConfigFile);

	m_strSymboPaths = szSymbolPaths;
	m_strSymboPaths += _T("c:\\windows\\symbols\\dll\\;");
	m_strSymboPaths += _T("c:\\windows\\system32\\;");

	size_t nBegin = 0;
	size_t nEnd = 0;
	StringT strPath;
	while (true)
	{
		nEnd = m_strSymboPaths.find(_T(';'), nBegin);
		if (StringT::npos == nEnd)
		{
			break;
		}

		strPath = m_strSymboPaths.substr(nBegin, nEnd-nBegin);

		if (*(strPath.rbegin()) != _T('\\'))
		{
			strPath += _T("\\");
		}

		m_vecSymbolPaths.push_back(strPath);
		nBegin = nEnd + 1;
	}

	if (m_vecSymbolPaths.empty())
	{
		return FALSE;
	}

	m_hProcess = (HANDLE)GetCurrentProcessId();

	DWORD dwOptions = SymGetOptions();

	dwOptions |= (SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_EXACT_SYMBOLS | SYMOPT_AUTO_PUBLICS | SYMOPT_IGNORE_NT_SYMPATH); 

	SymSetOptions(dwOptions);

	return SymInitialize(m_hProcess, "", FALSE);
}

void CSymbolHandler::LoadSymbols(std::vector<MOD_INFO*>& vec)
{
	WCHAR drive[_MAX_PATH];
	WCHAR dir[_MAX_PATH];
	WCHAR name[_MAX_PATH];
	WCHAR ext[_MAX_PATH];

	m_vecModSymbolInfo.clear();

	for (size_t i=0; i<vec.size(); i++)
	{
		::ZeroMemory(drive, sizeof(drive));
		::ZeroMemory(dir, sizeof(dir));
		::ZeroMemory(name, sizeof(name));
		::ZeroMemory(ext, sizeof(ext));

		::_wsplitpath_s(vec[i]->wcszModPath, 
			drive, _countof(drive), 
			dir, _countof(dir), 
			name, _countof(name), 
			ext, _countof(ext));

		::_wcslwr_s(dir, _countof(dir));
		StringT strDir = drive;
		strDir += dir;
		StringT strName = name;
		StringT strExt = ext;

		StringT strPdbSig = vec[i]->wcszPdbSig;

		StringT strPdbFilePath;
		BOOL bLoadSuccess = FALSE;

		MOD_SYMBOL_INFO	modSymbolInfo;
		ZeroMemory(&modSymbolInfo, sizeof(modSymbolInfo));
		RtlCopyMemory(&(modSymbolInfo.modinfo), vec[i], sizeof(MOD_INFO));

		USES_CONVERSION;
		if (!strDir.empty() && !strName.empty() && !strExt.empty() && !strPdbSig.empty())
		{		
			//DLL所在目录没有找到，则在sympath中的所有目录中枚举
			for (size_t j=0; (!bLoadSuccess)&&(j<m_vecSymbolPaths.size()); j++)
			{
				//构造PDB路径		sympathy\dllname.pdb\dllsig\dllname.pdb
				strPdbFilePath = m_vecSymbolPaths[j] + strName + _T(".pdb\\") + strPdbSig + _T("\\") + strName + _T(".pdb");
				if ((TRUE == ::PathFileExists(strPdbFilePath.c_str())) && (FALSE == PathIsDirectory(strPdbFilePath.c_str())))
				{
					::SymLoadModule64(m_hProcess, NULL, W2A(strPdbFilePath.c_str()), NULL, vec[i]->dwModuleBase, vec[i]->dwImageSize);
					if (ERROR_SUCCESS == GetLastError())
					{
						wcsncat_s(modSymbolInfo.wcszPdbPath, strPdbFilePath.c_str(), _countof(modSymbolInfo.wcszPdbPath));
						bLoadSuccess = TRUE;
						break;
					}
				}

				//构造PDB路径		sympath\dllname.pdb
				strPdbFilePath = m_vecSymbolPaths[j] + strName + _T(".pdb");
				if ((TRUE == ::PathFileExists(strPdbFilePath.c_str())) && (FALSE == PathIsDirectory(strPdbFilePath.c_str())))
				{
					::SymLoadModule64(m_hProcess, NULL, W2A(strPdbFilePath.c_str()), NULL, vec[i]->dwModuleBase, vec[i]->dwImageSize);
					if (ERROR_SUCCESS == GetLastError())
					{
						wcsncat_s(modSymbolInfo.wcszPdbPath, strPdbFilePath.c_str(), _countof(modSymbolInfo.wcszPdbPath));
						bLoadSuccess = TRUE;
						break;
					}
				}
			}
		}

		//最后尝试在dll所在目录加载PDB文件		dllpath\dllname.pdb\dllsig\dllname.pdb
		strPdbFilePath = strDir + strName + _T(".pdb\\") + strPdbSig + _T("\\") + strName + _T(".pdb");
		if ((!bLoadSuccess) && (TRUE == ::PathFileExists(strPdbFilePath.c_str())) && (FALSE == PathIsDirectory(strPdbFilePath.c_str())))
		{
			::SymLoadModule64(m_hProcess, NULL, W2A(strPdbFilePath.c_str()), NULL, vec[i]->dwModuleBase, vec[i]->dwImageSize);
			if (ERROR_SUCCESS == GetLastError())
			{					
				wcsncat_s(modSymbolInfo.wcszPdbPath, strPdbFilePath.c_str(), _countof(modSymbolInfo.wcszPdbPath));
				bLoadSuccess = TRUE;
			}
		}

		//											dllpath\dllname.pdb
		strPdbFilePath = strDir + strName + _T(".pdb");
		if ((!bLoadSuccess) && (TRUE == ::PathFileExists(strPdbFilePath.c_str())) && (FALSE == PathIsDirectory(strPdbFilePath.c_str())))
		{
			::SymLoadModule64(m_hProcess, NULL, W2A(strPdbFilePath.c_str()), NULL, vec[i]->dwModuleBase, vec[i]->dwImageSize);
			if (ERROR_SUCCESS == GetLastError())
			{
				wcsncat_s(modSymbolInfo.wcszPdbPath, strPdbFilePath.c_str(), _countof(modSymbolInfo.wcszPdbPath));
				bLoadSuccess = TRUE;
			}
		}

		m_vecModSymbolInfo.push_back(modSymbolInfo);
	}

	return ;
}


StringT CSymbolHandler::FrameNameFromAddr(STACK_FRAME sf)
{
	StringT strName;
	WCHAR wcsFunName[MAX_SYM_NAME];

	if (-1 == sf.iIndex)
	{
		::ZeroMemory(wcsFunName, sizeof(wcsFunName));
		swprintf_s(wcsFunName, _T("!UnKnowModule:0x%08x"), sf.dwAddr);
		strName = wcsFunName;
		return strName;
	}

	MOD_SYMBOL_INFO ModSyminfo = m_vecModSymbolInfo[sf.iIndex];

	WCHAR name[_MAX_PATH];
	WCHAR ext[_MAX_PATH];
	::ZeroMemory(name, sizeof(name));
	::ZeroMemory(ext, sizeof(ext));
	::_wsplitpath_s(ModSyminfo.modinfo.wcszModPath, 
		NULL, 0, 
		NULL, 0, 
		name, _countof(name), 
		ext, _countof(ext));

	strName = name;
	strName += ext;

	::ZeroMemory(wcsFunName, sizeof(wcsFunName));

	SYMBOL_INFO_PACKAGEW sip;
	ZeroMemory(&sip, sizeof(sip));
	sip.si.SizeOfStruct = sizeof(SYMBOL_INFO); 
	sip.si.MaxNameLen = sizeof(sip.name)/sizeof(sip.name[0]); 

	DWORD64 displacement = 0; 
	if (TRUE == SymFromAddrW(m_hProcess, sf.dwAddr, &displacement, &sip.si))
	{
		swprintf_s(wcsFunName, _T("!%s+0x%x"), sip.si.Name, sf.dwAddr - sip.si.Address);
	}
	else
	{
		swprintf_s(wcsFunName, _T("+0x%x"), sf.dwAddr - ModSyminfo.modinfo.dwModuleBase);
	}

	strName += wcsFunName;
	return strName;
}

BOOL CSymbolHandler::GetSourceFileAndLineNumber(STACK_FRAME sf, StringT& strSourceFile, DWORD& dwLineNumber)
{
	IMAGEHLP_LINEW64 line;
	line.SizeOfStruct = sizeof(line);
	DWORD displacement = 0;

	if (TRUE == SymGetLineFromAddrW64(m_hProcess, sf.dwAddr, &displacement, &line))
	{
		strSourceFile = line.FileName;
		dwLineNumber = line.LineNumber;
		return TRUE;
	}

	return FALSE;
}

void CSymbolHandler::ClearAllSymbols()
{
	for (UINT i=0; i<m_vecModSymbolInfo.size(); i++)
	{
		::SymUnloadModule64(m_hProcess, m_vecModSymbolInfo[i].modinfo.dwModuleBase);
	}

	m_vecModSymbolInfo.clear();
}