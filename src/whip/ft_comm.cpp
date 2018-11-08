// FT_Comm.cpp : implementation file
//

#include <QDebug>
#include "FT_Comm.h"


/////////////////////////////////////////////////////////////////////////////
// FT_Comm

FT_Comm::FT_Comm()
	: m_nSerDesc(1)
	, m_pOpen(NULL)
	, m_pOpenEx(NULL)
	, m_pListDevices(NULL)
	, m_pClose(NULL)
	, m_pRead(NULL)
	, m_pWrite(NULL)
	, m_pResetDevice(NULL)
	, m_pPurge(NULL)
	, m_pSetTimeouts(NULL)
	, m_pGetQueueStatus(NULL)
	, m_pSetLatencyTimer(NULL)
	, m_pGetLatencyTimer(NULL)
	, m_pEE_Read(NULL)
	, m_pEE_Program(NULL)
	, m_pEE_UARead(NULL)
	, m_pEE_UAWrite(NULL)
	, m_pEE_UASize(NULL)
	, m_pGetStatus(NULL)
	, m_pSetUSBParameters(NULL)
	, m_pSetBaudRate(NULL)
	, m_pSetDivisor(NULL)
	, m_pSetDataCharacteristics(NULL)
	, m_pSetFlowControl(NULL)
	, m_pSetDtr(NULL)
	, m_pClrDtr(NULL)
	, m_pSetRts(NULL)
	, m_pClrRts(NULL)
	, m_pGetModemStatus(NULL)
	, m_pSetChars(NULL)
	, m_pSetBreakOn(NULL)
	, m_pSetBreakOff(NULL)
	, m_pSetWaitMask(NULL)
	, m_pWaitOnMask(NULL)
	, m_pSetBitMode(NULL)
	, m_pGetBitMode(NULL)
{
    //m_nSerDesc = 1;
	//m_pClose = NULL;
}

FT_Comm::~FT_Comm()
{
    FreeLibrary(m_hmodule);
}

//****************************************************************************************
FT_STATUS FT_Comm::OpenBy(const QString &csDeviceName)
{
	QByteArray pData = csDeviceName.toLatin1();
	char *pString = pData.data();

	FT_STATUS status = FT_DEVICE_NOT_OPENED;
	ULONG x=0;
	if(m_nSerDesc == 0)
		status = OpenEx((PVOID)(LPCTSTR)pString, FT_OPEN_BY_DESCRIPTION);

	if(m_nSerDesc == 1)
		status = OpenEx((PVOID)(LPCTSTR)pString, FT_OPEN_BY_SERIAL_NUMBER);
	
	if((m_nSerDesc == 2) || (m_nSerDesc == -1))//if open by device OR no method was selected
	{
		if(csDeviceName.length() < 1)//nothing entered - open default device 0
		{
			status = Open((PVOID)x);//load default device 0
		}
	   else
	   {
			if(csDeviceName.length() > 2)//no Open() method selected...
			{
				qDebug() << "No Open Method Selected";
				return FT_DEVICE_NOT_OPENED;
			}

			//convert string to decimal number and open(x)
			char str[3];
			strcpy_s(str, 3, pString);
			x = atoi(str);
			if((x<0) || (x>64))
			{
				qDebug() << "Invalid number";
				return FT_DEVICE_NOT_OPENED; 
			}
			status = Open((PVOID)x);
	   }
	}

	return status;

} // FT_Comm::OpenBy()


