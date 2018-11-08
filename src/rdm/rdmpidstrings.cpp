// Copyright (c) 2017 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "rdmpidstrings.h"
#include "estardm.h"
#include "rdmEtcConsts.h"

QString RDM_PIDString::productCategoryToString(quint16 value)
{
    switch(value)
    {
    case E120_PRODUCT_CATEGORY_NOT_DECLARED:
        return QString("E120_PRODUCT_CATEGORY_NOT_DECLARED");
    case E120_PRODUCT_CATEGORY_FIXTURE:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE");
    case E120_PRODUCT_CATEGORY_FIXTURE_FIXED:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_FIXED");
    case E120_PRODUCT_CATEGORY_FIXTURE_MOVING_YOKE:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_MOVING_YOKE");
    case E120_PRODUCT_CATEGORY_FIXTURE_MOVING_MIRROR:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_MOVING_MIRROR");
    case E120_PRODUCT_CATEGORY_FIXTURE_OTHER:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_OTHER");
    case E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY");
    case E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_COLOR:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_COLOR");
    case E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_YOKE:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_YOKE");
    case E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_MIRROR:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_MIRROR");
    case E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_EFFECT:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_EFFECT");
    case E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_BEAM:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_BEAM");
    case E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_OTHER:
        return QString("E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_OTHER");
    case E120_PRODUCT_CATEGORY_PROJECTOR:
        return QString("E120_PRODUCT_CATEGORY_PROJECTOR");
    case E120_PRODUCT_CATEGORY_PROJECTOR_FIXED:
        return QString("E120_PRODUCT_CATEGORY_PROJECTOR_FIXED");
    case E120_PRODUCT_CATEGORY_PROJECTOR_MOVING_YOKE:
        return QString("E120_PRODUCT_CATEGORY_PROJECTOR_MOVING_YOKE");
    case E120_PRODUCT_CATEGORY_PROJECTOR_MOVING_MIRROR:
        return QString("E120_PRODUCT_CATEGORY_PROJECTOR_MOVING_MIRROR");
    case E120_PRODUCT_CATEGORY_PROJECTOR_OTHER:
        return QString("E120_PRODUCT_CATEGORY_PROJECTOR_OTHER");
    case E120_PRODUCT_CATEGORY_ATMOSPHERIC:
        return QString("E120_PRODUCT_CATEGORY_ATMOSPHERIC");
    case E120_PRODUCT_CATEGORY_ATMOSPHERIC_EFFECT:
        return QString("E120_PRODUCT_CATEGORY_ATMOSPHERIC_EFFECT");
    case E120_PRODUCT_CATEGORY_ATMOSPHERIC_PYRO:
        return QString("E120_PRODUCT_CATEGORY_ATMOSPHERIC_PYRO");
    case E120_PRODUCT_CATEGORY_ATMOSPHERIC_OTHER:
        return QString("E120_PRODUCT_CATEGORY_ATMOSPHERIC_OTHER");
    case E120_PRODUCT_CATEGORY_DIMMER:
        return QString("E120_PRODUCT_CATEGORY_DIMMER");
    case E120_PRODUCT_CATEGORY_DIMMER_AC_INCANDESCENT:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_AC_INCANDESCENT");
    case E120_PRODUCT_CATEGORY_DIMMER_AC_FLUORESCENT:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_AC_FLUORESCENT");
    case E120_PRODUCT_CATEGORY_DIMMER_AC_COLDCATHODE:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_AC_COLDCATHODE");
    case E120_PRODUCT_CATEGORY_DIMMER_AC_NONDIM:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_AC_NONDIM");
    case E120_PRODUCT_CATEGORY_DIMMER_AC_ELV:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_AC_ELV");
    case E120_PRODUCT_CATEGORY_DIMMER_AC_OTHER:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_AC_OTHER");
    case E120_PRODUCT_CATEGORY_DIMMER_DC_LEVEL:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_DC_LEVEL");
    case E120_PRODUCT_CATEGORY_DIMMER_DC_PWM:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_DC_PWM");
    case E120_PRODUCT_CATEGORY_DIMMER_CS_LED:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_CS_LED");
    case E120_PRODUCT_CATEGORY_DIMMER_OTHER:
        return QString("E120_PRODUCT_CATEGORY_DIMMER_OTHER");
    case E120_PRODUCT_CATEGORY_POWER:
        return QString("E120_PRODUCT_CATEGORY_POWER");
    case E120_PRODUCT_CATEGORY_POWER_CONTROL:
        return QString("E120_PRODUCT_CATEGORY_POWER_CONTROL");
    case E120_PRODUCT_CATEGORY_POWER_SOURCE:
        return QString("E120_PRODUCT_CATEGORY_POWER_SOURCE");
    case E120_PRODUCT_CATEGORY_POWER_OTHER:
        return QString("E120_PRODUCT_CATEGORY_POWER_OTHER");
    case E120_PRODUCT_CATEGORY_SCENIC:
        return QString("E120_PRODUCT_CATEGORY_SCENIC");
    case E120_PRODUCT_CATEGORY_SCENIC_DRIVE:
        return QString("E120_PRODUCT_CATEGORY_SCENIC_DRIVE");
    case E120_PRODUCT_CATEGORY_SCENIC_OTHER:
        return QString("E120_PRODUCT_CATEGORY_SCENIC_OTHER");
    case E120_PRODUCT_CATEGORY_DATA:
        return QString("E120_PRODUCT_CATEGORY_DATA");
    case E120_PRODUCT_CATEGORY_DATA_DISTRIBUTION:
        return QString("E120_PRODUCT_CATEGORY_DATA_DISTRIBUTION");
    case E120_PRODUCT_CATEGORY_DATA_CONVERSION:
        return QString("E120_PRODUCT_CATEGORY_DATA_CONVERSION");
    case E120_PRODUCT_CATEGORY_DATA_OTHER:
        return QString("E120_PRODUCT_CATEGORY_DATA_OTHER");
    case E120_PRODUCT_CATEGORY_AV:
        return QString("E120_PRODUCT_CATEGORY_AV");
    case E120_PRODUCT_CATEGORY_AV_AUDIO:
        return QString("E120_PRODUCT_CATEGORY_AV_AUDIO");
    case E120_PRODUCT_CATEGORY_AV_VIDEO:
        return QString("E120_PRODUCT_CATEGORY_AV_VIDEO");
    case E120_PRODUCT_CATEGORY_AV_OTHER:
        return QString("E120_PRODUCT_CATEGORY_AV_OTHER");
    case E120_PRODUCT_CATEGORY_MONITOR:
        return QString("E120_PRODUCT_CATEGORY_MONITOR");
    case E120_PRODUCT_CATEGORY_MONITOR_ACLINEPOWER:
        return QString("E120_PRODUCT_CATEGORY_MONITOR_ACLINEPOWER");
    case E120_PRODUCT_CATEGORY_MONITOR_DCPOWER:
        return QString("E120_PRODUCT_CATEGORY_MONITOR_DCPOWER");
    case E120_PRODUCT_CATEGORY_MONITOR_ENVIRONMENTAL:
        return QString("E120_PRODUCT_CATEGORY_MONITOR_ENVIRONMENTAL");
    case E120_PRODUCT_CATEGORY_MONITOR_OTHER:
        return QString("E120_PRODUCT_CATEGORY_MONITOR_OTHER");
    case E120_PRODUCT_CATEGORY_CONTROL:
        return QString("E120_PRODUCT_CATEGORY_CONTROL");
    case E120_PRODUCT_CATEGORY_CONTROL_CONTROLLER:
        return QString("E120_PRODUCT_CATEGORY_CONTROL_CONTROLLER");
    case E120_PRODUCT_CATEGORY_CONTROL_BACKUPDEVICE:
        return QString("E120_PRODUCT_CATEGORY_CONTROL_BACKUPDEVICE");
    case E120_PRODUCT_CATEGORY_CONTROL_OTHER:
        return QString("E120_PRODUCT_CATEGORY_CONTROL_OTHER");
    case E120_PRODUCT_CATEGORY_TEST:
        return QString("E120_PRODUCT_CATEGORY_TEST");
    case E120_PRODUCT_CATEGORY_TEST_EQUIPMENT:
        return QString("E120_PRODUCT_CATEGORY_TEST_EQUIPMENT");
    case E120_PRODUCT_CATEGORY_TEST_EQUIPMENT_OTHER:
        return QString("E120_PRODUCT_CATEGORY_TEST_EQUIPMENT_OTHER");
    case E120_PRODUCT_CATEGORY_OTHER:
    default:
        return QString("E120_PRODUCT_CATEGORY_OTHER");
    }
}

