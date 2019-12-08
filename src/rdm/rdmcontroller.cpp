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

#include "rdmcontroller.h"
#include "etc_include/RdmDeviceInfo.h"
#include "etc_include/RDM_CmdC.h"
#include "GadgetDLL.h"
#include "rdm/estardm.h"
#include "util.h"
#include <QTimer>

quint16 rdm_unpack_u16(const void *data, int index)
{
    const quint8 *bytes = static_cast<const quint8 *>(data);
    quint16 value = bytes[index];
    value = value << 8;
    value = value | bytes[index+1];
    return value;
}

quint8 rdm_unpack_u8(const void *data, int index)
{
    const quint8 *bytes = static_cast<const quint8 *>(data);
    quint8 value = bytes[index];
    return value;
}

QString rdm_unpack_string(const void*data, int index, int length)
{
    const char *bytes = static_cast<const char *>(data);

    QString result = QString::fromLatin1(&bytes[index], length-index);

    return result;
}


RDMController::RDMController(GadgetCaptureDevice *device, QObject *parent) : QObject(parent)
{
    m_gadget = device;
    m_runningCustomCommand = false;
}

void RDMController::startDiscovery()
{
    m_devices.clear();
    m_discoveryState = DISC_START_DISCOVERY;
    advanceDiscoveryStateMachine();
}

