/************************************************************************\
* File		:	NtApis.cpp
* Module	:	API Hook
* Created	:	shiontian @ 2013-01-30 
* Description:
* 需要Hook的目标函数的索引
* 
* 如果需要添加函数，则找到该函数所属类别
* 然后取消该类别下的一条注释占位
* 将函数添加到取消注释的地方
* 不要忘了同时在CApi类中更新API名字符串
* 
\************************************************************************/

#ifdef NT_LAYER_FUNCTION_HOOK


#define OB_TYPE_INDEX_TYPE              0x01 // [ObjT] "Type"  
#define OB_TYPE_INDEX_DIRECTORY         0x02 // [Dire] "Directory"  
#define OB_TYPE_INDEX_SYMBOLIC_LINK     0x03 // [Symb] "SymbolicLink"  
#define OB_TYPE_INDEX_TOKEN             0x04 // [Toke] "Token"  
#define OB_TYPE_INDEX_PROCESS           0x05 // [Proc] "Process"  
#define OB_TYPE_INDEX_THREAD            0x06 // [Thre] "Thread"  
#define OB_TYPE_INDEX_JOB               0x07 // [Job ] "Job"  
#define OB_TYPE_INDEX_EVENT             0x08 // [Even] "Event"  
#define OB_TYPE_INDEX_EVENT_PAIR        0x09 // [Even] "EventPair"  
#define OB_TYPE_INDEX_MUTANT            0x0a // [Muta] "Mutant"  
#define OB_TYPE_INDEX_CALLBACK          0x0b // [Call] "Callback"  
#define OB_TYPE_INDEX_SEMAPHORE         0x0c // [Sema] "Semaphore"  
#define OB_TYPE_INDEX_TIMER             0x0d // [Time] "Timer"  
#define OB_TYPE_INDEX_PROFILE           0x0e // [Prof] "Profile"  
#define OB_TYPE_INDEX_WINDOW_STATION    0x0f // [Wind] "WindowStation"  
#define OB_TYPE_INDEX_DESKTOP           0x10// [Desk] "Desktop"  
#define OB_TYPE_INDEX_SECTION           0x11 // [Sect] "Section"  
#define OB_TYPE_INDEX_KEY               0x12 // [Key ] "Key"  
#define OB_TYPE_INDEX_PORT              0x13 // [Port] "Port"  
#define OB_TYPE_INDEX_WAITABLE_PORT     0x14 // [Wait] "WaitablePort"  
#define OB_TYPE_INDEX_ADAPTER           0x15 // [Adap] "Adapter"  
#define OB_TYPE_INDEX_CONTROLLER        0x16 // [Cont] "Controller"  
#define OB_TYPE_INDEX_DEVICE            0x17 // [Devi] "Device"  
#define OB_TYPE_INDEX_DRIVER            0x18 // [Driv] "Driver"  
#define OB_TYPE_INDEX_IO_COMPLETION     0x19 // [IoCo] "IoCompletion"  
#define OB_TYPE_INDEX_FILE              0x1a // [File] "File"  
#define OB_TYPE_INDEX_WMI_GUID          0x1b // [WmiG] "WmiGuid"   



#define Index_ApiTable_Begin					0x0


#define Index_Close_Begin						0x00000000
#define Index_NtClose							0x00000001
//#define Index_NULL							0x00000002
//#define Index_NULL							0x00000003
//#define Index_NULL							0x00000004
//#define Index_NULL							0x00000005
//#define Index_NULL							0x00000006
//#define Index_NULL							0x00000007
//#define Index_NULL							0x00000008
//#define Index_NULL							0x00000009
//#define Index_NULL							0x0000000a
//#define Index_NULL							0x0000000b
//#define Index_NULL							0x0000000c
#define Index_CloseDesktop						0x0000000d
#define Index_CloseWindowStation				0x0000000e
#define Index_Close_End							0x0000000f


