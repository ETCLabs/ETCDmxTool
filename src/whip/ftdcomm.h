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

#pragma once

#include "Ftd2xx.h"

#include <QList>
#include <QString>
#include <QFile>

typedef enum 
{ 
	FTDCOMM_OK, 
	FTDCOMM_ERROR, 
	FTDCOMM_NOTETC, 
	FTDCOMM_ALREADYOPEN, 
	FTDCOMM_ALREADYCLOSED,
	FTDCOMM_INVALIDDEVICENUMBER
} FTDCommError;

class FTDComm
{
public:
	FTDComm(void);
	~FTDComm(void);

    struct FtdiDevice
    {
        FtdiDevice();
        FtdiDevice(int devNum, QString serial, QString desc);
        int deviceNum;
        QString serialNum;
        QString description;
    };

	FTDCommError Open(int DeviceNum);
	FTDCommError OpenBySerialNumber(QString SerialNumber);
	FTDCommError Close();
	bool GetDeviceOpen();
	FTDCommError SetupDevice(long BaudRate);
	int GetNumberOfDevices();

    QList<FtdiDevice> GetUsbWhips();

	QString GetDeviceDescription(int DeviceNum);
	QString GetDeviceSerialNumber(int DeviceNum);

	FTDCommError SendData(unsigned char * Data, int DataLength);
	FTDCommError SendBreak();
	FTDCommError SendHoldOff();
	FTDCommError SendBreakStartAndData(unsigned char StartCode, unsigned char * Data, int DataLength);

    int ReceiveData(unsigned short * Data, DWORD MaxLength);

private:

	

	FT_HANDLE DeviceHandle;
	bool DeviceOpen;
	int NumDevices;

	void PackOutgoingBytes(unsigned char * InputBuff, unsigned char *OutputBuff, int Length);
	int UnPackIncomingBytes(unsigned char * InputBuff, unsigned short * OutputBuff, int Length);
};