void RDMController::advanceDiscoveryStateMachine()
{
    int cmdTot = 0;

    switch(m_discoveryState)
    {
    case DISC_START_DISCOVERY:
            LogModel::log("Beginning RDM Discovery..", CDL_SEV_INF, 1);
            LogModel::log("Waiting 10s for device discovery", CDL_SEV_INF, 1);
            Gadget2_DoFullDiscovery(m_gadget->info().index, m_gadget->info().port);
            QTimer::singleShot(10000, this, SLOT(advanceDiscoveryStateMachine()));
            m_discoveryState = DISC_END_DISCOVERY;
            emit discoveryStarted();
            return;
    case DISC_END_DISCOVERY:
            // Copy the discovered things to our list
            LogModel::log("Finished Discovery", CDL_SEV_INF, 1);
            LogModel::log(tr("Discovered %1 Devices").arg(Gadget2_GetDiscoveredDevices()), CDL_SEV_INF, 1);
            for(int i=0; i<Gadget2_GetDiscoveredDevices(); i++)
            {
                RdmDeviceInfo *info = Gadget2_GetDeviceInfo(i);
                m_devices << info;
                LogModel::log(tr("Device %1 at index %2")
                         .arg(Util::formatRdmUid(info->manufacturer_id, info->device_id))
                         .arg(i),
                              CDL_SEV_INF, 1);
            }
            m_currentDevice = 0;
            m_discoveryState = DISC_GET_NAME;
            QTimer::singleShot(500, this, SLOT(advanceDiscoveryStateMachine()));
            break;
    case DISC_GET_NAME:
        foreach(RdmDeviceInfo *info, m_devices)
        {

            LogModel::log(tr("Request DEVICE_LABEL for %1")
                     .arg(Util::formatRdmUid(info->manufacturer_id, info->device_id)),
                           CDL_SEV_INF, 1);
            Gadget2_SendRDMCommand(m_gadget->info().index,
                               m_gadget->info().port,
                               E120_GET_COMMAND,
                               E120_DEVICE_LABEL,
                               0,
                               0,
                               Q_NULLPTR,
                               info->manufacturer_id,
                               info->device_id
                               );
         }
         m_discoveryState = DISC_GET_PERSONALITY_DESC;
         QTimer::singleShot(1000 * m_devices.count(), this, SLOT(advanceDiscoveryStateMachine()));
         break;
    case DISC_GET_PERSONALITY_DESC:
         LogModel::log(tr("Finished getting  DEVICE_LABELS, %1 responses").arg(Gadget2_GetNumResponses()),
                       CDL_SEV_INF,
                       1);
        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
        {
            RDM_CmdC *resp = Gadget2_GetResponse(i);
            if(resp->getResponseType() == E120_RESPONSE_TYPE_ACK)
            {
                QString label = QString::fromLatin1(static_cast<const char*>(resp->getBuffer()), resp->getLength());
                m_deviceLabels[resp->getDeviceId()] = label;
                LogModel::log(tr("%1 : DEVICE_LABEL is %2")
                         .arg(Util::formatRdmUid(resp->getManufacturerId(), resp->getDeviceId()))
                         .arg(label),
                              CDL_SEV_INF,
                              1);
            }
            Gadget2_ClearResponse(i);
        }

        foreach(RdmDeviceInfo *info, m_devices)
        {

            // Personalities are indexed 1-n
            for(quint8 pers_index=1; pers_index<=(info->dmx_personality & 0x00FF); pers_index++)
            {

                 LogModel::log(tr("Request DMX_PERSONALITY_DESCRIPTION for %1, personality %2")
                         .arg(Util::formatRdmUid(info->manufacturer_id, info->device_id))
                         .arg(pers_index),
                               CDL_SEV_INF,
                               1);
                cmdTot++;
                Gadget2_SendRDMCommand(m_gadget->info().index,
                                   m_gadget->info().port,
                                   E120_GET_COMMAND,
                                   E120_DMX_PERSONALITY_DESCRIPTION,
                                   0,
                                   1,
                                   reinterpret_cast<char*>(&pers_index),
                                   info->manufacturer_id,
                                   info->device_id
                                   );
            }
         }
        m_discoveryState = DISC_GET_MANUF_LABEL;
        QTimer::singleShot(100 * cmdTot, this, SLOT(advanceDiscoveryStateMachine()));
        break;
    case DISC_GET_MANUF_LABEL:
        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
        {
            RDM_CmdC *resp = Gadget2_GetResponse(i);
            if(resp->getResponseType() == E120_RESPONSE_TYPE_ACK  && resp->getParameter() == E120_DMX_PERSONALITY_DESCRIPTION)
            {
                const quint8 *pers_desc = static_cast<const quint8*>(resp->getBuffer());
                quint8 pers_index = pers_desc[0];
                quint16 pers_footprint = (pers_desc[1] << 8) & pers_desc[2];
                const char *namePtr = reinterpret_cast<const char*>(&pers_desc[3]);
                QString pers_name = QString::fromLatin1(namePtr, resp->getLength() - 3);

                if(!m_personalityLists.contains(resp->getDeviceId()))
                    m_personalityLists[resp->getDeviceId()] = RDMPersonalityList();
                m_personalityLists[resp->getDeviceId()].setPersonalityFootprint(pers_index, pers_footprint);
                m_personalityLists[resp->getDeviceId()].setPersonalityName(pers_index, pers_name);

                 LogModel::log(tr("%1 : Got Personality Info for personality %2 : Name %3, Footprint %4")
                         .arg(Util::formatRdmUid(resp->getManufacturerId(), resp->getDeviceId()))
                         .arg(pers_index)
                         .arg(pers_name)
                         .arg(pers_footprint),
                               CDL_SEV_INF,
                               1);

            }
        }
        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
            Gadget2_ClearResponse(i);


        foreach(RdmDeviceInfo *info, m_devices)
        {
             LogModel::log(tr("Request MANUFACTURER_LABEL for %1")
                         .arg(Util::formatRdmUid(info->manufacturer_id, info->device_id)),
                           CDL_SEV_INF,
                           1);
            Gadget2_SendRDMCommand(m_gadget->info().index,
                               m_gadget->info().port,
                               E120_GET_COMMAND,
                               E120_MANUFACTURER_LABEL,
                               0,
                               0,
                               Q_NULLPTR,
                               info->manufacturer_id,
                               info->device_id
                               );

         }
        m_discoveryState = DISC_GET_SUPPORTED_PARAMETERS;
        QTimer::singleShot(1000 * m_devices.count(), this, SLOT(advanceDiscoveryStateMachine()));
        break;

    case DISC_GET_SUPPORTED_PARAMETERS:
        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
        {
            RDM_CmdC *resp = Gadget2_GetResponse(i);
            if(resp->getResponseType() == E120_RESPONSE_TYPE_ACK && resp->getParameter() == E120_MANUFACTURER_LABEL)
            {
                QString label = QString::fromLatin1(static_cast<const char*>(resp->getBuffer()), resp->getLength());
                m_manufacturerLabels[resp->getDeviceId()] = label;
                LogModel::log(tr("%1 : MANUFACTURER_LABEL is %2")
                         .arg(Util::formatRdmUid(resp->getManufacturerId(), resp->getDeviceId()))
                         .arg(label),
                              CDL_SEV_INF,
                              1);
            }
        }


        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
            Gadget2_ClearResponse(i);

        foreach(RdmDeviceInfo *info, m_devices)
        {
             LogModel::log(tr("Request SUPPORTED_PARAMETERS for %1")
                         .arg(Util::formatRdmUid(info->manufacturer_id, info->device_id)),
                           CDL_SEV_INF,
                           1);
            Gadget2_SendRDMCommand(m_gadget->info().index,
                               m_gadget->info().port,
                               E120_GET_COMMAND,
                               E120_SUPPORTED_PARAMETERS,
                               0,
                               0,
                               Q_NULLPTR,
                               info->manufacturer_id,
                               info->device_id
                               );

         }
        m_discoveryState = DISC_GET_SENSOR_DEFINITION;
        QTimer::singleShot(1000 * m_devices.count(), this, SLOT(advanceDiscoveryStateMachine()));
        break;
    case DISC_GET_SENSOR_DEFINITION:

        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
        {
            RDM_CmdC *resp = Gadget2_GetResponse(i);
            if(resp->getResponseType() == E120_RESPONSE_TYPE_ACK && resp->getParameter() == E120_SUPPORTED_PARAMETERS)
            {
                QString supportedParams;
                QList<quint16> supportedParamValues;
                const quint8 *buffer = static_cast<const quint8*>(resp->getBuffer());
                for(int index=0; index<resp->getLength(); index+=2)
                {
                    quint16 value = ((quint16)buffer[index] << 8);
                    value = value | buffer[index+1];
                    QString text;
                    if(index>0) text = QString(", ");
                    text += Util::paramIdToString(value);
                    supportedParams += text;
                    supportedParamValues << value;
                }

                 LogModel::log(tr("%1 : SUPPORTED_PARAMETERS are %2")
                         .arg(Util::formatRdmUid(resp->getManufacturerId(), resp->getDeviceId()))
                         .arg(supportedParams),
                               CDL_SEV_INF,
                               1);

                m_supportedParams[resp->getDeviceId()] = supportedParamValues;
            }
        }

        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
            Gadget2_ClearResponse(i);

        foreach(RdmDeviceInfo *info, m_devices)
        {
            for(quint8 sensor=0; sensor<info->sensor_count; sensor++)
            {
                LogModel::log(tr("Request SENSOR_DEFINITION for %1, sensor %2")
                         .arg(Util::formatRdmUid(info->manufacturer_id, info->device_id))
                         .arg(sensor),
                         CDL_SEV_INF,
                            1);
                Gadget2_SendRDMCommand(m_gadget->info().index,
                               m_gadget->info().port,
                               E120_GET_COMMAND,
                               E120_SENSOR_DEFINITION,
                               0,
                               1,
                               reinterpret_cast<char*>(&sensor),
                               info->manufacturer_id,
                               info->device_id
                               );
            }

         }
        m_discoveryState = DISC_IDLE;
        QTimer::singleShot(1000 * m_devices.count(), this, SLOT(advanceDiscoveryStateMachine()));
        break;


    case DISC_IDLE:
        for(uint i=0; i<Gadget2_GetNumResponses(); i++)
        {
            RDM_CmdC *resp = Gadget2_GetResponse(i);
            if(resp->getResponseType() == E120_RESPONSE_TYPE_ACK && resp->getParameter() == E120_SENSOR_DEFINITION)
            {
                RDMSensor sensor;
                sensor.number                   =       rdm_unpack_u8(resp->getBuffer(), 0);
                sensor.type                     =       rdm_unpack_u8(resp->getBuffer(), 1);
                sensor.unit                     =       rdm_unpack_u8(resp->getBuffer(), 2);
                sensor.prefix                   =       rdm_unpack_u8(resp->getBuffer(), 3);
                sensor.range_min                =       rdm_unpack_u16(resp->getBuffer(), 4);
                sensor.range_max                =       rdm_unpack_u16(resp->getBuffer(), 6);
                sensor.normal_min               =       rdm_unpack_u16(resp->getBuffer(), 8);
                sensor.normal_max               =       rdm_unpack_u16(resp->getBuffer(), 10);
                sensor.recorded_value_support   =       rdm_unpack_u8(resp->getBuffer(), 12);
                sensor.description              =       rdm_unpack_string(resp->getBuffer(), 13, resp->getLength());

                LogModel::log(tr("%1 : Sensor %2 is %3")
                         .arg(Util::formatRdmUid(resp->getManufacturerId(), resp->getDeviceId()))
                         .arg(sensor.number)
                         .arg(sensor.description),
                              CDL_SEV_INF,
                              1);

                m_sensorDefinitions[resp->getDeviceId()].insert(sensor.number, sensor);
            }
        }
        //Gadget2_ToggleRDMDiscovery(m_gadget->info().index, m_gadget->info().port, false);
        emit discoveryFinished();
        break;
    }
}


