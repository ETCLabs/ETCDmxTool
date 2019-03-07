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

#ifndef RDMCONTROLLER_H
#define RDMCONTROLLER_H

#include <QObject>
#include <QHash>
#include "capturedevice.h"
#include "logmodel.h"

class RDM_CmdC;

struct RDMSensor
{
    quint8 number;
    quint8 type;
    quint8 unit;
    quint8 prefix;
    quint16 range_min;
    quint16 range_max;
    quint16 normal_min;
    quint16 normal_max;
    quint8 recorded_value_support;
    QString description;
};

struct RDMSensorValue
{
    quint8 number;
    quint16 value;
    quint16 lowestValue;
    quint16 highestValue;
    quint16 recordedValue;
};

typedef QMap<int, RDMSensor> RDMSensorList;
typedef QMap<int, RDMSensorValue> RDMSensorValueList;

class RDMPersonalityList
{
public:
    void setPersonalityName(int index, const QString &name) { m_personalityNames[index] = name;}
    void setPersonalityFootprint(int index, int footprint) { m_personalityFootprints[index] = footprint;}

    QString getPersonalityName(int index) { if(m_personalityNames.contains(index)) return m_personalityNames[index];
                                          return QString("Unknown");}
    int getPersonalityFootprint(int index) { if(m_personalityFootprints.contains(index)) return m_personalityFootprints[index];
        return 0;}
private:
    QHash<int, QString> m_personalityNames;
    QHash<int, int> m_personalityFootprints;
};

class RDMController : public QObject
{
    Q_OBJECT
public:
    explicit RDMController(GadgetCaptureDevice *device,QObject *parent = nullptr);
    QList<RdmDeviceInfo *> deviceList() { return m_devices;}
    QString getDeviceName(quint32 deviceId);
    QString getManufacturerName(quint32 deviceId);
    RDMPersonalityList getPersonalityList(quint32 deviceId);
    QList<quint16> getSupportedParameters(quint32 deviceId);
    RDMSensorList getSensorList(quint32 deviceId);
    RDMSensorValueList getSensorValues(quint32 deviceId);
    void executeCustomCommand(RDM_CmdC *command);
signals:
    void discoveryStarted();
    void discoveryFinished();
    void gotSensorValues();
    void customCommandComplete(quint8 response, const QByteArray &data);
public slots:
    void startDiscovery();
    void fetchSensorValues(RdmDeviceInfo *info);
private slots:
    void advanceDiscoveryStateMachine();
    void readSensorValues();
    void readCustomCommand();
private:
    GadgetCaptureDevice *m_gadget;
    enum DiscoveryState {
        DISC_IDLE,
        DISC_START_DISCOVERY,
        DISC_END_DISCOVERY,
        DISC_GET_NAME,
        DISC_GET_PERSONALITY_DESC,
        DISC_GET_MANUF_LABEL,
        DISC_GET_SUPPORTED_PARAMETERS,
        DISC_GET_SENSOR_DEFINITION
    };
    DiscoveryState m_discoveryState;
    QList <RdmDeviceInfo *> m_devices;
    QHash <quint64, QString> m_deviceLabels;
    QHash <quint64, QString> m_manufacturerLabels;
    QHash <quint64, RDMPersonalityList> m_personalityLists;
    QHash <quint64, QList<quint16> > m_supportedParams;
    QHash <quint64, RDMSensorList > m_sensorDefinitions;
    QHash <quint64, RDMSensorValueList > m_sensorValues;
    int m_currentDevice;
    bool m_runningCustomCommand;
    quint16 m_currentCustomParameter;
};

#endif // RDMCONTROLLER_H
