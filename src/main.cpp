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


#include "mainwindow.h"
#include <QApplication>
#include <QtGlobal>
#include <QStyleFactory>
#include "fancysliderstyle.h"
#include "selectdevicedialog.h"
#include "capturedevice.h"
#include "logmodel.h"
#include "commandlineparse.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    quint32 severity = CDL_SEV_UKN;
    switch (type) {
        case QtDebugMsg: severity = CDL_SEV_INF; break;
        case QtInfoMsg:  severity = CDL_SEV_INF; break;
        case QtWarningMsg: severity = CDL_SEV_WRN; break;
        case QtCriticalMsg: severity = CDL_SEV_ERR; break;
        case QtFatalMsg:  severity = CDL_SEV_ERR; break;
    }

    LogModel::log(msg, severity, 1);
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<QVector <quint16>>("QVector<quint16>");

    qInstallMessageHandler(messageHandler);

	QApplication a(argc, argv);
    a.setApplicationName("EtcDmxTool");
    a.setOrganizationName("ETC Inc");
    a.setApplicationVersion(VERSION);

    // Command line options
    auto cmdParse = new commandLineParse(a);
    switch (cmdParse->getResult())
    {
    case commandLineParse::ExitApp: a.quit(); return 0;
    case commandLineParse::ConsoleOnly: return a.exec();
    case commandLineParse::GuiOnly: break;
    }

    // Open GUI

    QProxyStyle *style = new FancyFaderStyle;
    style->setBaseStyle(QStyleFactory::create("Fusion"));
    a.setStyle(style);

    // Startup logging
    LogModel::getInstance();
    LogModel::log(QString("Application Starting, version %1").arg(VERSION)
                  , CDL_SEV_INF
                  , 1);

    // First, determine which USB-RDM Adapter to use
    ICaptureDevice *device = Q_NULLPTR;

    // Show the whip selection dialog
    SelectDeviceDialog dialog;

    int result = dialog.exec();

    if(result==QDialog::Accepted)
    {
        device =  dialog.selectedDevice();
    }

    MainWindow w(device);
    w.showMaximized();
    return a.exec();
}