QString RDMController::getDeviceName(quint32 deviceId)
{
    if(m_deviceLabels.contains(deviceId))
    {
        return m_deviceLabels[deviceId];
    }

    return tr("Not Supported");
}

QString RDMController::getManufacturerName(quint32 deviceId)
{
    if(m_manufacturerLabels.contains(deviceId))
    {
        return m_manufacturerLabels[deviceId];
    }

    return tr("Not Supported");
}


RDMPersonalityList RDMController::getPersonalityList(quint32 deviceId)
{
    if(m_personalityLists.contains(deviceId))
        return m_personalityLists[deviceId];
    return RDMPersonalityList();
}

QList<quint16> RDMController::getSupportedParameters(quint32 deviceId)
{
    if(m_supportedParams.contains(deviceId))
        return m_supportedParams[deviceId];
    return QList<quint16>();
}

RDMSensorList RDMController::getSensorList(quint32 deviceId)
{
    if(m_sensorDefinitions.contains(deviceId))
        return m_sensorDefinitions[deviceId];

    return RDMSensorList();
}


RDMSensorValueList RDMController::getSensorValues(quint32 deviceId)
{
    if(m_sensorValues.contains(deviceId))
        return m_sensorValues[deviceId];

    return RDMSensorValueList();
}

void RDMController::fetchSensorValues(RdmDeviceInfo *info)
{
    const RDMSensorList sensors = getSensorList(info->device_id);


    foreach(RDMSensor sensor, sensors)
    {
        LogModel::log(tr("Request SENSOR_VALUE for %1, sensor %2")
                     .arg(Util::formatRdmUid(info->manufacturer_id, info->device_id))
                     .arg(sensor.number),
                      CDL_SEV_INF,
                      1);

            Gadget2_SendRDMCommand(m_gadget->info().index,
                           m_gadget->info().port,
                           E120_GET_COMMAND,
                           E120_SENSOR_VALUE,
                           0,
                           1,
                           reinterpret_cast<char*>(&sensor.number),
                           info->manufacturer_id,
                           info->device_id
                           );
        }

    QTimer::singleShot(500*sensors.count(), this, SLOT(readSensorValues()));
}

