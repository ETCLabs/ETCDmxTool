#pragma once

#include <Qt>
#include <QString>

#include <windows.h>
#include <atlstr.h>
#include "ft_comm.h"

enum FTDCommError
{ 
	FTDCOMM_OK, 
	FTDCOMM_ERROR, 
	FTDCOMM_NOTETC, 
	FTDCOMM_ALREADYOPEN, 
	FTDCOMM_ALREADYCLOSED,
	FTDCOMM_INVALIDDEVICENUMBER
};

class CComm
{
public:
	CComm(void);
	~CComm(void);
	
	// Attempt to connect to the USB device. Returns TRUE if connection was made OK.
	bool Connect();

	int SendBlock(unsigned char *iDataOut, int iNumToSend);
	int GetBlock(unsigned char *iData, int iNumRead, bool i_bUnPack=true);
	int setBaudRate(unsigned char iBaud=0xA0);
	void CharToASCIIhex(unsigned char ucInput, unsigned char* pucOutMSB, unsigned char* pucOutLSB);
	void NumberToASCIIhex(unsigned short usNumb, CStringA& csDest);
	unsigned char AppendChecksum(CStringA &csCmdString, bool ibPreserveCmnd=true);
	unsigned char utilChar2Hex(unsigned char wdChar);
	void Close();
	void getVer(unsigned char *iData);
	void setVer(unsigned char *iData);
	void pack_USB_out(unsigned char *iOutBuff, unsigned char *iInBuff, int iLen);
	int UnPackFromUSB(unsigned char *iInBuff, unsigned char *iOutBuff, int len, int *sawBreak);
	bool m_bSFPresent;
	void SetSFSetting(bool isPresent);
	bool GetSFSetting();

	// For Desire. Copied(ish) from FTDComm.cpp
	FTDCommError SendData(unsigned char * Data, int DataLength);
	FTDCommError SendBreak();
	FTDCommError SendHoldOff();
	FTDCommError SendBreakStartAndData(unsigned char StartCode, unsigned char * Data, int DataLength);
	int ReceiveData(unsigned short * Data, int MaxLength);
	
	QString getDeviceName();

	FT_Comm         Comm;
	bool			m_bConnected;
	QString			m_csDevName;
private:
	void PackOutgoingBytes(unsigned char * InputBuff, unsigned char *OutputBuff, int Length);
	int UnPackIncomingBytes(unsigned char * InputBuff, unsigned short * OutputBuff, int Length);
};
