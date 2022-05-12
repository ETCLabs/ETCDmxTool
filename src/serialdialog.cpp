#include "serialdialog.h"
#include <QtSerialPort/QSerialPortInfo>

SerialDialog::SerialDialog(QWidget *parent) : QDialog(parent)
{
    m_dialog.setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(auto port, ports)
    {
        m_dialog.cbPort->addItem(port.portName());
    }

    foreach(auto baud, QSerialPortInfo::standardBaudRates())
    {
        m_dialog.cbSpeed->addItem(QString::number(baud));
        if(baud==115200) m_dialog.cbSpeed->setCurrentIndex(m_dialog.cbSpeed->count()-1);
    }

    // Data bits
    m_dialog.cbData->addItem(tr("5 bits"), QVariant(QSerialPort::Data5));
    m_dialog.cbData->addItem(tr("6 bits"), QVariant(QSerialPort::Data6));
    m_dialog.cbData->addItem(tr("7 bits"), QVariant(QSerialPort::Data7));
    m_dialog.cbData->addItem(tr("8 bits"), QVariant(QSerialPort::Data8));
    m_dialog.cbData->setCurrentIndex(m_dialog.cbData->count()-1);

    //parity
    m_dialog.cbParity->addItem(tr("None"),  QVariant(QSerialPort::NoParity));
    m_dialog.cbParity->addItem(tr("Even"),  QVariant(QSerialPort::EvenParity));
    m_dialog.cbParity->addItem(tr("Odd"),   QVariant(QSerialPort::OddParity));
    m_dialog.cbParity->addItem(tr("Space"), QVariant(QSerialPort::SpaceParity));
    m_dialog.cbParity->addItem(tr("Mark"),  QVariant(QSerialPort::MarkParity));
    // stop
    m_dialog.cbStop->addItem(tr("1"),       QVariant(QSerialPort::OneStop));
    m_dialog.cbStop->addItem(tr("1.5"),     QVariant(QSerialPort::OneAndHalfStop));
    m_dialog.cbStop->addItem(tr("2"),       QVariant(QSerialPort::TwoStop));

    // flow
    m_dialog.cbFlow->addItem(tr("None"),        QVariant(QSerialPort::NoFlowControl));
    m_dialog.cbFlow->addItem(tr("Hardware"),    QVariant(QSerialPort::HardwareControl));
    m_dialog.cbFlow->addItem(tr("XON/XOFF"),    QVariant(QSerialPort::SoftwareControl));
}

SerialDialog::~SerialDialog()
{

}
QString SerialDialog::portName()
{
    return m_dialog.cbPort->currentText();
}

QSerialPort::BaudRate SerialDialog::baud()
{
    return static_cast<QSerialPort::BaudRate>(m_dialog.cbSpeed->currentText().toInt());
}

QSerialPort::DataBits SerialDialog::dataBits()
{
    return static_cast<QSerialPort::DataBits>(m_dialog.cbData->currentData().toInt());
}

QSerialPort::Parity SerialDialog::parity()
{
    return static_cast<QSerialPort::Parity>(m_dialog.cbParity->currentData().toInt());
}
QSerialPort::StopBits SerialDialog::stopBits()
{
    return static_cast<QSerialPort::StopBits>(m_dialog.cbStop->currentData().toInt());
}

QSerialPort::FlowControl SerialDialog::flowControl()
{
    return static_cast<QSerialPort::FlowControl>(m_dialog.cbFlow->currentData().toInt());
}

