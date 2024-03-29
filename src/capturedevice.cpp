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

#include "capturedevice.h"
#include "GadgetDLL.h"
#include "ftd2xx.h"
#include "whip/ftdcomm.h"
#include "rdm/estardm.h"

#include <QThread>
#include <QTimer>
#include <QDir>
#include "logmodel.h"

/******************************************** ICaptureDevice *****************************/

ICaptureDevice::ICaptureDevice(const CaptureDeviceList::CaptureDeviceInfo &info) :
    m_mode(SniffMode),
    m_info(info),
    m_deviceOpen(false)
{
    memset(m_txLevels, 0,  sizeof(m_txLevels));
}

QList<Packet> ICaptureDevice::getPackets()
{
    m_listMutex.lock();
    QList<Packet> result = m_packetList;
    m_packetList.clear();
    m_listMutex.unlock();
    return result;
}

void ICaptureDevice::addPacket(const Packet &packet)
{
    m_listMutex.lock();
    m_packetList << packet;
    m_listMutex.unlock();
}

void ICaptureDevice::setDmxLevels(quint8 *levels, size_t length)
{
    m_listMutex.lock();
    Q_ASSERT(length <= 512);
    memcpy(m_txLevels, levels, length);
    m_listMutex.unlock();
}


/************************************** WhipCaptureDevice ********************************/

WhipCaptureDevice::WhipCaptureDevice(const CaptureDeviceList::CaptureDeviceInfo &info) : ICaptureDevice(info),
    m_rxTimer(Q_NULLPTR),
    m_comm(new FTDComm)
{
    m_deviceNum = static_cast<int>(info.index);
}