void RDMController::readSensorValues()
{
    for(uint i=0; i<Gadget2_GetNumResponses(); i++)
    {
        RDM_CmdC *resp = Gadget2_GetResponse(i);
        if(resp->getResponseType() == E120_RESPONSE_TYPE_ACK && resp->getParameter() == E120_SENSOR_VALUE)
        {
            RDMSensorValue value;
            value.number            = rdm_unpack_u8(resp->getBuffer(), 0);
            value.value             = rdm_unpack_u16(resp->getBuffer(), 1);
            value.lowestValue       = rdm_unpack_u16(resp->getBuffer(), 3);
            value.highestValue      = rdm_unpack_u16(resp->getBuffer(), 5);
            value.recordedValue     = rdm_unpack_u16(resp->getBuffer(), 7);
            m_sensorValues[resp->getDeviceId()][value.number] = value;

            LogModel::log(tr("Got SENSOR_VALUE for %1, sensor %2, value %3")
                     .arg(Util::formatRdmUid(resp->getManufacturerId(), resp->getDeviceId()))
                     .arg(value.number)
                     .arg(value.value),
                           CDL_SEV_INF,
                           1);
        }
    }

    emit gotSensorValues();

}

void RDMController::executeCustomCommand(RDM_CmdC *command)
{
    if(m_runningCustomCommand)
    {
        LogModel::log(tr("Error - already running a custom command"), CDL_SEV_INF, 1);
        return;
    }
    m_runningCustomCommand = true;
    m_currentCustomParameter = command->getParameter();
    LogModel::log(tr("Request %1 for %2")
                     .arg(command->getParameter())
                     .arg(Util::formatRdmUid(command->getManufacturerId(), command->getDeviceId())),
                  CDL_SEV_INF,
                  1);

            Gadget2_SendRDMCommand(m_gadget->info().index,
                           m_gadget->info().port,
                           command->getCommand(),
                           command->getParameter(),
                           command->getSubdevice(),
                           command->getLength(),
                           reinterpret_cast<const char*>(command->getBuffer()),
                           command->getManufacturerId(),
                           command->getDeviceId()
                           );

    QTimer::singleShot(1000, this, SLOT(readCustomCommand()));
}

void RDMController::readCustomCommand()
{
    int foundAt = -1;
    for(uint i=0; i<Gadget2_GetNumResponses(); i++)
    {
        RDM_CmdC *resp = Gadget2_GetResponse(i);
        if(resp->getParameter()==m_currentCustomParameter)
        {
            LogModel::log("Custom Command Complete", CDL_SEV_INF, 1);
            m_runningCustomCommand = false;
            emit customCommandComplete(resp->getResponseType(),
                                   QByteArray(static_cast<const char*>(resp->getBuffer()), resp->getLength()));
            foundAt = i;
        }
    }

    if (foundAt>-1) {
        Gadget2_ClearResponse(foundAt);
    }

    if(m_runningCustomCommand)
    {
        LogModel::log("Custom Command Timedout", CDL_SEV_INF, 1);
        m_runningCustomCommand = false;
    }
}
