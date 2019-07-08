/*
    From version 1.4 onwards, libftd2xx easily supports devices with
    custom Vendor and Product Ids.  Call FT_SetVIDPID for every VID+PID
    that you wish your D2XX application to support.  There is no longer
    any need to call FT_GetVIDPID, and the deprecated LibTable method
    no longer works.
    
    To build:
    1. Install libftd2xx.so in the /usr/local/lib directory.
    2. gcc -o setVIDPID main.c -lftd2xx -lpthread -lrt -Wl,-rpath,/usr/local/lib
*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../ftd2xx.h"

#ifndef _countof
    #define _countof(a) (sizeof(a)/sizeof(*(a)))
#endif /* _countof */



int main (void)
{
    FT_STATUS   ftStatus;
    size_t      i;
    int         retCode = EXIT_FAILURE;
    DWORD       libVersion = 0;
    DWORD       standardDevices = 0;
    DWORD       totalDevices = 0;
    /* Specify as many VID+PID pairs as you wish to support. */
    DWORD       vid[] = {0x0403, 0x7654, 0x7777, 0xdcba, 0x5a5a};
    DWORD       pid[] = {0x4242, 0x1234, 0xabcd, 0xa5a5, 0xdcba};

    (void)FT_GetLibraryVersion(&libVersion);
    printf("D2XX version %08X\n", (unsigned int)libVersion);

    if (libVersion < 0x00010400)
    {
        printf("This version of D2XX does not correctly support "
               "multiple calls to FT_SetVIDPID.\n");
        goto exit;
    }

    /* Initially, FT_ListDevices, FT_Open and FT_CreateDeviceInfoList 
     * only consider devices with standard FTDI Vendor and Product Ids. 
     */
    ftStatus = FT_ListDevices(&standardDevices, 
                              NULL, 
                              FT_LIST_NUMBER_ONLY);
    if (ftStatus != FT_OK) 
    {
        printf("FT_ListDevices failed: error code %d.\n", (int)ftStatus);
        goto exit;
    }

    printf(
        "%d device%s with standard FTDI Vendor and Product Ids detected.\n",
        (int)standardDevices,
        standardDevices == 1 ? "" : "s"
        );

    /* Tell D2XX about our additional VID+PID combinations. */
    assert(_countof(vid) == _countof(pid));
    for (i = 0; i < _countof(vid); i++)
    {
        ftStatus = FT_SetVIDPID(vid[i], pid[i]);
        if (ftStatus != FT_OK)
        {
            printf("FT_SetVIDPID failed: error code %d.\n", (int)ftStatus);
            goto exit;
        }
    }

    /* Now FT_ListDevices, FT_Open and FT_CreateDeviceInfoList will
     * consider devices with our custom Vendor and Product Ids, as 
     * well as the standard FTDI Vendor and Product Ids.
     */

    ftStatus = FT_ListDevices(&totalDevices, 
                              NULL, 
                              FT_LIST_NUMBER_ONLY);
    if (ftStatus != FT_OK) 
    {
        printf("FT_ListDevices failed: error code %d.\n", (int)ftStatus);
        goto exit;
    }

    printf(
        "%d supported device%s detected: %d standard, %d custom.\n",
        (int)totalDevices,
        totalDevices == 1 ? "" : "s",
        (int)standardDevices,
        (int)(totalDevices - standardDevices)
        );

    retCode = EXIT_SUCCESS;

exit:
    return retCode;
}