void FT_Comm::LoadDLL()
{
	m_hmodule = LoadLibrary(L"Ftd2xx.dll");	
	if(m_hmodule == NULL)
	{
		PostError(1000);
		return;
	}

	m_pWrite = (PtrToWrite)GetProcAddress(m_hmodule, "FT_Write");
	if (m_pWrite == NULL)
	{
		PostError(1001);
		return;
	}

    m_pSetEvent = (FT_SetEventNotification)GetProcAddress(m_hmodule, "FT_SetEventNotification");
	if (m_pSetEvent == NULL)
	{
		PostError(1002);
		return;
	}

	m_pRead = (PtrToRead)GetProcAddress(m_hmodule, "FT_Read");
	if (m_pRead == NULL)
	{
		PostError(1003);
		return;
	}

	m_pOpen = (PtrToOpen)GetProcAddress(m_hmodule, "FT_Open");
	if (m_pOpen == NULL)
	{
		PostError(1004);
		return;
	}

	m_pOpenEx = (PtrToOpenEx)GetProcAddress(m_hmodule, "FT_OpenEx");
	if (m_pOpenEx == NULL)
	{
		PostError(1005);
		return;
	}

	m_pListDevices = (PtrToListDevices)GetProcAddress(m_hmodule, "FT_ListDevices");
	if(m_pListDevices == NULL)
		{
			PostError(1006);
			return;
		}

	m_pClose = (PtrToClose)GetProcAddress(m_hmodule, "FT_Close");
	if (m_pClose == NULL)
	{
		PostError(1007);
		return;
	}

	m_pResetDevice = (PtrToResetDevice)GetProcAddress(m_hmodule, "FT_ResetDevice");
	if (m_pResetDevice == NULL)
	{
		PostError(1008);
		return;
	}

	m_pPurge = (PtrToPurge)GetProcAddress(m_hmodule, "FT_Purge");
	if (m_pPurge == NULL)
	{
		PostError(1009);
		return;
	}

	m_pSetTimeouts = (PtrToSetTimeouts)GetProcAddress(m_hmodule, "FT_SetTimeouts");
	if (m_pSetTimeouts == NULL)
	{
		PostError(1010);
		return;
	}

	m_pGetQueueStatus = (PtrToGetQueueStatus)GetProcAddress(m_hmodule, "FT_GetQueueStatus");
	if (m_pGetQueueStatus == NULL)
	{
		PostError(1011);
		return;
	}

	m_pSetLatencyTimer = (PtrToSetLatencyTimer)GetProcAddress(m_hmodule, "FT_SetLatencyTimer");
	if (m_pSetLatencyTimer == NULL)
	{
		PostError(1012);
		return;
	}

	m_pGetLatencyTimer = (PtrToGetLatencyTimer)GetProcAddress(m_hmodule, "FT_GetLatencyTimer");
	if (m_pGetLatencyTimer == NULL)
	{
		PostError(1013);
		return;
	}

	m_pEE_Read = (PtrToEE_Read)GetProcAddress(m_hmodule, "FT_EE_Read");
	if (m_pEE_Read == NULL)
	{
		PostError(1014);
		return;
	}

	m_pEE_Program = (PtrToEE_Program)GetProcAddress(m_hmodule, "FT_EE_Program");
	if (m_pEE_Program == NULL)
	{
		PostError(1015);
		return;
	}

	m_pEE_UARead = (PtrToEE_UARead)GetProcAddress(m_hmodule, "FT_EE_UARead");
	if (m_pEE_UARead == NULL)
	{
		PostError(1016);
		return;
	}
	
	m_pEE_UAWrite = (PtrToEE_UAWrite)GetProcAddress(m_hmodule, "FT_EE_UAWrite");
	if (m_pEE_UAWrite == NULL)
	{
		PostError(1017);
		return;
	}

	m_pEE_UASize = (PtrToEE_UASize)GetProcAddress(m_hmodule, "FT_EE_UASize");
	if (m_pEE_UASize == NULL)
	{
		PostError(1018);
		return;
	}

	m_pGetStatus = (PtrToGetStatus)GetProcAddress(m_hmodule, "FT_GetStatus");
	if (m_pGetStatus == NULL)
	{
		PostError(1019);
		return;
	}

	m_pSetUSBParameters = (PtrToSetUSBParameters)GetProcAddress(m_hmodule, "FT_SetUSBParameters");
	if (m_pSetUSBParameters == NULL)
	{
		PostError(1020);
		return;
	}

	m_pSetBaudRate = (PtrToSetBaudRate)GetProcAddress(m_hmodule, "FT_SetBaudRate");
	if (m_pSetBaudRate == NULL)
	{
		PostError(1021);
		return;
	}

	m_pSetDivisor = (PtrToSetDivisor)GetProcAddress(m_hmodule, "FT_SetDivisor");
	if (m_pSetDivisor == NULL)
	{
		PostError(1022);
		return;
	}

	m_pSetDataCharacteristics = (PtrToSetDataCharacteristics)GetProcAddress(m_hmodule, "FT_SetDataCharacteristics");
	if (m_pSetDataCharacteristics == NULL)
	{
		PostError(1023);
		return;
	}

	m_pSetFlowControl = (PtrToSetFlowControl)GetProcAddress(m_hmodule, "FT_SetFlowControl");
	if (m_pSetFlowControl == NULL)
	{
		PostError(1024);
		return;
	}

	m_pSetDtr = (PtrToSetDtr)GetProcAddress(m_hmodule, "FT_SetDtr");
	if (m_pSetDtr == NULL)
	{
		PostError(1025);
		return;
	}

	m_pClrDtr = (PtrToClrDtr)GetProcAddress(m_hmodule, "FT_ClrDtr");
	if (m_pClrDtr == NULL)
	{
		PostError(1026);
		return;
	}

	m_pSetRts = (PtrToSetRts)GetProcAddress(m_hmodule, "FT_SetRts");
	if (m_pSetRts == NULL)
	{
		PostError(1027);
		return;
	}

	m_pClrRts = (PtrToClrRts)GetProcAddress(m_hmodule, "FT_ClrRts");
	if (m_pClrRts == NULL)
	{
		PostError(1028);
		return;
	}

	m_pGetModemStatus = (PtrToGetModemStatus)GetProcAddress(m_hmodule, "FT_GetModemStatus");
	if (m_pGetModemStatus == NULL)
	{
		PostError(1029);
		return;
	}

	m_pSetChars = (PtrToSetChars)GetProcAddress(m_hmodule, "FT_SetChars");
	if (m_pSetChars == NULL)
	{
		PostError(1030);
		return;
	}

	m_pSetBreakOn = (PtrToSetBreakOn)GetProcAddress(m_hmodule, "FT_SetBreakOn");
	if (m_pSetBreakOn == NULL)
	{
		PostError(1031);
		return;
	}

	m_pSetBreakOff = (PtrToSetBreakOff)GetProcAddress(m_hmodule, "FT_SetBreakOff");
	if (m_pSetBreakOff == NULL)
	{
		PostError(1032);
		return;
	}

	m_pSetWaitMask = (PtrToSetWaitMask)GetProcAddress(m_hmodule, "FT_SetWaitMask");
	if (m_pSetWaitMask == NULL)
	{
		PostError(1033);
		return;
	}

	m_pWaitOnMask = (PtrToWaitOnMask)GetProcAddress(m_hmodule, "FT_WaitOnMask");
	if (m_pWaitOnMask == NULL)
	{
		PostError(1034);
		return;
	}

	m_pSetBitMode = (PtrToSetBitMode)GetProcAddress(m_hmodule, "FT_SetBitMode");
	if (m_pSetBitMode == NULL)
	{
		PostError(1035);
		return;
	}

	m_pGetBitMode = (PtrToGetBitMode)GetProcAddress(m_hmodule, "FT_GetBitMode");
	if (m_pGetBitMode == NULL)
	{
		PostError(1036);
		return;
	}
//	AfxMessageBox("All DLLs loaded.");

} // FT_Comm::LoadDLL()