QString RDM_PIDString::detailIdToString(quint16 detailId)
{
    switch (detailId)
    {
    case E120_PRODUCT_DETAIL_NOT_DECLARED : return QString("Not Declared");

    /* Generally applied to fixtures */
    case E120_PRODUCT_DETAIL_ARC : return QString("Arc Lamp");
    case E120_PRODUCT_DETAIL_METAL_HALIDE : return QString("Metal Halide Lamp");
    case E120_PRODUCT_DETAIL_INCANDESCENT  : return QString("Incandescent Lamp");
    case E120_PRODUCT_DETAIL_LED : return QString("LED Lamp");
    case E120_PRODUCT_DETAIL_FLUROESCENT : return QString("Fluorescent Lamp");
    case E120_PRODUCT_DETAIL_COLDCATHODE : return QString("Cold Cathode Lamp");
    case E120_PRODUCT_DETAIL_ELECTROLUMINESCENT : return QString("EL Lamp");
    case E120_PRODUCT_DETAIL_LASER  : return QString("Laser Source");
    case E120_PRODUCT_DETAIL_FLASHTUBE : return QString("Strobe or other flashtube");

    /* Generally applied to fixture accessories */
    case E120_PRODUCT_DETAIL_COLORSCROLLER : return QString("Color Scroller");
    case E120_PRODUCT_DETAIL_COLORWHEEL : return QString("Color Wheel");
    case E120_PRODUCT_DETAIL_COLORCHANGE  : return QString("Other Color Changer");
    case E120_PRODUCT_DETAIL_IRIS_DOUSER  : return QString("Iris Douser");
    case E120_PRODUCT_DETAIL_DIMMING_SHUTTER : return QString("Dimming Shutter");
    case E120_PRODUCT_DETAIL_PROFILE_SHUTTER  : return QString("Hard-edge beam shaping. Shutter blades");
    case E120_PRODUCT_DETAIL_BARNDOOR_SHUTTER : return QString("Soft-edge beam shaping. Barndoors");
    case E120_PRODUCT_DETAIL_EFFECTS_DISC : return QString("Effects Disc");
    case E120_PRODUCT_DETAIL_GOBO_ROTATOR : return QString("Gobo Rotator");

    /* Generally applied to Projectors */
    case E120_PRODUCT_DETAIL_VIDEO  : return QString("Video Projection");
    case E120_PRODUCT_DETAIL_SLIDE  : return QString("Slide Projection");
    case E120_PRODUCT_DETAIL_FILM   : return QString("Film Projection");
    case E120_PRODUCT_DETAIL_OILWHEEL : return QString("Oilwheel");
    case E120_PRODUCT_DETAIL_LCDGATE  : return QString("LCD Gate");

    /* Generally applied to Atmospheric Effects */
    case E120_PRODUCT_DETAIL_FOGGER_GLYCOL  : return QString("Glycol/Glycerin fogger");
    case E120_PRODUCT_DETAIL_FOGGER_MINERALOIL  : return QString("White Mineral oil fogger");
    case E120_PRODUCT_DETAIL_FOGGER_WATER  : return QString("Water fogger");
    case E120_PRODUCT_DETAIL_C02  : return QString("Dry Ice/Carbon Dioxide fogger");
    case E120_PRODUCT_DETAIL_LN2  : return QString("Nitrogen based fogger");
    case E120_PRODUCT_DETAIL_BUBBLE  : return QString("Bubble/Foam effect");
    case E120_PRODUCT_DETAIL_FLAME_PROPANE  : return QString("Propane Flame");
    case E120_PRODUCT_DETAIL_FLAME_OTHER  : return QString("Other Flame");
    case E120_PRODUCT_DETAIL_OLEFACTORY_STIMULATOR  : return QString("Scents");
    case E120_PRODUCT_DETAIL_SNOW  : return QString("Snow effect");
    case E120_PRODUCT_DETAIL_WATER_JET : return QString("Water Jet");
    case E120_PRODUCT_DETAIL_WIND  : return QString("Wind effect");
    case E120_PRODUCT_DETAIL_CONFETTI : return QString("Confetti effect");
    case E120_PRODUCT_DETAIL_HAZARD  : return QString("Hazardous effect");

    /* Generally applied to Dimmers/Power controllers */
    case E120_PRODUCT_DETAIL_PHASE_CONTROL  : return QString("Forward Phase Dimming");
    case E120_PRODUCT_DETAIL_REVERSE_PHASE_CONTROL  : return QString("Reverse Phase Dimming");
    case E120_PRODUCT_DETAIL_SINE  : return QString("Sinewave Dimming");
    case E120_PRODUCT_DETAIL_PWM   : return QString("PWM Dimming");
    case E120_PRODUCT_DETAIL_DC     : return QString("DC Voltage Dimming");
    case E120_PRODUCT_DETAIL_HFBALLAST  : return QString("HF Ballast");
    case E120_PRODUCT_DETAIL_HFHV_NEONBALLAST  : return QString("CC Ballast");
    case E120_PRODUCT_DETAIL_HFHV_EL  : return QString("EL Ballast");
    case E120_PRODUCT_DETAIL_MHR_BALLAST  : return QString("Metal Halide Ballast");
    case E120_PRODUCT_DETAIL_BITANGLE_MODULATION : return QString("Bitangle Dimming");
    case E120_PRODUCT_DETAIL_FREQUENCY_MODULATION : return QString("Frequency Modulation");
    case E120_PRODUCT_DETAIL_HIGHFREQUENCY_12V  : return QString("HF 12V");
    case E120_PRODUCT_DETAIL_RELAY_MECHANICAL    : return QString("Mechanical Relay");
    case E120_PRODUCT_DETAIL_RELAY_ELECTRONIC    : return QString("Electronic Relay");
    case E120_PRODUCT_DETAIL_SWITCH_ELECTRONIC   : return QString("Solid-State Switch");
    case E120_PRODUCT_DETAIL_CONTACTOR           : return QString("Contactor");

    /* Generally applied to Scenic drive */
    case E120_PRODUCT_DETAIL_MIRRORBALL_ROTATOR : return QString("Mirrorball Rotator");
    case E120_PRODUCT_DETAIL_OTHER_ROTATOR  : return QString("Other Rotator");
    case E120_PRODUCT_DETAIL_KABUKI_DROP    : return QString("Kabuki Drop");
    case E120_PRODUCT_DETAIL_CURTAIN        : return QString("Soft Curtain");
    case E120_PRODUCT_DETAIL_LINESET        : return QString("Lineset");
    case E120_PRODUCT_DETAIL_MOTOR_CONTROL  : return QString("Other Motor");
    case E120_PRODUCT_DETAIL_DAMPER_CONTROL : return QString("HVAC Damper");

    /* Generally applied to Data Distribution */
    case E120_PRODUCT_DETAIL_SPLITTER : return QString("DMX512 Splitter/Buffer/Repeater");
    case E120_PRODUCT_DETAIL_ETHERNET_NODE : return QString("DMX512 to/from Ethernet");
    case E120_PRODUCT_DETAIL_MERGE : return QString("DMX512 merger");
    case E120_PRODUCT_DETAIL_DATAPATCH : return QString("Electronic Datalink Patch");
    case E120_PRODUCT_DETAIL_WIRELESS_LINK : return QString("DMX512 Wireless Link");

    /* Generally applied to Data Conversion and Interfaces */
    case E120_PRODUCT_DETAIL_PROTOCOL_CONVERTOR : return QString("Protocol Convertor");
    case E120_PRODUCT_DETAIL_ANALOG_DEMULTIPLEX : return QString("DMX512 Demux");
    case E120_PRODUCT_DETAIL_ANALOG_MULTIPLEX   : return QString("DMX512 Mux");
    case E120_PRODUCT_DETAIL_SWITCH_PANEL       : return QString("Switch Panel");

    /* Generally applied to Audio or Video (AV) devices */
    case E120_PRODUCT_DETAIL_ROUTER : return QString("AV Router");
    case E120_PRODUCT_DETAIL_FADER  : return QString("Single Fader");
    case E120_PRODUCT_DETAIL_MIXER  : return QString("Multiple Faders");

    /* Generally applied to Controllers, Backup devices and Test Equipment */
    case E120_PRODUCT_DETAIL_CHANGEOVER_MANUAL     : return QString("Manual control takeover");
    case E120_PRODUCT_DETAIL_CHANGEOVER_AUTO       : return QString("Automatic control takeover");
    case E120_PRODUCT_DETAIL_TEST                  : return QString("Test equipment");

    /* Could be applied to any category */
    case E120_PRODUCT_DETAIL_GFI_RCD              : return QString("Includes GFI/RCD trip");
    case E120_PRODUCT_DETAIL_BATTERY              : return QString("Battery operated");
    case E120_PRODUCT_DETAIL_CONTROLLABLE_BREAKER : return QString("Controllable circuit breaker");

    case E120_PRODUCT_DETAIL_OTHER : return QString("Other");

    default: return QString("Manufacturer Specific");
    }
}

