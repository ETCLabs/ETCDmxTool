/******************************************************************************
 * Copyright 2019 ETC Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef GADGETDLL_H_
#define GADGETDLL_H_

#include <QtGlobal> // QT_OS_xxx
#include <stdint.h>

/* Linux/macOS notes
 *
 * Gadget support is currently only available on Windows
 * so predetermined values are returned by accompanying source file
 * to ensure code compatibility until such a time when full support is added
 */

#if defined(Q_OS_WIN)
#ifdef GADGET_DLL_EXPORT
#define GADGET_DLL_API   __declspec( dllexport )
#else
#define GADGET_DLL_API   __declspec( dllimport )
#endif
#else
#define GADGET_DLL_API
#define __stdcall
#endif

/* Classes for talking to RDM devices - defined elsewhere */
class RDM_CmdC;
struct RdmDeviceInfo;

#define GADGET_DLL_DMX_BREAK 0x8000
#define GADGET_DLL_FRAMING_ERROR 0x9000

/* The available speeds for Gadget DMX output */
enum Gadget2_DmxSpeed {
    GADGET_DLL_SPEED_MAX,
    GADGET_DLL_SPEED_FAST,
    GADGET_DLL_SPEED_MEDIUM,
    GADGET_DLL_SPEED_SLOW,
    GADGET_DLL_SPEED_SYNC,
    GADGET_DLL_SPEED_COUNT
};

