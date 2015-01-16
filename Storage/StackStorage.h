
#ifndef __CSTORAGE_H__
#define __CSTORAGE_H__

#pragma once

#include <vector>
#include "CallStackTypeDefine.h"
#include "CountAndTimeTypeDefine.h"

/************************************************************************\
 *  -------------------
 * | STACK_FILE_HEADER |
 * |-------------------|
 * | STACK_DATA_BUFFER |
 * |-------------------|
 * |  MOD_INFO_BUFFER  |
 * |-------------------|
 * |EXTENDED_DATABUFFER|
 *  -------------------
\************************************************************************/

typedef struct  __STACK_FILE_HEADER 
{
	DWORD	signature;		//
	WCHAR	wcsExePath[_MAX_PATH];
	DWORD	dwStackDataLen;
	DWORD	dwModInfoOffset;
	DWORD	dwModInfoLen;
	DWORD	dwExtendedOffset;
	DWORD	dwExtendedDataLen;
	
	__STACK_FILE_HEADER()
	{
		ZeroMemory(this, sizeof(__STACK_FILE_HEADER));
		this->signature = 'HSPY';
	}
}STACK_FILE_HEADER, *PSTACK_FILE_HEADER;

typedef std::basic_string<TCHAR> StringT;

class CStackStorage
{
public:
	static CStackStorage* GetInstance();

	BOOL InitSharedFileMapping(DWORD dwMaxSize);
	BOOL Open();
	void Close();
	
	void SetHeaderData(LPCWSTR lpExeFilePathName);
	LPVOID GetStackDataBufferTail();
	void IncreaseStackDataLen(DWORD dwLen);
	void SetModInfoData(std::vector<MOD_INFO>& vec);

#ifndef INJECTED_MODULE	
	BOOL CheckFileFormat();
	BOOL ReadFromFile(LPCTSTR lpFilePathName);
	BOOL WriteToFile(LPCTSTR lpFilePathName);

	void SetExtendedData(std::vector<COUNT_TIME>& vec);

	PCALL_STACK GetStackDataBuffer();
	PMOD_INFO GetModInfoBuffer();
	PCOUNT_TIME GetExtendedBuffer();

	BOOL GetCallStackData(std::vector<CALL_STACK*>& vec);
	BOOL GetModInfoData(std::vector<MOD_INFO*>& vec);
	BOOL GetExtendedData(std::vector<COUNT_TIME>& vec);
	BOOL GenerateStorageFileName(CString& str);

#endif

	
protected:
	CStackStorage(void);
	~CStackStorage(void);

private:
	HANDLE		m_hFileMapping;
	LPVOID		m_lpBuf;

	PSTACK_FILE_HEADER	m_pHeader;
	
	DWORD		m_dwStackDataLen;

	static TCHAR s_StorageMemName[_MAX_PATH];
};
#endif