//Type operation RING3未暴露
//#define Index_NULL							0x00010010
//#define Index_NULL							0x00010011
//#define Index_NULL							0x00010012
//#define Index_NULL							0x00010013
//#define Index_NULL							0x00010014
//#define Index_NULL							0x00010015
//#define Index_NULL							0x00010016
//#define Index_NULL							0x00010017
//#define Index_NULL							0x00010018
//#define Index_NULL							0x00010019
//#define Index_NULL							0x0001001a
//#define Index_NULL							0x0001001b
//#define Index_NULL							0x0001001c
//#define Index_NULL							0x0001001d
//#define Index_NULL							0x0001001e
//#define Index_NULL							0x0001001f

//Directory operation
#define Index_NtCreateDirectoryObject		0x00020020
#define Index_NtOpenDirectoryObject			0x00020021
//#define Index_NULL							0x00020022
//#define Index_NULL							0x00020023
//#define Index_NULL							0x00020024
//#define Index_NULL							0x00020025
//#define Index_NULL							0x00020026
//#define Index_NULL							0x00020027
//#define Index_NULL							0x00020028
//#define Index_NULL							0x00020029
//#define Index_NULL							0x0002002a
//#define Index_NULL							0x0002002b
//#define Index_NULL							0x0002002c
//#define Index_NULL							0x0002002d
//#define Index_NULL							0x0002002e
//#define Index_NULL							0x0002002f

//SymbolicLink operation
#define Index_NtCreateSymbolicLinkObject	0x00030030
#define Index_NtOpenSymbolicLinkObject		0x00030031
//#define Index_NULL							0x00030032
//#define Index_NULL							0x00030033
//#define Index_NULL							0x00030034
//#define Index_NULL							0x00030035
//#define Index_NULL							0x00030036
//#define Index_NULL							0x00030037
//#define Index_NULL							0x00030038
//#define Index_NULL							0x00030039
//#define Index_NULL							0x0003003a
//#define Index_NULL							0x0003003b
//#define Index_NULL							0x0003003c
//#define Index_NULL							0x0003003d
//#define Index_NULL							0x0003003e
//#define Index_NULL							0x0003003f

//Token operation 
#define Index_NtCreateToken					0x00040040
#define Index_NtDuplicateToken				0x00040041
#define Index_NtOpenProcessToken			0x00040042
#define Index_NtOpenProcessTokenEx			0x00040043
#define Index_NtOpenThreadToken				0x00040044
#define Index_NtOpenThreadTokenEx			0x00040045
//#define Index_NULL							0x00040046
//#define Index_NULL							0x00040047
//#define Index_NULL							0x00040048
//#define Index_NULL							0x00040049
//#define Index_NULL							0x0004004a
//#define Index_NULL							0x0004004b
//#define Index_NULL							0x0004004c
//#define Index_NULL							0x0004004d
//#define Index_NULL							0x0004004e
//#define Index_NULL							0x0004004f


//Process operation
#define Index_NtCreateProcess				0x00050050
#define Index_NtCreateProcessEx				0x00050051
#define Index_NtCreateUserProcess			0x00050052
#define Index_NtGetNextProcess				0x00050053
#define Index_NtOpenProcess					0x00050054
//#define Index_NULL							0x00050055
//#define Index_NULL							0x00050056
//#define Index_NULL							0x00050057
//#define Index_NULL							0x00050058
//#define Index_NULL							0x00050059
//#define Index_NULL							0x0005005a
//#define Index_NULL							0x0005005b
//#define Index_NULL							0x0005005c
//#define Index_NULL							0x0005005d
//#define Index_NULL							0x0005005e
//#define Index_NULL							0x0005005f


//Thread operation
#define Index_NtCreateThread				0x00060060
#define Index_NtCreateThreadEx				0x00060061
#define Index_NtGetNextThread				0x00060062
#define Index_NtOpenThread					0x00060063
//#define Index_NULL							0x00060064
//#define Index_NULL							0x00060065
//#define Index_NULL							0x00060066
//#define Index_NULL							0x00060067
//#define Index_NULL							0x00060068
//#define Index_NULL							0x00060069
//#define Index_NULL							0x0006006a
//#define Index_NULL							0x0006006b
//#define Index_NULL							0x0006006c
//#define Index_NULL							0x0006006d
//#define Index_NULL							0x0006006e
//#define Index_NULL							0x0006006f