extern "C"
{
    /***************** Type Definitions *****************/

    /* Status flags for when Gadget is being updated, passed to the Gadget2_UpdateStatusCallback */
    enum Gadget2_UpdateStatus
    {
        Gadget2_Update_Beginning,
        Gadget2_Update_BootloaderFound,
        Gadget2_Update_TransferringFile,
        Gadget2_Update_ReadyForReboot,
        Gadget2_Update_Error
    };

    /* Callback for log messages */
    typedef void(__stdcall Gadget2_LogCallback)(const char *LogData);

    /* Callback to provide progress updates as the gadget is being updated */
    typedef void(__stdcall Gadget2_UpdateStatusCallback)(Gadget2_UpdateStatus status, void* context);

    /***************** Information about the DLL *****************/

    /* Returns the version of the DLL being used */
    GADGET_DLL_API char * Gadget2_GetDllVersion();

    /***************** Startup and Shutdown *****************/

    /* Start up the Gadget2 interface. This should be called before any other functions are used */
    GADGET_DLL_API bool Gadget2_Connect();

    /* Shutdown the Gadget2 interface. Stops the threads; do not use other functions after calling this */
    GADGET_DLL_API void Gadget2_Disconnect();

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

    /* Set DMX Speed */
    GADGET_DLL_API void Gadget2_SetDmxSpeed(unsigned int DeviceNum, unsigned int PortNum, unsigned int Speed);

    /***************** Information and control of the Gadget *****************/
    /* Get the number of Gadget devices found on the PC */
    GADGET_DLL_API unsigned int Gadget2_GetNumGadgetDevices();

    /* Return the version number of the specified device as a string */
    GADGET_DLL_API const char * Gadget2_GetGadgetVersion(unsigned int DeviceNum);

    /* Return the 32-bit serial number of the specified device */
    GADGET_DLL_API uint32_t Gadget2_GetGadgetSerialNumber(unsigned int DeviceNum);

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
    GADGET_DLL_API unsigned int Gadget2_GetDiscoveredDevices();

    /* Get the RDMDeviceInfo structre from the device at Index in the array */
    GADGET_DLL_API RdmDeviceInfo * Gadget2_GetDeviceInfo(unsigned int Index);

    /* Get which Gadget index an RDM device was discovered on */
    GADGET_DLL_API unsigned int Gadget2_GetGadgetForDevice(unsigned int Index);

    /* Get the RDM Manufacturer ID for the device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceManfID(unsigned int Index);

    /* Return the RDM Device ID for the device at Index in the array*/
    GADGET_DLL_API unsigned int Gadget2_GetDeviceID(unsigned int Index);

    /* Return the software version label for the RDM device at Index in the array */
    GADGET_DLL_API unsigned char * Gadget2_GetDeviceSoftwareVersionLabel(unsigned int Index);

    /* Return the RDM Protocol version for the RDM device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceRDMProtocolVersion(unsigned int Index);

    /* Return the RDM Model ID for the RDM device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceModelID(unsigned int Index);

    /* Return the RDM Product Category for the RDM device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceProductCategoryType(unsigned int Index);

    /* Return the RDM Software version ID for the RDM device at Index in the array */
    GADGET_DLL_API unsigned int Gadget2_GetDeviceSoftwareVersionID(unsigned int Index);

    /* Return the RDM DMX footprint for the RDM device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXFootprint(unsigned int Index);

    /* Return the (integer) DMX personality for the RDM device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXPersonality(unsigned int Index);

    /* Return the DMX start address for the RDM device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceDMXStartAddress(unsigned int Index);

    /* Return the RDM Subdevice count for the RDM device at Index in the array */
    GADGET_DLL_API unsigned short Gadget2_GetDeviceSubdeviceCount(unsigned int Index);

    /* Return the RDM sensor count for the RDM device at Index in the array */
    GADGET_DLL_API unsigned char Gadget2_GetDeviceSensorCount(unsigned int Index);

    /* Return the number of responses in the RDM command queue */
    GADGET_DLL_API unsigned int Gadget2_GetNumResponses();

    /* Return the RDM response at Index in the RDM command queue */
    GADGET_DLL_API RDM_CmdC * Gadget2_GetResponse(unsigned int Index);

    /* Removes the RDM response at Index in the RDM command queue */
    GADGET_DLL_API void Gadget2_ClearResponse(unsigned int Index);

    /* Return the RDM command parameter at Index in the RDM command queue */
    GADGET_DLL_API unsigned char Gadget2_GetResponseCommand(unsigned int Index);

    /* Return the RDM response parameter at Index in the RDM command queue */
    GADGET_DLL_API unsigned short Gadget2_GetResponseParameter(unsigned int Index);

    /* Return the RDM response subdevice parameter at Index in the RDM command queue */
    GADGET_DLL_API unsigned short Gadget2_GetResponseSubdevice(unsigned int Index);

    /* Return the RDM response length for the response at Index in the RDM command queue */
    GADGET_DLL_API unsigned char Gadget2_GetResponseLength(unsigned int Index);

    /* Return a pointer to the response buffer for the response at Index in the RDM command queue */
    GADGET_DLL_API unsigned char * Gadget2_GetResponseBuffer(unsigned int Index);

    /* Return the RDM response type for the response at Index in the RDM command queue */
    GADGET_DLL_API unsigned char Gadget2_GetResponseResponseType(unsigned int Index);

    /* Return the RDM response manufacturer ID for the response at Index in the RDM command queue */
    GADGET_DLL_API unsigned short Gadget2_GetResponseManufacturer_id(unsigned int Index);

    /* Return the RDM response device ID for the response at Index in the RDM command queue */
    GADGET_DLL_API unsigned int Gadget2_GetResponseDevice_id(unsigned int Index);

    /* Return the opaque context data for the response at Index in the RDM command queue */
    GADGET_DLL_API const void * Gadget2_GetResponseContext(unsigned int Index);

    /* Send an RDM command */
    GADGET_DLL_API void Gadget2_SendRDMCommand(unsigned int DeviceNum, unsigned int PortNum, unsigned char Cmd, unsigned short ParameterID, unsigned short SubDevice, unsigned char DataLen, const char * Buffer, unsigned short ManfID, unsigned int DevID);

    /* Send an RDM command with a context value which can be retrieved with the response */
    GADGET_DLL_API void Gadget2_SendRDMCommandWithContext(unsigned int DeviceNum, unsigned int PortNum, unsigned char Cmd, unsigned short ParameterID, unsigned short SubDevice, unsigned char DataLen, const char * Buffer, unsigned short ManfID, unsigned int DevID, const void * Context);
}

#endif /* GADGETDLL_H_ */
