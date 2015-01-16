#include "CallStack.h"

extern const std::wstring GetModuleIndexString(void *pModBase, BOOL bPDB);

typedef struct __FRAME
{
	struct __FRAME * pOutFrame;
	DWORD dwret;
}FRAME, *PFRAME;

std::vector<MOD_INFO> CCallStack::s_vecModInfo;

/*
* 读取PEB中的模块列表，查找某一个地址所属的模块
*/
BOOL CCallStack::GetModuleInfoFromAddr(DWORD addr, RAW_MOD_INFO& modinfo)
{
	PPEB pPeb = NULL;

	/* 获取PEB地址*/
	__asm
	{
		push eax
		mov eax, fs:[0x30]
		mov pPeb, eax
		pop eax
	} 

	/* 从PEB中获取PEB_LDR_DATA结构地址 */
	PPEB_LDR_DATA pLdr = pPeb->Ldr;

	/* 从PEB_LDR_DATA结构中获取InLoadOrderModuleList.Flink
	* 该结构为LIST_ENTRY
	* 选取的LIST是InLoadOrderModuleList
	*/
	PLIST_ENTRY pListEntryOfInLoadOrderModuleList = pLdr->InLoadOrderModuleList.Flink;

	PLIST_ENTRY pCur = pListEntryOfInLoadOrderModuleList;

	PLDR_MODULE pLdrMod = NULL;

	BOOL bFound = FALSE;

	/* 前向遍历模块列表 */
	do 
	{
		/* 从LIST_ENTRY结构对齐到LDR_MODULE结构 */
		pLdrMod = CONTAINING_RECORD(pCur, LDR_MODULE, InLoadOrderModuleList);

		/* 判断该地址是否在当前模块 */
		if (addr > (DWORD)(pLdrMod->BaseAddress) && 
			addr < ((DWORD)(pLdrMod->BaseAddress) + pLdrMod->SizeOfImage))
		{
			/* 找到目标模块，取值，结束循环*/
			modinfo.dwModuleBase = (DWORD)pLdrMod->BaseAddress;
			modinfo.dwImageSize = (DWORD)pLdrMod->SizeOfImage;
			modinfo.dwTimeStamp = (DWORD)pLdrMod->TimeDateStamp;
			modinfo.pModPath = pLdrMod->FullDllName.Buffer;

			bFound = TRUE;
			break;
		}

		/* 遍历链表下一个LSIT_ENTRY */
		pCur = pCur->Flink;
	} while (pCur != pListEntryOfInLoadOrderModuleList);

	return bFound;
}


