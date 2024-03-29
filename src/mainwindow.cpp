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

#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QComboBox>
#include <QLabel>
#include <QTextStream>
#include <QTextCursor>
#include <QSortFilterProxyModel>
#include <QButtonGroup>
#include <QLineEdit>
#include <QSignalMapper>
#include <QDateTime>
#include <QListView>
#include <QStandardPaths>
#include <QDirIterator>
#include <QActionGroup>
#include <cmath>
#include <QThread>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QSettings>

#include <qscrollbar.h>
#include "fancysliderstyle.h"
#include "packettable.h"
#include "hexlineedit.h"
#include "file.h"
#include "capturedevice.h"
#include "customdataroles.h"
#include "levelindicator.h"
#include "updatedialog.h"
#include "serialdialog.h"

// PcapNg Export
#include "pcap/pcapng.h"

// RDM Controller
#include "rdm/rdmcontroller.h"
#include "rdm/estardm.h"
#include "rdm/rdmpidstrings.h"
#include "etc_include/RDM_CmdC.h"
#include "GadgetDLL.h"

// Logging
#include "logmodel.h"

// Scripting
#include "scripting.h"

#include "util.h"

// Available fade times, in msec
const QList<int> FADE_TIMES = {         500,     1000,   2000,   5000,   10000,  20000,  30000,  60000};
const QStringList FADE_TIME_DESCS = {   "0.5s",  "1s",   "2s",   "5s",   "10s",  "20s",  "30s",  "1min"};

// File Loading
bool loadGoddardDesigns(QTextStream &stream, const QString firstLine, PacketTable &packetTable);

// RDM Data Types
enum RDM_DATATYPES { RDMDATATYPE_UINT8, RDMDATATYPE_UINT16, RDMDATATYPE_STRING, RDMDATATYPE_HEX};
const QStringList RDM_DATATYPE_DESCS = {"Unsigned Int 8bit", "Unsigned Int 16bit", "String", "Hex Bytes"};


int packetOffsetToTextdisplayOffset(int packetpos)
{
	int result = 0;
	int line = packetpos / 16;
	int col = packetpos % 16;

	result =(54*line) +  6 + 3*col;

	return result;
}

QString prettifyHex(const QByteArray &data)
{
    QString result;
    for(int line=0; line<data.length(); line+=16)
    {
        QString text = QString("%1 ").arg(line, 4, 16, QChar('0')).toUpper();
        for(int i=0; i<16; i++)
            if(line+i<data.length())
                text.append(QString(" %1").arg(static_cast<unsigned char>( data[line+i]), 2, 16, QChar('0')).toUpper());

        // Fill out the last line
        if(text.length() < 52)
            text += QString(53 - text.length(), ' ');

        text.append("  ");

        for(int i=0; i<16; i++)
            if(line+i<data.length())
            {
                QChar c(data[line+i]);
                if(c.isPrint())
                    text.append(QString("%1").arg(c));
                else
                    text.append(QString("."));
            }

        text += "\r\n";
        result += text;
    }
    return result;
}




static MainWindow *mainWinPtr = Q_NULLPTR;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(strncmp(context.category, "js", 2)==0)
    {
        mainWinPtr->jsConsoleMessage(context, msg);
        return;
    }

    LogModel::getInstance()->log(msg, CDL_SEV_INF, 1);

    if(type==QtFatalMsg)
        abort();
}

