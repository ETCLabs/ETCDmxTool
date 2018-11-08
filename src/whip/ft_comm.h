#if !defined(AFX_FT_COMM_H__EA6A0816_5C05_4985_9DB5_F1AB1041E5CE__INCLUDED_)
#define AFX_FT_COMM_H__EA6A0816_5C05_4985_9DB5_F1AB1041E5CE__INCLUDED_

#pragma once

#include <Qt>
#include <QString>
#include "ftd2xx.h"

/////////////////////////////////////////////////////////////////////////////
// FT_Comm window

class FT_Comm
{
// Construction
public:
	FT_Comm();
    void LoadDLL();
    FT_STATUS OpenBy(const QString &csDeviceName);

    FT_HANDLE m_ftHandle;
    HMODULE m_hmodule;
    int		m_nSerDesc;

    typedef FT_STATUS (WINAPI *PtrToOpen)(PVOID, FT_HANDLE *); 
	PtrToOpen m_pOpen; 
	FT_STATUS Open(PVOID);

	typedef FT_STATUS (WINAPI *PtrToOpenEx)(PVOID, DWORD, FT_HANDLE *); 
	PtrToOpenEx m_pOpenEx; 
	FT_STATUS OpenEx(PVOID, DWORD);

	typedef FT_STATUS (WINAPI *PtrToListDevices)(PVOID, PVOID, DWORD);
	PtrToListDevices m_pListDevices; 
	FT_STATUS ListDevices(PVOID, PVOID, DWORD);

	typedef FT_STATUS (WINAPI *PtrToClose)(FT_HANDLE);
	PtrToClose m_pClose;
	FT_STATUS Close();

    typedef FT_STATUS (WINAPI *FT_SetEventNotification)(FT_HANDLE, DWORD, PVOID);
	FT_SetEventNotification m_pSetEvent;
	FT_STATUS SetEvent(FT_HANDLE, DWORD, PVOID);

	typedef FT_STATUS (WINAPI *PtrToRead)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
	PtrToRead m_pRead;
	FT_STATUS Read(LPVOID, DWORD, LPDWORD);

	typedef FT_STATUS (WINAPI *PtrToWrite)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
	PtrToWrite m_pWrite;
	FT_STATUS Write(LPVOID, DWORD, LPDWORD);

	typedef FT_STATUS (WINAPI *PtrToResetDevice)(FT_HANDLE);
	PtrToResetDevice m_pResetDevice;
	FT_STATUS ResetDevice();
	
	typedef FT_STATUS (WINAPI *PtrToPurge)(FT_HANDLE, ULONG);
	PtrToPurge m_pPurge;
	FT_STATUS Purge(ULONG);
	
	typedef FT_STATUS (WINAPI *PtrToSetTimeouts)(FT_HANDLE, ULONG, ULONG);
	PtrToSetTimeouts m_pSetTimeouts;
	FT_STATUS SetTimeouts(ULONG, ULONG);

	typedef FT_STATUS (WINAPI *PtrToGetQueueStatus)(FT_HANDLE, LPDWORD);
	PtrToGetQueueStatus m_pGetQueueStatus;
	FT_STATUS GetQueueStatus(LPDWORD);

	typedef FT_STATUS (WINAPI *PtrToSetLatencyTimer)(FT_HANDLE, UCHAR);
	PtrToSetLatencyTimer m_pSetLatencyTimer;
	FT_STATUS SetLatencyTimer(UCHAR);

	typedef FT_STATUS (WINAPI *PtrToGetLatencyTimer)(FT_HANDLE, PUCHAR);
	PtrToGetLatencyTimer m_pGetLatencyTimer;
	FT_STATUS GetLatencyTimer(PUCHAR);

	typedef FT_STATUS (WINAPI *PtrToEE_Read)(FT_HANDLE, PFT_PROGRAM_DATA);
	PtrToEE_Read m_pEE_Read;
	FT_STATUS EE_Read(PFT_PROGRAM_DATA);

	typedef FT_STATUS (WINAPI *PtrToEE_Program)(FT_HANDLE, PFT_PROGRAM_DATA);
	PtrToEE_Program m_pEE_Program;
	FT_STATUS EE_Program(PFT_PROGRAM_DATA);

	typedef FT_STATUS (WINAPI *PtrToEE_UARead)(FT_HANDLE, PUCHAR, DWORD, LPDWORD);
	PtrToEE_UARead m_pEE_UARead;
	FT_STATUS EE_UARead(PUCHAR, DWORD, LPDWORD);
	
