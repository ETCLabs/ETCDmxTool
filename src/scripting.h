#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <QObject>
#include <QJSEngine>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include "capturedevice.h"


class ScriptUtils : public QObject
{
    Q_OBJECT
public:
    ScriptUtils();

    Q_INVOKABLE const QString timestamp();
public slots:
    void sleep(int milliseconds);
};


/** ScriptSerial provides a serial port wrapper accessible from the script engine */
/* NB this has to run its own thread for serial port access */
class ScriptSerial : public QObject
{
    Q_OBJECT
public:
    ScriptSerial(QString name, QSerialPort::BaudRate baud, QSerialPort::DataBits dataBits,
                 QSerialPort::Parity parity, QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl
                 );
    ~ScriptSerial();

    Q_INVOKABLE QString readText();
public slots:
    void writeText(const QString &text);
private slots:
    void openPort();
    void readyRead();
private:
    QSerialPort *m_port;
    QThread *m_serialThread;
    QByteArray m_buffer;
    QMutex m_bufferMutex;
};


/* NB this has to run its own thread for file access */
class ScriptFile : public QObject
{
    Q_OBJECT
public:
    ScriptFile(const QString &filename);
    ~ScriptFile();
public slots:
    void write(const QString &text);
private slots:
    void open();
private:
    QFile *m_file;
    QThread *m_fileThread;
};

/* ScriptDMX provides acecess to output DMX levels from within the scripts */
class ScriptDMX : public QObject
{
    Q_OBJECT
public:
    ScriptDMX(ICaptureDevice *device);
public slots:
    void setLevel(quint16 address, quint8 value);
    void enable();
    void disable();
private:
    quint8 m_dmxLevels[512];
    ICaptureDevice *m_captureDevice;
};

class Scripting : public QObject
{
    Q_OBJECT
public:
    explicit Scripting(ICaptureDevice *device);
    QString lastErrorDescription() { return m_lastError;}
    int lastErrorLine() { return m_lastErrorLine;}
    void setSerialPort(QString name, QSerialPort::BaudRate baud, QSerialPort::DataBits dataBits,
                       QSerialPort::Parity parity, QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl
                       );
signals:
    void finished(bool error, bool interrupted);
public slots:
    bool isRunning();
    void run(const QString &script);
    void abort();
private slots:
    void initialize();
private:
    QJSEngine m_engine;
    bool m_running;
    ICaptureDevice *m_captureDevice;
    ScriptDMX *m_dmx;
    QString m_lastError;
    int m_lastErrorLine;
    QSerialPort *m_serialPort = Q_NULLPTR;

    QString m_name;
    QSerialPort::BaudRate m_baud;
    QSerialPort::DataBits m_dataBits;
    QSerialPort::Parity m_parity;
    QSerialPort::StopBits m_stopBits;
    QSerialPort::FlowControl m_flowControl;

    QString m_filename;

};

#endif // SCRIPTING_H