MainWindow::MainWindow(ICaptureDevice *captureDevice)
    : QMainWindow(Q_NULLPTR, Qt::WindowFlags())
    , m_firstPacket(true)
    , m_filterProxy(new QSortFilterProxyModel(this))
    , m_captureDevice(captureDevice)
    , m_controller(Q_NULLPTR)
{
    mainWinPtr = this;
    qInstallMessageHandler(myMessageOutput);

	ui.setupUi(this);

    ui.toolBar->setParent(ui.centralWidget);
    ui.snifferToolsLayout->addWidget(ui.toolBar);

    if(!captureDevice)
        LogModel::log(tr("Starting up, offline mode"), CDL_SEV_INF, 1);
    else {
        LogModel::log(tr("Starting up, using device %1").arg(captureDevice->description()), CDL_SEV_INF, 1);
    }


    // Setup the RDM controller. Currently supported by Gadget2 only, and if in offline mode
    GadgetCaptureDevice *gadgetDevice = dynamic_cast<GadgetCaptureDevice *>(m_captureDevice);

    if(gadgetDevice)
    {
        m_controller = new RDMController(gadgetDevice, this);
        connect(m_controller, &RDMController::discoveryStarted, [this] {
            ui.twRdmDevices->clear();
            ui.rdmProgressBar->setVisible(true);
            ui.rdmProgressBar->setMinimum(0);
            ui.rdmProgressBar->setMaximum(0);
            emit updateStatusBarMsg();
        });
        connect(m_controller, SIGNAL(discoveryFinished()), this, SLOT(gotDiscoveryData()));
        connect(m_controller, SIGNAL(gotSensorValues()), this, SLOT(updateRdmDisplay()));
        connect(m_controller, SIGNAL(customCommandComplete(quint8, QByteArray)), this, SLOT(rawCommandComplete(quint8, QByteArray)));
    }
    else if (m_captureDevice!=Q_NULLPTR)
    {
        ui.tbController->setEnabled(false);
    }

    ui.actionUpdateGadget->setEnabled(gadgetDevice!=Q_NULLPTR);

    if(m_captureDevice)
    {
        ui.statusBar->addPermanentWidget(new QLabel(tr("Using device %1").arg(m_captureDevice->description()),
                                                       this));
        connect(m_captureDevice, SIGNAL(packetsReady()), this, SLOT(readData()));
        connect(m_captureDevice, SIGNAL(discoveryDataReady()), this, SLOT(gotDiscoveryData()));

        // Status Bar
        connect(this, &MainWindow::updateStatusBarMsg, this, &MainWindow::doUpdatetStatusBarMsg);

        // Capture mode changes
        ui.actionStart_Capture->setChecked(false);
        ui.actionStart_Capture->setEnabled(m_captureDevice->info().deviceCapabilities & CaptureDeviceList::CAPABILITY_SNIFFER);
        ui.actionStop_Capture->setEnabled(false);
        ui.actionRestart_Capture->setEnabled(false);

        connect(m_captureDevice, &ICaptureDevice::sniffing, this, [=] {
            ui.actionStart_Capture->setChecked(true);
            ui.actionStart_Capture->setEnabled(true);
            ui.actionStop_Capture->setEnabled(true);
            ui.actionRestart_Capture->setEnabled(true);

            emit updateStatusBarMsg();
        },
        Qt::QueuedConnection);

        connect(m_captureDevice, &ICaptureDevice::closed, this, [=] {
            ui.actionStart_Capture->setChecked(false);
            ui.actionStart_Capture->setEnabled((m_captureDevice->info().deviceCapabilities & CaptureDeviceList::CAPABILITY_SNIFFER));
            ui.actionStop_Capture->setEnabled(false);
            ui.actionRestart_Capture->setEnabled(false);

            emit updateStatusBarMsg();
        },
        Qt::QueuedConnection);

        connect(m_captureDevice, &ICaptureDevice::transmitting, this, [=] {
            ui.actionStart_Capture->setChecked(false);
            ui.actionStart_Capture->setEnabled(false);
            ui.actionStop_Capture->setEnabled(false);
            ui.actionRestart_Capture->setEnabled(false);

            emit updateStatusBarMsg();
        },
        Qt::QueuedConnection);

        // Capture mode buttons
        connect(ui.actionStart_Capture, &QAction::triggered, [this]() {
            if (m_captureDevice->isOpen() && m_captureDevice->getMode() == ICaptureDevice::SniffMode)
                stopCapture();
            else
                startCapture();
        });
        connect(ui.actionStop_Capture, &QAction::triggered, [this]() { stopCapture(); });
        connect(ui.actionRestart_Capture, &QAction::triggered, [this]() {
           stopCapture();
           startCapture();
        });
    }
    else
    {
        ui.statusBar->addPermanentWidget(new QLabel(tr("Working Offline"), this));
    }


    ui.tbSniffer->setChecked(true);

    connect(ui.tbController,    SIGNAL(clicked(bool)), this, SLOT(modeButtonPressed(bool)));
    connect(ui.tbSniffer,       SIGNAL(clicked(bool)), this, SLOT(modeButtonPressed(bool)));
    connect(ui.tbTxMode,        SIGNAL(clicked(bool)), this, SLOT(modeButtonPressed(bool)));
    connect(ui.tbDmxView,       SIGNAL(clicked(bool)), this, SLOT(modeButtonPressed(bool)));
    connect(ui.tbScript,        SIGNAL(clicked(bool)), this, SLOT(modeButtonPressed(bool)));

    // Packet table
    // Filtering model
    m_filterProxy->setSourceModel(&m_packetTable);
    m_filterProxy->setDynamicSortFilter(true);  // Update filter whenever model changes
    m_filterProxy->setFilterKeyColumn(PacketTable::Protocol);
    m_filterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui.tableView->setModel(m_filterProxy);
    connect(ui.tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(selectionChanged(QModelIndex,QModelIndex)));

    ui.tableView->setColumnWidth(0, 200);
    ui.tableView->setColumnWidth(1, 160);
    ui.tableView->setColumnWidth(2, 160);
    ui.tableView->setColumnWidth(3, 100);
    ui.tableView->setColumnWidth(4, 300);
    ui.tableView->horizontalHeader()->setStretchLastSection(true);

	ui.treeWidget->setColumnWidth(0, 240);

    // Register dissectors
    m_packetTable.registerProtocolDissectors(&m_dissectorList);

    // Populate dissectors menu
    for (auto d : m_dissectorList.getDissectorList() )
    {
        auto action = new QAction(d->dissector->getProtocolName().toString(), this);
        action->setCheckable(true);
        action->setChecked(d->isEnabled());
        connect( action, &QAction::changed,
                [this, action, d]()
                {
                    d->setEnabled(action->isChecked());
                    this->ui.tableView->selectionModel()->reset();
                    this->ui.tableView->repaint();
                });
            ui.menuDissector->addAction(action);
    }

    // Setup filter editor
    m_filterLabel = new QLabel();
    m_filterLabel->setText(tr("Search  "));
	
    m_filterColumnCombo = new QComboBox();
    m_filterColumnCombo->addItem(tr("Source"), PacketTable::COLUMNS::Source);
    m_filterColumnCombo->addItem(tr("Destination"), PacketTable::COLUMNS::Destination);
    m_filterColumnCombo->addItem(tr("Protocol"), PacketTable::COLUMNS::Protocol);
    m_filterColumnCombo->addItem(tr("Information"), PacketTable::COLUMNS::Info);

    m_filterCombo = new QComboBox();
    m_filterCombo->addItem("*");
    m_filterCombo->setEditable(true);
    m_filterCombo->setMinimumContentsLength(10);

    m_filterModeCombo = new QComboBox();
    m_filterModeCombo->addItem(tr("Wildcard"), QRegExp::Wildcard);
    m_filterModeCombo->addItem(tr("RegEx"), QRegExp::RegExp);

    ui.snifferToolsLayout->addSpacerItem(new QSpacerItem(100, 0, QSizePolicy::Expanding));
    ui.snifferToolsLayout->addWidget(m_filterLabel);
    ui.snifferToolsLayout->addWidget(m_filterColumnCombo);
    ui.snifferToolsLayout->addWidget(m_filterCombo);
    ui.snifferToolsLayout->addWidget(m_filterModeCombo);

    connect(m_filterCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this, [=](int index) {
                Q_UNUSED(index);
                this->updateFilterPattern(this->m_filterCombo->currentText(), this->m_filterModeCombo->currentData().toInt());
            });
    connect(m_filterModeCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](int index) {
                Q_UNUSED(index);
                this->updateFilterPattern(this->m_filterCombo->currentText(), this->m_filterModeCombo->currentData().toInt());
            });

    connect(m_filterColumnCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](int index) {
                Q_UNUSED(index);
                this->setFilterColumn(this->m_filterColumnCombo->currentData().toUInt());
            });

    ui.stackedWidget->setCurrentIndex(0);
    ui.rdmProgressBar->setVisible(false);

    auto scene1Layout = new QHBoxLayout;
    auto scene2Layout = new QHBoxLayout;
    auto sceneOutputLayout = new QHBoxLayout;


    // Set up two scene preset
    ui.sbDmxStart->setMinimum(1);
    ui.sbDmxStart->setMaximum(512-FADER_COUNT+1);
    ui.sbDmxStart->setValue(1);

    memset(m_scene1Levels, 0, sizeof(m_scene1Levels));
    memset(m_scene2Levels, 0, sizeof(m_scene2Levels));
    for(int i=0; i<FADER_COUNT; i++)
    {
        for(int scene=0; scene<2; scene++)
        {
            QVBoxLayout *layout = new QVBoxLayout;
            QSlider *slider = new QSlider(this);
            slider->setMinimum(0);
            slider->setMaximum(255);
            slider->setTickInterval(1); // Hack - used to indicate to the style to show values
            slider->setTracking(true);
            connect(slider, SIGNAL(valueChanged(int)), this, SLOT(faderMoved(int)));
            slider->setOrientation(Qt::Vertical);
            slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
            slider->setMinimumWidth(30);
            QLabel *label = new QLabel(this);
            label->setAlignment(Qt::AlignHCenter);
            label->setText(QString::number(i+1));
            layout->addWidget(slider);
            layout->addWidget(label);
            if(scene==0) {
                m_scene1faderLabels << label;
                m_scene1sliders << slider;
                scene1Layout->addLayout(layout);
            }
            else {
                m_scene2faderLabels << label;
                m_scene2sliders << slider;
                scene2Layout->addLayout(layout);
            }
        }


        LevelIndicator *bar = new LevelIndicator(this);
        sceneOutputLayout->addWidget(bar);
        m_sceneOutputs << bar;
    }

    ui.frScene1->setLayout(scene1Layout);
    ui.frScene2->setLayout(scene2Layout);
    ui.frOutputs->setLayout(sceneOutputLayout);

    ui.slCrossfade->setMinimum(0);
    ui.slCrossfade->setMaximum(255);
    ui.slCrossfade->setValue(255);
    connect(ui.slCrossfade, SIGNAL(valueChanged(int)), this, SLOT(crossFaderMoved()));

    QMenu *fadeMenu = new QMenu(this);
    for(int i=0; i<FADE_TIME_DESCS.count(); i++)
    {
        QString s = FADE_TIME_DESCS[i];
        QAction *a = fadeMenu->addAction(s);
        a->setData(QVariant(FADE_TIMES[i]));
        connect(a, SIGNAL(triggered()), this, SLOT(startFade()));
    }
    ui.btnCrossfade->setMenu(fadeMenu);

    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui.tbController);
    btnGroup->addButton(ui.tbSniffer);
    btnGroup->addButton(ui.tbTxMode);
    btnGroup->addButton(ui.tbDmxView);
    btnGroup->addButton(ui.tbScript);
    btnGroup->setExclusive(true);

    // Setup Custom PID
    ui.twRawParams->clear();
    ui.twRawParams->setColumnCount(2);
    ui.twRawParams->setRowCount(3);

    QStringList headers;
    headers << tr("Command") << tr("Parameter") << tr("Subdevice");
    for(int i=0; i<headers.count(); i++)
    {
        QTableWidgetItem *hItem = new QTableWidgetItem();
        hItem->setText(headers[i]);
        hItem->setFlags(Qt::ItemIsEnabled);
        ui.twRawParams->setItem(i, 0, hItem);
    }

    m_commandCombo = new QComboBox(this);
    m_commandCombo->addItem(tr("GET_COMMAND"), E120_GET_COMMAND);
    m_commandCombo->addItem(tr("SET_COMMAND"), E120_SET_COMMAND);
    ui.twRawParams->setCellWidget(0, 1, m_commandCombo);
    connect(m_commandCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(composeRawCommand()));

    m_paramCombo = new QComboBox(this);
    m_paramCombo->setEditable(true);
    auto map  = Util::getAllRdmParameterIds();
    for (auto i = map.constBegin(); i != map.constEnd(); ++i)
        m_paramCombo->addItem(i.value(), i.key());
    ui.twRawParams->setCellWidget(1, 1, m_paramCombo);
    connect(m_paramCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(composeRawCommand()));

    m_subDeviceSpin = new QSpinBox(this);
    m_subDeviceSpin->setMinimum(0);
    m_subDeviceSpin->setMaximum(255);
    ui.twRawParams->setCellWidget(2, 1, m_subDeviceSpin);
    connect(m_subDeviceSpin, SIGNAL(valueChanged(int)), this, SLOT(composeRawCommand()));

    // Sniffer time format
    {
        auto actionGroup = new QActionGroup(this);
        actionGroup->addAction(ui.actionDateTime);
        actionGroup->addAction(ui.actionTimeOfDay);
        actionGroup->addAction(ui.actionSecondsSinceBeginning);
        actionGroup->addAction(ui.actionSecondsSincePrevious);

        connect(ui.actionDateTime, &QAction::triggered, [=]() { m_packetTable.setTimeFormat(PacketTable::DATE_AND_TIME); });
        connect(ui.actionTimeOfDay, &QAction::triggered, [=]() { m_packetTable.setTimeFormat(PacketTable::TIME_OF_DAY); });
        connect(ui.actionSecondsSinceBeginning, &QAction::triggered, [=]() { m_packetTable.setTimeFormat(PacketTable::SECONDS_SINCE_CAPTURE_START); });
        connect(ui.actionSecondsSincePrevious, &QAction::triggered, [=]() { m_packetTable.setTimeFormat(PacketTable::SECONDS_SINCE_PREVIOUS_PACKET); });

        connect(&m_packetTable, &PacketTable::timeFormatChange, [=]() {
            switch (m_packetTable.timeFormat())
            {
                case PacketTable::DATE_AND_TIME:
                    ui.actionDateTime->setChecked(true);
                    break;

                case PacketTable::TIME_OF_DAY:
                    ui.actionTimeOfDay->setChecked(true);
                    break;

                case PacketTable::SECONDS_SINCE_CAPTURE_START:
                    ui.actionSecondsSinceBeginning->setChecked(true);
                    break;

                case PacketTable::SECONDS_SINCE_PREVIOUS_PACKET:
                    ui.actionSecondsSincePrevious->setChecked(true);
                    break;
            }
        });
        m_packetTable.setTimeFormat(PacketTable::DATE_AND_TIME);
    }

    // Logging
    ui.lvLog->setModel(LogModel::getInstance());
    connect(ui.lvLog->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            ui.lvLog, SLOT(scrollToBottom()));
    ui.dwLogging->close();

    ui.sbLogVerbosity->setMinimum(CDL_VERB_MIN);
    ui.sbLogVerbosity->setMaximum(CDL_VERB_MAX);
    ui.sbLogVerbosity->setValue(LogModel::getInstance()->getVerbosityFilter());
    connect(ui.sbLogVerbosity, SIGNAL(valueChanged(int)), LogModel::getInstance(), SLOT(setVerbosityFilter(int)));

    QMenu *categoryMenu = new QMenu(this);
    QMapIterator<int, QString> i(CDL_CAT_STRINGS);
    while(i.hasNext())
    {
        i.next();
        QAction *a = categoryMenu->addAction(i.value());
        a->setData(QVariant(i.key()));
        a->setCheckable(true);
        a->setChecked(LogModel::getInstance()->getCategoryFilter() & i.key());
        connect(a, SIGNAL(toggled(bool)), this, SLOT(logCategoryToggle(bool)));
    }
    ui.tbCategory->setMenu(categoryMenu);

    QMenu *severityMenu = new QMenu(this);
    i = QMapIterator<int, QString>(CDL_SEV_STRINGS);
    while(i.hasNext())
    {
        i.next();
        QAction *a = severityMenu->addAction(i.value());
        a->setData(QVariant(i.key()));
        a->setCheckable(true);
        a->setChecked(LogModel::getInstance()->getSeverityFilter() & i.key());
        connect(a, SIGNAL(toggled(bool)), this, SLOT(logSeverityToggle(bool)));
    }
    ui.tbSeverity->setMenu(severityMenu);
    emit updateStatusBarMsg();

    setAcceptDrops(true);

    // Scripting
    m_scripting = new Scripting(m_captureDevice);
    connect(m_scripting, SIGNAL(finished(bool, bool)), this, SLOT(scriptFinished(bool, bool)));
    QSettings s;
    // Open the last opened script file for convienience
    if(s.contains("lastScriptFile"))
    {
        openScriptFile(s.value("lastScriptFile").toString());
    }
}

