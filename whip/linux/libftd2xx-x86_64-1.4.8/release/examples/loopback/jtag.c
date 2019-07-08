/*
 * Enable Multi-Protocol Synchronous Serial Engine (MPSSE) on an FTDI chip,
 * and use JTAG commands to loop DO back to DI.  Write a known sequence of 
 * bytes then expect to read them back.  Run this with an FT232H, FT2232D, 
 * FT2232H or FT4232H connected.  No additional hardware is needed.
 *
 * Build with:
 *     gcc jtag.c -o jtagloopback -Wall -Wextra -I..
 *         -lftd2xx -lpthread -lrt 
 *         -Wl,-rpath /usr/local/lib
 * 
 * Run with:
 *     sudo ./jtagloopback
 *
 * On Windows, build with:
 *     cl jtag.c ftd2xx.lib -I..
 * and run jtag.exe.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ftd2xx.h"



#define UNUSED_PARAMETER(x) (void)(x)

#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))



#ifdef _WIN32
/* Windows doesn't have gettimeofday but winsock.h does have a
 * definition of timeval:
 *
 * struct timeval 
 * {
 *     long  tv_sec;
 *     long  tv_usec;
 * };
 */
static int gettimeofday(struct timeval *tv, void *timezone)
{
    SYSTEMTIME st;

    UNUSED_PARAMETER(timezone);

    GetSystemTime(&st);

    tv->tv_sec = (long)(
        (st.wHour * 60 * 60) +
        (st.wMinute * 60) +
        (st.wSecond));

    tv->tv_usec = 0; // We're not using microseconds here.

    return 0;
}



static void timersub(struct timeval *a,
                     struct timeval *b,
                     struct timeval *res)
{
    res->tv_sec = a->tv_sec - b->tv_sec;
    res->tv_usec = 0;
}
#else
    #include <sys/time.h>
#endif // _WIN32



static void dumpBuffer(unsigned char *buffer, int elements)
{
    int j;

    for (j = 0; j < elements; j++)
    {
        if (j % 8 == 0)
        {
            if (j % 16 == 0)
                printf("\n%p: ", &buffer[j]);
            else
                printf("   "); // Separate two columns of eight bytes
        }
        printf("%02X ", (unsigned int)buffer[j]);
    }
    printf("\n\n");
}




static FT_STATUS sendJtagCommand(FT_HANDLE      ftHandle,
                                 unsigned char *sequence,
                                 const size_t   length)
{
    FT_STATUS  ftStatus = FT_OK;
    DWORD      bytesToWrite = (DWORD)length;
    DWORD      bytesWritten = 0;
  
    ftStatus = FT_Write(ftHandle, sequence, bytesToWrite, &bytesWritten);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_Write returned %d\n", (int)ftStatus);
        return ftStatus;
    }
    
    if (bytesWritten != bytesToWrite)
    {
        printf("Failure.  FT_Write wrote %d bytes instead of %d.\n",
               (int)bytesWritten,
               (int)bytesToWrite);
    }

    return ftStatus;
}



static FT_STATUS setupJtagLoopback(FT_HANDLE ftHandle,
                                   DWORD     bytesToWrite)
{
    FT_STATUS       ftStatus = FT_OK;
    unsigned char   setup[3] = 
    { 
        0x80, 0x08, 0x0B  // TMS start high; TDO is input
    };
    unsigned char   loopback[1] =
    {
        0x84              // Connect TDI/TDO loopback
    };
    unsigned char   setClock[3] =
    {    
        0x86, 0x04, 0x00  // TCK divisor: CLK = 6 MHz / (1 + 0004) == 1.2 MHz 
    }; 
    unsigned char   transceive[3] =
    {    
        0x31, 0x00, 0x00  // Write + read; length bytes set later.
    }; 
  
    ftStatus = sendJtagCommand(ftHandle, setup, sizeof setup);
    if (ftStatus != FT_OK) 
    {
        return ftStatus;
    }

    ftStatus = sendJtagCommand(ftHandle, setClock, sizeof setClock);
    if (ftStatus != FT_OK) 
    {
        return ftStatus;
    }

    ftStatus = sendJtagCommand(ftHandle, loopback, sizeof loopback);
    if (ftStatus != FT_OK) 
    {
        return ftStatus;
    }

    transceive[1] = (unsigned char)(bytesToWrite & 0x000000FF);
    transceive[2] = (unsigned char)((bytesToWrite & 0x0000FF00) >> 8);
    ftStatus = sendJtagCommand(ftHandle, transceive, sizeof transceive);

    return ftStatus;
}