//Job operation
#define Index_NtCreateJobObject				0x00070070
#define Index_NtOpenJobObject				0x00070071
//#define Index_NULL							0x00070072
//#define Index_NULL							0x00070073
//#define Index_NULL							0x00070074
//#define Index_NULL							0x00070075
//#define Index_NULL							0x00070076
//#define Index_NULL							0x00070077
//#define Index_NULL							0x00070078
//#define Index_NULL							0x00070079
//#define Index_NULL							0x0007007a
//#define Index_NULL							0x0007007b
//#define Index_NULL							0x0007007c
//#define Index_NULL							0x0007007d
//#define Index_NULL							0x0007007e
//#define Index_NULL							0x0007007f



//Event operation
#define Index_NtCreateEvent					0x00080080
#define Index_NtOpenEvent					0x00080081
//#define Index_NULL							0x00080082
//#define Index_NULL							0x00080083
//#define Index_NULL							0x00080084
//#define Index_NULL							0x00080085
//#define Index_NULL							0x00080086
//#define Index_NULL							0x00080087
//#define Index_NULL							0x00080088
//#define Index_NULL							0x00080089
//#define Index_NULL							0x0008008a
//#define Index_NULL							0x0008008b
//#define Index_NULL							0x0008008c
//#define Index_NULL							0x0008008d
//#define Index_NULL							0x0008008e
//#define Index_NULL							0x0008008f


//EventPair operation
#define Index_NtCreateEventPair				0x00090090
#define Index_NtOpenEventPair				0x00090091
//#define Index_NULL							0x00090092
//#define Index_NULL							0x00090093
//#define Index_NULL							0x00090094
//#define Index_NULL							0x00090095
//#define Index_NULL							0x00090096
//#define Index_NULL							0x00090097
//#define Index_NULL							0x00090098
//#define Index_NULL							0x00090099
//#define Index_NULL							0x0009009a
//#define Index_NULL							0x0009009b
//#define Index_NULL							0x0009009c
//#define Index_NULL							0x0009009d
//#define Index_NULL							0x0009009e
//#define Index_NULL							0x0009009f


//Mutant operation
#define Index_NtCreateMutant				0x000a00a0
#define Index_NtOpenMutant					0x000a00a1
//#define Index_NULL							0x000a00a2
//#define Index_NULL							0x000a00a3
//#define Index_NULL							0x000a00a4
//#define Index_NULL							0x000a00a5
//#define Index_NULL							0x000a00a6
//#define Index_NULL							0x000a00a7
//#define Index_NULL							0x000a00a8
//#define Index_NULL							0x000a00a9
//#define Index_NULL							0x000a00aa
//#define Index_NULL							0x000a00ab
//#define Index_NULL							0x000a00ac
//#define Index_NULL							0x000a00ad
//#define Index_NULL							0x000a00ae
//#define Index_NULL							0x000a00af


//CallBack operation RING3 忽略
//#define Index_NULL							0x000b00b0
//#define Index_NULL							0x000b00b1
//#define Index_NULL							0x000b00b2
//#define Index_NULL							0x000b00b3
//#define Index_NULL							0x000b00b4
//#define Index_NULL							0x000b00b5
//#define Index_NULL							0x000b00b6
//#define Index_NULL							0x000b00b7
//#define Index_NULL							0x000b00b8
//#define Index_NULL							0x000b00b9
//#define Index_NULL							0x000b00ba
//#define Index_NULL							0x000b00bb
//#define Index_NULL							0x000b00bc
//#define Index_NULL							0x000b00bd
//#define Index_NULL							0x000b00be
//#define Index_NULL							0x000b00bf

