#pragma once

#include <string>
#include <vector>

#include "CallStackTypeDefine.h"

typedef std::basic_string<TCHAR> StringT;

typedef struct __MOD_SYMBOL_INFO
{
	MOD_INFO	modinfo;
	WCHAR		wcszPdbPath[MAX_PATH];
}MOD_SYMBOL_INFO, *PMOD_SYMBOL_INFO;

class CSymbolHandler
{
public:

	static CSymbolHandler* GetInstance();

	BOOL Init(LPCTSTR lpConfigFile);
	void LoadSymbols(std::vector<MOD_INFO*>& vec);
	StringT FrameNameFromAddr(STACK_FRAME sf);
	BOOL GetSourceFileAndLineNumber(STACK_FRAME sf, StringT& strSourceFile, DWORD& dwLineNumber);

	void ClearAllSymbols();

	CSymbolHandler(void);
	~CSymbolHandler(void);

	
private:
	std::vector<StringT>			m_vecSymbolPaths;
	std::vector<MOD_SYMBOL_INFO>	m_vecModSymbolInfo;	
	StringT							m_strSymboPaths;
	HANDLE							m_hProcess;
};
