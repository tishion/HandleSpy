#pragma once

#include "stdafx.h"
#include <windows.h>
#include <string>
#include <assert.h>

#define CV_SIGNATURE_NB10   '01BN'
#define CV_SIGNATURE_RSDS   'SDSR'

// CodeView header 
struct CV_HEADER 
{
	DWORD CvSignature; // NBxx
	LONG  Offset;      // Always 0 for NB10
};

// CodeView NB10 debug information 
// (used when debug information is stored in a PDB 2.00 file) 
struct CV_INFO_PDB20 
{
	CV_HEADER  Header; 
	DWORD      Signature;       // seconds since 01.01.1970
	DWORD      Age;             // an always-incrementing value 
	BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file 
};

// CodeView RSDS debug information 
// (used when debug information is stored in a PDB 7.00 file) 
struct CV_INFO_PDB70 
{
	DWORD      CvSignature; 
	GUID       Signature;       // unique identifier 
	DWORD      Age;             // an always-incrementing value 
	BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file 
};

BOOL IsPEFile(LPVOID ImageBase)
{
    PIMAGE_DOS_HEADER  pDH = NULL;
    PIMAGE_NT_HEADERS  pNtH = NULL;
	
    if(!ImageBase)
		return FALSE;
	
    pDH = (PIMAGE_DOS_HEADER)ImageBase;
    if (pDH->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;
	
    pNtH = (PIMAGE_NT_HEADERS32)UIntToPtr(PtrToUint(pDH) + pDH->e_lfanew);
    if (pNtH->Signature != IMAGE_NT_SIGNATURE )
        return FALSE;
	
    return TRUE;
}

PIMAGE_NT_HEADERS  GetNtHeaders(LPVOID ImageBase)
{
	if(!IsPEFile(ImageBase))
		return NULL;

	PIMAGE_NT_HEADERS  pNtH;
	PIMAGE_DOS_HEADER  pDH;
	pDH = (PIMAGE_DOS_HEADER)ImageBase;
	pNtH = (PIMAGE_NT_HEADERS)UIntToPtr(PtrToUint(pDH) + pDH->e_lfanew);
	return pNtH;
}

PIMAGE_FILE_HEADER   GetFileHeader(LPVOID ImageBase)
{
    PIMAGE_DOS_HEADER  pDH=NULL;
    PIMAGE_NT_HEADERS  pNtH=NULL;
    PIMAGE_FILE_HEADER pFH=NULL;
    
    if (!IsPEFile(ImageBase))
		return NULL;
    pDH = (PIMAGE_DOS_HEADER)ImageBase;
    pNtH = (PIMAGE_NT_HEADERS)UIntToPtr(PtrToUint(pDH) + pDH->e_lfanew);
    pFH = &pNtH->FileHeader;
    return pFH;
}

PIMAGE_OPTIONAL_HEADER GetOptionalHeader(LPVOID ImageBase)
{
    PIMAGE_DOS_HEADER  pDH=NULL;
    PIMAGE_NT_HEADERS  pNtH=NULL;
    PIMAGE_OPTIONAL_HEADER pOH=NULL;
	
	if (!IsPEFile(ImageBase))
		return NULL;

    pDH = (PIMAGE_DOS_HEADER)ImageBase;
    pNtH = (PIMAGE_NT_HEADERS)UIntToPtr(PtrToUint(pDH) + pDH->e_lfanew);
    pOH = &pNtH->OptionalHeader;
    return pOH;
}

PIMAGE_SECTION_HEADER GetFirstSectionHeader(LPVOID ImageBase)
{
	PIMAGE_NT_HEADERS     pNtH=NULL;
    PIMAGE_SECTION_HEADER pSH=NULL;
    
    pNtH = GetNtHeaders(ImageBase);
   	pSH = IMAGE_FIRST_SECTION(pNtH);
	return  pSH;
}

const std::wstring GetModuleIndexString(void *pModBase, BOOL bPDB)
{
	wchar_t sig[128] = {0};
	if (!IsPEFile(pModBase))
	{
		assert(0);
		return std::wstring();
	}

	if (!bPDB) //time stamp and size of the image for a executable file
	{
		IMAGE_NT_HEADERS *pNTHeaders = GetNtHeaders(pModBase);
		assert(pNTHeaders);
		if (pNTHeaders)
			swprintf_s(sig, L"%08X%x", pNTHeaders->FileHeader.TimeDateStamp, pNTHeaders->OptionalHeader.SizeOfImage);
	}
	else // signature and age for a pdb file.
	{
		do
		{
			PIMAGE_NT_HEADERS     pNtH=NULL;
			PIMAGE_OPTIONAL_HEADER pOH=NULL;

			pNtH = GetNtHeaders(pModBase);
			if (!pNtH)
				return std::wstring();
			pOH = GetOptionalHeader(pModBase);
			if (!pOH)
				return std::wstring();

			IMAGE_DEBUG_DIRECTORY *pDebugDir = (IMAGE_DEBUG_DIRECTORY*)((BYTE*)pModBase + pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress);

			ULONG size = pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
			
			//assert(pDebugDir && size == sizeof(IMAGE_DEBUG_DIRECTORY));
			if (!pDebugDir || size == 0 || size % sizeof(IMAGE_DEBUG_DIRECTORY) != 0)
				break;

			if (IsBadReadPtr(pDebugDir, size))
			{
				assert(0);
				break;
			}
			if (!pDebugDir->AddressOfRawData)
				break;

			LPBYTE pDebugInfo = (LPBYTE)pModBase + pDebugDir->AddressOfRawData;
			ULONG ulDebugSize = pDebugDir->SizeOfData;
			assert(pDebugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW);
			if (pDebugDir->Type != IMAGE_DEBUG_TYPE_CODEVIEW)
				break;

			if (IsBadReadPtr(pDebugInfo, ulDebugSize))
			{
				assert(0);
				break;
			}
				
			DWORD CvSignature = *(DWORD*)pDebugInfo; 
			if (CvSignature == CV_SIGNATURE_NB10)//CodeView format: NB10
			{
				// NB10 -> PDB 2.00 
				CV_INFO_PDB20* pCvInfo = (CV_INFO_PDB20*)pDebugInfo; 
				if (IsBadReadPtr( pDebugInfo, sizeof(CV_INFO_PDB20)) 
					|| IsBadStringPtrA( (CHAR*)pCvInfo->PdbFileName, UINT_MAX)) 
				{
					assert(0);
					break;
				}

				swprintf_s(sig, L"%08X%x", pCvInfo->Signature, pCvInfo->Age);
			}
			else if( CvSignature == CV_SIGNATURE_RSDS ) 
			{
				// RSDS -> PDB 7.00 
				CV_INFO_PDB70* pCvInfo = (CV_INFO_PDB70*)pDebugInfo; 
				if (IsBadReadPtr(pDebugInfo, sizeof(CV_INFO_PDB70)) 
					|| IsBadStringPtrA((CHAR*)pCvInfo->PdbFileName, UINT_MAX)) 
				{
					assert(0);
					break;
				}

				swprintf_s(sig, L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%x", 
					pCvInfo->Signature.Data1, pCvInfo->Signature.Data2, pCvInfo->Signature.Data3,
					pCvInfo->Signature.Data4[0], pCvInfo->Signature.Data4[1], pCvInfo->Signature.Data4[2], 
					pCvInfo->Signature.Data4[3], pCvInfo->Signature.Data4[4], pCvInfo->Signature.Data4[5],
					pCvInfo->Signature.Data4[6], pCvInfo->Signature.Data4[7],
					pCvInfo->Age); 
			}
			else 
			{
				assert(0);
				break;
			}

		} while (0);
	}


	//assert(wcslen(sig) > 0);
	return std::wstring(sig);
}