//Semaphore operation
#define Index_NtCreateSemaphore				0x000c00c0
#define Index_NtOpenSemaphore				0x000c00c1
//#define Index_NULL							0x000c00c2
//#define Index_NULL							0x000c00c3
//#define Index_NULL							0x000c00c4
//#define Index_NULL							0x000c00c5
//#define Index_NULL							0x000c00c6
//#define Index_NULL							0x000c00c7
//#define Index_NULL							0x000c00c8
//#define Index_NULL							0x000c00c9
//#define Index_NULL							0x000c00ca
//#define Index_NULL							0x000c00cb
//#define Index_NULL							0x000c00cc
//#define Index_NULL							0x000c00cd
//#define Index_NULL							0x000c00ce
//#define Index_NULL							0x000c00cf

//Timer operation
#define Index_NtCreateTimer					0x000d00d0
#define Index_NtOpenTimer					0x000d00d1
//#define Index_NULL							0x000d00d2
//#define Index_NULL							0x000d00d3
//#define Index_NULL							0x000d00d4
//#define Index_NULL							0x000d00d5
//#define Index_NULL							0x000d00d6
//#define Index_NULL							0x000d00d7
//#define Index_NULL							0x000d00d8
//#define Index_NULL							0x000d00d9
//#define Index_NULL							0x000d00da
//#define Index_NULL							0x000d00db
//#define Index_NULL							0x000d00dc
//#define Index_NULL							0x000d00dd
//#define Index_NULL							0x000d00de
//#define Index_NULL							0x000d00df


//Profile operation
#define Index_NtCreateProfile				0x000e00e0
#define Index_NtCreateProfileEx				0x000e00e1
//#define Index_NULL							0x000e00e2
//#define Index_NULL							0x000e00e3
//#define Index_NULL							0x000e00e4
//#define Index_NULL							0x000e00e5
//#define Index_NULL							0x000e00e6
//#define Index_NULL							0x000e00e7
//#define Index_NULL							0x000e00e8
//#define Index_NULL							0x000e00e9
//#define Index_NULL							0x000e00ea
//#define Index_NULL							0x000e00eb
//#define Index_NULL							0x000e00ec
//#define Index_NULL							0x000e00ed
//#define Index_NULL							0x000e00ee
//#define Index_NULL							0x000e00ef

//WindowStation operation
#define Index_CreateWindowStationA			0x000f00f0
#define Index_CreateWindowStationW			0x000f00f1
#define Index_OpenWindowStationA			0x000f00f2
#define Index_OpenWindowStationW			0x000f00f3
//#define Index_NULL							0x000f00f4
//#define Index_NULL							0x000f00f5
//#define Index_NULL							0x000f00f6
//#define Index_NULL							0x000f00f7
//#define Index_NULL							0x000f00f8
//#define Index_NULL							0x000f00f9
//#define Index_NULL							0x000f00fa
//#define Index_NULL							0x000f00fb
//#define Index_NULL							0x000f00fc
//#define Index_NULL							0x000f00fd
//#define Index_NULL							0x000f00fe
//#define Index_NULL							0x000f00ff

//Desktop operation
#define Index_CreateDesktopA				0x00100100
#define Index_CreateDesktopW				0x00100101
#define Index_OpenDesktopA					0x00100102
#define Index_OpenDesktopW					0x00100103
#define Index_OpenInputDesktop				0x00100104
//#define Index_NULL							0x00100105
//#define Index_NULL							0x00100106
//#define Index_NULL							0x00100107
//#define Index_NULL							0x00100108
//#define Index_NULL							0x00100109
//#define Index_NULL							0x0010010a
//#define Index_NULL							0x0010010b
//#define Index_NULL							0x0010010c
//#define Index_NULL							0x0010010d
//#define Index_NULL							0x0010010e
//#define Index_NULL							0x0010010f

