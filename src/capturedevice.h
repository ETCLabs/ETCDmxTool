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

#ifndef CAPTUREDEVICE_H
#define CAPTUREDEVICE_H
#include <QObject>
#include <QMutex>
#include "whip/ftdcomm.h"
#include "packetbuffer.h"
#include "etc_include/RdmDeviceInfo.h"
#include "GadgetDLL.h"


class ICaptureDevice;

// A class which lists all the supported capture devices
class CaptureDeviceList
{
public:

    enum DevType{
        DEVTYPE_WHIP,
        DEVTYPE_GADGET
    };

    enum DeviceCapability {
        CAPABILITY_SNIFFER = 0x20,
        CAPABILITY_CONTROLLER = 0x40,
        CAPABILITY_DMX_SENDER = 0x80
    };

    struct CaptureDeviceInfo {
        DevType type;
        int index;
        QString description;
        unsigned int port;
        int deviceCapabilities;
    };

    CaptureDeviceList();

    QStringList deviceNames();
    int count() const { return m_devList.count();}
    ICaptureDevice *getDevice(int index);

private:

    QList<CaptureDeviceInfo> m_devList;


    FTDComm m_comm;
};


class ICaptureDevice : public QObject
{
    Q_OBJECT
public:
    enum CaptureDeviceMode {
        SniffMode,
        TransmitMode,
        ControllerMode
    };
    ICaptureDevice(const CaptureDeviceList::CaptureDeviceInfo &info);
    virtual bool open() = 0;
    bool isOpen() const { return m_deviceOpen; }
    virtual void close() = 0;
    virtual void setMode(CaptureDeviceMode mode) { m_mode = mode;}
    virtual CaptureDeviceMode getMode() { return m_mode; }
    virtual void setDmxLevels(quint8 *levels, size_t length);
    QList<Packet> getPackets();
    QString description() const { return m_info.description;}
    const CaptureDeviceList::CaptureDeviceInfo info() const { return m_info;}
signals:
    void packetsReady();
    void closed();
    void sniffing();
    void transmitting();
protected:
    void addPacket(const Packet &packet);
    CaptureDeviceMode m_mode;
    QList<Packet> m_packetList;
    QMutex m_listMutex;
    quint8 m_txLevels[513];
    CaptureDeviceList::CaptureDeviceInfo m_info;
    bool m_deviceOpen;
};

class WhipCaptureDevice : public ICaptureDevice
{
    Q_OBJECT
public:
    WhipCaptureDevice(const CaptureDeviceList::CaptureDeviceInfo &info);
    bool open();
    void close();
private slots:
    void onTimer();
private:
    QTimer *m_rxTimer;
    int m_deviceNum;
    FTDComm *m_comm;
    QByteArray m_packetBuffer;
};

class GadgetCaptureDevice : public ICaptureDevice
{
    Q_OBJECT
public:
    GadgetCaptureDevice(const CaptureDeviceList::CaptureDeviceInfo &info);
    virtual ~GadgetCaptureDevice();
    bool open();
    void close();
    QList<RdmDeviceInfo *> getDeviceInfo() { return m_infoList;}
    void handleGadgetUpdate(Gadget2_UpdateStatus status);
public slots:
    void doDiscovery();
    void updateFirmware(const QString &firmwarePath);
signals:
    void discoveryDataReady();
    void updateProgressText(QString text);
    void updateComplete();
private slots:
    void readData();
    void sendData();
    void startReading();
    void startSending();
    void stopSending();
    void stopReading();
    void discoveryFinished();
private:
    QTimer *m_rxTimer;
    unsigned int m_deviceNum;
    unsigned int m_port;
    QThread *m_gadgetReadThread;
    QByteArray m_packetBuffer;
    QList<RdmDeviceInfo *> m_infoList;
};


#endif // CAPTUREDEVICE_H
