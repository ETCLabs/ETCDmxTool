/*
	UA read sample.

	To build use the following gcc statement 
	(assuming you have the d2xx library in the /usr/local/lib directory).
	gcc -o readua main.c -L. -lftd2xx -Wl,-rpath,/usr/local/lib
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../../../ftd2xx.h"

int main(int argc, char *argv[])
{
	unsigned char * pucUAdata;
	DWORD 	dwUASize, dwUARead;
	FILE * fp;
	FT_HANDLE	ftHandle0;
	FT_STATUS	ftStatus;
	int iport;
	
	if(argc > 1) {
		sscanf(argv[1], "%d", &iport);
	}
	else {
		iport = 0;
	}
	
	printf("opening port %d\n", iport);
	ftStatus = FT_Open(iport, &ftHandle0);
	if(ftStatus == FT_OK) {
		printf("ftHandle0 = %p\n", ftHandle0);
	}
	else {
		/* 
			This can fail if the ftdi_sio driver is loaded
		 	use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
		 */
		printf("FT_Open(%d) failed\n", iport);
		return 1;
	}
	
	ftStatus = FT_EE_UASize(ftHandle0, &dwUASize);
	if(ftStatus == FT_OK)
		printf("dwUASize = %d\n", (int)dwUASize);
	else {
		printf("Could not read UA size\n");
		FT_Close(ftHandle0);
		return 1;
	}
	pucUAdata = (unsigned char *)malloc(dwUASize);
	if(pucUAdata == NULL) {
		printf("Out of resources\n");
		FT_Close(ftHandle0);
		return 1;
	}
	ftStatus = FT_EE_UARead(ftHandle0, pucUAdata, dwUASize, &dwUARead);
	if(ftStatus == FT_OK) {
		fp = fopen("UA_DATA.bin", "w+");
		fwrite(pucUAdata, 1, dwUARead, fp);
		fclose(fp);
	}
	else{
		printf("could not read UA\n");
	}
	free(pucUAdata);
	FT_Close(ftHandle0);

	return 0;
}