bool WhipCaptureDevice::open()
{
    FTDCommError Result = m_comm->Open(m_deviceNum);
    LogModel::log(QString("Opening USB whip device %1").arg(m_deviceNum),
                  CDL_SEV_INF,
                  1);

    if(Result != FTDCOMM_OK)
        return false;

    Result = m_comm->SetupDevice(250000);
    if(Result != FTDCOMM_OK)
        return false;

    LogModel::log(QString("Starting Whip RX Timer for device %1").arg(m_deviceNum),
                  CDL_SEV_INF,
                  1);

    if(!m_rxTimer)
    {
        m_rxTimer = new QTimer(this);
        connect(m_rxTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
        m_rxTimer->start(100);
    }

    m_deviceOpen = true;
    emit sniffing();
    return true;
}

void WhipCaptureDevice::close()
{
    m_comm->Close();
    m_deviceOpen = false;
    emit closed();
}


#define RX_BUF_SIZE 65536
void WhipCaptureDevice::onTimer()
{
    if(m_mode == SniffMode)
    {
        unsigned short RXBuffer[RX_BUF_SIZE];
        int length = m_comm->ReceiveData(RXBuffer, RX_BUF_SIZE/2);
        if(!length) return;

        for(int i=0; i<length; i++)
        {
            if((RXBuffer[i]==0xbb00) & (m_packetBuffer.length()>0))
            {
                // Break - end of packet
                addPacket(m_packetBuffer);
                m_packetBuffer.clear();
            }
            else
            {
                m_packetBuffer.append(static_cast<char>(RXBuffer[i] & 0xFF));
            }
        }

        if(length>0)
            emit packetsReady();
    }
    else if(m_enabled)
    {
        m_listMutex.lock();
        m_comm->SendBreakStartAndData(0, m_txLevels, 512);
        m_listMutex.unlock();
    }
}



/***************************** GadgetCaptureDevice ****************************************/

void __stdcall GadgetUpdateCallback(Gadget2_UpdateStatus status, void* context)
{
    GadgetCaptureDevice *device = static_cast<GadgetCaptureDevice *>(context);
    device->handleGadgetUpdate(status);
}


GadgetCaptureDevice::GadgetCaptureDevice(const CaptureDeviceList::CaptureDeviceInfo &info) : ICaptureDevice(info)
{
    m_deviceNum = m_info.index;
    m_port = m_info.port;
    m_rxTimer = Q_NULLPTR;
    m_gadgetReadThread = new QThread();
    this->moveToThread(m_gadgetReadThread);

    Gadget2_Connect();
    Gadget2_SetUpdateStatusCallback(GadgetUpdateCallback, static_cast<void*>(this));
    m_gadgetReadThread->start();
}

GadgetCaptureDevice::~GadgetCaptureDevice()
{
    m_gadgetReadThread->quit();
    m_gadgetReadThread->wait();
    Gadget2_Disconnect();
}

void GadgetCaptureDevice::readData()
{
    unsigned int numBytes = Gadget2_GetNumberOfRXRawBytes(m_deviceNum, m_port);
    if(numBytes>0)
    {
        quint16* buffer = new quint16[numBytes];
        Gadget2_GetRXRawBytes(m_deviceNum, m_port, buffer, numBytes);
        LogModel::log(QString("Gadget received %1 bytes").arg(numBytes),
                      CDL_SEV_INF,
                      5);

        for(unsigned int i = 0; i < numBytes; ++i)
        {
            if(buffer[i]==0x8000)
            {
                // Break - end of packet
                addPacket(m_packetBuffer);
                m_packetBuffer.clear();
            }
            else
            {
                m_packetBuffer.append(static_cast<char>(buffer[i] & 0xFF));
            }
        }
        delete[] buffer;

        emit packetsReady();
    }
}

bool GadgetCaptureDevice::open()
{
    switch (m_mode)
    {
        case ICaptureDevice::SniffMode:
            QMetaObject::invokeMethod(this, "startReading");
            return true;
        case ICaptureDevice::TransmitMode:
            QMetaObject::invokeMethod(this, "startSending");
            return true;
        case ICaptureDevice::ControllerMode:
            emit transmitting();
            return true;
    }
    return false;
}

void GadgetCaptureDevice::startReading()
{
    Gadget2_SetRawReceiveMode(m_deviceNum, m_port);
    m_rxTimer = new QTimer(this);
    m_rxTimer->setInterval(0);
    connect(m_rxTimer, SIGNAL(timeout()), this, SLOT(readData()));
    m_rxTimer->start();

    m_deviceOpen = true;
    emit sniffing();
}

void GadgetCaptureDevice::startSending()
{
    m_rxTimer = new QTimer(this);
    m_rxTimer->setInterval(100);
    connect(m_rxTimer, SIGNAL(timeout()), this, SLOT(sendData()));
    m_rxTimer->start();

    m_deviceOpen = true;
    emit transmitting();
}

void GadgetCaptureDevice::stopSending()
{
    if(m_rxTimer)
    {
        m_rxTimer->stop();
        m_rxTimer->deleteLater();
        m_rxTimer = Q_NULLPTR;
    }
    Gadget2_DisableDMX(m_deviceNum, m_port);

    m_deviceOpen = false;
    emit closed();
}

void GadgetCaptureDevice::sendData()
{
    if(m_enabled)
        Gadget2_SendDMX(m_deviceNum, m_port, m_txLevels, 512);
}

void GadgetCaptureDevice::setDmxEnabled(bool enabled)
{
    m_enabled = enabled;
    if(!enabled)
        Gadget2_DisableDMX(m_deviceNum, m_port);
}

void GadgetCaptureDevice::stopReading()
{
    Gadget2_SendRDMCommand(m_deviceNum, m_port, E120_SET_COMMAND, E120_IDENTIFY_DEVICE, 0x00, 0, Q_NULLPTR, 0x6574, 0x01); // Dummy RDM command to stop recieving
    if(m_rxTimer)
    {
        m_rxTimer->stop();
        m_rxTimer->deleteLater();
        m_rxTimer = Q_NULLPTR;
    }
}

void GadgetCaptureDevice::close()
{
    QMetaObject::invokeMethod(this, "stopReading");
    QMetaObject::invokeMethod(this, "stopSending");
}

void GadgetCaptureDevice::doDiscovery()
{
    Gadget2_DoFullDiscovery(m_deviceNum, m_port);
    QTimer::singleShot(15000, this, SLOT(discoveryFinished()));
}

void GadgetCaptureDevice::discoveryFinished()
{
    m_infoList.clear();
    for(unsigned int i = 0; i < Gadget2_GetDiscoveredDevices(); i++)
        m_infoList << Gadget2_GetDeviceInfo(i);

    emit discoveryDataReady();
}


void GadgetCaptureDevice::updateFirmware(const QString &firmwarePath)
{
    if(QThread::currentThread() == this->thread())
    {
        QString nativePath = QDir::toNativeSeparators(firmwarePath);
        wchar_t *filename = new wchar_t[nativePath.length() + 1];
        wmemset(filename, 0, nativePath.length() + 1);
        nativePath.toWCharArray(filename);
        Gadget2_PerformFirmwareUpdate(m_deviceNum, filename);
        delete[] filename;
    }
    else {
        QMetaObject::invokeMethod(this, "updateFirmware", Q_ARG(QString, firmwarePath));
    }
}

void GadgetCaptureDevice::handleGadgetUpdate(Gadget2_UpdateStatus status)
{
    switch(status)
    {
    case Gadget2_Update_Beginning:
        emit updateProgressText(tr("Beginning Gadget2 Update - setting device into Bootloader"));
        break;
    case Gadget2_Update_BootloaderFound:
        emit updateProgressText(tr("Found Gadget2 in Bootloader mode"));
        break;
    case Gadget2_Update_TransferringFile:
        emit updateProgressText(tr("Transferring update file to Gadget2"));
        break;
    case Gadget2_Update_ReadyForReboot:
        emit updateProgressText(tr("Gadget2 update Complete. Please power cycle the device and retstart the appliction. ETCDMXTool will now exit"));
        emit updateComplete();
        break;
    case Gadget2_Update_Error:
        emit updateProgressText(tr("Gadget2 update failed. Please power cycle the device, restart the appliction, and try again. ETCDMXTool will now exit"));
        emit updateComplete();
        break;
    }
}

/**************************** CaptureDeviceList ****************************/

CaptureDeviceList::CaptureDeviceList()
{
    int result = Gadget2_Connect();
    // Wait for the threads to start up to discover gadgets
    QThread::msleep(3000);

    if(result==1)
    {
        for(unsigned int i=0; i < Gadget2_GetNumGadgetDevices(); i++)
        {
            // Assume two ports per gadget
            // TODO IO cards with more ports..
            for(unsigned int port=1; port<=Gadget2_GetPortCount(i); port++)
            {
                const QString version = QString::fromUtf8(Gadget2_GetGadgetVersion(i));
                const unsigned int serial = Gadget2_GetGadgetSerialNumber(i);
                const QString deviceType = QString::fromUtf8(Gadget2_GetGadgetType(i));

                CaptureDeviceInfo info;
                info.type = DEVTYPE_GADGET;
                info.index = i;
                info.port = port;
                info.description = QString("%1 S/N %2 (v%3) - Port %4")
                        .arg(deviceType)
                        .arg(serial)
                        .arg(version)
                        .arg(port);
                info.deviceCapabilities = CAPABILITY_CONTROLLER | CAPABILITY_DMX_SENDER;
                // Only Gadget2 v1.2 and above can sniff...
                if (
                    deviceType == QLatin1String("Gadget 2") &&
                    !version.startsWith(QLatin1String("1.0.")) &&
                    !version.startsWith(QLatin1String("1.1."))
                    ) {
                    info.deviceCapabilities |= CAPABILITY_SNIFFER;
                }
                m_devList << info;
            }
        }
    }

    Gadget2_Disconnect();

    FTDComm::FtdiDevice SelectedDevice;
    int TempNumDevices = 0;

    if(FT_ListDevices(&TempNumDevices, Q_NULLPTR, FT_LIST_NUMBER_ONLY) != FT_OK)
    {
        return;
    }

    QList<FTDComm::FtdiDevice> devices = m_comm.GetUsbWhips();
    int i=0;
    foreach(FTDComm::FtdiDevice device, devices)
    {
        CaptureDeviceInfo info;
        info.type = DEVTYPE_WHIP;
        info.index = device.deviceNum;
        info.description = QStringLiteral("Whip S/N %1 - %2").arg(device.serialNum).arg(device.description);
        info.deviceCapabilities = CAPABILITY_DMX_SENDER | CAPABILITY_SNIFFER;

        m_devList << info;
        i++;
    }
}

QStringList CaptureDeviceList::deviceNames()
{
    QStringList result;
    foreach(CaptureDeviceInfo i, m_devList)
        result << i.description;
    return result;
}

ICaptureDevice *CaptureDeviceList::getDevice(int deviceIndex)
{
    if(deviceIndex>=m_devList.count())
        return Q_NULLPTR;

    CaptureDeviceInfo info = m_devList[deviceIndex];

    if(info.type==DEVTYPE_GADGET)
    {
        GadgetCaptureDevice *device = new GadgetCaptureDevice(info);
        return device;
    }

    if(info.type==DEVTYPE_WHIP)
    {
        WhipCaptureDevice *device = new WhipCaptureDevice(info);
        return device;
    }

    return Q_NULLPTR;
}