int main(int argc, char *argv[])
{
    int             retCode = -1; // Assume failure
    int             f = 0;
    DWORD           driverVersion = 0;
    FT_STATUS       ftStatus = FT_OK;
    FT_HANDLE       ftHandle = NULL;
    int             portNum = 0; // First device found
    size_t          bufferSize = 16 * 1024;
    DWORD           bytesToWrite;
    DWORD           bytesWritten = 0;
    DWORD           bytesReceived = 0;
    DWORD           bytesRead = 0;
    struct timeval  startTime;
    int             journeyDuration;
    unsigned char  *writeBuffer = NULL;
    unsigned char  *readBuffer = NULL;
    int             queueChecks = 0;

    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);
    
    // Make printfs immediate (no buffer)
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("Opening FTDI device %d.\n", portNum);
    
    ftStatus = FT_Open(portNum, &ftHandle);
    if (ftStatus != FT_OK) 
    {
        printf("FT_Open(%d) failed, with error %d.\n", portNum, (int)ftStatus);
        printf("On Linux, lsmod can check if ftdi_sio (and usbserial) are present.\n");
        printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
        goto exit;
    }

    assert(ftHandle != NULL);

    ftStatus = FT_GetDriverVersion(ftHandle, &driverVersion);
    if (ftStatus != FT_OK)
    {
        printf("Failure.  FT_GetDriverVersion returned %d.\n",
               (int)ftStatus);
        goto exit;
    }

    printf("D2XX version : %x.%x.%x\n", 
           (unsigned int)((driverVersion & 0x00FF0000) >> 16),
           (unsigned int)((driverVersion & 0x0000FF00) >> 8),
           (unsigned int)(driverVersion & 0x000000FF) 
           );

    ftStatus = FT_ResetDevice(ftHandle);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_ResetDevice returned %d.\n", (int)ftStatus);
        goto exit;
    }

    ftStatus = FT_SetBitMode(ftHandle, 0x00, FT_BITMODE_RESET);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_SetBitMode returned %d\n", (int)ftStatus);
        goto exit;
    }

    ftStatus = FT_SetLatencyTimer(ftHandle, 2);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_SetLatencyTimer returned %d\n", (int)ftStatus);
        goto exit;
    }

    ftStatus = FT_SetTimeouts(ftHandle, 3000, 3000);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_SetTimeouts returned %d\n", (int)ftStatus);
        goto exit;
    }
    
    ftStatus = FT_SetBitMode(ftHandle, 0x0B, FT_BITMODE_MPSSE);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_SetBitMode returned %d\n", (int)ftStatus);
        goto exit;
    }
    
    bytesToWrite = bufferSize;
    ftStatus = setupJtagLoopback(ftHandle, bytesToWrite);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_ResetDevice returned %d.\n", (int)ftStatus);
        goto exit;
    }

    writeBuffer = (unsigned char *)malloc((size_t)bufferSize);
    if (writeBuffer == NULL)
        goto exit;

    // Fill write buffer with consecutive values.
    for (f = 0; f < (int)bufferSize; f++) 
    {
        writeBuffer[f] = (unsigned char)f;
    }
    
    printf("\nWriting %d bytes to JTAG loopback...\n", 
           (int)bytesToWrite);

    ftStatus = FT_Write(ftHandle, 
                        writeBuffer,
                        bytesToWrite, 
                        &bytesWritten);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_Write returned %d\n", (int)ftStatus);
        goto exit;
    }
    
    if (bytesWritten != bytesToWrite)
    {
        printf("Failure.  FT_Write wrote %d bytes instead of %d.\n",
               (int)bytesWritten,
               (int)bytesToWrite);
        goto exit;
    }

    printf("%d bytes written.\n", (int)bytesWritten);
    
    // Keep checking queue until D2XX has received all the bytes we wrote.
    journeyDuration = 1;  // One second should be enough
    
    gettimeofday(&startTime, NULL);
    
    for (bytesReceived = 0, queueChecks = 0; 
         bytesReceived < bytesWritten; 
         queueChecks++)
    {
        // Periodically check for time-out 
        if (queueChecks % 512 == 0)
        {
            struct timeval now;
            struct timeval elapsed;
            
            gettimeofday(&now, NULL);
            timersub(&now, &startTime, &elapsed);

            if (elapsed.tv_sec > (long int)journeyDuration)
            {
                // We've waited too long.  Give up.
                printf("\nTimed out after %ld seconds\n", elapsed.tv_sec);
                break;
            }
            
            // Display number of bytes D2XX has received
            printf("%s%d", 
                   queueChecks == 0 ? "Number of bytes in D2XX receive-queue: " : ", ",
                   (int)bytesReceived);
        }

        ftStatus = FT_GetQueueStatus(ftHandle, &bytesReceived);
        if (ftStatus != FT_OK)
        {
            printf("\nFailure.  FT_GetQueueStatus returned %d.\n",
                   (int)ftStatus);
            goto exit;
        }
    }

    printf("\nGot %d (of %d) bytes.\n", (int)bytesReceived, (int)bytesWritten);

    // Even if D2XX has the wrong number of bytes, create our
    // own buffer so we can read and display them.
    readBuffer = (unsigned char *)calloc(bytesReceived, sizeof(unsigned char));
    if (readBuffer == NULL)
    {
        printf("Failed to allocate %d bytes.\n", (int)bytesReceived);
        goto exit;
    }

    // Then copy D2XX's buffer to ours.
    ftStatus = FT_Read(ftHandle, readBuffer, bytesReceived, &bytesRead);
    if (ftStatus != FT_OK)
    {
        printf("Failure.  FT_Read returned %d.\n", (int)ftStatus);
        goto exit;
    }

    if (bytesRead != bytesReceived)
    {
        printf("Failure.  FT_Read only read %d (of %d) bytes.\n",
               (int)bytesRead,
               (int)bytesReceived);
        goto exit;
    }
    
    if (0 != memcmp(writeBuffer, readBuffer, bytesRead))
    {
        printf("Failure.  Read-buffer does not match write-buffer.\n");
        printf("Write buffer:\n");
        dumpBuffer(writeBuffer, bytesReceived);
        printf("Read buffer:\n");
        dumpBuffer(readBuffer, bytesReceived);
        goto exit;
    }

    printf("Received bytes match transmitted bytes.\n");

    // Check that queue hasn't gathered any additional unexpected bytes
    bytesReceived = 4242; // deliberately junk
    ftStatus = FT_GetQueueStatus(ftHandle, &bytesReceived);
    if (ftStatus != FT_OK)
    {
        printf("Failure.  FT_GetQueueStatus returned %d.\n",
               (int)ftStatus);
        goto exit;
    }

    if (bytesReceived != 0)
    {
        printf("Failure.  %d bytes in input queue -- expected none.\n",
               (int)bytesReceived);
        goto exit;
    }

    // Success
    printf("\nTest PASSED.\n");
    retCode = 0;

exit:
    free(readBuffer);
    free(writeBuffer);

    if (ftHandle != NULL)
    {
        (void)FT_SetBitMode(ftHandle, 0x00, FT_BITMODE_RESET);
        FT_Close(ftHandle);
    }

    return retCode;
}
