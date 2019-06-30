#include "commandlineparse.h"
#include "capturedevice.h"
#include "file.h"
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

commandLineParse::commandLineParse(const QCoreApplication &app, QObject *parent) : QObject(parent)
{
    parser.setApplicationDescription(
                QString("%1 (%2) by %3")
                .arg(QCoreApplication::applicationName())
                .arg(QCoreApplication::applicationVersion())
                .arg(QCoreApplication::organizationName())
    );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(optSniff);
    parser.addOption(optCompress);

    parser.process(app);

    // Sniffer
    if (parser.isSet(optSniff)) doSniff();

}

commandLineParse::~commandLineParse()
{}

void commandLineParse::doSniff()
    {
    QString fileName = parser.value(optSniff);
    if (fileName.isEmpty()) return;

    parseResult = HideGui;
    auto devList = new CaptureDeviceList();
    if (!devList->count())
    {
        qStdErr() << "No sniffing devices found, aborting" << endl;
        parseResult = ExitApp;
        return;
    }

        ICaptureDevice *captureDevice = Q_NULLPTR;
        for (int n = 0; n < devList->count(); n++)
        {
            if (devList->getDevice(n)->info().deviceCapabilities & CaptureDeviceList::CAPABILITY_SNIFFER)
            {
                captureDevice = devList->getDevice(n);
                qStdOut() << "Using device: " << captureDevice->info().description << endl;
            }
            if (captureDevice) break;
        }
        delete devList;

        if (!captureDevice)
        {
            qStdErr() << "No suitable sniffing devices found, aborting" << endl;
            parseResult = ExitApp;
            return;
        }

        if (!captureDevice->open())
        {
            qStdErr() << "Unable to open sniffing device, aborting" << endl;
            parseResult = ExitApp;
            return;
        }

        QObject::connect(captureDevice, &ICaptureDevice::packetsReady, [=]() {
            QList<Packet> packets = captureDevice->getPackets();
            for(auto p: packets)
            {
                if(p.length()>0) {
                    p.timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
                    packetTable.appendPacket(p);
                }
            }
        });
        FileSave *fileSave = new FileSave(
                    packetTable, fileName,
                    parser.isSet(optCompress) ? FileSave::compressed : FileSave::original,
                    FileSave::StreamToFile);
        if (!fileSave)
        {
            qStdErr() << fileName << "is not writable" << endl;
            captureDevice->close();
            parseResult = ExitApp;
            return;
        }

        qStdOut() << "Sniffing direct to: " << fileName << endl;
}
