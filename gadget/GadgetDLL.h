#ifndef __GadgetExport_h
#define __GadgetExport_h

#ifdef GADGET_DLL_EXPORT
#define GADGET_DLL_API   __declspec( dllexport )
#else
#define GADGET_DLL_API   __declspec( dllimport )
#endif

class RDM_CmdC;
struct RdmDeviceInfo;

#define GADGET_DLL_DMX_BREAK 0x8000
#define GADGET_DLL_FRAMING_ERROR 0x9000
typedef void(__stdcall Gadget2_LogCallback)(const char *LogData);

extern "C"
{
    GADGET_DLL_API char * Gadget2_GetDllVersion();

    GADGET_DLL_API int Gadget2_Connect();

    GADGET_DLL_API int Gadget2_Disconnect(void);

	/***************** Logging *****************/
	/* Set the callback for log data */
	GADGET_DLL_API void Gadget2_SetLogCallback(Gadget2_LogCallback *Callback);

	/* Set the verbosity of log messages */
	GADGET_DLL_API void Gadget2_SetLogFilter(int verb, int cat, int sev);

	/***************** DMX Transmission *****************/
	/* Send DMX */
	GADGET_DLL_API void Gadget2_SendDMX(unsigned int DeviceNum, unsigned int PortNum, unsigned char * Buffer, unsigned int Size);

	/* Disable DMX */
	GADGET_DLL_API void Gadget2_DisableDMX(unsigned int DeviceNum, unsigned int PortNum);

	/***************** Information and control of the Gadget *****************/
	/* Get the number of Gadget devices found on the PC */
	GADGET_DLL_API unsigned int Gadget2_GetNumGadgetDevices();

	/* Return the version number of the specified device as a string */
	GADGET_DLL_API unsigned char * Gadget2_GetGadgetVersion(unsigned int DeviceNum);

	/* Return the serial number of the specified device as a string */
	GADGET_DLL_API unsigned int Gadget2_GetGadgetSerialNumber(unsigned int DeviceNum);

	/* Return the type of the specified device as a string */
	GADGET_DLL_API const char * Gadget2_GetGadgetType(unsigned int DeviceNum);

	/* Return the number of DMX ports the specified device has */
	GADGET_DLL_API unsigned char Gadget2_GetPortCount(unsigned int DeviceNum);

	/* Set the callback for status updates on the firmware update of Gadget2. Context is passed back with the callback */
	GADGET_DLL_API void Gadget2_SetUpdateStatusCallback(Gadget2_UpdateStatusCallback *Callback, void* Context);

	/* Perform an update of the Gadget2. FirmwarePath should point to the S-Record path for the Gadget Firmware */
	GADGET_DLL_API void Gadget2_PerformFirmwareUpdate(unsigned int DeviceNum, const wchar_t *FirmwarePath);

	/***************** Raw access mode *****************/
	/* Set the device into Raw Access mode*/
	GADGET_DLL_API int Gadget2_SetRawReceiveMode(unsigned int DeviceNum, unsigned int PortNum);

	/* Return the number of bytes in the raw acess receive queue */
	GADGET_DLL_API unsigned int Gadget2_GetNumberOfRXRawBytes(unsigned int DeviceNum, unsigned int PortNum);

	/* Get raw bytes from the raw access queue. Raw bytes are copied into the provided Data pointer, so it must be of size Length and must have enough space for the data*/
	GADGET_DLL_API void Gadget2_GetRXRawBytes(unsigned int DeviceNum, unsigned int PortNum, unsigned short * Data, unsigned int Length);

	/* Send raw bytes via the Gadget2 */
	GADGET_DLL_API void Gadget2_SendRawBytes(unsigned int DeviceNum, unsigned int PortNum, unsigned char * Data, unsigned int Length);

	/* Send a break, startcode and data */
	GADGET_DLL_API void Gadget2_SendBreakAndData(unsigned int DeviceNum, unsigned int PortNum, unsigned char StartCode, unsigned char * Data, unsigned int Length);

	/***************** RDM Interface Functions *****************/

	/* Do full RDM discovery on the specified device and port */
    GADGET_DLL_API void Gadget2_DoFullDiscovery(unsigned int DeviceID, unsigned int PortNum);

	/* Turn RDM on or off. Turning it on enables background discovery, and RDM on the port*/
	GADGET_DLL_API void Gadget2_SetRDMEnabled(unsigned int DeviceNum, unsigned int PortNum, unsigned char Enable);

	/* Return the number of discovered devices */
    GADGET_DLL_API unsigned int Gadget2_GetDiscoveredDevices(void);

    GADGET_DLL_API RdmDeviceInfo * Gadget2_GetDeviceInfo(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceManfID(unsigned int Index);

    GADGET_DLL_API unsigned int Gadget2_GetDeviceID(unsigned int Index);

    GADGET_DLL_API unsigned char * Gadget2_GetDeviceSoftwareVersionLabel(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceRDMProtocolVersion(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceModelID(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceProductCategoryType(unsigned int Index);

    GADGET_DLL_API unsigned int Gadget2_GetDeviceSoftwareVersionID(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXFootprint(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXPersonality(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXStartAddress(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetDeviceSubdeviceCount(unsigned int Index);

    GADGET_DLL_API unsigned char Gadget2_GetDeviceSensorCount(unsigned int Index);

    GADGET_DLL_API unsigned int Gadget2_GetNumResponses(void);

    GADGET_DLL_API RDM_CmdC * Gadget2_GetResponse(unsigned int Index);

    GADGET_DLL_API void Gadget2_ClearResponse(unsigned int Index);

    GADGET_DLL_API unsigned char Gadget2_GetResponseCommand(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetResponseParameter(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetResponseSubdevice(unsigned int Index);

    GADGET_DLL_API unsigned char Gadget2_GetResponseLength(unsigned int Index);

    GADGET_DLL_API unsigned char * Gadget2_GetResponseBuffer(unsigned int Index);

    GADGET_DLL_API unsigned char Gadget2_GetResponseResponseType(unsigned int Index);

    GADGET_DLL_API unsigned short Gadget2_GetResponseManufacturer_id(unsigned int Index);

    GADGET_DLL_API unsigned int Gadget2_GetResponseDevice_id(unsigned int Index);

    GADGET_DLL_API void Gadget2_SendRDMCommand(unsigned int DeviceNum, unsigned int PortNum, unsigned char Cmd, unsigned short ParameterID, unsigned short SubDevice, unsigned char DataLen, const char * Buffer, unsigned short ManfID, unsigned int DevID);

    GADGET_DLL_API void Gadget2_SendDMX(unsigned int DeviceNum, unsigned int PortNum, unsigned char * Buffer, unsigned int Size);

    GADGET_DLL_API void Gadget2_DisableDMX(unsigned int DeviceNum, unsigned int PortNum);

    GADGET_DLL_API void Gadget2_ToggleRDMDiscovery(unsigned int DeviceNum, unsigned int PortNum, unsigned char Enable);

    GADGET_DLL_API unsigned int Gadget2_GetNumGadgetDevices();

    GADGET_DLL_API unsigned char * Gadget2_GetGadgetVersion(unsigned int DeviceNum);

    GADGET_DLL_API unsigned int Gadget2_GetGadgetSerialNumber(unsigned int DeviceNum);

    GADGET_DLL_API int Gadget2_SetRawReceiveMode(unsigned int DeviceNum, unsigned int PortNum);

    GADGET_DLL_API unsigned int Gadget2_GetNumberOfRXRawBytes(unsigned int DeviceNum, unsigned int PortNum);

    GADGET_DLL_API void Gadget2_GetRXRawBytes(unsigned int DeviceNum, unsigned int PortNum, unsigned short * Data, unsigned int Length);

    GADGET_DLL_API void Gadget2_SendRawBytes(unsigned int DeviceNum, unsigned int PortNum, unsigned char * Data, unsigned int Length);

    GADGET_DLL_API void Gadget2_SendBreakAndData(unsigned int DeviceNum, unsigned int PortNum, unsigned char StartCode, unsigned char * Data, unsigned int Length);
	
	GADGET_DLL_API void Gadget2_SetLogCallback(Gadget2_LogCallback *Callback);

	GADGET_DLL_API void Gadget2_SetLogFilter(int verb, int cat, int sev);
}



#endif