/*
* 获取当前函数调用点的完整调用堆栈
*
*/
DWORD CCallStack::GetCurrentCallStack(const PCALL_STACK pcs, DWORD dwMaxFrameCount)
{
	DWORD dwBytesWritten = 0;
	
	if (NULL == pcs)
	{
		return 0;
	}
	ZeroMemory(pcs, sizeof(CALL_STACK));

	PTEB pTeb = NULL;

	/* 获取TEB地址*/
	__asm
	{
		push eax
		mov eax, fs:[0x18]
		mov pTeb, eax
		pop eax
	}

	DWORD dwStackBase = (DWORD)(pTeb->NtTib.StackBase);
	DWORD dwStackLimit = (DWORD)(pTeb->NtTib.StackLimit);

 	DWORD dwEbp = 0;
	PFRAME pFrame = NULL;

	_asm mov dwEbp, ebp;

	DWORD dwFrameCount = 0;
	PSTACK_FRAME pStackFrame = pcs->frame;

	pFrame = (PFRAME)dwEbp;

	do 
	{
		/*
		 * 首先判断拿到的pFrame的值是否在栈内，不能通过栈的整边界来判断
		 * pFrame的值一定要大于栈的最低地址并且小于栈底（最高地址）减去一个DWORD的地址
		 * 因为如果pFrame的值等于栈的最高地址减去一个DWORD的地址时去访问pFrame->dwRet必然引起av异常
		 */
		if ((DWORD)pFrame > (dwStackBase - sizeof(DWORD)) || (DWORD)pFrame < dwStackLimit)
		{
			break;
		}

		if (IsBadReadPtr((PVOID)(pFrame->pOutFrame), sizeof(DWORD)) || NULL == pFrame->pOutFrame ||
			IsBadReadPtr((PVOID)(pFrame->pOutFrame->pOutFrame), sizeof(DWORD)) || NULL == pFrame->pOutFrame->pOutFrame ||
			IsBadReadPtr((PVOID)(pFrame->pOutFrame->dwret), sizeof(DWORD)) || NULL == pFrame->pOutFrame->dwret)
		{
			break;
		}

		pFrame = pFrame->pOutFrame;
		
		//if (IsBadReadPtr((PVOID)(pFrame), sizeof(DWORD)) || NULL == pFrame || 
		//	IsBadReadPtr((PVOID)(pFrame->dwret), sizeof(DWORD)) || NULL == pFrame->dwret)
		//{
		//	break;
		//}
		
		if (dwFrameCount >= dwMaxFrameCount)
		{
			break;
		}

		pStackFrame->dwAddr = pFrame->dwret;	/* 记录返回地址 */
		pStackFrame->iIndex = -1;				/* 记录模块序号 */

		dwFrameCount += 1;
		dwBytesWritten += sizeof(STACK_FRAME);
		
		RAW_MOD_INFO rawmi;
		if (TRUE == GetModuleInfoFromAddr(pStackFrame->dwAddr, rawmi))
		{
			/* 查找是否已经记录该木块 */
			for (int i=0; i<(int)s_vecModInfo.size(); i++)
			{
				if (rawmi.dwModuleBase == s_vecModInfo[i].dwModuleBase && 
					rawmi.dwTimeStamp == s_vecModInfo[i].dwTimeStamp)
				{
					pStackFrame->iIndex = i;
					break;
				}
			}

			/* 没有记录该模块 */
			if (-1 == pStackFrame->iIndex)
			{
				if (s_vecModInfo.size() < MAX_MODULE_COUNT)
				{
					/* 如果没有超过最大模块记录数，记录该模块 */
					MOD_INFO modinfo;
					modinfo.dwModuleBase = rawmi.dwModuleBase;
					modinfo.dwImageSize = rawmi.dwImageSize;
					modinfo.dwTimeStamp = rawmi.dwTimeStamp;
					std::wstring wstrPdbSig = GetModuleIndexString((void*)(modinfo.dwModuleBase), TRUE);

					wcscpy_s(modinfo.wcszPdbSig, _countof(modinfo.wcszPdbSig), wstrPdbSig.c_str());

					if (NULL != rawmi.pModPath)
					{
						wcscpy_s(modinfo.wcszModPath, _countof(modinfo.wcszModPath), rawmi.pModPath);
					}
					else
					{
						wcscpy_s(modinfo.wcszModPath, _countof(modinfo.wcszModPath), L"UnknowModule");
					}

					pStackFrame->iIndex = s_vecModInfo.size();
					s_vecModInfo.push_back(modinfo);
				}
			}
		}

		pStackFrame += 1;

	} while (true);

	if (dwFrameCount > 0)
	{	
		pcs->nFrameCount = dwFrameCount;
		pcs->dwTimeStamp = GetTimeStamp();
		pcs->dwReserve = 0xabababab;
		dwBytesWritten = dwBytesWritten + sizeof(CALL_STACK) - sizeof(STACK_FRAME);
	}
	
	return dwBytesWritten;
}


DWORD CCallStack::GetTimeStamp()
{
	DWORD dwTimeStamp = 0;
	UINT uPeriod = 1;
	timeBeginPeriod(uPeriod);
	dwTimeStamp = timeGetTime();
	timeEndPeriod(uPeriod);

	return dwTimeStamp;
}

std::vector<MOD_INFO>& CCallStack::GetModInfoVector()
{
	return s_vecModInfo;
}

void CCallStack::ClearModInfo()
{
	s_vecModInfo.clear();
}


//int GetCallStack(void *_ebp, void *buf, int nMaxLevel)
//{
//	unsigned char *pbBuf = (unsigned char *)buf;
//	DWORD dwStackHigh, dwStackLow;
//	__asm
//	{
//		mov edx,fs:[4];
//		mov dwStackHigh,edx;
//		mov edx,fs:[8];
//		mov dwStackLow,edx;
//	}		
//
//	PDWORD p_frame = (PDWORD)_ebp;
//	int i = 0;
//	while (i < nMaxLevel)
//	{
//		if (p_frame > (DWORD*)dwStackHigh || p_frame < (DWORD*)dwStackLow)
//			break;	
//		DWORD ret = p_frame[1];
//		if (ret == 0)
//			break;
//
//		BYTE index;
//		if (!GetModuleInfoByAddr((void*)ret, &index))
//			index = MAX_MODULE_COUNT - 1;//unknown module!
//
//		*(DWORD*)pbBuf = ret;
//		pbBuf += 4;
//		*(BYTE*)pbBuf = index;
//		pbBuf += 1;
//		i++;
//
//		PDWORD p_prevFrame = p_frame;
//		p_frame = (PDWORD)p_frame[0];
//
//		if (p_frame > (DWORD*)dwStackHigh || p_frame < (DWORD*)dwStackLow)
//		{
//			p_prevFrame += 7;
//			p_frame =  (PDWORD)p_prevFrame[0];
//			if (p_frame > (DWORD*)dwStackHigh || p_frame < (DWORD*)dwStackLow)
//			{
//				break;
//			}
//		}
//
//		if ((DWORD)p_frame & 3)    // Frame pointer must be aligned on a
//			break;                  // DWORD boundary.  Bail if not so.
//
//		if (p_frame <= p_prevFrame)
//			break;
//	}
//	return i;
//}