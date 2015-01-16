#ifndef NT_LAYER_FUNCTION_HOOK

#define Index_ApiTable_Begin					0x0

#define Index_Close_Begin						0x00000000
#define Index_NtClose							0x00000001
#define Index_CloseHandle						0x00000002
#define Index_RegCloseKey						0x00000003
#define Index_FindClose							0x00000004
#define Index_CloseDesktop						0x00000005
#define Index_CloseWindowStation				0x00000006
//#define Index_NULL							0x00000007
//#define Index_NULL							0x00000008
//#define Index_NULL							0x00000009
//#define Index_NULL							0x0000000a
//#define Index_NULL							0x0000000b
//#define Index_NULL							0x0000000c
//#define Index_NULL							0x0000000d
//#define Index_NULL							0x0000000e
#define Index_Close_End							0x0000000f

//只有一个句柄返回值为LONG
#define Index_RegCreateKeyA						0x00000010
#define Index_RegCreateKeyW						0x00000011
#define Index_RegCreateKeyExA					0x00000012
#define Index_RegCreateKeyExW					0x00000013
#define Index_RegCreateKeyTransactedA			0x00000014
#define Index_RegCreateKeyTransactedW			0x00000015
#define Index_RegOpenKeyA						0x00000016
#define Index_RegOpenKeyW						0x00000017
#define Index_RegOpenKeyExA						0x00000018
#define Index_RegOpenKeyExW						0x00000019
#define Index_RegOpenKeyTransactedA				0x0000001a
#define Index_RegOpenKeyTransactedW				0x0000001b
//#define Index_NULL							0x0000001c
//#define Index_NULL							0x0000001d
//#define Index_NULL							0x0000001e
//#define Index_NULL							0x0000001f


//只有一个句柄，返回值为HANDLE
#define Index_CreateFileA						0x00000020
#define Index_CreateFileW						0x00000021

#define Index_OpenFile							0x00000022

#define Index_CreateFileMappingA				0x00000023
#define Index_CreateFileMappingW				0x00000024
#define Index_OpenFileMappingA					0x00000025
#define Index_OpenFileMappingW					0x00000026

#define Index_CreateProcessA					0x00000027
#define Index_CreateProcessW					0x00000028
#define Index_OpenProcess						0x00000029

#define Index_CreateThread						0x0000002a
#define Index_CreateRemoteThread				0x0000002b
#define Index_OpenThread						0x0000002c

#define Index_CreateEventA						0x0000002d
#define Index_CreateEventW						0x0000002e
#define Index_OpenEventA						0x0000002f
#define Index_OpenEventW						0x00000030

#define Index_CreateMutexA						0x00000031
#define Index_CreateMutexW						0x00000032
#define Index_OpenMutexA						0x00000033
#define Index_OpenMutexW						0x00000034

#define Index_CreateSemaphoreA					0x00000035
#define Index_CreateSemaphoreW					0x00000036
#define Index_OpenSemaphoreA					0x00000037
#define Index_OpenSemaphoreW					0x00000038

#define Index_CreateJobObjectA					0x00000039
#define Index_CreateJobObjectW					0x0000003a
#define Index_OpenJobObjectA					0x0000003b
#define Index_OpenJobObjectW					0x0000003c

#define Index_CreateWaitableTimerA				0x0000003d
#define Index_CreateWaitableTimerW				0x0000003e
#define Index_OpenWaitableTimerA				0x0000003f
#define Index_OpenWaitableTimerW				0x00000040

#define Index_CreateDesktopA					0x00000041
#define Index_CreateDesktopW					0x00000042
#define Index_OpenDesktopA						0x00000043
#define Index_OpenDesktopW						0x00000044
#define Index_OpenInputDesktop					0x00000045

#define Index_CreateWindowStationA				0x00000046
#define Index_CreateWindowStationW				0x00000047
#define Index_OpenWindowStationA				0x00000048
#define Index_OpenWindowStationW				0x00000049

#define Index_CreatePipe						0x0000004a
#define Index_CreateNamedPipeA					0x0000004b
#define Index_CreateNamedPipeW					0x0000004c

#define Index_CreateMailslotA					0x0000004d
#define Index_CreateMailslotW					0x0000004e

#define Index_DuplicateHandle					0x0000004f

#define Index_CreateToolhelp32Snapshot			0x00000050

#define Index_FindFirstFileA					0x00000051
#define Index_FindFirstFileW					0x00000052
#define Index_FindFirstFileExA					0x00000053
#define Index_FindFirstFileExW 					0x00000054


#define Index_ApiTable_End						0x00000055

#endif