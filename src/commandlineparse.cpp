#include "commandlineparse.h"
#include "capturedevice.h"
#include "file.h"
#include "logmodel.h"
#include <QTextStream>
#include <QDateTime>
#include <QDir>

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
    if (parser.isSet(optSniff))
    {
        showConsole();
        doSniff();
    }
}

commandLineParse::~commandLineParse()
{
#if defined(Q_OS_WIN)
    if (pStdout) fclose(pStdout);
    if (pStderr) fclose(pStderr);
#endif
}

void commandLineParse::showConsole()
{
    // Show a console
    #if defined(Q_OS_WIN)
        AllocConsole();
        AttachConsole(GetCurrentProcessId());

        freopen_s(&pStdout, "CONOUT$", "w", stdout);
        freopen_s(&pStderr, "CONOUT$", "w", stderr);
    #endif
}

void commandLineParse::doSniff()
{
    QFile f(parser.value(optSniff));
    QFileInfo fi(f);
    if (!f.open(QFile::WriteOnly))
    {
        LogModel::log(tr("%1 is not writable").arg(fi.absoluteFilePath()), CDL_SEV_ERR, 1);
        parseResult = ExitApp;
        return;
    }
    f.close();

    parseResult = ConsoleOnly;
    auto devList = new CaptureDeviceList();
    if (!devList->count())
    {
        LogModel::log(tr("No sniffing devices found, aborting"), CDL_SEV_ERR, 1);
        parseResult = ExitApp;
        return;
    }

        ICaptureDevice *captureDevice = Q_NULLPTR;
        for (int n = 0; n < devList->count(); n++)
        {
            if (devList->getDevice(n)->info().deviceCapabilities & CaptureDeviceList::CAPABILITY_SNIFFER)
            {
                captureDevice = devList->getDevice(n);
                LogModel::log(tr("Using device: %1").arg(captureDevice->info().description), CDL_SEV_INF, 1);
            }
            if (captureDevice) break;
        }
        delete devList;

        if (!captureDevice)
        {
            LogModel::log(tr("No suitable sniffing devices found, aborting"), CDL_SEV_ERR, 1);
            parseResult = ExitApp;
            return;
        }

        if (!captureDevice->open())
        {
            LogModel::log(tr("Unable to open sniffing device, aborting"), CDL_SEV_ERR, 1);
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
                    packetTable, fi.absoluteFilePath(),
                    parser.isSet(optCompress) ? FileSave::compressed : FileSave::original,
                    FileSave::StreamToFile);
        if (fileSave)
            LogModel::log(tr("Sniffing direct to: %1").arg(fi.absoluteFilePath()), CDL_SEV_INF, 1);
}
