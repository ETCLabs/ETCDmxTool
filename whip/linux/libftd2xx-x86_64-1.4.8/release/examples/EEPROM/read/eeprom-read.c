/*
 * FT_EEPROM_Read demonstration.
 * Dumps the EEPROM fields of any connected FTDI device.
 *
 * Copy libftd2xx.a, ftd2xx.h, WinTypes.h into current directory.
 *
 * Compile:
 *     cc eeprom-read.c -L. -lftd2xx -lpthread
 *
 * On Mac, the above line needs these extra dependencies:
 *     -lobjc -framework IOKit -framework CoreFoundation
 *
 * Run:
 *     sudo ./a.out
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ftd2xx.h"



typedef union Eeprom_Generic
{
    FT_EEPROM_232B     b;
    FT_EEPROM_232R     r;
    FT_EEPROM_232H     singleH;
    FT_EEPROM_2232     dual;
    FT_EEPROM_2232H    dualH;
    FT_EEPROM_4232H    quadH;
    FT_EEPROM_X_SERIES x;
}
Eeprom_Generic;



static const char *deviceName(FT_DEVICE deviceType)
{
    switch(deviceType)
    {
        default:
            return "Unknown";

        case FT_DEVICE_BM:
            return "232 B";

        case FT_DEVICE_2232C:
            return "Dual 232";

        case FT_DEVICE_232R:
            return "232 R";

        case FT_DEVICE_2232H:
            return "Dual Hi-Speed 232";

        case FT_DEVICE_4232H:
            return "Quad Hi-Speed 232";

        case FT_DEVICE_232H:
            return "Hi-Speed 232";

        case FT_DEVICE_X_SERIES:
            return "X Series";
    }
}



static void displayHeader (FT_EEPROM_HEADER *header)
{
    printf("deviceType: %s\n", deviceName(header->deviceType));

    printf("VendorId: %04x\n", header->VendorId);
    printf("ProductId: %04x\n", header->ProductId);
    printf("SerNumEnable: %02x\n", header->SerNumEnable);

    printf("MaxPower: %04x\n", header->MaxPower);
    printf("SelfPowered: %02x\n", header->SelfPowered);
    printf("RemoteWakeup: %02x\n", header->RemoteWakeup);

    printf("PullDownEnable: %02x\n", header->PullDownEnable);
}



static void displayDual (FT_EEPROM_2232 *dual)
{
    printf("AIsHighCurrent: %02x\n", dual->AIsHighCurrent);
    printf("BIsHighCurrent: %02x\n", dual->BIsHighCurrent);

    printf("AIsFifo: %02x\n", dual->AIsFifo);
    printf("AIsFifoTar: %02x\n", dual->AIsFifoTar);
    printf("AIsFastSer: %02x\n", dual->AIsFastSer);
    printf("BIsFifo: %02x\n", dual->BIsFifo);
    printf("BIsFifoTar: %02x\n", dual->BIsFifoTar);
    printf("BIsFastSer: %02x\n", dual->BIsFastSer);

    printf("ADriverType: %02x\n", dual->ADriverType);
    printf("BDriverType: %02x\n", dual->BDriverType);
}



static void displayR (FT_EEPROM_232R *r)
{
    printf("IsHighCurrent: %02x\n", r->IsHighCurrent);

    printf("UseExtOsc: %02x\n", r->UseExtOsc);
    printf("InvertTXD: %02x\n", r->InvertTXD);
    printf("InvertRXD: %02x\n", r->InvertRXD);
    printf("InvertRTS: %02x\n", r->InvertRTS);
    printf("InvertCTS: %02x\n", r->InvertCTS);
    printf("InvertDTR: %02x\n", r->InvertDTR);
    printf("InvertDSR: %02x\n", r->InvertDSR);
    printf("InvertDCD: %02x\n", r->InvertDCD);
    printf("InvertRI: %02x\n", r->InvertRI);
    printf("Cbus0: %02x\n", r->Cbus0);
    printf("Cbus1: %02x\n", r->Cbus1);
    printf("Cbus2: %02x\n", r->Cbus2);
    printf("Cbus3: %02x\n", r->Cbus3);
    printf("Cbus4: %02x\n", r->Cbus4);

    printf("DriverType: %02x\n", r->DriverType);
}



static void displayDualH (FT_EEPROM_2232H *dualH)
{
    printf("ALSlowSlew: %02x\n", dualH->ALSlowSlew);
    printf("ALSchmittInput: %02x\n", dualH->ALSchmittInput);
    printf("ALDriveCurrent: %02x\n", dualH->ALDriveCurrent);
    printf("AHSlowSlew: %02x\n", dualH->AHSlowSlew);
    printf("AHSchmittInput: %02x\n", dualH->AHSchmittInput);
    printf("AHDriveCurrent: %02x\n", dualH->AHDriveCurrent);
    printf("BLSlowSlew: %02x\n", dualH->BLSlowSlew);
    printf("BLSchmittInput: %02x\n", dualH->BLSchmittInput);
    printf("BLDriveCurrent: %02x\n", dualH->BLDriveCurrent);
    printf("BHSlowSlew: %02x\n", dualH->BHSlowSlew);
    printf("BHSchmittInput: %02x\n", dualH->BHSchmittInput);
    printf("BHDriveCurrent: %02x\n", dualH->BHDriveCurrent);

    printf("AIsFifo: %02x\n", dualH->AIsFifo);
    printf("AIsFifoTar: %02x\n", dualH->AIsFifoTar);
    printf("AIsFastSer: %02x\n", dualH->AIsFastSer);
    printf("BIsFifo: %02x\n", dualH->BIsFifo);
    printf("BIsFifoTar: %02x\n", dualH->BIsFifoTar);
    printf("BIsFastSer: %02x\n", dualH->BIsFastSer);
    printf("PowerSaveEnable: %02x\n", dualH->PowerSaveEnable);

    printf("ADriverType: %02x\n", dualH->ADriverType);
    printf("BDriverType: %02x\n", dualH->BDriverType);
}



static void displayQuadH (FT_EEPROM_4232H *quadH)
{
    printf("ASlowSlew: %02x\n", quadH->ASlowSlew);
    printf("ASchmittInput: %02x\n", quadH->ASchmittInput);
    printf("ADriveCurrent: %02x\n", quadH->ADriveCurrent);
    printf("BSlowSlew: %02x\n", quadH->BSlowSlew);
    printf("BSchmittInput: %02x\n", quadH->BSchmittInput);
    printf("BDriveCurrent: %02x\n", quadH->BDriveCurrent);
    printf("CSlowSlew: %02x\n", quadH->CSlowSlew);
    printf("CSchmittInput: %02x\n", quadH->CSchmittInput);
    printf("CDriveCurrent: %02x\n", quadH->CDriveCurrent);
    printf("DSlowSlew: %02x\n", quadH->DSlowSlew);
    printf("DSchmittInput: %02x\n", quadH->DSchmittInput);
    printf("DDriveCurrent: %02x\n", quadH->DDriveCurrent);

    printf("ARIIsTXDEN: %02x\n", quadH->ARIIsTXDEN);
    printf("BRIIsTXDEN: %02x\n", quadH->BRIIsTXDEN);
    printf("CRIIsTXDEN: %02x\n", quadH->CRIIsTXDEN);
    printf("DRIIsTXDEN: %02x\n", quadH->DRIIsTXDEN);

    printf("ADriverType: %02x\n", quadH->ADriverType);
    printf("BDriverType: %02x\n", quadH->BDriverType);
    printf("CDriverType: %02x\n", quadH->CDriverType);
    printf("DDriverType: %02x\n", quadH->DDriverType);
}



static void displaySingleH (FT_EEPROM_232H *singleH)
{
    printf("ACSlowSlew: %02x\n", singleH->ACSlowSlew);
    printf("ACSchmittInput: %02x\n", singleH->ACSchmittInput);
    printf("ACDriveCurrent: %02x\n", singleH->ACDriveCurrent);
    printf("ADSlowSlew: %02x\n", singleH->ADSlowSlew);
    printf("ADSchmittInput: %02x\n", singleH->ADSchmittInput);
    printf("ADDriveCurrent: %02x\n", singleH->ADDriveCurrent);

    printf("Cbus0: %02x\n", singleH->Cbus0);
    printf("Cbus1: %02x\n", singleH->Cbus1);
    printf("Cbus2: %02x\n", singleH->Cbus2);
    printf("Cbus3: %02x\n", singleH->Cbus3);
    printf("Cbus4: %02x\n", singleH->Cbus4);
    printf("Cbus5: %02x\n", singleH->Cbus5);
    printf("Cbus6: %02x\n", singleH->Cbus6);
    printf("Cbus7: %02x\n", singleH->Cbus7);
    printf("Cbus8: %02x\n", singleH->Cbus8);
    printf("Cbus9: %02x\n", singleH->Cbus9);

    printf("FT1248Cpol: %02x\n", singleH->FT1248Cpol);
    printf("FT1248Lsb: %02x\n", singleH->FT1248Lsb);
    printf("FT1248FlowControl: %02x\n", singleH->FT1248FlowControl);

    printf("IsFifo: %02x\n", singleH->IsFifo);
    printf("IsFifoTar: %02x\n", singleH->IsFifoTar);
    printf("IsFastSer: %02x\n", singleH->IsFastSer);
    printf("IsFT1248    : %02x\n", singleH->IsFT1248    );
    printf("PowerSaveEnable: %02x\n", singleH->PowerSaveEnable);

    printf("DriverType: %02x\n", singleH->DriverType);
}



static void displayX (FT_EEPROM_X_SERIES *x)
{
    printf("ACSlowSlew: %02x\n", x->ACSlowSlew);
    printf("ACSchmittInput: %02x\n", x->ACSchmittInput);
    printf("ACDriveCurrent: %02x\n", x->ACDriveCurrent);
    printf("ADSlowSlew: %02x\n", x->ADSlowSlew);
    printf("ADSchmittInput: %02x\n", x->ADSchmittInput);
    printf("ADDriveCurrent: %02x\n", x->ADDriveCurrent);

    printf("Cbus0: %02x\n", x->Cbus0);
    printf("Cbus1: %02x\n", x->Cbus1);
    printf("Cbus2: %02x\n", x->Cbus2);
    printf("Cbus3: %02x\n", x->Cbus3);
    printf("Cbus4: %02x\n", x->Cbus4);
    printf("Cbus5: %02x\n", x->Cbus5);
    printf("Cbus6: %02x\n", x->Cbus6);

    printf("InvertTXD: %02x\n", x->InvertTXD);
    printf("InvertRXD: %02x\n", x->InvertRXD);
    printf("InvertRTS: %02x\n", x->InvertRTS);
    printf("InvertCTS: %02x\n", x->InvertCTS);
    printf("InvertDTR: %02x\n", x->InvertDTR);
    printf("InvertDSR: %02x\n", x->InvertDSR);
    printf("InvertDCD: %02x\n", x->InvertDCD);
    printf("InvertRI: %02x\n", x->InvertRI);

    printf("BCDEnable: %02x\n", x->BCDEnable);
    printf("BCDForceCbusPWREN: %02x\n", x->BCDForceCbusPWREN);
    printf("BCDDisableSleep: %02x\n", x->BCDDisableSleep);

    printf("I2CSlaveAddress: %04x\n", x->I2CSlaveAddress);
    printf("I2CDeviceId: %04x\n", x->I2CDeviceId);
    printf("I2CDisableSchmitt: %02x\n", x->I2CDisableSchmitt);

    printf("FT1248Cpol: %02x\n", x->FT1248Cpol);
    printf("FT1248Lsb: %02x\n", x->FT1248Lsb);
    printf("FT1248FlowControl: %02x\n", x->FT1248FlowControl);

    printf("RS485EchoSuppress: %02x\n", x->RS485EchoSuppress);
    printf("PowerSaveEnable: %02x\n", x->PowerSaveEnable);

    printf("DriverType: %02x\n", x->DriverType);
}



static int readEeprom (DWORD locationId, FT_DEVICE deviceType)
{
    int                  success = 0;
    FT_STATUS            ftStatus;
    FT_HANDLE            ftHandle = (FT_HANDLE)NULL;
    char                 manufacturer[64];
    char                 manufacturerId[64];
    char                 description[64];
    char                 serialNumber[64];
    Eeprom_Generic      *eeprom = NULL;
    FT_EEPROM_HEADER    *header;

    ftStatus = FT_OpenEx((PVOID)(uintptr_t)locationId,
                         FT_OPEN_BY_LOCATION,
                         &ftHandle);
    if (ftStatus != FT_OK)
    {
        printf("FT_OpenEx failed (error code %d)\n", (int)ftStatus);
        goto exit;
    }

    /* Allocate enough to hold biggest EEPROM structure */
    eeprom = calloc(1, sizeof(*eeprom));
    if (eeprom == NULL)
    {
        printf("Allocation failure.\n");
        goto exit;
    }

    /* EEPROM_HEADER is first member of every type of eeprom */
    header = (FT_EEPROM_HEADER *)eeprom;
    header->deviceType = deviceType;

    manufacturer[0] = '\0';
    manufacturerId[0] = '\0';
    description[0] = '\0';
    serialNumber[0] = '\0';

    ftStatus = FT_EEPROM_Read(ftHandle,
                              eeprom,
                              sizeof(*eeprom),
                              manufacturer,
                              manufacturerId,
                              description,
                              serialNumber);
    if (ftStatus != FT_OK)
    {
        printf("FT_EEPROM_Read failed (error code %d)\n", (int)ftStatus);
        goto exit;
    }

    printf("Manufacturer = %s\n", manufacturer);
    printf("ManufacturerId = %s\n", manufacturerId);
    printf("Description = %s\n", description);
    printf("Serial number = %s\n", serialNumber);

    displayHeader((FT_EEPROM_HEADER *)eeprom);

    switch(deviceType)
    {
        default:
        case FT_DEVICE_BM:
            // No further info to display
            break;

        case FT_DEVICE_2232C:
            displayDual(&eeprom->dual);
            break;

        case FT_DEVICE_232R:
            displayR(&eeprom->r);
            break;

        case FT_DEVICE_2232H:
            displayDualH(&eeprom->dualH);
            break;

        case FT_DEVICE_4232H:
            displayQuadH(&eeprom->quadH);
            break;

        case FT_DEVICE_232H:
            displaySingleH(&eeprom->singleH);
            break;

        case FT_DEVICE_X_SERIES:
            displayX(&eeprom->x);
            break;
    }

    printf("\n");

    // Success
    success = 1;