//Section operation
#define Index_NtCreateSection				0x00110110
#define Index_NtOpenSection					0x00110111
//#define Index_NULL							0x00110112
//#define Index_NULL							0x00110113
//#define Index_NULL							0x00110114
//#define Index_NULL							0x00110115
//#define Index_NULL							0x00110116
//#define Index_NULL							0x00110117
//#define Index_NULL							0x00110118
//#define Index_NULL							0x00110119
//#define Index_NULL							0x0011011a
//#define Index_NULL							0x0011011b
//#define Index_NULL							0x0011011c
//#define Index_NULL							0x0011011d
//#define Index_NULL							0x0011011e
//#define Index_NULL							0x0011011f

//Key operation
#define Index_NtCreateKey					0x00120120
#define Index_NtCreateKeyTransacted			0x00120121
#define Index_NtOpenKey						0x00120122
#define Index_NtOpenKeyEx					0x00120123
#define Index_NtOpenKeyTransacted			0x00120124
#define Index_NtOpenKeyTransactedEx			0x00120125
//#define Index_NULL							0x00120126
//#define Index_NULL							0x00120127
//#define Index_NULL							0x00120128
//#define Index_NULL							0x00120129
//#define Index_NULL							0x0012012a
//#define Index_NULL							0x0012012b
//#define Index_NULL							0x0012012c
//#define Index_NULL							0x0012012d
//#define Index_NULL							0x0012012e
//#define Index_NULL							0x0012012f


//Port operation
#define Index_NtCreatePort							0x00130130
#define Index_NtConnectPort							0x00130131
#define Index_NtSecureConnectPort					0x00130132
#define Index_NtAcceptConnectPort					0x00130133
//#define Index_NULL							0x00130134
//#define Index_NULL							0x00130135
//#define Index_NULL							0x00130136
//#define Index_NULL							0x00130137
//#define Index_NULL							0x00130138
//#define Index_NULL							0x00130139
//#define Index_NULL							0x0013013a
//#define Index_NULL							0x0013013b
//#define Index_NULL							0x0013013c
//#define Index_NULL							0x0013013d
//#define Index_NULL							0x0013013e
//#define Index_NULL							0x0013013f


//WaitablePort operation
#define Index_NtCreateWaitablePort				0x00140140
//#define Index_NULL							0x00140141
//#define Index_NULL							0x00140142
//#define Index_NULL							0x00140143
//#define Index_NULL							0x00140144
//#define Index_NULL							0x00140145
//#define Index_NULL							0x00140146
//#define Index_NULL							0x00140147
//#define Index_NULL							0x00140148
//#define Index_NULL							0x00140149
//#define Index_NULL							0x0014014a
//#define Index_NULL							0x0014014b
//#define Index_NULL							0x0014014c
//#define Index_NULL							0x0014014d
//#define Index_NULL							0x0014014e
//#define Index_NULL							0x0014014f


//Adapter operation RING3未暴露
//#define Index_NULL							0x00150150
//#define Index_NULL							0x00150151
//#define Index_NULL							0x00150152
//#define Index_NULL							0x00150153
//#define Index_NULL							0x00150154
//#define Index_NULL							0x00150155
//#define Index_NULL							0x00150156
//#define Index_NULL							0x00150157
//#define Index_NULL							0x00150158
//#define Index_NULL							0x00150159
//#define Index_NULL							0x0015015a
//#define Index_NULL							0x0015015b
//#define Index_NULL							0x0015015c
//#define Index_NULL							0x0015015d
//#define Index_NULL							0x0015015e
//#define Index_NULL							0x0015015f


//Controller operation RING3未暴露
//#define Index_NULL							0x00160160
//#define Index_NULL							0x00160161
//#define Index_NULL							0x00160162
//#define Index_NULL							0x00160163
//#define Index_NULL							0x00160164
//#define Index_NULL							0x00160165
//#define Index_NULL							0x00160166
//#define Index_NULL							0x00160167
//#define Index_NULL							0x00160168
//#define Index_NULL							0x00160169
//#define Index_NULL							0x0016016a
//#define Index_NULL							0x0016016b
//#define Index_NULL							0x0016016c
//#define Index_NULL							0x0016016d
//#define Index_NULL							0x0016016e
//#define Index_NULL							0x0016016f