//****************************************************************************************
FT_STATUS FT_Comm::SetBreakOn()
{
	if (!m_pSetBreakOn)
	{
		PostError(1100); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetBreakOn)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetBreakOff()
{
	if (!m_pSetBreakOff)
	{
		PostError(1101); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetBreakOff)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetWaitMask(DWORD Mask)
{
	if (!m_pSetWaitMask)
	{
		PostError(1102);
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetWaitMask)(m_ftHandle, Mask);
}	

//****************************************************************************************
FT_STATUS FT_Comm::WaitOnMask(DWORD Mask)
{
	if (!m_pWaitOnMask)
	{
		PostError(1103); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pWaitOnMask)(m_ftHandle, Mask);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetBitMode(UCHAR ucMask, UCHAR ucEnable)
{
	if (!m_pSetBitMode)
	{
		PostError(1104); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetBitMode)(m_ftHandle, ucMask, ucEnable);
}	

//****************************************************************************************
FT_STATUS FT_Comm::GetBitMode(PUCHAR pucMode)
{
	if (!m_pGetBitMode)
	{
		PostError(1105);
		return FT_INVALID_HANDLE;
	}

	return (*m_pGetBitMode)(m_ftHandle, pucMode);
}	


//****************************************************************************************
FT_STATUS FT_Comm::SetFlowControl(USHORT FlowControl, UCHAR XonChar, UCHAR XoffChar)
{
	if (!m_pSetFlowControl)
	{
		PostError(1106); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetFlowControl)(m_ftHandle, FlowControl, XonChar, XoffChar);
}	


//****************************************************************************************
FT_STATUS FT_Comm::SetDtr()
{
	if (!m_pSetDtr)
	{
		PostError(1107); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetDtr)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::ClrDtr()
{
	if (!m_pClrDtr)
	{
		PostError(1108);
		return FT_INVALID_HANDLE;
	}

	return (*m_pClrDtr)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetRts()
{
	if (!m_pSetRts)
	{
		PostError(1109); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetRts)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::ClrRts()
{
	if (!m_pClrRts)
	{
		PostError(1110); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pClrRts)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::GetModemStatus(ULONG *pModemStatus)
{
	if (!m_pGetModemStatus)
	{
		PostError(1111);
		return FT_INVALID_HANDLE;
	}

	return (*m_pGetModemStatus)(m_ftHandle, pModemStatus);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetChars(UCHAR EventChar, UCHAR EventCharEnabled, UCHAR ErrorChar, UCHAR ErrorCharEnabled)
{
	if (!m_pSetChars)
	{
		PostError(1112);
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetChars)(m_ftHandle, EventChar, EventCharEnabled, ErrorChar, ErrorCharEnabled);
}	



//****************************************************************************************
FT_STATUS FT_Comm::SetDataCharacteristics(UCHAR WordLength, UCHAR StopBits, UCHAR Parity)
{
	if (!m_pSetDataCharacteristics)
	{
		PostError(1113); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetDataCharacteristics)(m_ftHandle, WordLength, StopBits, Parity);
}	


//****************************************************************************************
FT_STATUS FT_Comm::SetDivisor(USHORT Divisor)
{
	if (!m_pSetDivisor)
	{
		PostError(1114);
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetDivisor)(m_ftHandle, Divisor);
}	


//****************************************************************************************
FT_STATUS FT_Comm::SetBaudRate(ULONG BaudRate)
{
	if (!m_pSetBaudRate)
	{
		PostError(1115); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetBaudRate)(m_ftHandle, BaudRate);
}	


//****************************************************************************************
FT_STATUS FT_Comm::SetUSBParameters(ULONG ulInTransferSize, ULONG ulOutTransferSize)
{
	if (!m_pSetUSBParameters)
	{
		PostError(1116);
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetUSBParameters)(m_ftHandle, ulInTransferSize, ulOutTransferSize);
}	




//****************************************************************************************
FT_STATUS FT_Comm::GetStatus(LPDWORD lpdwAmountInRxQueue, LPDWORD lpdwAmountInTxQueue, LPDWORD lpdwEventStatus)
{
	if (!m_pGetStatus)
	{
		PostError(1117); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pGetStatus)(m_ftHandle, lpdwAmountInRxQueue, lpdwAmountInTxQueue, lpdwEventStatus);
}	



//****************************************************************************************
FT_STATUS FT_Comm::Read(LPVOID lpvBuffer, DWORD dwBuffSize, LPDWORD lpdwBytesRead)
{
	if (!m_pRead)
	{
		PostError(1118);
		return FT_INVALID_HANDLE;
	}

	return (*m_pRead)(m_ftHandle, lpvBuffer, dwBuffSize, lpdwBytesRead);
}	


//****************************************************************************************
FT_STATUS FT_Comm::Write(LPVOID lpvBuffer, DWORD dwBuffSize, LPDWORD lpdwBytes)
{
	if (!m_pWrite)
	{
		PostError(1119); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pWrite)(m_ftHandle, lpvBuffer, dwBuffSize, lpdwBytes);
}	

//****************************************************************************************
FT_STATUS FT_Comm::Open(PVOID pvDevice)
{
	if (!m_pOpen)
	{
		PostError(1120); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pOpen)(pvDevice, &m_ftHandle );
}	

//****************************************************************************************
FT_STATUS FT_Comm::OpenEx(PVOID pArg1, DWORD dwFlags)
{
	if (!m_pOpenEx)
	{
		PostError(1121); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pOpenEx)(pArg1, dwFlags, &m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::ListDevices(PVOID pArg1, PVOID pArg2, DWORD dwFlags)
{
	if (!m_pListDevices)
	{
		PostError(1122); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pListDevices)(pArg1, pArg2, dwFlags);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetEvent(FT_HANDLE ftHandle, DWORD Mask, PVOID Param)
{
	if (!m_pSetEvent)
	{
		PostError(1123); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pSetEvent)(ftHandle, Mask, Param);
}

//****************************************************************************************
FT_STATUS FT_Comm::Close()
{
	if (!m_pClose)
	{
		//AfxMessageBox("FT_Close is not valid!"); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pClose)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::ResetDevice()
{
	if (!m_pResetDevice)
	{
		PostError(1124); 
		return FT_INVALID_HANDLE;
	}
	
	return (*m_pResetDevice)(m_ftHandle);
}	

//****************************************************************************************
FT_STATUS FT_Comm::Purge(ULONG dwMask)
{
	if (!m_pPurge)
	{
		PostError(1125);
		return FT_INVALID_HANDLE;
	}

	return (*m_pPurge)(m_ftHandle, dwMask);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetTimeouts(ULONG dwReadTimeout, ULONG dwWriteTimeout)
{
	if (!m_pSetTimeouts)
	{
		PostError(1126); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetTimeouts)(m_ftHandle, dwReadTimeout, dwWriteTimeout);
}	

//****************************************************************************************
FT_STATUS FT_Comm::GetQueueStatus(LPDWORD lpdwAmountInRxQueue)
{
	if (!m_pGetQueueStatus)
	{
		PostError(1127); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pGetQueueStatus)(m_ftHandle, lpdwAmountInRxQueue);
}	

//****************************************************************************************
FT_STATUS FT_Comm::SetLatencyTimer(UCHAR ucLatency)
{
	if (!m_pSetLatencyTimer)
	{
		PostError(1128); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pSetLatencyTimer)(m_ftHandle, ucLatency);
}	

//****************************************************************************************
FT_STATUS FT_Comm::GetLatencyTimer(PUCHAR pucLatency)
{
	if (!m_pGetLatencyTimer)
	{
		PostError(1129); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pGetLatencyTimer)(m_ftHandle, pucLatency);
}	


//****************************************************************************************
FT_STATUS FT_Comm::EE_Read(PFT_PROGRAM_DATA lpData)
{
	if (!m_pEE_Read)
	{
		PostError(1130); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pEE_Read)(m_ftHandle, lpData);
}	


//****************************************************************************************
FT_STATUS FT_Comm::EE_Program(PFT_PROGRAM_DATA lpData)
{
	if (!m_pEE_Program)
	{
		PostError(1131);
		return FT_INVALID_HANDLE;
	}

	return (*m_pEE_Program)(m_ftHandle, lpData);
}	


//****************************************************************************************
FT_STATUS FT_Comm::EE_UARead(PUCHAR pucData, DWORD dwDataLen, LPDWORD lpdwBytesRead)
{
	if (!m_pEE_UARead)
	{
		PostError(1132); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pEE_UARead)(m_ftHandle, pucData, dwDataLen, lpdwBytesRead);
}	
//****************************************************************************************
FT_STATUS FT_Comm::EE_UAWrite(PUCHAR pucData, DWORD dwDataLen)
{
	if (!m_pEE_UAWrite)
	{
		PostError(1133); 
		return FT_INVALID_HANDLE;
	}

	return (*m_pEE_UAWrite)(m_ftHandle, pucData, dwDataLen);
}	
//****************************************************************************************
FT_STATUS FT_Comm::EE_UASize(LPDWORD lpdwSize)
{
	if (!m_pEE_UASize)
	{
		PostError(1134);
		return FT_INVALID_HANDLE;
	}

	return (*m_pEE_UASize)(m_ftHandle, lpdwSize);
}

void FT_Comm::PostError(int errorID)
{
	//qDebug() << "Comm Error " << errorID;
}	