exit:
    (void)FT_Close(ftHandle);
    free(eeprom);
    return success;
}



int main (int argc, char *argv[])
{
    FT_STATUS                 ftStatus;
    FT_DEVICE_LIST_INFO_NODE *devInfo = NULL;
    DWORD                     numDevs = 0;
    int                       i;
    int                       retCode = 0;

    /* Unused parameters */
    (void)argc;
    (void)argv;

    /* Discover how many FTDI devices are connected */
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK)
    {
        printf("FT_CreateDeviceInfoList failed (error code %d)\n",
               (int)ftStatus);
        goto exit;
    }

    if (numDevs == 0)
    {
        printf("No devices connected.\n");
        goto exit;
    }

    /* Allocate storage */
    devInfo = calloc((size_t)numDevs,
                     sizeof(FT_DEVICE_LIST_INFO_NODE));
    if (devInfo == NULL)
    {
        printf("Allocation failure.\n");
        goto exit;
    }

    /* Populate the list of info nodes */
    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
    if (ftStatus != FT_OK)
    {
        printf("FT_GetDeviceInfoList failed (error code %d)\n",
               (int)ftStatus);
        goto exit;
    }

    /* Display info (including EEPROM fields) for each connected FTDI device */
    for (i = 0; i < (int)numDevs; i++)
    {
        printf("Device %d:\n",i);
        printf("  Flags = 0x%x\n",devInfo[i].Flags);
        printf("  Type = 0x%x\n",devInfo[i].Type);
        printf("  ID = 0x%04x\n",devInfo[i].ID);
        printf("  LocId = 0x%x\n",devInfo[i].LocId);
        printf("  SerialNumber = %s\n",devInfo[i].SerialNumber);
        printf("  Description = %s\n",devInfo[i].Description);
        printf("  ftHandle = %p\n",devInfo[i].ftHandle);
        if (!readEeprom(devInfo[i].LocId, devInfo[i].Type))
        {
            goto exit;
        }
    }

exit:
    free(devInfo);
    return retCode;
}
