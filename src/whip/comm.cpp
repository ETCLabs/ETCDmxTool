#include ".\comm.h"

#include "const_struct_enums.h"



CComm::CComm(void)
{
	m_bConnected = false;
	m_bSFPresent = false;
}

CComm::~CComm(void)
{
}

void CComm::Close()
{
	Comm.Close();
}

void CComm::getVer(unsigned char *iData)
{
	DWORD a_dw;

	Comm.EE_UARead(iData, 14, &a_dw);
}

void CComm::setVer(unsigned char *iData)
{
	Comm.EE_UAWrite(iData, 14);
}

QString CComm::getDeviceName() 
{
	FT_STATUS   a_ftStatus;
	DWORD       a_dwNumDevs;
	DWORD       a_dw = 0;
	QString result;
	char* a_pchBufPtrs[64]; // pointer to array of 64 pointers


		a_ftStatus = Comm.ListDevices(&a_dwNumDevs, NULL, FT_LIST_NUMBER_ONLY);

		if(a_ftStatus == FT_OK) 
		{			
			a_dwNumDevs = (a_dwNumDevs & 0x003F);
			for(a_dw = 0; a_dw < a_dwNumDevs; a_dw++)
			{
				a_pchBufPtrs[a_dw] = new char[64];
			}
			a_pchBufPtrs[a_dw] = NULL;

			//a_ftStatus = m_pDlgMulti->CommIn.ListDevices(a_pchBufPtrs, &a_dwNumDevs, FT_LIST_ALL | FT_OPEN_BY_DESCRIPTION);
			a_ftStatus = Comm.ListDevices(a_pchBufPtrs, &a_dwNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
			if(FT_SUCCESS(a_ftStatus)) 
			{				
					// Find the one that contains "ETC" - ignore non-etc adapters - TJS
					for(a_dw = 0; a_dw < a_dwNumDevs; a_dw++)
					{
						if( strstr(a_pchBufPtrs[a_dw], "ETC") )
							result = a_pchBufPtrs[a_dw];
					}
			}


				// Free RAM to avoid memory leaks.
			for(a_dw = 0; a_dw < a_dwNumDevs; a_dw++)
			{
				delete a_pchBufPtrs[a_dw]; 
			}

				// if only one device, then select it and return to caller
			//if(a_dwNumDevs <= 1)
			//{
			//	a_csName = a_csTemp;
			//}

			//UpdateData(false);
			
		} // ListDevices call was successful

	return result;	
}

bool CComm::Connect()
{
	bool retval = false;
	FT_STATUS     a_ulStatus = 0;
	
	m_bConnected = false;

    Comm.LoadDLL();

	m_csDevName = getDeviceName();

	if( m_csDevName == "")
	{
		m_csDevName = "ETC_0001"; 
	}
	
	//open the device
    a_ulStatus |= Comm.OpenBy(m_csDevName);

	if(FT_OK == a_ulStatus)
	{		
		m_bConnected = true;
		retval = true;
	}
	Sleep(150);

	return retval;

}

int CComm::setBaudRate(unsigned char iBaud/*=0xA0*/)
{
	unsigned char retBuff[MAX_COMM_BLOCK];
	DWORD a_dwRetBytes = 0;
	int a_iBail;
	int retval = 0;

	if ( m_bConnected )
	{
		retval = Comm.Write(&iBaud, 1, &a_dwRetBytes); // tells injector to ignore DMX for a few seconds

		if ( retval != FT_OK )
		{
			Connect();  // attempt reconnection
		}

			// chew up the reply, it is an echo for baud changes
		a_iBail = 10;		// only wait for 1 second
		do
		{
			retval = GetBlock(retBuff, MAX_COMM_BLOCK);
			Sleep(1);
		}while (!retval && a_iBail--);
	}

	return retval;	
}

int CComm::GetBlock(unsigned char *iData, int iNumRead, bool i_bUnPack /* =true*/)
{
	DWORD EventDWord = 0;
    DWORD RxBytes = 0;
    DWORD TxBytes = 0;
	unsigned char a_ucRxbuf[2048];
	int bail = 500;
	int	a_bSawBrk;

	if ( m_bConnected )
	{
		do
		{
			Comm.GetStatus(&RxBytes,&TxBytes,&EventDWord);
			if (RxBytes)
			{
					RxBytes = (RxBytes<2048) ? RxBytes : 2048;
					Comm.Read(a_ucRxbuf, RxBytes, &RxBytes);
					if ( i_bUnPack )
					{
						if( RxBytes < MAX_COMM_BLOCK )
						{
							UnPackFromUSB(a_ucRxbuf, iData, RxBytes, &a_bSawBrk);
						}
					}
					else
					{
						memcpy(iData,a_ucRxbuf,RxBytes);	
					}
			}
		}while( (RxBytes == 0) && (bail-- > 1) );
	}

	return RxBytes;
}

/*
	0xA0 - legacy DMX hold off
	0xA1 - 250k pins 2&3
	0xA2 - 57.6 pins 4&5
	0xA3 - 38.4 pins 2&3
	... - unsused.
	0xAF - Prep for download for interface PIC
*/
int CComm::SendBlock(unsigned char *iDataOut, int iNumToSend)
{
	int retval = 0;
	DWORD a_dwRetBytes = 0;
		// change the HoldOff character based on type of device talking to.
	//unsigned char cDwnldHoldOff[] = {0xA0};

	if ( m_bConnected )
	{
		//Comm.Write(&iBaud, 1, &a_dwRetBytes); // tells injector to ignore DMX for a few seconds
		retval = Comm.Write(iDataOut,iNumToSend, &a_dwRetBytes);

		if ( retval != FT_OK )
		{
			Connect();  // attempt reconnection
		}
		else
		{
			retval = a_dwRetBytes;
		}	
	}

	return retval;
}

/**************************************************************************************************
	NAME:
	CParse::CharToASCIIhex()

	DESCRIPTION:
	This procedure will take an input character and convert to two ASCII-hex encoded characters.
	The hex characters are represented by 0 thru 9 and A thru F.

	INPUTS:
	unsigned char  ucInput   = the input number treated as binary
	unsigned char* pucOutMSB = where to store the upper 4 bits of ucInput turned to ASCII-hex
	unsigned char* pucOutLSB = where to store the lower 4 bits of ucInput turned to ASCII-hex

	RETURN:
	void
**************************************************************************************************/
void CComm::CharToASCIIhex(unsigned char ucInput, unsigned char* pucOutMSB, unsigned char* pucOutLSB)
{
	unsigned char a_ucTemp;

	// Upper nibble
	a_ucTemp = ucInput;
	//
	// move the upper 4 bits to the lower nibble
	a_ucTemp = static_cast<unsigned char>(a_ucTemp >> 4);
	//
	if(a_ucTemp > 9)
		a_ucTemp = static_cast<unsigned char>(a_ucTemp + 0x11 - 0x0A);
	a_ucTemp = static_cast<unsigned char>(a_ucTemp + 0x30);
	*pucOutMSB = a_ucTemp;

	// Lower nibble
	a_ucTemp = ucInput;
	//
	// mask the upper nibble away
	a_ucTemp = static_cast<unsigned char>(a_ucTemp & 0x0F);
	//
	if(a_ucTemp > 9)
		a_ucTemp = static_cast<unsigned char>(a_ucTemp + 0x11 - 0x0A);
	a_ucTemp = static_cast<unsigned char>(a_ucTemp + 0x30);
	*pucOutLSB = a_ucTemp;

} // CParse::CharToASCIIhex()

/**************************************************************************************************
	NAME:
	CParse::NumberToASCIIhex()

	DESCRIPTION:
	This procedure will add the 16 bit input number to the CStringA reference input variable.  The
	number is treated as an unsigned quantity and it is ASCIIhex encoded into four characters.
	The characters are stored Most Significant first and Least Significant last.  The characters
	are appended to the end of the input CStringA.

	INPUTS:
	unsigned short usNumb = the unsigned 16 bit input number 
	CStringA&       csDest = the destination CStringA for the characters.

	RETURN:
	void
**************************************************************************************************/
void CComm::NumberToASCIIhex(unsigned short usNumb, CStringA& csDest)
{
	char a_szTemp[10];

	CharToASCIIhex(static_cast<unsigned char>(usNumb/256), reinterpret_cast<unsigned char*>(&a_szTemp[0]),
																reinterpret_cast<unsigned char*>(&a_szTemp[1]));
	CharToASCIIhex(static_cast<unsigned char>(usNumb & 0xFF), reinterpret_cast<unsigned char*>(&a_szTemp[2]),
																	reinterpret_cast<unsigned char*>(&a_szTemp[3]));
	a_szTemp[4] = 0;
	csDest += a_szTemp;

} // CParse::NumberToASCIIhex()

  
/**************************************************************************************************
	NAME:
	CDownloaderDlg::AppendChecksum()

	DESCRIPTION:
	This function will append a simple modulo-256 checksum to the end of the incoming string. The
	checksum is encoded into two ASCII-hex characters.  The function will also return the single
	8 bit value of the checksum in the for of an unsigned char.

	INPUTS:
	CStringA& csCmdString = a reference tot he CSring that is to get the modulo-256 checksum character
	bool ibPreserveCmnd = do we need to do checksum that has the cmnd munched or not

	RETURN:
	unsigned char = the modulo-256 checksum character

**************************************************************************************************/
unsigned char CComm::AppendChecksum(CStringA& csCmdString, bool ibPreserveCmnd/*=true*/)
{
	char          a_szCHKSUM[3] = {0};
	unsigned char a_ucCHKSUM    = 0;
	int           a_nCHKSUM     = 0;
	int           a_n = 0;;
	//CParse        a_cparse;
	int len = 0;

	if ( !ibPreserveCmnd )
	{
		a_nCHKSUM += csCmdString.GetAt(0);
		a_nCHKSUM += csCmdString.GetAt(1);
		a_nCHKSUM += (utilChar2Hex(csCmdString.GetAt(2))<<4) + utilChar2Hex(csCmdString.GetAt(3));
		a_nCHKSUM += (utilChar2Hex(csCmdString.GetAt(4))<<4) + utilChar2Hex(csCmdString.GetAt(5));
		a_n = 6;
	}
	len = csCmdString.GetLength();
	for( ; a_n < len; a_n++)
			a_nCHKSUM += csCmdString.GetAt(a_n);

	a_ucCHKSUM = static_cast<unsigned char>(a_nCHKSUM & 0xFF);	// Modulo-256 checksum
	CharToASCIIhex(a_ucCHKSUM, reinterpret_cast<unsigned char*>(&a_szCHKSUM[0]),
																		reinterpret_cast<unsigned char*>(&a_szCHKSUM[1]));
	csCmdString += a_szCHKSUM;

	return a_ucCHKSUM;

} // CDownloaderDlg::AppendChecksum()


/**************************************************************************************************
	NAME:
	CDownloaderDlg::utilChar2Hex()

**************************************************************************************************/
unsigned char CComm::utilChar2Hex(unsigned char wdChar)
{
	if(wdChar > '9')
	{
		return (unsigned char)((wdChar & 0x07u) + 0x09u);
	}
	else
	{
		return (unsigned char)(wdChar & 0x0Fu);
	}

} // CDownloaderDlg::utilChar2Hex()


/*
	usb encoding scheme
	1000 xxxx - low nibble of data character
	1001 xxxx - high nibble of data character
	1010 xxxx - DMX hold off command sent to PIC
	1011 xxxx - DMX hold off and high nibble combined command to PIC
	1100 xxxx - send break command to PIC
	1101 xxxx - send break and high nibble command to PIC
	1110 xxxx - send break, dmx hold off command to PIC
	1111 xxxx - send break, dmx hold off, high nibble command to PIC
*/
void CComm::pack_USB_out(unsigned char *iOutBuff, unsigned char *iInBuff, int iLen)
{
	int i, y;

			// pack bytes
	for ( i=0, y=0; y<(iLen); y++, i+=2)
	{
			// high nibble	top code is 1001xxxx
		iOutBuff[i] = iInBuff[y];
		iOutBuff[i] = iOutBuff[i] >> 4;
		iOutBuff[i] |= 0x90;

			// low nibble	top code is 1000xxxx
		iOutBuff[i+1] = iInBuff[y];
		iOutBuff[i+1] &= 0x0F;	// clear out top nibble
		iOutBuff[i+1] |= 0x80;	// set top bit
	}
}

/*
	usb encoding scheme
	0000 0000 - incoming break
	1000 xxxx - low nibble of data character
	1001 xxxx - high nibble of data character
	1010 xxxx - DMX hold off command sent to PIC
	1011 xxxx - DMX hold off and high nibble combined command to PIC
	1100 xxxx - send break command to PIC
	1101 xxxx - send break and high nibble command to PIC
	1110 xxxx - send break, dmx hold off command to PIC
	1111 xxxx - send break, dmx hold off, high nibble command to PIC

	here we are only needing to worry about break, and then characters 1000, and 1001
	examples:
		RDM response packet with Break
		0x00 | 0x9F | 0x80 | 0x90 | 0x81 | 0x91 | 0x87 | ...
		translates into
			Break | 0XF0 | 0x01 | 0x17 | ...

		RDM discovery response (no break)---this is also downloader packets to MCC
		0x9F | 0x8E | 0x9F | 0x8E | 0x9F | 0x8E | 0x9F | 0x8E |...
		translates into
			0xFE | 0xFE | 0xFE | 0xFE | ...


	The return value is the true length of the resultant packet.
*/
int CComm::UnPackFromUSB(unsigned char *iInBuff, unsigned char *iOutBuff, int len, int *sawBreak)
{
	unsigned char cTemp = 0;
	int index = 0;
	bool bGotHigh = false;

	if ( iInBuff &&iOutBuff )
	{
		for ( int x=0; x<len; x++ )
		{
				// break
			if ( iInBuff[x] == 0x00 )
			{
				iOutBuff[index++] = 0x00;
				*sawBreak = 1;
			}
			else
			{
				if (iInBuff[x] & 0x80)
				{
						// high nibble
					if ( iInBuff[x] & 0x10 )
					{
						cTemp = iInBuff[x];
						cTemp = (cTemp << 4) & (unsigned char) 0xff;
						bGotHigh = true;
					}
					else if (bGotHigh)	// low nibble
					{
						cTemp |= ( iInBuff[x] & 0x0F );
						iOutBuff[index++] = cTemp;
						bGotHigh = false;
					}	
				}
			} // not break

		} // loop for all bytes
	}

	return index;

} // UnPackFromUSB

void CComm::SetSFSetting(bool isPresent){
	m_bSFPresent = isPresent;
}

bool CComm::GetSFSetting(){
	return m_bSFPresent;
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
void CComm::PackOutgoingBytes(unsigned char * InputBuff, unsigned char *OutputBuff, int Length)
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

FTDCommError CComm::SendData(unsigned char * Data, int DataLength)
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(m_bConnected)
	{
		unsigned char * PackedData = new unsigned char[DataLength * 2];
		DWORD BytesSend = 0;
		PackOutgoingBytes(Data, PackedData, DataLength);
		if(Comm.Write(PackedData, DataLength * 2, &BytesSend) == FT_OK)
		{
			Result = FTDCOMM_OK;
		}
		delete PackedData;
	}
	return Result;
}

FTDCommError CComm::SendBreakStartAndData(unsigned char StartCode, unsigned char * Data, int DataLength)
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(m_bConnected)
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

FTDCommError CComm::SendBreak()
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(m_bConnected)
	{
		unsigned char BreakChar = 0xC0;
		DWORD BytesSend = 0;
		if(Comm.Write(&BreakChar, 1, &BytesSend) == FT_OK)
		{
			Result = FTDCOMM_OK;
		}				
	}
	return Result;
}

