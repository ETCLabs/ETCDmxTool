#ifndef __GadgetExport_h
#define __GadgetExport_h



#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

class RDM_CmdC;
struct RdmDeviceInfo;

extern "C" 
{
	DllExport char * Gadget2_GetDllVersion();

	DllExport int Gadget2_Connect();

	DllExport int Gadget2_Disconnect(void);

	DllExport void Gadget2_StartDiscovery(int DeviceID, int PortNum);

	DllExport int Gadget2_GetDiscoveredDevices(void);

	DllExport RdmDeviceInfo * Gadget2_GetDeviceInfo(int Index);

	DllExport unsigned short Gadget2_GetDeviceManfID(int Index);

	DllExport unsigned int Gadget2_GetDeviceID(int Index);

	DllExport unsigned char * Gadget2_GetDeviceSoftwareVersionLabel(int Index);

	DllExport unsigned short Gadget2_GetDeviceRDMProtocolVersion(int Index);

	DllExport unsigned short Gadget2_GetDeviceModelID(int Index);

	DllExport unsigned short Gadget2_GetDeviceProductCategoryType(int Index);

	DllExport unsigned int Gadget2_GetDeviceSoftwareVersionID(int Index);

	DllExport unsigned short Gadget2_GetDeviceDMXFootprint(int Index);

	DllExport unsigned short Gadget2_GetDeviceDMXPersonality(int Index);

	DllExport unsigned short Gadget2_GetDeviceDMXStartAddress(int Index);

	DllExport unsigned short Gadget2_GetDeviceSubdeviceCount(int Index);

	DllExport unsigned char Gadget2_GetDeviceSensorCount(int Index);
		
	DllExport int Gadget2_GetNumResponses(void);

	DllExport RDM_CmdC * Gadget2_GetResponse(int Index);

	DllExport void Gadget2_ClearResponse(int Index);

	DllExport unsigned char Gadget2_GetResponseCommand(int Index);
	
	DllExport unsigned short Gadget2_GetResponseParameter(int Index);
	
	DllExport unsigned short Gadget2_GetResponseSubdevice(int Index);
	
	DllExport unsigned char Gadget2_GetResponseLength(int Index);
	
	DllExport unsigned char * Gadget2_GetResponseBuffer(int Index);
	
	DllExport unsigned char Gadget2_GetResponseResponseType(int Index);
	
	DllExport unsigned short Gadget2_GetResponseManufacturer_id(int Index);
	
	DllExport unsigned int Gadget2_GetResponseDevice_id(int Index);

	DllExport void Gadget2_SendRDMCommand(int DeviceNum, int PortNum, unsigned char Cmd, unsigned short ParameterID, unsigned short SubDevice, unsigned char DataLen, const char * Buffer, unsigned short ManfID, unsigned int DevID);

	DllExport void Gadget2_SendDMX(int DeviceNum, int PortNum, unsigned char * Buffer, int Size);

	DllExport void Gadget2_DisableDMX(int DeviceNum, int PortNum);

	DllExport void Gadget2_ToggleRDMDiscovery(int DeviceNum, int PortNum, unsigned char Enable);

	DllExport unsigned int Gadget2_GetNumGadgetDevices();

	DllExport unsigned char * Gadget2_GetGadgetVersion(int DeviceNum);

	DllExport unsigned int Gadget2_GetGadgetSerialNumber(int DeviceNum);

	DllExport int Gadget2_GetNumberOfRXRawBytes(int DeviceNum);

	DllExport void Gadget2_GetRXRawBytes(int DeviceNum, unsigned short * Data, int Length);
	
	DllExport void Gadget2_SendRawBytes(int DeviceNum, int PortNum, unsigned char * Data, int Length);

	DllExport void Gadget2_SendBreakAndData(int DeviceNum, int PortNum, unsigned char StartCode, unsigned char * Data, int Length);

}



#endif