MainWindow::~MainWindow()
{}

bool MainWindow::isValidMimeData(const QMimeData* mimeData)
{
    if (mimeData->hasUrls() && (mimeData->urls().count() == 1))
        for (auto url : mimeData->urls())
            if (url.scheme() == "file") return true;

    return false;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (ui.actionOpen_File->isEnabled() == false) return;

    if (isValidMimeData(event->mimeData())) event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    if (ui.actionOpen_File->isEnabled() == false) return;

    if (isValidMimeData(event->mimeData()))
        openFile(event->mimeData()->urls().first().toLocalFile());
}

void MainWindow::doUpdatetStatusBarMsg()
{
    QString statusStr = tr("Unknown");
    if (m_captureDevice)
    {
        if (m_captureDevice->isOpen())
        {
            switch (m_captureDevice->getMode())
            {
                case ICaptureDevice::SniffMode:
                    statusStr = tr("Capturing...");
                    break;
                case ICaptureDevice::TransmitMode:
                    statusStr = tr("Transmiter running...");
                    break;
                case ICaptureDevice::ControllerMode:
                    statusStr = tr("Controller running...");
                    break;
            }
        } else {
            statusStr = tr("Not active");
        }
    }

    if (m_controller)
    {
        if (ui.stackedWidget->currentIndex() == 2)
            statusStr = tr("Controller running...");
    }

    ui.statusBar->showMessage(statusStr);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);

    ui.twRawParams->setColumnWidth(0, ui.twRawParams->width()/2);
}


void MainWindow::faderMoved(int value)
{
    QSlider *slider = dynamic_cast<QSlider *>(sender());

    int index;

    if((index = m_scene1sliders.indexOf(slider)) > -1)
    {
        m_scene1Levels[index + ui.sbDmxStart->value() - 1] = static_cast<quint8>(value);
    }
    if((index = m_scene2sliders.indexOf(slider)) > -1)
    {
        m_scene2Levels[index + ui.sbDmxStart->value() - 1] = static_cast<quint8>(value);
    }

    updateTxLevels();
}

void MainWindow::on_btnToggleDmx_toggled(bool checked)
{
    if(checked)
    {
        ui.btnToggleDmx->setText(tr("Enable DMX"));
        if(m_captureDevice) m_captureDevice->setDmxEnabled(false);
    }
    else {
        ui.btnToggleDmx->setText(tr("Disable DMX"));
        if(m_captureDevice) m_captureDevice->setDmxEnabled(true);
    }
}