QString RDM_PIDString::statusTypeToString(quint8 statusType)
{
    switch (statusType)
    {
    case E120_STATUS_NONE :
        return QString("E120_STATUS_NONE");
    case E120_STATUS_GET_LAST_MESSAGE :
        return QString("E120_STATUS_GET_LAST_MESSAGE");
    case E120_STATUS_ADVISORY :
        return QString("E120_STATUS_ADVISORY");
    case E120_STATUS_WARNING :
        return QString("E120_STATUS_WARNING");
    case E120_STATUS_ERROR :
        return QString("E120_STATUS_ERROR");
    case E120_STATUS_ADVISORY_CLEARED :    /* Added in E1.20-2010 version */
        return QString("E120_STATUS_ADVISORY_CLEARED");
    case E120_STATUS_WARNING_CLEARED :     /* Added in E1.20-2010 version */
        return QString("E120_STATUS_WARNING_CLEARED");
    case E120_STATUS_ERROR_CLEARED :
        return QString("E120_STATUS_ERROR_CLEARED");
    default:
        return QString("Unknown Status Type");
    }
}

QString RDM_PIDString::commandClassToString(quint8 commandClass)
{
        switch(commandClass)
        {
        case E120_DISCOVERY_COMMAND:
                return "Discovery Command";
        case E120_DISCOVERY_COMMAND_RESPONSE:
                return "Discovery Command Response";
        case E120_GET_COMMAND:
                return "Get Command";
        case E120_GET_COMMAND_RESPONSE:
                return "Get Command Response";
        case E120_SET_COMMAND:
                return "Set Command";
        case E120_SET_COMMAND_RESPONSE:
                return "Set Command Response";
        }
        return "Unknown";
}