	typedef FT_STATUS (WINAPI *PtrToEE_UAWrite)(FT_HANDLE, PUCHAR, DWORD);
	PtrToEE_UAWrite m_pEE_UAWrite;
	FT_STATUS EE_UAWrite(PUCHAR, DWORD);

	typedef FT_STATUS (WINAPI *PtrToEE_UASize)(FT_HANDLE, LPDWORD);
	PtrToEE_UASize m_pEE_UASize;
	FT_STATUS EE_UASize(LPDWORD);

	typedef FT_STATUS (WINAPI *PtrToGetStatus)(FT_HANDLE, LPDWORD, LPDWORD, LPDWORD);
	PtrToGetStatus m_pGetStatus;
	FT_STATUS GetStatus(LPDWORD, LPDWORD, LPDWORD);

	typedef FT_STATUS (WINAPI *PtrToSetUSBParameters)(FT_HANDLE, ULONG, ULONG);
	PtrToSetUSBParameters m_pSetUSBParameters;
	FT_STATUS SetUSBParameters(ULONG, ULONG);

	typedef FT_STATUS (WINAPI *PtrToSetBaudRate)(FT_HANDLE, ULONG);
	PtrToSetBaudRate m_pSetBaudRate;
	FT_STATUS SetBaudRate(ULONG);

	typedef FT_STATUS (WINAPI *PtrToSetDivisor)(FT_HANDLE, USHORT);
	PtrToSetDivisor m_pSetDivisor;
	FT_STATUS SetDivisor(USHORT);

	typedef FT_STATUS (WINAPI *PtrToSetDataCharacteristics)(FT_HANDLE, UCHAR, UCHAR, UCHAR);
	PtrToSetDataCharacteristics m_pSetDataCharacteristics;
	FT_STATUS SetDataCharacteristics(UCHAR, UCHAR, UCHAR);

	typedef FT_STATUS (WINAPI *PtrToSetFlowControl)(FT_HANDLE, USHORT, UCHAR, UCHAR);
	PtrToSetFlowControl m_pSetFlowControl;
	FT_STATUS SetFlowControl(USHORT, UCHAR, UCHAR);

	typedef FT_STATUS (WINAPI *PtrToSetDtr)(FT_HANDLE);
	PtrToSetDtr m_pSetDtr;
	FT_STATUS SetDtr();

	typedef FT_STATUS (WINAPI *PtrToClrDtr)(FT_HANDLE);
	PtrToClrDtr m_pClrDtr;
	FT_STATUS ClrDtr();

	typedef FT_STATUS (WINAPI *PtrToSetRts)(FT_HANDLE);
	PtrToSetRts m_pSetRts;
	FT_STATUS SetRts();

	typedef FT_STATUS (WINAPI *PtrToClrRts)(FT_HANDLE);
	PtrToClrRts m_pClrRts;
	FT_STATUS ClrRts();

	typedef FT_STATUS (WINAPI *PtrToGetModemStatus)(FT_HANDLE, PULONG);
	PtrToGetModemStatus m_pGetModemStatus;
	FT_STATUS GetModemStatus(PULONG);

	typedef FT_STATUS (WINAPI *PtrToSetChars)(FT_HANDLE, UCHAR, UCHAR, UCHAR, UCHAR);
	PtrToSetChars m_pSetChars;
	FT_STATUS SetChars(UCHAR, UCHAR, UCHAR, UCHAR);

	typedef FT_STATUS (WINAPI *PtrToSetBreakOn)(FT_HANDLE);
	PtrToSetBreakOn m_pSetBreakOn;
	FT_STATUS SetBreakOn();

	typedef FT_STATUS (WINAPI *PtrToSetBreakOff)(FT_HANDLE);
	PtrToSetBreakOff m_pSetBreakOff;
	FT_STATUS SetBreakOff();

	typedef FT_STATUS (WINAPI *PtrToSetWaitMask)(FT_HANDLE, DWORD);
	PtrToSetWaitMask m_pSetWaitMask;
	FT_STATUS SetWaitMask(DWORD);

	typedef FT_STATUS (WINAPI *PtrToWaitOnMask)(FT_HANDLE, DWORD);
	PtrToWaitOnMask m_pWaitOnMask;
	FT_STATUS WaitOnMask(DWORD);

	typedef FT_STATUS (WINAPI *PtrToSetBitMode)(FT_HANDLE, UCHAR, UCHAR);
	PtrToSetBitMode m_pSetBitMode;
	FT_STATUS SetBitMode(UCHAR, UCHAR);

