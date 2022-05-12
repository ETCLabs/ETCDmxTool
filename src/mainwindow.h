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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QLabel>
#include "ui_mainwindow.h"
#include "whip/ftdcomm.h"
#include <QElapsedTimer>
#include <QProgressBar>
#include <QComboBox>

#include "dissectors.h"
#include "packettable.h"

static QBrush BACKGROUND_INVALID(QColor(255, 80, 80));

class QSortFilterProxyModel;
class ICaptureDevice;
class RDMController;
struct RdmDeviceInfo;
class LevelIndicator;
class Scripting;

constexpr int SCRIPT_SAVE_TIMEOUT = 5000;

class MainWindow : public QMainWindow
{
	Q_OBJECT

    static const int FADER_COUNT = 24;
public:
    MainWindow(ICaptureDevice *captureDevice);
	~MainWindow();
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent* event);

public slots:
    void readData();
    void on_actionSave_File_triggered();
    void on_actionSave_As_triggered();
    void on_actionOpen_File_triggered();
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void on_actionViewLog_triggered();
    void on_actionUpdateGadget_triggered();
	void on_treeWidget_currentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void on_actionExport_to_PcapNg_triggered();
    void modeButtonPressed(bool checked);
    void on_twRdmDevices_currentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void on_tbDmxStartPrev_pressed();
    void on_tbDmxStartNext_pressed();
    void on_sbDmxStart_valueChanged(int value);
    void on_clbGetSensorData_pressed();
    void on_tbAddRawCmdData_pressed();
    void on_tbRemoveRawCmdData_pressed();
    void on_btnSendCustomRDM_pressed();
    void logCategoryToggle(bool checked);
    void logSeverityToggle(bool checked);
    void on_tbSaveLog_pressed();
    void on_btnToggleDmx_toggled(bool checked);
    void on_actionCaptureInfo_triggered();
    void on_actionDiscardDMX_triggered();
    void on_btnRunScript_pressed();
    void on_btnAbortScript_pressed();
    void on_teScriptEdit_textChanged();
    void jsConsoleMessage(const QMessageLogContext &context, const QString &msg);
    void on_btnSerialSetup_pressed();
private slots:
    void updateFilterPattern(const QString &pattern, int PatternSyntax = QRegExp::Wildcard);
    void setFilterColumn(unsigned int column);
    void selectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void faderMoved(int value);
    void updateTxLevels();
    void startFade();
    void fadeTick();
    void crossFaderMoved();

    // RDM Controller
    void on_clbDiscoverRdm_pressed();
    void gotDiscoveryData();
    void updateRdmDisplay();
    void rawDataTypeComboChanged(int index);
    void setupRawDataEditor(int datatype, int row);
    void composeRawCommand();
    void rawCommandComplete(quint8 response, const QByteArray &data);

    // Scripting
    void scriptFinished(bool error, bool interrupted);
private:
    Q_SIGNAL void updateStatusBarMsg();
    Q_SLOT void doUpdatetStatusBarMsg();
    bool isValidMimeData(const QMimeData* mimeData);
    bool openFile(const QString &fileName);
    bool openScriptFile(const QString &fileName);

protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void closeEvent(QCloseEvent *e);
private:
    enum RdmInfoRows
    {
        RDMINFOROW_PROTOCOLVERSION = 0,
        RDMINFOROW_MODELID,
        RDMINFOROW_CATEGORY,
        RDMINFOROW_SWVER,
        RDMINFOROW_DMXFOOTPRINT,
        RDMINFOROW_CURRENTPERS,
        RDMINFOROW_STARTADDR,
        RDMINFOROW_SUBDEV_COUNT,
        RDMINFOROW_SENSORCOUNT,
        RDMINFOROW_DEVLABEL,
        RDMINFOROW_MFRLABEL,
        RDMINFOROW_PARAMCOUNT,
        RDMINFOROW_PARAMLIST,
        RDMINFOROWCOUNT
    };

    enum RdmSensorCols
    {
        RDMSENSORCOL_VALUE,
        RDMSENSORCOL_TYPE,
        RDMSENSORCOL_UNIT,
        RDMSENSORCOL_PREFIX,
        RDMSENSORCOL_RANGEMIN,
        RDMSENSORCOL_RANGEMAX,
        RDMSENSORCOL_NORMMIN,
        RDMSENSORCOL_NORMMAX,
        RDMSENSORCOL_RECVALUE,
        RDMSENSORCOLCOUNT
    };

    // Should correspond to the stacked widget indices
    enum OperationMode
    {
        OPMODE_SNIFFER = 0,
        OPMODE_DMXCONTROL,
        OPMODE_RDMCONTROL,
        OPMODE_DMXVIEW,
        OPMODE_SCRIPT
    };

    Ui::MainWindowClass ui;

    bool m_firstPacket;

    QElapsedTimer m_tickTimer;
	void startCapture();
	void stopCapture();
	void displayPacketDetail(const Packet &packet);
	
	void displayDmxPacket(const Packet &p, QTreeWidgetItem *parent);

    void updateTreeWidget(int currentRow);

	void highlightPacketBytes(int start, int end);

    RdmDeviceInfo *selectedDevice();

    bool loadScriptFile(const QString &filename);
    void saveScriptFile();

    void saveTextFile();

    dissectors m_dissectorList;
    PacketTable m_packetTable;
    QSortFilterProxyModel *m_filterProxy;

    ICaptureDevice *m_captureDevice;

    QLabel *m_filterLabel;
    QComboBox *m_filterColumnCombo;
    QComboBox *m_filterCombo;
    QComboBox *m_filterModeCombo;

    QList<QSlider *> m_scene1sliders;
    QList<QLabel *> m_scene1faderLabels;
    QList<QSlider *> m_scene2sliders;
    QList<QLabel *> m_scene2faderLabels;
    QList<LevelIndicator *> m_sceneOutputs;

    quint8 m_scene1Levels[512];
    quint8 m_scene2Levels[512];
    quint8 m_txLevels[512];

    QTimer *m_fadeTimer;
    QElapsedTimer m_fadeCounter;
    int m_fadeLength;
    bool fadeAtoB;
    RDMController *m_controller;
    QHash<QObject*, int> m_comboToRow;
    QList<QComboBox*> m_customPropCombo;
    QHash<int, QWidget*> m_customPropEdits;
    QByteArray m_customCommandParamData;
    quint16 m_customCommandPid;
    QComboBox *m_commandCombo;
    QComboBox *m_paramCombo;
    QSpinBox *m_subDeviceSpin;
    OperationMode m_mode = OPMODE_SNIFFER;
    QString m_scriptFileName;
    bool m_scriptFileModified = false;
    QString m_captureFileName;
    bool m_captureFileModified = false;
    Scripting *m_scripting = Q_NULLPTR;
};

#endif // MAINWINDOW_H
