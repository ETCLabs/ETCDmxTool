/*
 * Writes a known sequence of bytes then expects to read them back.
 * Run this with a loopback device fitted to one of FTDI's USB-RS232 
 * converter cables.
 * A loopback device has:
 *   1.  Receive Data    connected to    Transmit Data
 *   2.  Data Set Ready  connected to    Data Terminal Ready
 *   3.  Ready To Send   connected to    Clear To Send
 *
 * Build with:
 *     gcc main.c -o loopback -Wall -Wextra 
 *         -lftd2xx -lpthread -lrt 
 *         -Wl,-rpath /usr/local/lib
 * 
 * Run with:
 *     sudo ./loopback
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../ftd2xx.h"



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



int main(int argc, char *argv[])
{
    int             retCode = -1; // Assume failure
    int             f = 0;
    DWORD           driverVersion = 0;
    FT_STATUS       ftStatus = FT_OK;
    FT_HANDLE       ftHandle = NULL;
    int             portNum = 0; // First device found
    size_t          bufferSize = 64 * 1024;
    DWORD           bytesToWrite;
    DWORD           bytesWritten = 0;
    DWORD           bytesReceived = 0;
    DWORD           bytesRead = 0;
    struct timeval  startTime;
    int             journeyDuration;
    unsigned char  *writeBuffer = NULL;
    unsigned char  *readBuffer = NULL;
    int             queueChecks = 0;
    ULONG           rates[] = {300, 600, 1200, 2400, 4800, 9600,
                               19200, 38400, 57600, 115200, 
                               230400, 460800, 576000, 921600,
                               1500000, 2000000, 3000000};
                    // TODO: detect high-speed device and use 8240000

    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);
    
    // Make printfs immediate (no buffer)
    setvbuf(stdout, NULL, _IONBF, 0);

    writeBuffer = (unsigned char *)malloc((size_t)bufferSize);
    if (writeBuffer == NULL)
        goto exit;

    // Fill write buffer with consecutive values
    for (f = 0; f < (int)bufferSize; f++) 
    {
        writeBuffer[f] = (unsigned char)f + 64;
    }
    
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

    printf("Using D2XX version %08x\n", driverVersion);

	ftStatus = FT_ResetDevice(ftHandle);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_ResetDevice returned %d.\n", (int)ftStatus);
        goto exit;
    }
    
    // Flow control is needed for higher baud rates
    ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0, 0);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_SetFlowControl returned %d.\n", (int)ftStatus);
        goto exit;
    }

    ftStatus = FT_SetDataCharacteristics(ftHandle, 
                                         FT_BITS_8,
                                         FT_STOP_BITS_1,
                                         FT_PARITY_NONE);
    if (ftStatus != FT_OK) 
    {
        printf("Failure.  FT_SetDataCharacteristics returned %d.\n",
               (int)ftStatus);
        goto exit;
    }
    
    for (f = 0; f < (int)ARRAY_SIZE(rates); f++)
    {
        ftStatus = FT_SetBaudRate(ftHandle, rates[f]);
        if (ftStatus != FT_OK) 
        {
            printf("Failure.  FT_SetBaudRate(%d) returned %d.\n", 
                   (int)rates[f],
                   (int)ftStatus);
            goto exit;
        }
        
        // Assert Request-To-Send to prepare receiver
        ftStatus = FT_SetRts(ftHandle);
        if (ftStatus != FT_OK) 
        {
            printf("Failure.  FT_SetRts returned %d.\n", (int)ftStatus);
            goto exit;
        }

        if (rates[f] < 57600)
        {
            // Keep test duration reasonable by transferring fewer 
            // bytes at low baud rates.
            bytesToWrite = rates[f] / 4;
        }
        else
        {
            bytesToWrite = bufferSize;
        }

        printf("\nBaud rate %d.  Writing %d bytes to loopback device...\n", 
               (int)rates[f],
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
        // Estimate total time to write and read, so we can time-out.
        // Each byte has 8 data bits plus a stop bit and perhaps a 1-bit gap.
        journeyDuration = bytesWritten * (8 + 1 + 1) / (int)rates[f];
        journeyDuration += 1;  // Round up
        journeyDuration *= 2;  // It's a return journey
        printf("Estimate %d seconds remain.\n", journeyDuration);
        
        gettimeofday(&startTime, NULL);
        
        for (bytesReceived = 0, queueChecks = 0; 
             bytesReceived < bytesWritten; 
             queueChecks++)
        {
            // Periodically check for time-out 
            if (queueChecks % 32 == 0)
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
        free(readBuffer); // Free previous iteration's buffer.
		readBuffer = (unsigned char *)calloc(bytesReceived, sizeof(unsigned char));
        if (readBuffer == NULL)
        {
            printf("Failed to allocate %d bytes.\n", bytesReceived);
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

        // Fail if D2XX's queue lacked (or had surplus) bytes.
        if (bytesReceived != bytesWritten)
        {
            printf("Failure.  D2XX received %d bytes but we expected %d.\n",
                   (int)bytesReceived,
                   (int)bytesWritten);
            dumpBuffer(readBuffer, bytesReceived);
            goto exit;
        }

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
    }

    // Success
    printf("\nTest PASSED.\n");
    retCode = 0;

exit:
    free(readBuffer);
    free(writeBuffer);

    if (ftHandle != NULL)
        FT_Close(ftHandle);

    return retCode;
}
