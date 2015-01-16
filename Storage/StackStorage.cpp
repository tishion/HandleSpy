#include "StdAfx.h"
#include "StackStorage.h"

TCHAR CStackStorage::s_StorageMemName[] = {_T("HandleSpy_{26C567F1-1CCA-46f8-B342-1B0F1EEF58B6}_StackStorageMemName")};

CStackStorage* CStackStorage::GetInstance()
{
	static CStackStorage s_storage;
	return &s_storage;
}

CStackStorage::CStackStorage(void)
{
	m_hFileMapping = NULL;
	m_lpBuf = NULL;

	m_dwStackDataLen = 0;
}

CStackStorage::~CStackStorage(void)
{
	if (NULL != m_lpBuf)
	{
		UnmapViewOfFile(m_lpBuf);
		m_lpBuf = NULL;
	}

	if (NULL != m_hFileMapping && INVALID_HANDLE_VALUE != m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}
}

BOOL CStackStorage::InitSharedFileMapping(DWORD dwBufferSize)
{
	if (NULL != m_lpBuf)
	{
		UnmapViewOfFile(m_lpBuf);
		m_lpBuf = NULL;
	}

	if (NULL != m_hFileMapping && INVALID_HANDLE_VALUE != m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}

	if (dwBufferSize == 0)				//打开已存在的
	{
		m_hFileMapping = OpenFileMapping(
			FILE_MAP_READ | FILE_MAP_WRITE, 
			FALSE,
			s_StorageMemName);
	}
	else if (dwBufferSize > 0)			//创建新的
	{
		m_hFileMapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,						// use paging file
			NULL,										// default security 
			PAGE_READWRITE,								// read/write access
			0,											// max. object size 
			dwBufferSize,								// buffer size  
			s_StorageMemName);							// name of mapping object
	}

	if (NULL == m_hFileMapping || INVALID_HANDLE_VALUE == m_hFileMapping) 
	{ 
		return FALSE;
	}

	if (dwBufferSize > 0)		//如果是新创建则清零
	{
		LPVOID lpBuf = MapViewOfFile(
			m_hFileMapping,											// handle to map object
			FILE_MAP_READ | FILE_MAP_WRITE,							// read/write permission
			0, 0, dwBufferSize);

		if (NULL == lpBuf)
		{
			CloseHandle(m_hFileMapping);
			m_hFileMapping = NULL;
			return FALSE;
		}

		ZeroMemory(lpBuf, dwBufferSize);

		UnmapViewOfFile(m_lpBuf);
		m_lpBuf = NULL;
	}

	m_dwStackDataLen = 0;

	return TRUE;
}

BOOL CStackStorage::Open()
{
	// 出错
	if (NULL == m_hFileMapping) 
	{ 
		return NULL;
	}

	m_lpBuf = (LPDWORD)MapViewOfFile(
		m_hFileMapping,											// handle to map object
		FILE_MAP_READ | FILE_MAP_WRITE,							// read/write permission
		0, 
		0, 
		0);

	// 出错
	if (NULL == m_lpBuf) 
	{ 
		return NULL;
	}

	m_pHeader = (PSTACK_FILE_HEADER)m_lpBuf;

	return TRUE;
}

void CStackStorage::Close()
{
	if (NULL != m_lpBuf)
	{
		UnmapViewOfFile(m_lpBuf);
		m_lpBuf = NULL;
	}

	if (NULL != m_hFileMapping && INVALID_HANDLE_VALUE != m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}
}


LPVOID CStackStorage::GetStackDataBufferTail()
{
	return  (LPVOID)((PUCHAR)m_lpBuf + sizeof(STACK_FILE_HEADER) + m_dwStackDataLen);
}

void  CStackStorage::IncreaseStackDataLen(DWORD dwLen)
{
	m_dwStackDataLen += dwLen;
	m_pHeader->dwStackDataLen = m_dwStackDataLen;
}

void CStackStorage::SetModInfoData(std::vector<MOD_INFO>& vec)
{
	DWORD dwOffset = 0;	
	dwOffset += sizeof(STACK_FILE_HEADER);
	dwOffset += m_pHeader->dwStackDataLen;

	m_pHeader->dwModInfoOffset = dwOffset;

	PUCHAR pBuf = ((PUCHAR)m_lpBuf + dwOffset);

	for (int i=0; i<(int)vec.size(); i++)
	{
		RtlCopyMemory(pBuf, &(vec[i]), sizeof(MOD_INFO));
		pBuf += sizeof(MOD_INFO);
	}
	
	m_pHeader->dwModInfoLen = (DWORD)(vec.size() * sizeof(MOD_INFO));
}

void CStackStorage::SetHeaderData(LPCWSTR lpExeFilePathName)
{
	m_pHeader->signature = 'HSPY';
	wcscpy_s(m_pHeader->wcsExePath, _countof(m_pHeader->wcsExePath), lpExeFilePathName);
}

#ifndef INJECTED_MODULE

BOOL CStackStorage::CheckFileFormat()
{
	if (NULL == m_hFileMapping ||
		NULL == m_lpBuf ||
		NULL == m_pHeader)
	{
		return FALSE;
	}

	DWORD dwValidSig = 'HSPY';

	if (dwValidSig != m_pHeader->signature)
	{
		return FALSE;
	}

	return TRUE;
}