	typedef FT_STATUS (WINAPI *PtrToGetBitMode)(FT_HANDLE, PUCHAR);
	PtrToGetBitMode m_pGetBitMode;
	FT_STATUS GetBitMode(PUCHAR);

	void PostError(int errorID);

public:
	virtual ~FT_Comm();
};

/////////////////////////////////////////////////////////////////////////////
//
// Error Definitions
//
//
// 1000 = Can't Load Ftd2xx.dll
// 1001 = Can't Find FT_Write
// 1002 = Can't Find FT_SetEventNotification
// 1003 = Can't Find FT_Read
// 1004 = Can't Find FT_Open
// 1005 = Can't Find FT_OpenEx
// 1006 = Can't Find FT_ListDevices
// 1007 = Can't Find FT_Close
// 1008 = Can't Find FT_ResetDevice
// 1009 = Can't Find FT_Purge
// 1010 = Can't Find FT_SetTimeouts
// 1011 = Can't Find FT_GetQueueStatus
// 1012 = Can't Find FT_SetLatencyTimer
// 1013 = Can't Find FT_GetLatencyTimer
// 1014 = Can't Find FT_EE_Read
// 1015 = Can't Find FT_EE_Program
// 1016 = Can't Find FT_EE_UARead
// 1017 = Can't Find FT_EE_UAWrite
// 1018 = Can't Find FT_EE_UASize
// 1019 = Can't Find FT_GetStatus
// 1020 = Can't Find FT_SetUSBParameters
// 1021 = Can't Find FT_SetBaudRate
// 1022 = Can't Find FT_SetDivisor
// 1023 = Can't Find FT_SetDataCharacteristics
// 1024 = Can't Find FT_SetFlowControl
// 1025 = Can't Find FT_SetDtr
// 1026 = Can't Find FT_ClrDtr
// 1027 = Can't Find FT_SetRts
// 1028 = Can't Find FT_ClrRts
// 1029 = Can't Find FT_GetModemStatus
// 1030 = Can't Find FT_SetChars
// 1031 = Can't Find FT_SetBreakOn
// 1032 = Can't Find FT_SetBreakOff
// 1033 = Can't Find FT_SetWaitMask
// 1034 = Can't Find FT_WaitOnMask
// 1035 = Can't Find FT_SetBitMode
// 1036 = Can't Find FT_GetBitMode
//
//
//////////////////////////////////////////////////////////////////////// 
//
//
// 1100 = FT_SetBreakOn is not valid!
// 1101 = FT_SetBreakOff is not valid!
// 1102 = FT_SetWaitMask is not valid!
// 1103 = FT_WaitOnMask is not valid!
// 1104 = FT_SetBitMode is not valid!
// 1105 = FT_GetBitMode is not valid!
// 1106 = FT_SetFlowControl is not valid!
// 1107 = FT_SetDtr is not valid!
// 1108 = FT_ClrDtr is not valid!
// 1109 = FT_SetRts is not valid!
// 1110 = FT_ClrRts is not valid!
// 1111 = FT_GetModemStatus is not valid!
// 1112 = FT_SetChars is not valid!
// 1113 = FT_SetDataCharacteristics is not valid!
// 1114 = FT_SetDivisor is not valid!
// 1115 = FT_SetBaudRate is not valid!
// 1116 = FT_SetUSBParameters is not valid!
// 1117 = FT_GetStatus is not valid!
// 1118 = FT_Read is not valid!
// 1119 = FT_Write is not valid!
// 1120 = FT_Open is not valid!
// 1121 = FT_OpenEx is not valid!
// 1122 = FT_ListDevices is not valid!
// 1123 = FT_SetEventNotification is not valid!
// 1124 = FT_ResetDevice is not valid!
// 1125 = FT_Purge is not valid!
// 1126 = FT_SetTimeouts is not valid!
// 1127 = FT_GetQueueStatus is not valid!
// 1128 = FT_SetLatencyTimer is not valid!
// 1129 = FT_GetLatencyTimer is not valid!
// 1130 = FT_EE_Read is not valid!
// 1131 = FT_EE_Program is not valid!
// 1132 = FT_EE_UARead is not valid!
// 1133 = FT_EE_UAWrite is not valid!
// 1134 = FT_EE_UASize is not valid!
//
//
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FT_COMM_H__EA6A0816_5C05_4985_9DB5_F1AB1041E5CE__INCLUDED_)