void MainWindow::modeButtonPressed(bool checked)
{
    if(!checked) return;

    QToolButton *button = dynamic_cast<QToolButton *>(sender());
    if(!button) return;

    if(button==ui.tbSniffer)
        m_mode = OPMODE_SNIFFER;
    if(button == ui.tbTxMode)
        m_mode = OPMODE_DMXCONTROL;
    if(button == ui.tbController)
        m_mode = OPMODE_RDMCONTROL;
    if(button == ui.tbDmxView)
        m_mode = OPMODE_DMXVIEW;
    if(button == ui.tbScript)
        m_mode = OPMODE_SCRIPT;

    ui.stackedWidget->setCurrentIndex(m_mode);

    stopCapture();

    switch(m_mode)
    {
    case OPMODE_SNIFFER: // Sniffer mode
        ui.actionSave_File->setEnabled(true);
        ui.actionSave_As->setEnabled(true);
        ui.actionOpen_File->setEnabled(true);
        ui.actionExport_to_PcapNg->setEnabled(true);
        ui.menuCapture->setEnabled(true);
        if(m_captureDevice)
        {
            m_captureDevice->setMode(ICaptureDevice::SniffMode);
        }
        setWindowFilePath(m_captureFileName);
        setWindowModified(m_captureFileModified);
        LogModel::log(tr("Switched to Sniffer Mode"), CDL_SEV_INF, 1);
        break;
    case OPMODE_DMXCONTROL: // DMX Sender Mode
        ui.actionSave_File->setEnabled(false);
        ui.actionSave_As->setEnabled(false);
        ui.actionOpen_File->setEnabled(false);
        ui.actionExport_to_PcapNg->setEnabled(false);
        ui.menuCapture->setEnabled(false);
        if(m_captureDevice)
        {
            // Start transmitting on entry to screen
            m_captureDevice->setMode(ICaptureDevice::TransmitMode);
            m_captureDevice->open();
        }
        setWindowFilePath("");
        setWindowModified(false);
        LogModel::log(tr("Switched to DMX Transmit Mode"), CDL_SEV_INF, 1);
        break;
    case OPMODE_RDMCONTROL: // RDM Controller Mode
        ui.menuCapture->setEnabled(false);
        ui.actionSave_File->setEnabled(false);
        ui.actionSave_As->setEnabled(false);
        ui.actionOpen_File->setEnabled(false);
        ui.actionExport_to_PcapNg->setEnabled(false);
        ui.menuCapture->setEnabled(false);
        ui.twRdmController->setCurrentIndex(0);
        // Auto start discovery
        if (m_controller) m_controller->startDiscovery();
        setWindowFilePath("");
        setWindowModified(false);
        LogModel::log(tr("Switched to RDM Controller Mode"), CDL_SEV_INF, 1);
        break;
    case OPMODE_DMXVIEW: // DMX View Mode
        ui.menuCapture->setEnabled(false);
        ui.actionSave_File->setEnabled(false);
        ui.actionSave_As->setEnabled(false);
        ui.actionOpen_File->setEnabled(false);
        ui.actionExport_to_PcapNg->setEnabled(false);
        ui.menuCapture->setEnabled(false);
        if(m_captureDevice)
        {
            m_captureDevice->setMode(ICaptureDevice::SniffMode);
            m_captureDevice->open();
        }
        setWindowFilePath("");
        setWindowModified(false);
        break;
    case OPMODE_SCRIPT: // Script mode
        ui.menuCapture->setEnabled(false);
        ui.actionSave_File->setEnabled(true);
        ui.actionSave_As->setEnabled(true);
        ui.actionOpen_File->setEnabled(true);
        ui.actionExport_to_PcapNg->setEnabled(false);
        ui.menuCapture->setEnabled(false);
        setWindowFilePath(m_scriptFileName);
        setWindowModified(m_scriptFileModified);
        break;
    }
}

