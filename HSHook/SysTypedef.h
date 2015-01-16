typedef LONG NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;


typedef struct _LDR_MODULE {
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

typedef struct _PEB_LDR_DATA {
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB {
	BYTE                          Reserved1[0x02];
	BYTE                          BeingDebugged;
	BYTE                          Reserved2[0x01];
	PVOID                         Reserved3[0x02];
	PPEB_LDR_DATA                 Ldr;
	PVOID						  ProcessParameters;/*PRTL_USER_PROCESS_PARAMETERS*/
	BYTE						  Reserved4[0x24];
	PVOID						  ApiSetMap;
	BYTE                          Reserved5[0x40];
	PVOID                         Reserved6[52];
	PVOID						  PostProcessInitRoutine;/*PPS_POST_PROCESS_INIT_ROUTINE*/
	BYTE                          Reserved7[0x80];
	PVOID                         Reserved8[0x01];
	ULONG                         SessionId;
} PEB, *PPEB;

typedef struct _TEB {
	NT_TIB NtTib; 
	BYTE  Reserved1[0x078c];
	PVOID Reserved2[0x019c];
	PVOID TlsSlots[0x40];
	BYTE  Reserved3[0x08];
	PVOID Reserved4[0x1a];
	PVOID ReservedForOle;
	PVOID Reserved5[0x04];
	PVOID TlsExpansionSlots;
} TEB, *PTEB;