//Device operation RING3未暴露
//#define Index_NULL							0x00170170
//#define Index_NULL							0x00170171
//#define Index_NULL							0x00170172
//#define Index_NULL							0x00170173
//#define Index_NULL							0x00170174
//#define Index_NULL							0x00170175
//#define Index_NULL							0x00170176
//#define Index_NULL							0x00170177
//#define Index_NULL							0x00170178
//#define Index_NULL							0x00170179
//#define Index_NULL							0x0017017a
//#define Index_NULL							0x0017017b
//#define Index_NULL							0x0017017c
//#define Index_NULL							0x0017017d
//#define Index_NULL							0x0017017e
//#define Index_NULL							0x0017017f


//Driver operation RING3未暴露
//#define Index_NULL							0x00180180
//#define Index_NULL							0x00180181
//#define Index_NULL							0x00180182
//#define Index_NULL							0x00180183
//#define Index_NULL							0x00180184
//#define Index_NULL							0x00180185
//#define Index_NULL							0x00180186
//#define Index_NULL							0x00180187
//#define Index_NULL							0x00180188
//#define Index_NULL							0x00180189
//#define Index_NULL							0x0018018a
//#define Index_NULL							0x0018018b
//#define Index_NULL							0x0018018c
//#define Index_NULL							0x0018018d
//#define Index_NULL							0x0018018e
//#define Index_NULL							0x0018018f



//IoCompletion operation
#define Index_NtCreateIoCompletion			0x00190190
#define Index_NtOpenIoCompletion			0x00190191
//#define Index_NULL							0x00190192
//#define Index_NULL							0x00190193
//#define Index_NULL							0x00190194
//#define Index_NULL							0x00190195
//#define Index_NULL							0x00190196
//#define Index_NULL							0x00190197
//#define Index_NULL							0x00190198
//#define Index_NULL							0x00190199
//#define Index_NULL							0x0019019a
//#define Index_NULL							0x0019019b
//#define Index_NULL							0x0019019c
//#define Index_NULL							0x0019019d
//#define Index_NULL							0x0019019e
//#define Index_NULL							0x0019019f


//File operation
#define Index_NtCreateFile					0x001a01a0
#define Index_NtCreateMailslotFile			0x001a01a1
#define Index_NtCreateNamedPipeFile			0x001a01a2
#define Index_NtOpenFile					0x001a01a3
//#define Index_NULL							0x001a01a4
//#define Index_NULL							0x001a01a5
//#define Index_NULL							0x001a01a6
//#define Index_NULL							0x001a01a7
//#define Index_NULL							0x001a01a8
//#define Index_NULL							0x001a01a9
//#define Index_NULL							0x001a01aa
//#define Index_NULL							0x001a01ab
//#define Index_NULL							0x001a01ac
//#define Index_NULL							0x001a01ad
//#define Index_NULL							0x001a01ae
//#define Index_NULL							0x001a01af

//WmiGuid operation RING3未暴露
//#define Index_NULL							0x001b01b0
//#define Index_NULL							0x001b01b1
//#define Index_NULL							0x001b01b2
//#define Index_NULL							0x001b01b3
//#define Index_NULL							0x001b01b4
//#define Index_NULL							0x001b01b5
//#define Index_NULL							0x001b01b6
//#define Index_NULL							0x001b01b7
//#define Index_NULL							0x001b01b8
//#define Index_NULL							0x001b01b9
//#define Index_NULL							0x001b01ba
//#define Index_NULL							0x001b01bb
//#define Index_NULL							0x001b01bc
//#define Index_NULL							0x001b01bd
//#define Index_NULL							0x001b01be
//#define Index_NULL							0x001b01bf


#define Index_NtDuplicateObject					0x001c01c0

#define Index_ApiTable_End						0x000001c1


#endif
