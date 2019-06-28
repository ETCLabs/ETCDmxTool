/* Linux notes
 *
 * Gadget support is not yet avaliable on linux
 * so pre-determined values are returned by accompanying source file
 * to ensure code compatibility until such a time when full support is added
 */
#include "GadgetDLL.h"

#if defined(Q_OS_LINUX)

static char GadgetVersion[] = "0.0.0";
static char GadgetType[] = "Fake Device";

/***************** Information about the DLL *****************/
/* Returns the version of the DLL being used */
char * Gadget2_GetDllVersion()
{
    return GadgetVersion;
}

/***************** Startup and Shutdown *****************/

/* Start up the Gadget2 interface. This should be called before any other functions are used */
GADGET_DLL_API int Gadget2_Connect()
{
    return 0;
}

/* Shutdown the Gadget2 interface. Stops the threads; do not use other functions after calling this */
GADGET_DLL_API int Gadget2_Disconnect(void)
{
    return 0;
};


/***************** Logging *****************/
/* Set the callback for log data */
GADGET_DLL_API void Gadget2_SetLogCallback(Gadget2_LogCallback *Callback)
{
    Q_UNUSED(Callback);
    return;
}

/* Set the verbosity of log messages */
GADGET_DLL_API void Gadget2_SetLogFilter(int verb, int cat, int sev)
{
    Q_UNUSED(verb);
    Q_UNUSED(cat);
    Q_UNUSED(sev);
}

/***************** DMX Transmission *****************/
/* Send DMX */
GADGET_DLL_API void Gadget2_SendDMX(unsigned int DeviceNum, unsigned int PortNum, unsigned char * Buffer, unsigned int Size)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    Q_UNUSED(Buffer);
    Q_UNUSED(Size);
}

/* Disable DMX */
GADGET_DLL_API void Gadget2_DisableDMX(unsigned int DeviceNum, unsigned int PortNum)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
}

/***************** Information and control of the Gadget *****************/
/* Get the number of Gadget devices found on the PC */
GADGET_DLL_API unsigned int Gadget2_GetNumGadgetDevices()
{
    return 0;
}

/* Return the version number of the specified device as a string */
GADGET_DLL_API unsigned char * Gadget2_GetGadgetVersion(unsigned int DeviceNum)
{
    Q_UNUSED(DeviceNum);
    return reinterpret_cast<unsigned char *>(GadgetVersion);
}

/* Return the serial number of the specified device as a string */
GADGET_DLL_API unsigned int Gadget2_GetGadgetSerialNumber(unsigned int DeviceNum)
{
    Q_UNUSED(DeviceNum);
    return 0;
}

/* Return the type of the specified device as a string */
GADGET_DLL_API const char * Gadget2_GetGadgetType(unsigned int DeviceNum)
{
    Q_UNUSED(DeviceNum);
    return GadgetType;
}

/* Return the number of DMX ports the specified device has */
GADGET_DLL_API unsigned char Gadget2_GetPortCount(unsigned int DeviceNum)
{
    Q_UNUSED(DeviceNum);
    return 0;
}

/* Set the callback for status updates on the firmware update of Gadget2. Context is passed back with the callback */
GADGET_DLL_API void Gadget2_SetUpdateStatusCallback(Gadget2_UpdateStatusCallback *Callback, void* Context)
{
    Q_UNUSED(Callback);
    Q_UNUSED(Context);
}

/* Perform an update of the Gadget2. FirmwarePath should point to the S-Record path for the Gadget Firmware */
GADGET_DLL_API void Gadget2_PerformFirmwareUpdate(unsigned int DeviceNum, const wchar_t *FirmwarePath)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(FirmwarePath);
}

/***************** Raw access mode *****************/
/* Set the device into Raw Access mode*/
GADGET_DLL_API int Gadget2_SetRawReceiveMode(unsigned int DeviceNum, unsigned int PortNum)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    return 0;
}

/* Return the number of bytes in the raw acess receive queue */
GADGET_DLL_API unsigned int Gadget2_GetNumberOfRXRawBytes(unsigned int DeviceNum, unsigned int PortNum)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    return 0;
}

/* Get raw bytes from the raw access queue. Raw bytes are copied into the provided Data pointer, so it must be of size Length and must have enough space for the data*/
GADGET_DLL_API void Gadget2_GetRXRawBytes(unsigned int DeviceNum, unsigned int PortNum, unsigned short * Data, unsigned int Length)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    Q_UNUSED(Data);
    Q_UNUSED(Length);
}

/* Send raw bytes via the Gadget2 */
GADGET_DLL_API void Gadget2_SendRawBytes(unsigned int DeviceNum, unsigned int PortNum, unsigned char * Data, unsigned int Length)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    Q_UNUSED(Data);
    Q_UNUSED(Length);
}

/* Send a break, startcode and data */
GADGET_DLL_API void Gadget2_SendBreakAndData(unsigned int DeviceNum, unsigned int PortNum, unsigned char StartCode, unsigned char * Data, unsigned int Length)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    Q_UNUSED(StartCode);
    Q_UNUSED(Data);
    Q_UNUSED(Length);
}