QString RDM_PIDString::responseTypeToString(quint8 responseType)
{
        switch(responseType)
        {
        case E120_RESPONSE_TYPE_ACK:
                return "ACK_OK";
        case E120_RESPONSE_TYPE_ACK_TIMER:
                return "ACK_TIMER";
        case E120_RESPONSE_TYPE_NACK_REASON:
                return "NACK";
        case E120_RESPONSE_TYPE_ACK_OVERFLOW:
                return "ACK_OVERFLOW";
        }
        return "Unknown Response";
}

QString RDM_PIDString::nackReasonToString(quint16 reasonCode)
{
        switch(reasonCode)
        {
        case E120_NR_UNKNOWN_PID:
                return "UNKNOWN_PID";
        case E120_NR_FORMAT_ERROR:
                return "FORMAT_ERROR";
        case E120_NR_HARDWARE_FAULT:
                return "HARDWARE_FAULT";
        case E120_NR_PROXY_REJECT:
                return "PROXY_REJECT";
        case E120_NR_WRITE_PROTECT:
                return "WRITE_PROTECT";
        case E120_NR_UNSUPPORTED_COMMAND_CLASS:
                return "UNSUPPORTED_COMMAND_CLASS";
        case E120_NR_DATA_OUT_OF_RANGE:
                return "DATA_OUT_OF_RANGE";
        case E120_NR_BUFFER_FULL:
                return "BUFFER_FULL";
        case E120_NR_PACKET_SIZE_UNSUPPORTED:
                return "PACKET_SIZE_UNSUPPORTED";
        case E120_NR_SUB_DEVICE_OUT_OF_RANGE:
                return "SUB_DEVICE_OUT_OF_RANGE";
        case E120_NR_PROXY_BUFFER_FULL:
                return "PROXY_BUFFER_FULL";
        }
        return "Unknown Reason";
}

