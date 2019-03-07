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

extern "C"
{
    GADGET_DLL_API char * Gadget2_GetDllVersion();

    GADGET_DLL_API int Gadget2_Connect();

    GADGET_DLL_API int Gadget2_Disconnect(void);

    GADGET_DLL_API void Gadget2_StartDiscovery(unsigned int DeviceID, unsigned int PortNum);

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

}



#endif