void MainWindow::readData()
{
    if(m_captureDevice && m_mode==OPMODE_SNIFFER)
    {
        QList<Packet> packets = m_captureDevice->getPackets();
        foreach(Packet p, packets)
        {
            if(p.length()>0) {
                p.timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
                m_packetTable.appendPacket(p);
            }
        }
        ui.tableView->scrollToBottom();
    }

    if(m_captureDevice && m_mode==OPMODE_DMXVIEW)
    {
        QList<Packet> packets = m_captureDevice->getPackets();
        if(packets.length()==0) return;
        Packet p = packets.last();
        if(p.length()>0)
        {
            char startcode = p.at(0);
            if(startcode != 0) return;
            int packetLength = qMin(p.length(), 513); // Deal with occasional wrong packet lengths from Gadget2
            for(int i=1; i<packetLength; i++)
                ui.dmxGridWidget->setCellValue(i-1, QString::number(static_cast<unsigned char>(p.at(i))));
            ui.dmxGridWidget->update();
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::startCapture()
{
    if(!m_captureDevice)
    {
        QMessageBox::warning(this, tr("Working Offline"), tr("Currently no capture device has been selected. Restart the application to select a device."));
        return;
    }


    if(!m_captureDevice->open())
    {
        QMessageBox::warning(this, tr("Couldn't Open Device"), tr("Unable to open the selected capture device"));
        return;
    }

    m_packetTable.clearAll();
	
	ui.treeWidget->clear();
	ui.textEdit->clear();
    m_firstPacket = true;
    m_tickTimer.start();
}

void MainWindow::stopCapture()
{
    if (m_captureDevice)
        m_captureDevice->close();
}

void MainWindow::updateTreeWidget(int currentRow)
{
	ui.treeWidget->clear();
    if(currentRow<0 || currentRow>=m_packetTable.rowCount()) return;
    const Packet &packet = m_packetTable.getPacket(currentRow);

    QTreeWidgetItem *rootItem = new QTreeWidgetItem();
    DissectorPlugin *dissector = m_dissectorList.getDissector(packet);
    if (dissector != Q_NULLPTR)
    {
        dissector->dissectPacket(packet, rootItem);
        rootItem->setText(0, dissector->getProtocolName().toString());
    } else {
        rootItem->setText(0, "Unkown Protocol");
    }

	ui.treeWidget->addTopLevelItem(rootItem);
	ui.treeWidget->expandAll();	

	ui.textEdit->clear();
    ui.textEdit->append(prettifyHex(packet));

	//Scroll to the top
	QScrollBar *vScrollBar = ui.textEdit->verticalScrollBar();
	vScrollBar->triggerAction(QScrollBar::SliderToMinimum);
}

void MainWindow::on_actionSave_File_triggered()
{
    if(m_mode==OPMODE_SNIFFER)
    {
        if(m_captureFileName.isEmpty())
        {
            on_actionSave_As_triggered();
            return;
        }
        saveTextFile();
    }

    if(m_mode==OPMODE_SCRIPT)
    {
        if(m_scriptFileName.isEmpty())
        {
            on_actionSave_As_triggered();
            return;
        }
        saveScriptFile();
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    if(m_mode==OPMODE_SNIFFER)
    {
        QSettings settings;
        QString defaultPath = settings.value("textPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
        QString filename = QFileDialog::getSaveFileName(this, "Enter Filename", defaultPath, "Text Files (*.txt)");
        if(filename.isEmpty()) return;
        settings.setValue("textPath", filename);
        m_captureFileName = filename;
        saveTextFile();
    }
    if(m_mode==OPMODE_SCRIPT)
    {
        QSettings settings;
        QString defaultPath = settings.value("scriptPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
        QString filename = QFileDialog::getSaveFileName(this, "Enter Filename", defaultPath, "Script Files (*.js)");
        if(filename.isEmpty()) return;
        settings.setValue("scriptPath", filename);
        m_scriptFileName = filename;
        saveScriptFile();
    }
}

void MainWindow::saveTextFile()
{
    FileSave *f = new FileSave(m_packetTable, m_captureFileName);
    QThread *fileSaveThread = new QThread(this);
    f->moveToThread(fileSaveThread);
    fileSaveThread->start();
    connect(fileSaveThread, &QThread::finished, fileSaveThread, &QObject::deleteLater);
    connect(f, SIGNAL(Finished()), fileSaveThread, SLOT(quit()),        Qt::DirectConnection);

    // Needs to be queued so the action happens in the GUI thread
    connect(f, &FileSave::Started, this, [=]() {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }, Qt::QueuedConnection);
    connect(f, &FileSave::Finished, this, [=]() {
        QApplication::restoreOverrideCursor();
        f->deleteLater();
        setWindowModified(false);
        m_captureFileModified = false;
    }, Qt::QueuedConnection);

    QMetaObject::invokeMethod(f, "doSave", Qt::QueuedConnection);

}

void MainWindow::on_actionOpen_File_triggered()
{
    if(m_mode==OPMODE_SNIFFER)
    {
        QSettings settings;
        QString defaultPath = settings.value("textPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
        QString filename = QFileDialog::getOpenFileName(this, "Open File", defaultPath, "Text Files (*.txt)");
        if(openFile(filename))
            settings.setValue("textPath", filename);
    }
    if(m_mode==OPMODE_SCRIPT)
    {
        QSettings settings;
        QString defaultPath = settings.value("scriptPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
        QString filename = QFileDialog::getOpenFileName(this, "Open File", defaultPath, "Script Files (*.js)");
        if(openScriptFile(filename))
            settings.setValue("scriptPath", filename);
    }
}

bool MainWindow::openFile(const QString &filename)
{
    if (filename.isEmpty()) return false;
    if (!QFileInfo::exists(filename)) return false;

    // Stop any capture and Clear old
    stopCapture();
    m_packetTable.clearAll();
    ui.treeWidget->clear();
    ui.textEdit->clear();

    FileOpen *f = new FileOpen(m_packetTable, filename);
    QThread *fileOpenThread = new QThread(this);
    f->moveToThread(fileOpenThread);
    fileOpenThread->start();
    connect(fileOpenThread, &QThread::finished, fileOpenThread, &QObject::deleteLater);
    connect(f, SIGNAL(Finished()), fileOpenThread, SLOT(quit()), Qt::DirectConnection);

    // Needs to be queued so the action happens in the GUI thread
    connect(f, &FileOpen::Started, this, [=]() {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }, Qt::QueuedConnection);
    connect(f, &FileOpen::Finished, this, [=]() {
        QApplication::restoreOverrideCursor();
        if (ui.tableView->model()->rowCount())
            ui.tableView->setCurrentIndex(ui.tableView->model()->index(0, 0));
        setWindowFilePath(filename);
        m_captureFileName = filename;
        m_captureFileModified = false;
        if(m_mode==OPMODE_SNIFFER) setWindowModified(false);
        f->deleteLater();
    }, Qt::QueuedConnection);


    QMetaObject::invokeMethod(f, "doRead", Qt::QueuedConnection);
    return true;
}

#define INDEXCOL_WIDTH 6
#define BYTES_PER_LINE 16
#define CHARS_PER_BYTE 3
#define ROW_LENGTH 72

void MainWindow::highlightPacketBytes(int start, int end)
{
	int startRow = start / 16;
	int startCol = start % 16;

	int endRow = end / 16;
	int endCol = end % 16;

	for(int row = startRow; row<=endRow; row++)
	{
		int startIndex;
		if(row==startRow)
			startIndex = (row*ROW_LENGTH) + INDEXCOL_WIDTH + startCol * CHARS_PER_BYTE;
		else
			startIndex = (row*ROW_LENGTH) + INDEXCOL_WIDTH;
		int endIndex;
		if(row!=endRow)
			endIndex = (row*ROW_LENGTH) + INDEXCOL_WIDTH + BYTES_PER_LINE*CHARS_PER_BYTE - 1; // -1 removes last whitespace
		else
			endIndex = (row*ROW_LENGTH) + INDEXCOL_WIDTH + endCol * CHARS_PER_BYTE - 1; // -1 removes last whitespace
		// Highlight all this row
		QTextCursor c(ui.textEdit->document());
		c.setPosition(startIndex);
		c.setPosition(endIndex, QTextCursor::KeepAnchor);
	
		QTextCharFormat fmt = c.charFormat();
		fmt.setBackground(Qt::yellow);
		c.setCharFormat(fmt);
	}
}

void MainWindow::on_treeWidget_currentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	// Remove any old formatting
	if(previous)
	{
		QTextCursor c(ui.textEdit->document());
		c.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
	
		QTextCharFormat fmt = c.charFormat();
		fmt.setBackground(Qt::white);
		c.setCharFormat(fmt);
	}
	if(current)
	{
        int first = current->data(0, CustomDataRoles::DATA_ROLE_STARTOFFSET).toInt();
        int last = current->data(0, CustomDataRoles::DATA_ROLE_ENDOFFSET).toInt();
		highlightPacketBytes(first, last);
    }
}

void MainWindow::updateFilterPattern(const QString &pattern, int PatternSyntax)
{
    if (PatternSyntax == QRegExp::RegExp) {
        m_filterProxy->setFilterRegExp(pattern);
    } else if (PatternSyntax == QRegExp::Wildcard) {
        if (pattern == "*")
            m_filterProxy->setFilterWildcard(QString());
        else
            m_filterProxy->setFilterWildcard(pattern);
    }
}

void MainWindow::selectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    updateTreeWidget(m_filterProxy->mapToSource(current).row());
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this,
                    tr("About ETCDmxTool"),
                    tr("ETCDmxTool\nSimple RDM/DMX/RS485 sniffer using the ETC USB Whip and Gadget\n(c) 2016 ETC Inc.\nVersion %1\nGadget DLL version %2")
                       .arg(VERSION)
                       .arg(Gadget2_GetDllVersion()));
}

void MainWindow::setFilterColumn(unsigned int column)
{
    if (column < PacketTable::COLCOUNT)
        m_filterProxy->setFilterKeyColumn(column);
}

void MainWindow::crossFaderMoved()
{
    ui.btnCrossfade->setEnabled(
                ui.slCrossfade->value()==0 || ui.slCrossfade->value()==255);
    updateTxLevels();
}

void MainWindow::updateTxLevels()
{
    int crossFadePos = ui.slCrossfade->value();
    for(int i=0; i<512; i++)
    {
        int aValue = (m_scene1Levels[i] * crossFadePos) / 255;
        int bValue =  (m_scene2Levels[i] * (255-crossFadePos)) / 255;
        m_txLevels[i] = 0xFF & aValue ;
        m_txLevels[i] += 0xFF & bValue;
    }

    for(int i=0; i<FADER_COUNT; i++)
    {
        m_sceneOutputs[i]->setValue(m_txLevels[i + ui.sbDmxStart->value() - 1]);
    }
    if(m_captureDevice)
        m_captureDevice->setDmxLevels(m_txLevels, 512);
}

void MainWindow::startFade()
{
    QAction *a = dynamic_cast<QAction *>(sender());
    if(!a) return;
    m_fadeLength = a->data().toInt();
    m_fadeTimer = new QTimer(this);
    m_fadeCounter.start();
    connect(m_fadeTimer, SIGNAL(timeout()), this, SLOT(fadeTick()));
    fadeAtoB = ui.slCrossfade->value() == 255;
    m_fadeTimer->start(0);
}

void MainWindow::fadeTick()
{
    float pos = (float)m_fadeCounter.elapsed() / (float) m_fadeLength;
    int handlePos = 255*pos;
    if(fadeAtoB)
        ui.slCrossfade->setValue(255-handlePos);
    else
        ui.slCrossfade->setValue(handlePos);
    ui.btnCrossfade->setText(tr("%1% Complete").arg(floor(100*pos)));

    if(handlePos>=255)
    {
        m_fadeTimer->stop();
        m_fadeTimer->deleteLater();
        m_fadeTimer = Q_NULLPTR;
        ui.btnCrossfade->setText(tr("Crossfade"));
    }
}

void MainWindow::on_clbDiscoverRdm_pressed()
{
    m_controller->startDiscovery();
}

void MainWindow::gotDiscoveryData()
{
    foreach(RdmDeviceInfo *info, m_controller->deviceList())
    {
        QTreeWidgetItem *devNode = new QTreeWidgetItem();
        QString name = Util::formatRdmUid(info->manufacturer_id, info->device_id);
        if(!m_controller->getDeviceName(info->device_id).isEmpty())
            name += QString(" (%1)").arg(m_controller->getDeviceName(info->device_id));
        devNode->setText(0, name);
        ui.twRdmDevices->addTopLevelItem(devNode);
    }

    ui.rdmProgressBar->setVisible(false);
}

void MainWindow::on_twRdmDevices_currentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    updateRdmDisplay();
}

void MainWindow::updateRdmDisplay()
{
    int index = ui.twRdmDevices->currentIndex().row();

    if(!m_controller) return;
    if(m_controller->deviceList().count()<=index) return;
    if(index<0) return;

    RdmDeviceInfo *info = m_controller->deviceList().at(index);

    if(!info) return;

    int row,col;
    col = 0;
    for(row=RDMINFOROW_PROTOCOLVERSION; row<RDMINFOROWCOUNT; row++)
    {
        QString value;
        QTableWidgetItem *item = new QTableWidgetItem();

        switch(row)
        {
        case RDMINFOROW_PROTOCOLVERSION:
            {
                quint16 major, minor;
                major = (info->rdm_protocol_version & 0xFF00) >> 8;
                minor = info->rdm_protocol_version & 0x00FF;

                value = QString("%1.%2")
                        .arg(major)
                        .arg(minor);
            }
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_MODELID:
            value = QString::number(info->device_model_id);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_CATEGORY:
            value = RDM_PIDString::productCategoryToString(info->product_category_type);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_SWVER:
            if(info->software_version_label_valid)
                value = QString("%1 (").arg(reinterpret_cast<char*>(info->software_version_label));
            value += QString("Numeric from DEVICE_INFO : %1").arg(info->software_version_id);
            value += QString("(0x%1)").arg(info->software_version_id, 0, 16);
            if(info->software_version_label_valid)
                value += QString(")");
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_DMXFOOTPRINT:
            value = QString::number(info->dmx_footprint);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_CURRENTPERS:
            value = QString("%1 (%2 of %3)")
                    .arg(m_controller->getPersonalityList(info->device_id).getPersonalityName((info->dmx_personality & 0xFF00) >> 8))
                    .arg((info->dmx_personality & 0xFF00) >> 8)
                    .arg(info->dmx_personality & 0x00FF);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_STARTADDR:
            if(info->dmx_start_address == 0xFFFF)
                value = tr("None (0xFFFF)");
            else
                value = QString::number(info->dmx_start_address);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_SUBDEV_COUNT:
            value = QString::number(info->subdevice_count);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_SENSORCOUNT:
            value = QString::number(info->sensor_count);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_DEVLABEL:
            value = m_controller->getDeviceName(info->device_id);
            break;
        case RDMINFOROW_MFRLABEL:
            value = m_controller->getManufacturerName(info->device_id);
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_PARAMCOUNT:
            value = QString::number(m_controller->getSupportedParameters(info->device_id).count());
            item->setFlags(Qt::ItemIsEnabled);
            break;
        case RDMINFOROW_PARAMLIST:
            for(int i=0; i<m_controller->getSupportedParameters(info->device_id).count(); i++)
            {
                quint16 param = m_controller->getSupportedParameters(info->device_id)[i];
                if(i>0) value += QString(", ");
                value += Util::paramIdToString(param);
            }
            item->setFlags(Qt::ItemIsEnabled);
            break;
        default:
            value = tr("Not Supported Yet");
            item->setFlags(Qt::ItemIsEnabled);
            break;
        }


        item->setText(value);
        ui.twDeviceBasics->setItem(row, col, item);
    }
    ui.twDeviceBasics->resizeRowToContents(RDMINFOROW_PARAMLIST);

    ui.twSensors->clear();
    RDMSensorList sensors = m_controller->getSensorList(info->device_id);
    ui.twSensors->setRowCount(sensors.count());

    ui.twSensors->setColumnCount(RDMSENSORCOLCOUNT);
    QStringList headers;
    headers << tr("Value") << tr("Type") << tr("Unit") << tr("Prefix") << tr("Range Minimum Value") << tr("Range Maximum Value")
            << tr("Normal Minimum Value") << tr("Normal Maximum Value") << tr("Recorded Value Support");
    ui.twSensors->setHorizontalHeaderLabels(headers);


    QStringList vhLabels;
    row = 0;

    RDMSensorValueList sensorValues = m_controller->getSensorValues(info->device_id);

    foreach(const RDMSensor sensor, sensors)
    {
        vhLabels << QString("%1 (%2)")
                .arg(sensor.description)
                .arg(sensor.number);



        for(int col = 0; col<RDMSENSORCOLCOUNT; col++)
        {
            QString value;
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsSelectable);

            switch(col)
            {
            case RDMSENSORCOL_VALUE:
                if(sensorValues.contains(sensor.number))
                    value = Util::formatSensorValue(sensor.unit, sensor.prefix, sensorValues[sensor.number].value);
                else
                    value = tr("No Data");
                break;
            case RDMSENSORCOL_TYPE:
                value = Util::sensorTypeToString(sensor.type);
                break;
            case RDMSENSORCOL_UNIT:
                value = Util::sensorUnitToString(sensor.unit);
                break;
            case RDMSENSORCOL_PREFIX:
                value = QString("10^%1")
                        .arg(Util::sensorUnitPrefixPower(sensor.prefix));
                break;
            case RDMSENSORCOL_RANGEMIN:
                value = QString::number(sensor.range_min);
                break;
                case RDMSENSORCOL_RANGEMAX:
                value = QString::number(sensor.range_max);
                break;
            case RDMSENSORCOL_NORMMIN:
                value = QString::number(sensor.normal_min);
                break;
            case RDMSENSORCOL_NORMMAX:
                value = QString::number(sensor.normal_max);
                break;
            case RDMSENSORCOL_RECVALUE:
                value = sensor.recorded_value_support > 0 ? "Yes" : "No";
                break;
            default:
                break;
            }
            item->setText(value);
            ui.twSensors->setItem(row, col, item);

        }

        row++;

    }

    ui.twSensors->setVerticalHeaderLabels(vhLabels);
}


void MainWindow::on_actionExport_to_PcapNg_triggered()
{
    QSettings settings;
    QString defaultPath = settings.value("pcapPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    QString filename = QFileDialog::getSaveFileName(this, tr("Export to PcapNg"), QString(defaultPath), "pcapng (*.pcapng)");
	if(filename.isEmpty()) return;
    settings.setValue("pcapPath", filename);

	QFile file(filename);
	bool ok = file.open(QIODevice::WriteOnly);
	if(!ok)
		return;

	pcapng exportData;

	// Section Header
	file.write(exportData.SectionHeaderBlock(
		QString(), 
		QString(), 
		tr("FixLinkSniffer\nSimple RDM/DMX/RS485 sniffer using the ETC USB Whip\n(c) 2016 ETC Inc.\nVersion %1").arg(VERSION)
		));

	// Interface Description Block
	file.write(exportData.InterfaceDescriptionBlock(
		pcapng_linktype::LINKTYPE_SLIP,
		513,
		tr("ETC USB Whip"),
		tr("Simple USB RS485 interface"),
		QByteArray(),
		QByteArray(),
		QByteArray(),
		quint64(),
		250000, // DMX Baud Rate
		3, // Millisecond resolution on timestamp
		quint32(),
		QString(),
		QString(),
		quint8(),
		quint64()
		));

	// Packets
	for(int i=0; i<m_packetTable.rowCount(); ++i)
	{
		const Packet &packet = m_packetTable.getPacket(i);

		//// Simple Packet Block
		//file.write(exportData.SimplePacketBlock(
		//	packet.length(),
		//	packet
		//));

		// Enhanced Packet Block 
		file.write(exportData.EnhancedPacketBlock(
			0,
            packet.timestamp,
			packet.length(),
			packet.length(),
			packet,
			pcapng::reception_promiscuous,
			QByteArray(),
			quint64()
		));
    }

	file.close();
}

void MainWindow::on_tbDmxStartPrev_pressed()
{
    int value = ui.sbDmxStart->value();
    value -= FADER_COUNT;
    if(value > 0)
        ui.sbDmxStart->setValue(value);
}

void MainWindow::on_tbDmxStartNext_pressed()
{
    int value = ui.sbDmxStart->value();
    value += FADER_COUNT;
    if(value+FADER_COUNT <= 512)
        ui.sbDmxStart->setValue(value);
}

void MainWindow::on_sbDmxStart_valueChanged(int value)
{
    for(int i=0; i<m_scene1faderLabels.count(); i++)
    {
        m_scene1faderLabels[i]->setText(QString::number(value + i));
        m_scene2faderLabels[i]->setText(QString::number(value + i));
        m_scene1sliders[i]->blockSignals(true);
        m_scene2sliders[i]->blockSignals(true);
        m_scene1sliders[i]->setValue(m_scene1Levels[value + i - 1]);
        m_scene2sliders[i]->setValue(m_scene2Levels[value + i - 1]);
        m_scene1sliders[i]->blockSignals(false);
        m_scene2sliders[i]->blockSignals(false);
        m_sceneOutputs[i]->setValue(m_txLevels[value + i - 1]);
    }
}

RdmDeviceInfo *MainWindow::selectedDevice()
{
    if(!m_controller) return Q_NULLPTR;
    int index = ui.twRdmDevices->currentIndex().row();
    if(m_controller->deviceList().count()<=index) return Q_NULLPTR;
    if(index<0) return Q_NULLPTR;

    return m_controller->deviceList().at(index);
}

void MainWindow::on_clbGetSensorData_pressed()
{
    RdmDeviceInfo *info = selectedDevice();

    if(!info) return;

    m_controller->fetchSensorValues(info);
}

void MainWindow::on_tbAddRawCmdData_pressed()
{
    ui.twRawParams->setRowCount(ui.twRawParams->rowCount() + 1);

    QWidget *cellWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout();
    QLabel *label = new QLabel(tr("Data, Type : "), cellWidget);
    QComboBox *combo = new QComboBox(cellWidget);
    m_comboToRow[combo] = ui.twRawParams->rowCount() - 1;
    m_customPropCombo << combo;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(rawDataTypeComboChanged(int)));
    combo->addItems(RDM_DATATYPE_DESCS);
    layout->setContentsMargins(10, 0, 10, 0);

    layout->addWidget(label);
    layout->addWidget(combo);
    cellWidget->setLayout(layout);

    ui.twRawParams->setCellWidget(ui.twRawParams->rowCount()-1, 0, cellWidget);
    composeRawCommand();
}

void MainWindow::on_tbRemoveRawCmdData_pressed()
{
    int rowToRemove = ui.twRawParams->rowCount() - 1;
    if(rowToRemove < 3) return;

    ui.twRawParams->setRowCount(rowToRemove);
    m_customPropCombo.removeLast();
    m_customPropEdits.remove(rowToRemove);
    composeRawCommand();

}

void MainWindow::rawDataTypeComboChanged(int index)
{
    if(!m_comboToRow.contains(sender())) return;

    int row = m_comboToRow[sender()];

    setupRawDataEditor(index, row);
    composeRawCommand();
}

void MainWindow::setupRawDataEditor(int datatype, int row)
{
    QWidget *editor = Q_NULLPTR;
    switch(datatype)
    {
    case RDMDATATYPE_UINT8:
    {
        QSpinBox *sb = new QSpinBox(this);
        sb->setMinimum(0);
        sb->setMaximum(0xFF);
        editor = sb;
        connect(sb, SIGNAL(valueChanged(int)), this, SLOT(composeRawCommand()));
    }
        break;
    case RDMDATATYPE_UINT16:
    {
        QSpinBox *sb = new QSpinBox(this);
        sb->setMinimum(0);
        sb->setMaximum(0xFFFF);
        editor = sb;
        connect(sb, SIGNAL(valueChanged(int)), this, SLOT(composeRawCommand()));
    }
        break;
    case RDMDATATYPE_STRING:
    {
        QLineEdit *le = new QLineEdit(this);
        le->setMaxLength(32);
        editor = le;
        connect(le, SIGNAL(textChanged(QString)), this, SLOT(composeRawCommand()));
    }
        break;
    case RDMDATATYPE_HEX:
    {
        QLineEdit *le = new HexLineEdit(this);
        editor = le;
        connect(le, SIGNAL(textChanged(QString)), this, SLOT(composeRawCommand()));
    }

    }

    if (editor)
    {
        ui.twRawParams->setCellWidget(row, 1, editor);

        m_customPropEdits[row] = editor;
    }
}

void MainWindow::composeRawCommand()
{
    QByteArray data;
    m_customCommandParamData.clear();

    // First pack the custom m_customCommandParamData
    for(int i=0; i<m_customPropCombo.count(); i++)
    {
        int row = i + 3;
        int type = m_customPropCombo[i]->currentIndex();
        switch(type)
        {
        case RDMDATATYPE_UINT8:
        {
            QSpinBox *sb = dynamic_cast<QSpinBox *>(m_customPropEdits[row]);
            quint8 value = 0xFF & sb->value();
            m_customCommandParamData.append(static_cast<char>(value));
        }
            break;
        case RDMDATATYPE_UINT16:
        {
            QSpinBox *sb = dynamic_cast<QSpinBox *>(m_customPropEdits[row]);
            quint8 value_high = 0xFF & (sb->value() >> 8);
            quint8 value_lo = 0xFF & sb->value();

            m_customCommandParamData.append(static_cast<char>(value_high));
            m_customCommandParamData.append(static_cast<char>(value_lo));
        }
            break;
        case RDMDATATYPE_STRING:
        {
            QLineEdit *le = dynamic_cast<QLineEdit *>(m_customPropEdits[row]);
            m_customCommandParamData.append(le->text().toLatin1());
        }
            break;
        case RDMDATATYPE_HEX:
        {
            HexLineEdit *hle = dynamic_cast<HexLineEdit *>(m_customPropEdits[row]);
            m_customCommandParamData.append(hle->currentValue());
        }
            break;
        }
    }


    // Command Class
    quint8 commandClass = m_commandCombo->currentData().toInt();

    data.append(static_cast<char>(commandClass));

    QString text = m_paramCombo->currentText();
    if(Util::getAllRdmParameterIds().values().contains(text))
    {
        m_customCommandPid = m_paramCombo->currentData().toInt();
    }
    else
    {
        // Deal with Custom PIDs entered in hex
        QString pidText = m_paramCombo->currentText();
        if(pidText.startsWith("0x", Qt::CaseInsensitive))
            pidText.remove(0, 2);
        bool ok = false;
        m_customCommandPid = 0xFFFF & pidText.toInt(&ok, 16);
        if(!ok)
            QMessageBox::warning(this, tr("Invalid PID"), tr("Enter custom pids in hexadecimal format)"));
    }
    data.append((m_customCommandPid & 0xFF00) >> 8);
    data.append(m_customCommandPid & 0xFF);

    // Param Data
    quint8 paramDataLength = m_customCommandParamData.length();
    data.append(static_cast<char>(paramDataLength));

    data.append(m_customCommandParamData);

    ui.teSendCommand->setPlainText(prettifyHex(data));
}

void MainWindow::on_btnSendCustomRDM_pressed()
{
    composeRawCommand();
    RdmDeviceInfo *info = selectedDevice();
    if(!info) return;

    char *dataPtr = Q_NULLPTR;
    if(m_customCommandParamData.length()>0)
    {
        dataPtr = new char[m_customCommandParamData.length()];
        memcpy(dataPtr, m_customCommandParamData.data(), m_customCommandParamData.length());
    }

    quint8 action =  m_commandCombo->currentData().toInt();

    RDM_CmdC *command = new RDM_CmdC(
                action,
                m_customCommandPid,
                0xFF & m_subDeviceSpin->value(),
                0xFF & m_customCommandParamData.length(),
                dataPtr,
                info->manufacturer_id,
                info->device_id
                );

    m_controller->executeCustomCommand(command);
}

void MainWindow::rawCommandComplete(quint8 response, const QByteArray &data)
{
    ui.teResponseData->clear();
    switch(response)
    {
    case E120_RESPONSE_TYPE_ACK:
        ui.teResponseData->appendPlainText(tr("ACK - Executed OK"));
        break;
    case E120_RESPONSE_TYPE_ACK_TIMER:
        ui.teResponseData->appendPlainText(tr("ACK_TIMER - Executed OK"));
        break;
    case E120_RESPONSE_TYPE_NACK_REASON:
        ui.teResponseData->appendPlainText(tr("NACK - Command Error"));
        break;
    case E120_RESPONSE_TYPE_ACK_OVERFLOW:
        ui.teResponseData->appendPlainText(tr("ACK_OVERFLOW"));
        break;
    }

    ui.teResponseData->appendPlainText(QString("\r\n"));
    ui.teResponseData->appendPlainText(prettifyHex(data));
}

void MainWindow::on_actionViewLog_triggered()
{
    if(!ui.dwLogging->isVisible())
        ui.dwLogging->show();
}

void MainWindow::on_actionUpdateGadget_triggered()
{
    QString defaultPath;
    QStringList desktopLoc = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    if(desktopLoc.count()>0) defaultPath = desktopLoc.first();

    // Look in the default location, C:\etc\nodesbin
    if(QDir("C:/etc/nodesbin").exists())
    {
        defaultPath = "C:/etc/nodesbin";
        QStringList filter;
        filter << "*Gadget_II*";
        QDirIterator it(defaultPath, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        QStringList files;
        while (it.hasNext())
            files << it.next();
        files.sort();
        if(files.count()>0)
            defaultPath = files.last();
    }

    QString firmwareFile = QFileDialog::getOpenFileName(this, tr("Select Gadget Firmware"), defaultPath);
    if(firmwareFile.isEmpty())
        return;
    if(!m_captureDevice) return;
    GadgetCaptureDevice *d = dynamic_cast<GadgetCaptureDevice *>(m_captureDevice);
    if(!d) return;

    UpdateDialog dialog(this);
    connect(d, SIGNAL(updateProgressText(QString)), &dialog, SLOT(setStatusText(QString)));
    connect(d, SIGNAL(updateComplete()), &dialog, SLOT(doneAndRestart()));

    d->updateFirmware(firmwareFile);

    dialog.exec();
}

void MainWindow::logCategoryToggle(bool checked)
{
    QAction *a = dynamic_cast<QAction *>(sender());
    if(!a) return;

    int category = LogModel::getInstance()->getCategoryFilter();
    if(checked)
        category = category | a->data().toInt();
    else
        category = category & ~(a->data().toInt());
    LogModel::getInstance()->setCategoryFilter(category);
}

void MainWindow::logSeverityToggle(bool checked)
{
    QAction *a = dynamic_cast<QAction *>(sender());
    if(!a) return;

    int severity = LogModel::getInstance()->getSeverityFilter();
    if(checked)
        severity = severity | a->data().toInt();
    else
        severity = severity & ~(a->data().toInt());
    LogModel::getInstance()->setSeverity(severity);
}

void MainWindow::on_tbSaveLog_pressed()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                 tr("Save Log File"),
                                 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                 tr("Text Files (*.txt)"));
    if(filename.isEmpty())
        return;

    QFile file(filename);
    bool ok = file.open(QIODevice::WriteOnly);
    if(!ok)
    {
        QMessageBox::warning(this,
                             tr("Couldn't Open File"),
                             tr("Unable to open file %1 to save").arg(filename)
                             );
        return;
    }
    LogModel::getInstance()->saveFile(&file);
}

void MainWindow::on_actionCaptureInfo_triggered()
{
    QDialog dialog(this);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowTitle(tr("Capture Statistics"));
    QPlainTextEdit edit(&dialog);
    edit.appendPlainText(tr("%1 Total Packets").arg(m_packetTable.rowCount()));
    QVBoxLayout layout;
    dialog.setLayout(&layout);
    layout.addWidget(&edit);


    edit.appendPlainText("\n\n*********** Protocols *************");
    QSet<QString> m_uniqueIds;
    QHash<QString, int> packetCountHash;
    for(int row=0; row<m_packetTable.rowCount(); row++)
    {
        QString protocol = m_packetTable.data(m_packetTable.index(row, PacketTable::Protocol), Qt::DisplayRole).toString();

        if(packetCountHash.contains(protocol))
        {
            int value = packetCountHash[protocol] + 1;
            packetCountHash[protocol] = value;
        }
        else
        {
            packetCountHash[protocol] = 1;
        }
        m_uniqueIds.insert(m_packetTable.data(m_packetTable.index(row, PacketTable::Source), Qt::DisplayRole).toString());
        m_uniqueIds.insert(m_packetTable.data(m_packetTable.index(row, PacketTable::Destination), Qt::DisplayRole).toString());
    }

    QHashIterator<QString, int> i(packetCountHash);
    while(i.hasNext())
    {
        i.next();
        edit.appendPlainText(QString("%1 : %2")
                             .arg(i.key())
                             .arg(i.value()));
    }

    edit.appendPlainText("\n\n*********** Unique IDs *************");

    QStringList idList;
    foreach(QString s, m_uniqueIds)
        idList << s;
    idList.sort();
    foreach(QString s, idList)
        edit.appendPlainText(s);

    edit.moveCursor(QTextCursor::Start);

    dialog.exec();
}


void MainWindow::on_actionDiscardDMX_triggered()
{
    int result = QMessageBox::question(this, tr("Discard DMX Data"), tr("This will discard all DMX packets - are you sure?"));
    if(result == QMessageBox::Yes)
    {
        m_packetTable.discardDmxData();
    }
}

bool MainWindow::loadScriptFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Could not open script"), tr("Unable to open file %1").arg(filename));
        return false;
    }

    ui.teScriptEdit->clear();
    ui.teScriptEdit->setPlainText(QString::fromUtf8(file.readAll()));
    file.close();
    return true;
}