QString RDM_PIDString::statusMessageToString(quint16 statusMessage)
{
        switch(statusMessage)
        {
        case E120_STS_CAL_FAIL:
                return "E120_STS_CAL_FAIL";
        case E120_STS_SENS_NOT_FOUND:
                return "E120_STS_SENS_NOT_FOUND";
        case E120_STS_SENS_ALWAYS_ON:
                return "E120_STS_SENS_ALWAYS_ON";
        case E120_STS_FEEDBACK_ERROR:
                return "E120_STS_FEEDBACK_ERROR";
        case E120_STS_INDEX_ERROR:
                return "E120_STS_INDEX_ERROR";
        case E120_STS_LAMP_DOUSED:
                return "E120_STS_LAMP_DOUSED";
        case E120_STS_LAMP_STRIKE:
                return "E120_STS_LAMP_STRIKE";
        case E120_STS_LAMP_ACCESS_OPEN:
                return "E120_STS_LAMP_ACCESS_OPEN";
        case E120_STS_LAMP_ALWAYS_ON:
                return "E120_STS_LAMP_ALWAYS_ON";
        case E120_STS_OVERTEMP:
                return "E120_STS_OVERTEMP";
        case E120_STS_UNDERTEMP:
                return "E120_STS_UNDERTEMP";
        case E120_STS_SENS_OUT_RANGE:
                return "E120_STS_SENS_OUT_RANGE";
        case E120_STS_OVERVOLTAGE_PHASE:
                return "E120_STS_OVERVOLTAGE_PHASE";
        case E120_STS_UNDERVOLTAGE_PHASE:
                return "E120_STS_UNDERVOLTAGE_PHASE";
        case E120_STS_OVERCURRENT:
                return "E120_STS_OVERCURRENT";
        case E120_STS_UNDERCURRENT:
                return "E120_STS_UNDERCURRENT";
        case E120_STS_PHASE:
                return "E120_STS_PHASE";
        case E120_STS_PHASE_ERROR:
                return "E120_STS_PHASE_ERROR";
        case E120_STS_AMPS:
                return "E120_STS_AMPS";
        case E120_STS_VOLTS:
                return "E120_STS_VOLTS";
        case E120_STS_DIMSLOT_OCCUPIED:
                return "E120_STS_DIMSLOT_OCCUPIED";
        case E120_STS_BREAKER_TRIP:
                return "E120_STS_BREAKER_TRIP";
        case E120_STS_WATTS:
                return "E120_STS_WATTS";
        case E120_STS_DIM_FAILURE:
                return "E120_STS_DIM_FAILURE";
        case E120_STS_DIM_PANIC:
                return "E120_STS_DIM_PANIC";
        case E120_STS_LOAD_FAILURE:
                return "E120_STS_LOAD_FAILURE";
        case E120_STS_READY:
                return "E120_STS_READY";
        case E120_STS_NOT_READY:
                return "E120_STS_NOT_READY";
        case E120_STS_LOW_LIST:
                return "E120_STS_LOW_LIST";
        case E120_STS_EEPROM_ERROR:
                return "E120_STS_EEPROM_ERROR";
        case E120_STS_RAM_ERROR:
                return "E120_STS_RAM_ERROR";
        case E120_STS_FPGA_ERROR:
                return "E120_STS_FPGA_ERROR";
        case E120_STS_PROXY_BOARD_CAST_DROPPED:
                return "E120_STS_PROXY_BOARD_CAST_DROPPED";
        case E120_STS_ASC_RXOK:
                return "E120_STS_ASC_RXOK";
        case E120_STS_ASC_DROPPED:
                return "E120_STS_ASC_DROPPED";
        case E120_STS_DMXNSCNONE:
                return "E120_STS_DMXNSCNONE";
        case E120_STS_DMXNSCLOSS:
                return "E120_STS_DMXNSCLOSS";
        case E120_STS_DMXNSCERROR:
                return "E120_STS_DMXNSCERROR";
        case E120_STS_DMXNSCOK:
                return "E120_STS_DMXNSCOK";
        }
        return "Unknown Message";
}

QString RDM_PIDString::dataTypeToString(quint8 dataType)
{   // Table A-15
        switch(dataType)
        {
        // Signed types
        case E120_DS_SIGNED_BYTE : return QString("8 bit signed. int8_t");
        case E120_DS_SIGNED_WORD : return QString("16 bit signed. int16_t");
        case E120_DS_SIGNED_DWORD : return QString("32 bit signed. int32_t");
                // Unsigned types
        case E120_DS_UNSIGNED_BYTE : return QString("8 bit unsigned. uint8_t");
        case E120_DS_UNSIGNED_WORD : return QString("16 bit unsigned. uint16_t");
        case E120_DS_UNSIGNED_DWORD : return QString("32 bit unsigned. uint32_t");
                // Non-numeric types
        case E120_DS_NOT_DEFINED : return QString("Undefined");
        case E120_DS_BIT_FIELD : return QString("Bitfield");
        case E120_DS_ASCII :  return QString("ASCII text");
        default :
                if ((dataType > 0x7F) && (dataType < 0xE0))
                {
                        return QString("Manufacturer-Specific");
                }
        }
        return QString("INVALID");
}
