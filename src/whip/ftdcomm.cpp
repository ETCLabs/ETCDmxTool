// Copyright (c) 2017 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "ftdcomm.h"
#include <QString>
#include <QFile>

#define ETC_VID 0x14d5
#define WHIP_PID 0x1003

FTDComm::FTDComm(void)
{
	NumDevices = 0;
	DeviceHandle = 0;
	DeviceOpen = false;
}

FTDComm::~FTDComm(void)
{
}

FTDCommError FTDComm::Open(int DeviceNum)
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(!DeviceOpen)
	{
		if(DeviceNum >= 0 && DeviceNum < GetNumberOfDevices())
		{
			QString DeviceSerialNum;
			DeviceSerialNum = GetDeviceSerialNumber(DeviceNum);
			if(DeviceSerialNum.contains("ETC", Qt::CaseInsensitive))
			{
				FT_STATUS Err = FT_Open(DeviceNum,&DeviceHandle);
				if(Err == FT_OK)
				{
					DeviceOpen = true;
					Result = FTDCOMM_OK;
				}
			}
			else
			{
				Result = FTDCOMM_NOTETC;
			}
		}
		else
		{
			Result = FTDCOMM_INVALIDDEVICENUMBER;
		}
	}
	else
	{
		Result = FTDCOMM_ALREADYOPEN;
	}
	return Result;
}

FTDCommError FTDComm::OpenBySerialNumber(QString SerialNumber)
{
	FTDCommError Result = FTDCOMM_ERROR;
	return Result;
}

FTDCommError FTDComm::Close()
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(DeviceOpen && DeviceHandle)
	{
		if(FT_Close(DeviceHandle) == FT_OK)
		{
			DeviceOpen = false;
			Result = FTDCOMM_OK;
		}
	}
	else
	{
		Result = FTDCOMM_ALREADYCLOSED;
	}
	return Result;
}

bool FTDComm::GetDeviceOpen()
{
	return DeviceOpen;
}

// 0xA0 - legacy DMX hold off
// 0xA1 - 250k pins 2&3
// 0xA2 - 57.6 pins 4&5
// 0xA3 - 38.4 pins 2&3
// ... - unsused.
// 0xAF - Prep for download for interface PIC
FTDCommError FTDComm::SetupDevice(long BaudRate)
{
    Q_UNUSED(BaudRate);
	FTDCommError Result = FTDCOMM_ERROR;
	if(/*FT_SetBaudRate(DeviceHandle, BaudRate) == FT_OK &&*/
	   /*FT_SetTimeouts(DeviceHandle, 1000, 1000) == FT_OK &&*/
	   (FT_SetDivisor(DeviceHandle, 6) == FT_OK) &&	
	   (FT_SetDataCharacteristics(DeviceHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE) == FT_OK) &&
	   (FT_SetFlowControl(DeviceHandle, FT_FLOW_NONE, 0, 0) == FT_OK) )
	{
		Result = FTDCOMM_OK;
		unsigned char PICBaudRate = 0xA1;//250K pins 2&3
		DWORD BytesWritten;
		FT_Write(DeviceHandle, &PICBaudRate, 1, &BytesWritten);
	}
	return Result;
}

int FTDComm::GetNumberOfDevices()
{
	int TempNumDevices = 0;
	if(FT_ListDevices(&TempNumDevices, NULL, FT_LIST_NUMBER_ONLY) == FT_OK)
	{
		NumDevices = TempNumDevices;
	}
	else
	{
        NumDevices = -1;
	}
    return NumDevices;
}

QList<FTDComm::FtdiDevice> FTDComm::GetUsbWhips()
{
    QList<FTDComm::FtdiDevice> result;

#if defined(Q_OS_LINUX) || defined(Q_OS_Q_OS_OSX)
    FT_SetVIDPID(ETC_VID, WHIP_PID);
#endif

    int devCount = GetNumberOfDevices();
    for (int i=0; i<devCount; ++i)
    {
        QString DevSn = GetDeviceSerialNumber(i);
        QString DevDesc = GetDeviceDescription(i);
        if (DevSn.contains("ETC", Qt::CaseInsensitive))
        {
            result.append(FtdiDevice(i, DevSn, DevDesc));
        }
    }
    return result;
}

QString FTDComm::GetDeviceDescription(int DeviceNum)
{
    quintptr devIndex = DeviceNum;
    char Buffer[64];

    FT_STATUS RetVal = FT_ListDevices((PVOID)devIndex, Buffer, FT_LIST_BY_INDEX|FT_OPEN_BY_DESCRIPTION);
    if(RetVal == FT_OK)
    {
        return QString::fromLatin1(Buffer);
    }
    return QString();
}

QString FTDComm::GetDeviceSerialNumber(int DeviceNum)
{
    quintptr devIndex = DeviceNum;
    char Buffer[64];

    FT_STATUS RetVal = FT_ListDevices((PVOID)devIndex, Buffer, FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER);
    if(RetVal == FT_OK)
	{
        return QString::fromLatin1(Buffer);
	}
    return QString();
}