void MainWindow::saveScriptFile()
{
    QFile file(m_scriptFileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Could not open script"), tr("Unable to write to file %1").arg(m_scriptFileName));
        return;
    }

    file.write(ui.teScriptEdit->toPlainText().toUtf8());
    file.close();
    m_scriptFileModified = false;
    setWindowModified(false);
}

void MainWindow::on_btnRunScript_pressed()
{
    if(m_captureDevice) {
        m_captureDevice->setMode(ICaptureDevice::TransmitMode);
        m_captureDevice->open();
    }
    m_scripting->run(ui.teScriptEdit->toPlainText());
    ui.btnRunScript->setEnabled(false);
    ui.btnAbortScript->setEnabled(true);
    jsConsoleMessage(QMessageLogContext(), tr("Script Started"));
    ui.teScriptEdit->clearErrors();
}

void MainWindow::on_btnAbortScript_pressed()
{
    m_scripting->abort();
    jsConsoleMessage(QMessageLogContext(), tr("Script Aborted"));
    ui.btnRunScript->setEnabled(true);
    ui.btnAbortScript->setEnabled(false);
}

void MainWindow::scriptFinished(bool error, bool interrupted)
{
    ui.btnRunScript->setEnabled(true);
    ui.btnAbortScript->setEnabled(false);
    if(error && !interrupted)
    {
        ui.teScriptEdit->setErrorOnLine(m_scripting->lastErrorLine()-1, m_scripting->lastErrorDescription());

        jsConsoleMessage(QMessageLogContext(), tr("Script Error - %1 at line %2")
                         .arg(m_scripting->lastErrorDescription())
                         .arg(m_scripting->lastErrorLine()));
    }
}

