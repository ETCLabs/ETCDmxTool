#include "scripting.h"
#include <QDebug>
#include <QMutexLocker>
#include <QDateTime>

/************************************************ ScriptUtils ************************************************/

ScriptUtils::ScriptUtils()
{

}

const QString ScriptUtils::timestamp()
{
    return QDateTime::currentDateTime().toString();
}

void ScriptUtils::sleep(int milliseconds)
{
    QThread::msleep(milliseconds);
}


/************************************************ ScriptSerial ************************************************/
ScriptSerial::ScriptSerial(QString name, QSerialPort::BaudRate baud, QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity, QSerialPort::StopBits stopBits,
                           QSerialPort::FlowControl flowControl)

{
    m_serialThread = new QThread();
    m_serialThread->setObjectName("SerialThread");
    this->moveToThread(m_serialThread);
    m_port = new QSerialPort;
    m_port->moveToThread(m_serialThread);
    m_port->setPortName(name);
    m_port->setBaudRate(baud);
    m_port->setDataBits(dataBits);
    m_port->setParity(parity);
    m_port->setStopBits(stopBits);
    m_port->setFlowControl(flowControl);

    connect(m_serialThread, SIGNAL(started()), this, SLOT(openPort()), Qt::QueuedConnection);

    m_serialThread->start();
}

ScriptSerial::~ScriptSerial()
{
    m_serialThread->exit();
    m_serialThread->wait();
    delete m_serialThread;
    delete m_port;
}

void ScriptSerial::openPort()
{
    m_port->open(QIODevice::ReadWrite);
    connect(m_port, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void ScriptSerial::readyRead()
{
    m_bufferMutex.lock();
    m_buffer.append(m_port->readAll());
    m_bufferMutex.unlock();
}


QString ScriptSerial::readText()
{
    QMutexLocker locker(&m_bufferMutex);
    if(m_buffer.indexOf(13)>-1)
    {
        QString result = QString(m_buffer);
        m_buffer.clear();
        return result;
    }
    else
    {
        return QString();
    }
}

void ScriptSerial::writeText(const QString &text)
{
    if(QThread::currentThread()==this->thread())
        m_port->write(text.toLatin1());
    else
        QMetaObject::invokeMethod(this, "writeText", Qt::QueuedConnection, Q_ARG(QString, text));
}


/************************************************ ScriptFile ************************************************/
ScriptFile::ScriptFile(const QString &filename)

{
    m_fileThread = new QThread();
    m_fileThread->setObjectName("FileThread");
    this->moveToThread(m_fileThread);

    m_file = new QFile(filename);

    connect(m_fileThread, SIGNAL(started()), this, SLOT(open()), Qt::QueuedConnection);

    m_fileThread->start();
}

ScriptFile::~ScriptFile()
{
    m_fileThread->exit();
    m_fileThread->wait();
    delete m_fileThread;
    delete m_fileThread;
}

void ScriptFile::open()
{
    m_file->open(QIODevice::WriteOnly);
}

void ScriptFile::write(const QString &text)
{
    if(QThread::currentThread()==this->thread())
        m_file->write(text.toLatin1());
    else
        QMetaObject::invokeMethod(this, "write", Qt::QueuedConnection, Q_ARG(QString, text));
}

/************************************************ ScriptDMX ************************************************/
ScriptDMX::ScriptDMX(ICaptureDevice *device) : QObject(Q_NULLPTR), m_captureDevice(device)
{
    memset(m_dmxLevels, 0, 512);
}

void ScriptDMX::setLevel(quint16 address, quint8 value)
{
    if(address>511) return;
    m_dmxLevels[address] = value;
    if(m_captureDevice)
        m_captureDevice->setDmxLevels(m_dmxLevels, 512);
}

void ScriptDMX::enable()
{
    if(m_captureDevice)
        m_captureDevice->setDmxEnabled(true);
}

void ScriptDMX::disable()
{
    if(m_captureDevice)
        m_captureDevice->setDmxEnabled(false);
}

Scripting::Scripting(ICaptureDevice *device) : QObject(Q_NULLPTR), m_captureDevice(device)
{
    QThread *thread = new QThread();
    thread->setObjectName("ScriptThread");
    m_running = false;
    m_captureDevice = device;
    this->moveToThread(thread);
    connect(thread, &QThread::started, this, &Scripting::initialize, Qt::QueuedConnection);
    thread->start();
}

void Scripting::initialize()
{
    m_dmx = new ScriptDMX(m_captureDevice);
    m_dmx->setObjectName("dmx");

    m_engine.installExtensions(QJSEngine::ConsoleExtension);
}

bool Scripting::isRunning()
{
    return m_running;
}

void Scripting::run(const QString &script)
{
    if(QThread::currentThread()!=this->thread())
    {
        QMetaObject::invokeMethod(this, "run", Qt::QueuedConnection, Q_ARG(QString, script));
        return;
    }

    m_engine.setInterrupted(false);

    QJSValue dmxValue = m_engine.newQObject(m_dmx);
    m_engine.globalObject().setProperty("dmx", dmxValue);

    ScriptUtils *scriptUtils = new ScriptUtils();
    QJSValue utilsValue = m_engine.newQObject(scriptUtils);
    m_engine.globalObject().setProperty("utils", utilsValue);

    ScriptSerial *serial = Q_NULLPTR;
    if(!m_name.isEmpty())
    {
        serial = new ScriptSerial(m_name, m_baud, m_dataBits, m_parity, m_stopBits, m_flowControl);
        QJSValue serialValue = m_engine.newQObject(serial);
        m_engine.globalObject().setProperty("serial", serialValue);
    }


    ScriptFile *file = Q_NULLPTR;
    if(!m_filename.isEmpty())
    {
        file = new ScriptFile(m_filename);
        QJSValue fileValue = m_engine.newQObject(file);
        m_engine.globalObject().setProperty("file", fileValue);
    }

    m_running = true;
    QJSValue value = m_engine.evaluate(script);
    m_running = false;

    if(serial)
        delete serial;
    if(file)
        delete file;
    delete scriptUtils;


    if(value.isError())
    {
        m_lastError = value.property("message").toString();
        m_lastErrorLine = value.property("lineNumber").toInt();
    }

    emit finished(value.isError(), m_engine.isInterrupted());
}

void Scripting::setSerialPort(QString name, QSerialPort::BaudRate baud, QSerialPort::DataBits dataBits,
QSerialPort::Parity parity, QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl)
{
    m_name = name;
    m_baud = baud;
    m_dataBits = dataBits;
    m_parity = parity;
    m_stopBits = stopBits;
    m_flowControl = flowControl;
}

void Scripting::abort()
{
    if(m_running)
    {
        m_engine.setInterrupted(true);
    }
}