FTDCommError FTDComm::SendData(unsigned char * Data, int DataLength)
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(DeviceOpen)
    {
		unsigned char * PackedData = new unsigned char[DataLength * 2];
		DWORD BytesSend = 0;
		PackOutgoingBytes(Data, PackedData, DataLength);
		if(FT_Write(DeviceHandle, PackedData, DataLength * 2, &BytesSend) == FT_OK)
		{
			Result = FTDCOMM_OK;
		}
        delete[] PackedData;
	}
	return Result;
}

FTDCommError FTDComm::SendBreakStartAndData(unsigned char StartCode, unsigned char * Data, int DataLength)
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(DeviceOpen)
	{	
		if(SendBreak() == FTDCOMM_OK)
		{
			if(SendData(&StartCode, 1) == FTDCOMM_OK)
			{
				Result = SendData(Data, DataLength);
				SendHoldOff();
			}
		}
	}
	return Result;
}

FTDCommError FTDComm::SendBreak()
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(DeviceOpen)
	{
		unsigned char BreakChar = 0xC0;
		DWORD BytesSend = 0;
		if(FT_Write(DeviceHandle, &BreakChar, 1, &BytesSend) == FT_OK)
		{
			Result = FTDCOMM_OK;
		}				
	}
	return Result;
}

FTDCommError FTDComm::SendHoldOff()
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(DeviceOpen)
	{
		unsigned char BreakChar = 0xA0;
		DWORD BytesSend = 0;
		if(FT_Write(DeviceHandle, &BreakChar, 1, &BytesSend) == FT_OK)
		{
			Result = FTDCOMM_OK;
		}				
	}
	return Result;
}

int FTDComm::ReceiveData(unsigned short * Data, DWORD MaxLength)
{
	int BytesReceived = 0;
	MaxLength = MaxLength * 2;
	unsigned char * PackedData = new unsigned char [MaxLength];
	if(DeviceOpen)
	{
		DWORD RXBytes, TXBytes, Events;
		if(FT_GetStatus(DeviceHandle, &RXBytes, &TXBytes, &Events) == FT_OK)
		{
			if(RXBytes > MaxLength)
				RXBytes = MaxLength;
			if(RXBytes > 0)
			{
				if(FT_Read(DeviceHandle, PackedData, RXBytes, &RXBytes) == FT_OK)
                {
					BytesReceived = UnPackIncomingBytes(PackedData, Data, RXBytes);
				}
			}
		}
	}
    delete[] PackedData;
	return BytesReceived;
}


//
//	usb encoding scheme
//	1000 xxxx - low nibble of data character
//	1001 xxxx - high nibble of data character
//	1010 xxxx - DMX hold off command sent to PIC
//	1011 xxxx - DMX hold off and high nibble combined command to PIC
//	1100 xxxx - send break command to PIC
//	1101 xxxx - send break and high nibble command to PIC
//	1110 xxxx - send break, dmx hold off command to PIC
//	1111 xxxx - send break, dmx hold off, high nibble command to PIC
//
void FTDComm::PackOutgoingBytes(unsigned char * InputBuff, unsigned char *OutputBuff, int Length)
{
	int i, y;
	for(i = 0, y = 0; y < Length; y++, i+=2)
	{
		// high nibble	top code is 1001xxxx
		OutputBuff[i] = InputBuff[y];
		OutputBuff[i] = OutputBuff[i] >> 4;
		OutputBuff[i] |= 0x90;

		// low nibble	top code is 1000xxxx
		OutputBuff[i+1] = InputBuff[y];
		OutputBuff[i+1] &= 0x0F;	// clear out top nibble
		OutputBuff[i+1] |= 0x80;	// set top bit
	}
}

int FTDComm::UnPackIncomingBytes(unsigned char * InputBuff, unsigned short * OutputBuff, int Length)
{
	unsigned short Temp = 0;
	int OutputIndex = 0;
	bool GotHighNibble = false;

	if(InputBuff && OutputBuff)
	{
		for(int x = 0; x < Length; x++)
		{
			if(InputBuff[x] == 0x00)
			{
				OutputBuff[OutputIndex++] = 0xBB00;
			}
			else
			{
				if(InputBuff[x] & 0x80)
				{
					if (InputBuff[x] & 0x10)
					{// High Nibble
						Temp = InputBuff[x];
						Temp = (Temp << 4);
						GotHighNibble = true;
					}
					else if(GotHighNibble)
					{// low nibble
						Temp |= (InputBuff[x] & 0x0F);
						OutputBuff[OutputIndex++] = Temp & 0xFF;
						GotHighNibble = false;
					}	
				}
			} 
		}
	}
	return OutputIndex;
}

FTDComm::FtdiDevice::FtdiDevice() :
    deviceNum(-1)
{
}

FTDComm::FtdiDevice::FtdiDevice(int devNum, QString serial, QString desc) :
    deviceNum(devNum)
  , serialNum(serial)
  , description(desc)
{
}