FTDCommError CComm::SendHoldOff()
{
	FTDCommError Result = FTDCOMM_ERROR;
	if(m_bConnected)
	{
		unsigned char BreakChar = 0xA0;
		DWORD BytesSend = 0;
		if(Comm.Write(&BreakChar, 1, &BytesSend) == FT_OK)
		{
			Result = FTDCOMM_OK;
		}				
	}
	return Result;
}

int CComm::UnPackIncomingBytes(unsigned char * InputBuff, unsigned short * OutputBuff, int Length)
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

int CComm::ReceiveData(unsigned short * Data, int MaxLength)
{
	int BytesReceived = 0;
	MaxLength = MaxLength * 2;
	unsigned char * PackedData = new unsigned char [MaxLength];
	if(m_bConnected)
	{
		DWORD RXBytes, TXBytes, Events; 
		if(Comm.GetStatus(&RXBytes, &TXBytes, &Events) == FT_OK)
		{
			if(RXBytes > MaxLength)
				RXBytes = MaxLength;
			if(RXBytes > 0)
			{
				if(Comm.Read(PackedData, RXBytes, &RXBytes) == FT_OK)
				{
					BytesReceived = UnPackIncomingBytes(PackedData, Data, RXBytes);
				}
			}
		}
	}
	delete PackedData;
	return BytesReceived;
}