void CStackStorage::SetExtendedData(std::vector<COUNT_TIME>& vec)
{
	DWORD dwOffset = 0;	
	dwOffset += sizeof(STACK_FILE_HEADER);
	dwOffset += m_pHeader->dwStackDataLen;
	dwOffset += m_pHeader->dwModInfoLen;

	m_pHeader->dwExtendedOffset = dwOffset;

	PUCHAR pBuf = (PUCHAR)((DWORD)m_lpBuf + dwOffset);  

	for (int i=0; i<(int)vec.size(); i++)
	{
		RtlCopyMemory(pBuf, &(vec[i]), sizeof(COUNT_TIME));
		pBuf += sizeof(COUNT_TIME);
	}

	m_pHeader->dwExtendedDataLen = vec.size() * sizeof(COUNT_TIME);
}

PCALL_STACK CStackStorage::GetStackDataBuffer()
{
	return (PCALL_STACK)((PUCHAR)m_lpBuf + sizeof(STACK_FILE_HEADER));
}

PMOD_INFO CStackStorage::GetModInfoBuffer()
{
	return (PMOD_INFO)((PUCHAR)m_lpBuf + m_pHeader->dwModInfoOffset);
}

PCOUNT_TIME CStackStorage::GetExtendedBuffer()
{
	return (PCOUNT_TIME)((PUCHAR)m_lpBuf + m_pHeader->dwExtendedOffset);
}

BOOL CStackStorage::GetCallStackData(std::vector<CALL_STACK*>& vec)
{
	vec.clear();

	if (NULL == m_lpBuf)
	{
		return FALSE;
	}

	PUCHAR pBuf = ((PUCHAR)m_lpBuf + sizeof(STACK_FILE_HEADER));
	PCALL_STACK pCs = NULL;
	DWORD dwLen = 0;

	while (dwLen < m_pHeader->dwStackDataLen)
	{
		pCs = (PCALL_STACK)((PUCHAR)pBuf + dwLen); 
		vec.push_back(pCs);
		dwLen += sizeof(CALL_STACK);
		dwLen += ((pCs->nFrameCount-1) * sizeof(STACK_FRAME));
	}

	return TRUE;
}

BOOL CStackStorage::GetModInfoData(std::vector<MOD_INFO*>& vec)
{
	vec.clear();

	if (NULL == m_lpBuf)
	{
		return FALSE;
	}

	PMOD_INFO pMi = (PMOD_INFO)((PUCHAR)m_lpBuf + m_pHeader->dwModInfoOffset);
	int nCount = (m_pHeader->dwModInfoLen / sizeof(MOD_INFO));

	for (int i=0; i<nCount; i++)
	{
		vec.push_back(pMi+i);
	}

	return TRUE;
}

BOOL CStackStorage::GetExtendedData(std::vector<COUNT_TIME>& vec)
{
	vec.clear();

	if (NULL == m_lpBuf)
	{
		return FALSE;
	}

	PCOUNT_TIME pCt = (PCOUNT_TIME)((PUCHAR)m_lpBuf + m_pHeader->dwExtendedOffset);
	int nCount = (m_pHeader->dwExtendedDataLen / sizeof(COUNT_TIME));

	for (int i=0; i<nCount; i++)
	{
		vec.push_back(*(pCt+i));
	}

	return TRUE;
}

BOOL CStackStorage::GenerateStorageFileName(CString& str)
{
	if (NULL == m_pHeader)
	{
		return FALSE;
	}

	WCHAR name[_MAX_PATH];
	::ZeroMemory(name, sizeof(name));
	::_wsplitpath_s(m_pHeader->wcsExePath, NULL, 0, NULL, 0, name, _countof(name), NULL, 0);

	SYSTEMTIME systime;
	::GetSystemTime(&systime);

	str.Format(_T("%s_%d%d%d%d%d%d%d.hsl"), 
		name, 
		systime.wYear,
		systime.wMonth, 
		systime.wDay, 
		systime.wHour, 
		systime.wMinute, 
		systime.wSecond,
		systime.wMilliseconds);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////

BOOL CStackStorage::WriteToFile(LPCTSTR lpFilePathName)
{
	HANDLE hFile = CreateFile(
		lpFilePathName, 
		GENERIC_WRITE |GENERIC_READ, 
		FILE_SHARE_WRITE |FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		NULL, 
		NULL); 

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwNumberOfBytesWritten = 0;
	DWORD dwLen = 0;
	dwLen += sizeof(STACK_FILE_HEADER);
	dwLen += m_pHeader->dwStackDataLen;
	dwLen += m_pHeader->dwModInfoLen;
	dwLen += m_pHeader->dwExtendedDataLen;

	if (FALSE == WriteFile(hFile, m_lpBuf, dwLen, &dwNumberOfBytesWritten, NULL))
	{
		return FALSE;
	}

	CloseHandle(hFile);

	return TRUE;
}

BOOL CStackStorage::ReadFromFile(LPCTSTR lpFilePathName)
{
	HANDLE hFile = CreateFile(
		lpFilePathName, 
		GENERIC_WRITE |GENERIC_READ, 
		FILE_SHARE_WRITE |FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		NULL, 
		NULL); 

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (NULL != m_lpBuf)
	{
		UnmapViewOfFile(m_lpBuf);
		m_lpBuf = NULL;
	}

	if (NULL != m_hFileMapping && INVALID_HANDLE_VALUE != m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}


	m_hFileMapping = CreateFileMapping(
		hFile,
		NULL,										// default security 
		PAGE_READWRITE,								// read/write access
		0,											// max. object size 
		0,											// buffer size  
		NULL);										// name of mapping object

	CloseHandle(hFile);

	if (NULL == m_hFileMapping || INVALID_HANDLE_VALUE == m_hFileMapping) 
	{ 
		return FALSE;
	}

	return TRUE;
}

#endif