/***************** RDM Interface Functions *****************/

/* Do full RDM discovery on the specified device and port */
GADGET_DLL_API void Gadget2_DoFullDiscovery(unsigned int DeviceID, unsigned int PortNum)
{
    Q_UNUSED(DeviceID);
    Q_UNUSED(PortNum);
    return;
}

/* Turn RDM on or off. Turning it on enables background discovery, and RDM on the port*/
GADGET_DLL_API void Gadget2_SetRDMEnabled(unsigned int DeviceNum, unsigned int PortNum, unsigned char Enable)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    Q_UNUSED(Enable);
}

/* Return the number of discovered devices */
GADGET_DLL_API unsigned int Gadget2_GetDiscoveredDevices(void)
{
    return 0;
}

/* Get the RDMDeviceInfo structre from the device at Index in the array */
GADGET_DLL_API RdmDeviceInfo * Gadget2_GetDeviceInfo(unsigned int Index)
{
    Q_UNUSED(Index);
    return Q_NULLPTR;
}

/* Get the RDM Manufacturer ID for the device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceManfID(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM Device ID for the device at Index in the array*/
GADGET_DLL_API unsigned int Gadget2_GetDeviceID(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the software version label for the RDM device at Index in the array */
GADGET_DLL_API unsigned char * Gadget2_GetDeviceSoftwareVersionLabel(unsigned int Index)
{
    Q_UNUSED(Index);
    return reinterpret_cast<unsigned char *>(GadgetVersion);
}

/* Return the RDM Protocol version for the RDM device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceRDMProtocolVersion(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM Model ID for the RDM device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceModelID(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM Product Category for the RDM device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceProductCategoryType(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM Software version ID for the RDM device at Index in the array */
GADGET_DLL_API unsigned int Gadget2_GetDeviceSoftwareVersionID(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM DMX footprint for the RDM device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXFootprint(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the (integer) DMX personality for the RDM device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXPersonality(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the DMX start address for the RDM device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXStartAddress(unsigned int Index){
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM Subdevice count for the RDM device at Index in the array */
GADGET_DLL_API unsigned short Gadget2_GetDeviceSubdeviceCount(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM sensor count for the RDM device at Index in the array */
GADGET_DLL_API unsigned char Gadget2_GetDeviceSensorCount(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the number of responses in the RDM command queue */
GADGET_DLL_API unsigned int Gadget2_GetNumResponses(void)
{
    return 0;
}

/* Return the RDM response at Index in the RDM command queue */
GADGET_DLL_API RDM_CmdC * Gadget2_GetResponse(unsigned int Index)
{
    Q_UNUSED(Index);
    return Q_NULLPTR;
}

/* Removes the RDM response at Index in the RDM command queue */
GADGET_DLL_API void Gadget2_ClearResponse(unsigned int Index)
{
    Q_UNUSED(Index);
}

/* Return the RDM command parameter at Index in the RDM command queue */
GADGET_DLL_API unsigned char Gadget2_GetResponseCommand(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM response parameter at Index in the RDM command queue */
GADGET_DLL_API unsigned short Gadget2_GetResponseParameter(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM response subdevice parameter at Index in the RDM command queue */
GADGET_DLL_API unsigned short Gadget2_GetResponseSubdevice(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM response length for the response at Index in the RDM command queue */
GADGET_DLL_API unsigned char Gadget2_GetResponseLength(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return a pointer to the response buffer for the response at Index in the RDM command queue */
GADGET_DLL_API unsigned char * Gadget2_GetResponseBuffer(unsigned int Index)
{
    Q_UNUSED(Index);
    return Q_NULLPTR;
}

/* Return the RDM response type for the response at Index in the RDM command queue */
GADGET_DLL_API unsigned char Gadget2_GetResponseResponseType(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM response manufacturer ID for the response at Index in the RDM command queue */
GADGET_DLL_API unsigned short Gadget2_GetResponseManufacturer_id(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Return the RDM response device ID for the response at Index in the RDM command queue */
GADGET_DLL_API unsigned int Gadget2_GetResponseDevice_id(unsigned int Index)
{
    Q_UNUSED(Index);
    return 0;
}

/* Send an RDM command */
GADGET_DLL_API void Gadget2_SendRDMCommand(unsigned int DeviceNum, unsigned int PortNum, unsigned char Cmd, unsigned short ParameterID, unsigned short SubDevice, unsigned char DataLen, const char * Buffer, unsigned short ManfID, unsigned int DevID)
{
    Q_UNUSED(DeviceNum);
    Q_UNUSED(PortNum);
    Q_UNUSED(Cmd);
    Q_UNUSED(ParameterID);
    Q_UNUSED(SubDevice);
    Q_UNUSED(DataLen);
    Q_UNUSED(Buffer);
    Q_UNUSED(ManfID);
    Q_UNUSED(DevID);
}

#endif