void MainWindow::on_teScriptEdit_textChanged()
{
    this->setWindowModified(true);
    m_scriptFileModified = true;
}

void MainWindow::jsConsoleMessage(const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QString stamped = QDateTime::currentDateTime().toString("HH:mm:ss") + QString(" ") + msg;
    if(QThread::currentThread()==this->thread())
    {
        ui.teScriptConsole->appendPlainText(stamped);
    }
    else {
        QMetaObject::invokeMethod(ui.teScriptConsole, "appendPlainText", Qt::QueuedConnection, Q_ARG(QString, stamped));
    }
}

void MainWindow::on_btnSerialSetup_pressed()
{
    SerialDialog dialog;
    if(dialog.exec() != QDialog::Accepted) return;

    m_scripting->setSerialPort(dialog.portName(), dialog.baud(), dialog.dataBits(), dialog.parity(), dialog.stopBits(), dialog.flowControl());

    ui.btnSerialSetup->setText(tr("Serial Port: %1 (%2 baud)")
                               .arg(dialog.portName())
                               .arg(dialog.baud()));
}

bool MainWindow::openScriptFile(const QString &fileName)
{
    if (fileName.isEmpty()) return false;
    if (!QFileInfo::exists(fileName)) return false;

    QFile scriptFile(fileName);
    if(!scriptFile.open(QIODevice::ReadOnly))
        return false;

    QString script = QString::fromUtf8(scriptFile.readAll());
    scriptFile.close();
    ui.teScriptEdit->setPlainText(script);
    m_scriptFileModified = false;
    m_scriptFileName = fileName;
    if(m_mode==OPMODE_SCRIPT)
    {
        setWindowFilePath(fileName);
        setWindowModified(false);
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(m_mode==OPMODE_SCRIPT && m_scriptFileModified)
    {
        int result = QMessageBox::question(this,
                tr("Unsaved Changes"),
                tr("You have modified your script file. Want to save changes?"),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        switch(result)
        {
        case QMessageBox::Yes:
            on_actionSave_File_triggered();
            e->accept();
            return;
        case QMessageBox::No:
            e->accept();
            return;
        default:
            e->ignore();
            return;
        }
    }

    e->accept();
}
