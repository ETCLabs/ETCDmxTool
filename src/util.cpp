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

#include "util.h"

#include "rdm/estardm.h"
#include "customdataroles.h"

#include <QTreeWidgetItem>
#include <cmath>

void Util::setItemInvalid(QTreeWidgetItem *item)
{
    item->setData(0, Qt::BackgroundRole, Packet::Invalid::INVALID_PARAMETER_BACKGROUND);
}

void Util::setPacketByteHighlight(QTreeWidgetItem *item, int start, int length)
{
    item->setData(0, DATA_ROLE_STARTOFFSET, QVariant(start));
    item->setData(0, DATA_ROLE_ENDOFFSET, QVariant(start + length));
}

quint8 Util::unpackU8(const Packet &p, int start)
{

    // Unpack 8 bit uint
    quint8 result = 0;
    if (p.size() < 1) return result;
    result |= static_cast<quint8>((0xff&p[start]));

    return result;
}

qint8 Util::unpack8(const Packet &p, int start)
{
    // Unpack signed 8 bit int
    qint8 result = 0;
    if (p.size() < 1) return result;
    result |= static_cast<qint8>((0xff&p[start]));

    return result;
}

quint16 Util::unpackU16(const Packet &p, int start)
{
    // Unpack 16 bit uint
    quint16 result = 0;
    if (p.size() < 2) return result;
    result |= static_cast<quint16>((0xff&p[start])  << 8);
    result |= static_cast<quint16>((0xff&p[start+1])<< 0);

    return result;
}

qint16 Util::unpack16(const Packet &p, int start)
{
    // Unpack signed 16 bit int
    qint16 result = 0;
    if (p.size() < 2) return result;
    result |= static_cast<qint16>((0xff&p[start])  << 8);
    result |= static_cast<qint16>((0xff&p[start+1])<< 0);

    return result;
}

unsigned int Util::unpackU24(const Packet &p, int start)
{
    // Unpack 24 bit unsigned int
    unsigned int result = 0;
    if (p.size() < 3) return result;
    result |= static_cast<quint32>((0xff&p[start])   << 16);
    result |= static_cast<quint32>((0xff&p[start+1]) << 8);
    result |= static_cast<quint32>((0xff&p[start+2]) << 0);

    return result;
}

signed int Util::unpack24(const Packet &p, int start)
{
    // Unpack 24 bit signed int
    int result = 0;
    if (p.size() < 3) return result;
    result |= static_cast<int>( (0xff&p[start])   << 16);
    result |= static_cast<int>( (0xff&p[start+1]) << 8);
    result |= static_cast<int>( (0xff&p[start+2]) << 0);

    return result;
}

quint32 Util::unpackU32(const Packet &p, int start)
{
    // Unpack 32 bit unsigned int
    quint32 result = 0;
    if (p.size() < 4) return result;
    result |= static_cast<quint32>( (0xff&p[start])   << 24);
    result |= static_cast<quint32>( (0xff&p[start+1]) << 16);
    result |= static_cast<quint32>( (0xff&p[start+2]) << 8);
    result |= static_cast<quint32>( (0xff&p[start+3]) << 0);

    return result;
}

qint32 Util::unpack32(const Packet &p, int start)
{
    // Unpack 32 bit signed int
    qint32 result = 0;
    if (p.size() < 4) return result;
    result |= static_cast<qint32>((0xff&p[start])   << 24);
    result |= static_cast<qint32>((0xff&p[start+1]) << 16);
    result |= static_cast<qint32>((0xff&p[start+2]) << 8);
    result |= static_cast<qint32>((0xff&p[start+3]) << 0);

    return result;
}

QString Util::strTimeFromTenths(quint16 tenths)
{
    return QString("%1 sec").arg((double(tenths) / 10), 0, 'f', 1);
}

QString Util::displayBits(const Packet &p, int offset, int pdLength)
{
    QString result;
    for (int i=0; i<pdLength; ++i)
    {
        quint8 byte = p[offset+i];
        for (quint8 bit=128; bit > 0; bit = bit >> 1)
        {
            if (byte & bit) result.append("1"); else result.append("0");
        }
    }
    return result;
}

void Util::dissectGenericData(const Packet &p, QTreeWidgetItem *parent, int offset, int pdLength, quint8 dataTypeFlags)
{
    if (pdLength == 0) return; // No data
    if (dataTypeFlags & GenericDataNumber)
    {
        // Number
        switch(pdLength)
        {
        case 1:
        {
            quint8 pd = p[offset];
            qint8 *pds = reinterpret_cast<qint8*>(&pd);
            QTreeWidgetItem *i = new QTreeWidgetItem();
            i->setText(0, "Number");
            i->setText(1, QString("%1 / %2").arg(pd).arg(*pds));
            Util::setPacketByteHighlight(i, offset, pdLength);
            parent->addChild(i);
        } break;
        case 2:
        {
            quint16 pd = unpackU16(p, offset);
            qint16 *pds = reinterpret_cast<qint16*>(&pd);
            QTreeWidgetItem *i = new QTreeWidgetItem();
            i->setText(0, "Number");
            i->setText(1, QString("%1 / %2").arg(pd).arg(*pds));
            Util::setPacketByteHighlight(i, offset, pdLength);
            parent->addChild(i);
        } break;
        case 4:
        {
            quint32 pd = unpackU32(p, offset);
            qint32 *pds = reinterpret_cast<qint32*>(&pd);
            QTreeWidgetItem *i = new QTreeWidgetItem();
            i->setText(0, "Number");
            i->setText(1, QString("%1 / %2").arg(pd).arg(*pds));
            Util::setPacketByteHighlight(i, offset, pdLength);
            parent->addChild(i);
        } break;
        default: ;
        }
    }

    if (dataTypeFlags & GenericDataText)
    {
        // Text
        QByteArray textArray = p.mid(offset, pdLength);
        QTreeWidgetItem *i = new QTreeWidgetItem();
        i->setText(0, "Text");
        i->setText(1, QString::fromLatin1(textArray.data()));
        Util::setPacketByteHighlight(i, offset, pdLength);
        parent->addChild(i);
    }

    if (dataTypeFlags & GenericDataBitset)
    {
        // Bitset
        // Bitset display has arbitrary limit of 32 bits. This is not part of the standard.
        if (pdLength < 5)
        {
            QTreeWidgetItem *i = new QTreeWidgetItem();
            i->setText(0, "Bitset");
            i->setText(1, displayBits(p, offset, pdLength));
            Util::setPacketByteHighlight(i, offset, pdLength);
            parent->addChild(i);
        }
    }

    if (dataTypeFlags & GenericDataBool)
    {
        quint8 pd = p[offset];
        if (pd == 0)
        {
            QTreeWidgetItem *i = new QTreeWidgetItem();
            i->setText(0, "Boolean");
            i->setText(1, "Disabled");
            Util::setPacketByteHighlight(i, offset, pdLength);
            parent->addChild(i);
        }
        else if (pd == 1)
        {
            QTreeWidgetItem *i = new QTreeWidgetItem();
            i->setText(0, "Boolean");
            i->setText(1, "Enabled");
            Util::setPacketByteHighlight(i, offset, pdLength);
            parent->addChild(i);
        }
    }
}


QString Util::formatRdmUid(quint16 manufacturer, quint32 devId)
{
    QString result = QString("%1:%2")
            .arg(manufacturer, 4, 16, QChar('0'))
            .arg(devId, 8, 16, QChar('0'));
    result = result.toUpper();
    return result;
}

QString Util::formatRdmUid(quint64 combinedId)
{
    quint16 manufactuer = (combinedId & 0xFFFF00000000) >> 32;
    quint32 device = combinedId & 0x0000FFFFFFFF;

    return formatRdmUid(manufactuer, device);
}

QString Util::paramIdToString(quint16 paramId)
{
    switch(paramId)
    {
    case E120_DISC_UNIQUE_BRANCH:
        return "DISC_UNIQUE_BRANCH";
    case E120_DISC_MUTE:
        return "DISC_MUTE";
    case E120_DISC_UN_MUTE:
        return "DISC_UN_MUTE";
    case E120_PROXIED_DEVICES:
        return "PROXIED_DEVICES";
    case E120_PROXIED_DEVICE_COUNT:
        return "PROXIED_DEVICE_COUNT";
    case E120_COMMS_STATUS:
        return "COMMS_STATUS";
    case E120_QUEUED_MESSAGE:
        return "QUEUED_MESSAGE";
    case E120_STATUS_MESSAGES:
        return "STATUS_MESSAGES";
    case E120_STATUS_ID_DESCRIPTION:
        return "STATUS_ID_DESCRIPTION";
    case E120_CLEAR_STATUS_ID:
        return "CLEAR_STATUS_ID";
    case E120_SUB_DEVICE_STATUS_REPORT_THRESHOLD:
        return "SUB_DEVICE_STATUS_REPORT_THRESHOLD";
    case E120_SUPPORTED_PARAMETERS:
        return "SUPPORTED_PARAMETERS";
    case E120_PARAMETER_DESCRIPTION:
        return "PARAMETER_DESCRIPTION";
    case E120_DEVICE_INFO:
        return "DEVICE_INFO";
    case E120_PRODUCT_DETAIL_ID_LIST:
        return "PRODUCT_DETAIL_ID_LIST";
    case E120_DEVICE_MODEL_DESCRIPTION:
        return "DEVICE_MODEL_DESCRIPTION";
    case E120_MANUFACTURER_LABEL:
        return "MANUFACTURER_LABEL";
    case E120_DEVICE_LABEL:
        return "DEVICE_LABEL";
    case E120_FACTORY_DEFAULTS:
        return "FACTORY_DEFAULTS";
    case E120_LANGUAGE_CAPABILITIES:
        return "LANGUAGE_CAPABILITIES";
    case E120_LANGUAGE:
        return "LANGUAGE";
    case E120_SOFTWARE_VERSION_LABEL:
        return "SOFTWARE_VERSION_LABEL";
    case E120_BOOT_SOFTWARE_VERSION_ID:
        return "BOOT_SOFTWARE_VERSION_ID";
    case E120_BOOT_SOFTWARE_VERSION_LABEL:
        return "BOOT_SOFTWARE_VERSION_LABEL";
    case E120_DMX_PERSONALITY:
        return "DMX_PERSONALITY";
    case E120_DMX_PERSONALITY_DESCRIPTION:
        return "DMX_PERSONALITY_DESCRIPTION";
    case E120_DMX_START_ADDRESS:
        return "DMX_START_ADDRESS";
    case E120_SLOT_INFO:
        return "SLOT_INFO";
    case E120_SLOT_DESCRIPTION:
        return "SLOT_DESCRIPTION";
    case E120_DEFAULT_SLOT_VALUE:
        return "DEFAULT_SLOT_VALUE";
    case E137_1_DMX_BLOCK_ADDRESS:
        return "E137_1_DMX_BLOCK_ADDRESS";
    case E137_1_DMX_FAIL_MODE:
        return "E137_1_DMX_FAIL_MODE";
    case E137_1_DMX_STARTUP_MODE:
        return "E137_1_DMX_STARTUP_MODE";
    case E120_SENSOR_DEFINITION:
        return "SENSOR_DEFINITION";
    case E120_SENSOR_VALUE:
        return "SENSOR_VALUE";
    case E120_RECORD_SENSORS:
        return "RECORD_SENSORS";
    case E137_1_DIMMER_INFO:
        return "E137_1_DIMMER_INFO";
    case E137_1_MINIMUM_LEVEL:
        return "E137_1_MINIMUM_LEVEL";
    case E137_1_MAXIMUM_LEVEL:
        return "E137_1_MAXIMUM_LEVEL";
    case E137_1_CURVE:
        return "E137_1_CURVE";
    case E137_1_CURVE_DESCRIPTION:
        return "E137_1_CURVE_DESCRIPTION";
    case E137_1_OUTPUT_RESPONSE_TIME:
        return "E137_1_OUTPUT_RESPONSE_TIME";
    case E137_1_OUTPUT_RESPONSE_TIME_DESCRIPTION:
        return "E137_1_OUTPUT_RESPONSE_TIME_DESCRIPTION";
    case E137_1_MODULATION_FREQUENCY:
        return "E137_1_MODULATION_FREQUENCY";
    case E137_1_MODULATION_FREQUENCY_DESCRIPTION:
        return "E137_1_MODULATION_FREQUENCY_DESCRIPTION";
    case E120_DEVICE_HOURS:
        return "DEVICE_HOURS";
    case E120_LAMP_HOURS:
        return "LAMP_HOURS";
    case E120_LAMP_STRIKES:
        return "LAMP_STRIKES";
    case E120_LAMP_STATE:
        return "LAMP_STATE";
    case E120_LAMP_ON_MODE:
        return "LAMP_ON_MODE";
    case E120_DEVICE_POWER_CYCLES:
        return "DEVICE_POWER_CYCLES";
    case E137_1_BURN_IN									:
        return "E137_1_BURN_IN";
    case E120_DISPLAY_INVERT:
        return "DISPLAY_INVERT";
    case E120_DISPLAY_LEVEL:
        return "DISPLAY_LEVEL";
    case E120_PAN_INVERT:
        return "PAN_INVERT";
    case E120_TILT_INVERT:
        return "TILT_INVERT";
    case E120_PAN_TILT_SWAP:
        return "PAN_TILT_SWAP";
    case E120_REAL_TIME_CLOCK:
        return "REAL_TIME_CLOCK";
    case E137_1_LOCK_PIN:
        return "E137_1_LOCK_PIN";
    case E137_1_LOCK_STATE:
        return "E137_1_LOCK_STATE";
    case E137_1_LOCK_STATE_DESCRIPTION:
        return "E137_1_LOCK_STATE_DESCRIPTION";
    case E120_IDENTIFY_DEVICE:
        return "IDENTIFY_DEVICE";
    case E120_RESET_DEVICE:
        return "RESET_DEVICE";
    case E120_POWER_STATE:
        return "POWER_STATE";
    case E120_PERFORM_SELFTEST:
        return "PERFORM_SELFTEST";
    case E120_SELF_TEST_DESCRIPTION:
        return "SELF_TEST_DESCRIPTION";
    case E120_CAPTURE_PRESET:
        return "CAPTURE_PRESET";
    case E120_PRESET_PLAYBACK:
        return "PRESET_PLAYBACK";
    case E137_1_IDENTIFY_MODE:
        return "E137_1_IDENTIFY_MODE";
    case E137_1_PRESET_INFO:
        return "E137_1_PRESET_INFO";
    case E137_1_PRESET_STATUS:
        return "E137_1_PRESET_STATUS";
    case E137_1_PRESET_MERGEMODE:
        return "E137_1_PRESET_MERGEMODE";
    case E137_1_POWER_ON_SELF_TEST:
        return "E137_1_POWER_ON_SELF_TEST";
    }
    if(paramId >= 0x8000 && paramId<=0xFFDF)
    {
        QString format("%1");
        format = format.arg(paramId, 4, 16);
        format = format.toUpper();
        return QString("Manufacturer Specific (%1)").arg(format);
    }
    return QObject::tr("Unknown (0x%1)").arg(paramId, 0, 16);
}


QString Util::sensorTypeToString(quint8 type)
{
    switch (type)
    {
    case E120_SENS_TEMPERATURE : return "Temperature";
    case E120_SENS_VOLTAGE : return "Voltage";
    case E120_SENS_CURRENT : return "Current";
    case E120_SENS_FREQUENCY : return "Frequency";
    case E120_SENS_RESISTANCE : return "Resistance";
    case E120_SENS_POWER : return "Power";
    case E120_SENS_MASS : return "Mass";
    case E120_SENS_LENGTH : return "Length";
    case E120_SENS_AREA : return "Area";
    case E120_SENS_VOLUME : return "Volume";
    case E120_SENS_DENSITY : return "Density";
    case E120_SENS_VELOCITY : return "Velocity";
    case E120_SENS_ACCELERATION : return "Acceleration";
    case E120_SENS_FORCE : return "Force";
    case E120_SENS_ENERGY : return "Energy";
    case E120_SENS_PRESSURE : return "Pressure";
    case E120_SENS_TIME : return "Time";
    case E120_SENS_ANGLE : return "Angle";
    case E120_SENS_POSITION_X : return "Position X";
    case E120_SENS_POSITION_Y : return "Position Y";
    case E120_SENS_POSITION_Z : return "Position Z";
    case E120_SENS_ANGULAR_VELOCITY : return "Angular Velocity";
    case E120_SENS_LUMINOUS_INTENSITY : return "Luminous Intensity";
    case E120_SENS_LUMINOUS_FLUX : return "Luminous Flux";
    case E120_SENS_ILLUMINANCE : return "Illuminance";
    case E120_SENS_CHROMINANCE_RED : return "Chrominance Red";
    case E120_SENS_CHROMINANCE_GREEN : return "Chrominance Green";
    case E120_SENS_CHROMINANCE_BLUE : return "Chrominance Blue";
    case E120_SENS_CONTACTS : return "Contact Closure";
    case E120_SENS_MEMORY : return "Memory";
    case E120_SENS_ITEMS : return "Item Count";
    case E120_SENS_HUMIDITY : return "Humidity";
    case E120_SENS_COUNTER_16BIT : return "Counter";
    case E120_SENS_OTHER : return "Other";
    }
    return QString("Unknown Sensor");
}

int Util::sensorUnitPrefixPower(quint8 prefix)
{
    int power = 0;
    switch (prefix & 0x0F)
    {
    case E120_PREFIX_NONE : power = 0;
        break;
    case E120_PREFIX_DECI : power = 1;
        break;
    case E120_PREFIX_CENTI : power = 2;
        break;
    case E120_PREFIX_MILLI : power = 3;
        break;
    case E120_PREFIX_MICRO : power = 6;
        break;
    case E120_PREFIX_NANO : power = 9;
        break;
    case E120_PREFIX_PICO : power = 12;
        break;
    case E120_PREFIX_FEMPTO : power = 15;
        break;
    case E120_PREFIX_ATTO : power = 18;
        break;
    case E120_PREFIX_ZEPTO : power = 21;
        break;
    case E120_PREFIX_YOCTO : power = 24;
        break;
    default:;
    }

    if (!(prefix & 0xF0))
    {   // Negative
        power = power * -1;
    }
    return power;
}

QString Util::sensorUnitToString(quint8 unit)
{
    switch(unit)
    {
    case E120_UNITS_NONE : return QString("None");
    case E120_UNITS_CENTIGRADE : return QString("Centigrade");
    case E120_UNITS_VOLTS_DC : return QString("Volts DC");
    case E120_UNITS_VOLTS_AC_PEAK : return QString("Volts AC Peak");
    case E120_UNITS_VOLTS_AC_RMS : return QString("Volts AC RMS");
    case E120_UNITS_AMPERE_DC : return QString("Amps DC");
    case E120_UNITS_AMPERE_AC_PEAK : return QString("Amps AC Peak");
    case E120_UNITS_AMPERE_AC_RMS : return QString("Amps AC RMS");
    case E120_UNITS_HERTZ : return QString("Hz");
    case E120_UNITS_OHM : return QString("Ohms");
    case E120_UNITS_WATT : return QString("Watts");
    case E120_UNITS_KILOGRAM : return QString("kilograms");
    case E120_UNITS_METERS : return QString("metres");
    case E120_UNITS_METERS_SQUARED : return QString("m2");
    case E120_UNITS_METERS_CUBED : return QString("m3");
    case E120_UNITS_KILOGRAMMES_PER_METER_CUBED : return QString("kg/m3");
    case E120_UNITS_METERS_PER_SECOND : return QString("m/s");
    case E120_UNITS_METERS_PER_SECOND_SQUARED : return QString("m/s/s");
    case E120_UNITS_NEWTON : return QString("Newtons");
    case E120_UNITS_JOULE : return QString("Joules");
    case E120_UNITS_PASCAL : return QString("Pascals");
    case E120_UNITS_SECOND : return QString("Seconds");
    case E120_UNITS_DEGREE : return QString("Degrees");
    case E120_UNITS_STERADIAN : return QString("Steradian");
    case E120_UNITS_CANDELA : return QString("Candela");
    case E120_UNITS_LUMEN : return QString("Lumen");
    case E120_UNITS_LUX : return QString("Lux");
    case E120_UNITS_IRE : return QString("IRE Chrominance");
    case E120_UNITS_BYTE : return QString("Bytes");
    }
    return "Unknown unit";
}

QString Util::sensorUnitToShortString(quint8 unit)
{
    switch(unit)
    {
    case E120_UNITS_NONE : return QString("");
    case E120_UNITS_CENTIGRADE : return QString("C");
    case E120_UNITS_VOLTS_DC : return QString("VDC");
    case E120_UNITS_VOLTS_AC_PEAK : return QString("VAC");
    case E120_UNITS_VOLTS_AC_RMS : return QString("VAC RMS");
    case E120_UNITS_AMPERE_DC : return QString("A (DC)");
    case E120_UNITS_AMPERE_AC_PEAK : return QString("A (AC, PK)");
    case E120_UNITS_AMPERE_AC_RMS : return QString("A (AC, RMS)");
    case E120_UNITS_HERTZ : return QString("Hz");
    case E120_UNITS_OHM : return QString(QChar(0x2126)); // Ohm symbol..
    case E120_UNITS_WATT : return QString("W");
    case E120_UNITS_KILOGRAM : return QString("Kg");
    case E120_UNITS_METERS : return QString("m");
    case E120_UNITS_METERS_SQUARED : return QString("m2");
    case E120_UNITS_METERS_CUBED : return QString("m3");
    case E120_UNITS_KILOGRAMMES_PER_METER_CUBED : return QString("kg/m3");
    case E120_UNITS_METERS_PER_SECOND : return QString("m/s");
    case E120_UNITS_METERS_PER_SECOND_SQUARED : return QString("m/s/s");
    case E120_UNITS_NEWTON : return QString("N");
    case E120_UNITS_JOULE : return QString("J");
    case E120_UNITS_PASCAL : return QString("Pa");
    case E120_UNITS_SECOND : return QString("s");
    case E120_UNITS_DEGREE : return QString("deg");
    case E120_UNITS_STERADIAN : return QString("ste");
    case E120_UNITS_CANDELA : return QString("cd");
    case E120_UNITS_LUMEN : return QString("lm");
    case E120_UNITS_LUX : return QString("lux");
    case E120_UNITS_IRE : return QString("IRE");
    case E120_UNITS_BYTE : return QString("Byte");
    }
    return "?";
}

QString Util::formatSensorValue(quint8 unit, quint8 prefix, quint16 value)
{
    double float_value;
    float_value = value * pow(10, sensorUnitPrefixPower(prefix));

    return QString("%1 %2")
            .arg(float_value, 0, 'f', 1)
            .arg(sensorUnitToShortString(unit));
}

QMap<quint16, QString> Util::getAllRdmParameterIds()
{
    QMap<quint16, QString> result;
    result[E120_DISC_UNIQUE_BRANCH] = QString("E120_DISC_UNIQUE_BRANCH");
    result[E120_DISC_MUTE] = QString("E120_DISC_MUTE");
    result[E120_DISC_UN_MUTE] = QString("E120_DISC_UN_MUTE");
    result[E120_PROXIED_DEVICES] = QString("E120_PROXIED_DEVICES");
    result[E120_PROXIED_DEVICE_COUNT] = QString("E120_PROXIED_DEVICE_COUNT");
    result[E120_COMMS_STATUS] = QString("E120_COMMS_STATUS");
    result[E120_QUEUED_MESSAGE] = QString("E120_QUEUED_MESSAGE");
    result[E120_STATUS_MESSAGES] = QString("E120_STATUS_MESSAGES");
    result[E120_STATUS_ID_DESCRIPTION] = QString("E120_STATUS_ID_DESCRIPTION");
    result[E120_CLEAR_STATUS_ID] = QString("E120_CLEAR_STATUS_ID");
    result[E120_SUB_DEVICE_STATUS_REPORT_THRESHOLD] = QString("E120_SUB_DEVICE_STATUS_REPORT_THRESHOLD");
    result[E120_SUPPORTED_PARAMETERS] = QString("E120_SUPPORTED_PARAMETERS");
    result[E120_PARAMETER_DESCRIPTION] = QString("E120_PARAMETER_DESCRIPTION");
    result[E120_DEVICE_INFO] = QString("E120_DEVICE_INFO");
    result[E120_PRODUCT_DETAIL_ID_LIST] = QString("E120_PRODUCT_DETAIL_ID_LIST");
    result[E120_DEVICE_MODEL_DESCRIPTION] = QString("E120_DEVICE_MODEL_DESCRIPTION");
    result[E120_MANUFACTURER_LABEL] = QString("E120_MANUFACTURER_LABEL");
    result[E120_DEVICE_LABEL] = QString("E120_DEVICE_LABEL");
    result[E120_FACTORY_DEFAULTS] = QString("E120_FACTORY_DEFAULTS");
    result[E120_LANGUAGE_CAPABILITIES] = QString("E120_LANGUAGE_CAPABILITIES");
    result[E120_LANGUAGE] = QString("E120_LANGUAGE");
    result[E120_SOFTWARE_VERSION_LABEL] = QString("E120_SOFTWARE_VERSION_LABEL");
    result[E120_BOOT_SOFTWARE_VERSION_ID] = QString("E120_BOOT_SOFTWARE_VERSION_ID");
    result[E120_BOOT_SOFTWARE_VERSION_LABEL] = QString("E120_BOOT_SOFTWARE_VERSION_LABEL");
    result[E120_DMX_PERSONALITY] = QString("E120_DMX_PERSONALITY");
    result[E120_DMX_PERSONALITY_DESCRIPTION] = QString("E120_DMX_PERSONALITY_DESCRIPTION");
    result[E120_DMX_START_ADDRESS] = QString("E120_DMX_START_ADDRESS");
    result[E120_SLOT_INFO] = QString("E120_SLOT_INFO");
    result[E120_SLOT_DESCRIPTION] = QString("E120_SLOT_DESCRIPTION");
    result[E120_DEFAULT_SLOT_VALUE] = QString("E120_DEFAULT_SLOT_VALUE");
    result[E137_1_DMX_BLOCK_ADDRESS] = QString("E137_1_DMX_BLOCK_ADDRESS");
    result[E137_1_DMX_FAIL_MODE] = QString("E137_1_DMX_FAIL_MODE");
    result[E137_1_DMX_STARTUP_MODE] = QString("E137_1_DMX_STARTUP_MODE");
    result[E120_SENSOR_DEFINITION] = QString("E120_SENSOR_DEFINITION");
    result[E120_SENSOR_VALUE] = QString("E120_SENSOR_VALUE");
    result[E120_RECORD_SENSORS] = QString("E120_RECORD_SENSORS");
    result[E137_1_DIMMER_INFO] = QString("E137_1_DIMMER_INFO");
    result[E137_1_MINIMUM_LEVEL] = QString("E137_1_MINIMUM_LEVEL");
    result[E137_1_MAXIMUM_LEVEL] = QString("E137_1_MAXIMUM_LEVEL");
    result[E137_1_CURVE] = QString("E137_1_CURVE");
    result[E137_1_CURVE_DESCRIPTION] = QString("E137_1_CURVE_DESCRIPTION");
    result[E137_1_OUTPUT_RESPONSE_TIME] = QString("E137_1_OUTPUT_RESPONSE_TIME");
    result[E137_1_OUTPUT_RESPONSE_TIME_DESCRIPTION] = QString("E137_1_OUTPUT_RESPONSE_TIME_DESCRIPTION");
    result[E137_1_MODULATION_FREQUENCY] = QString("E137_1_MODULATION_FREQUENCY");
    result[E137_1_MODULATION_FREQUENCY_DESCRIPTION] = QString("E137_1_MODULATION_FREQUENCY_DESCRIPTION");
    result[E120_DEVICE_HOURS] = QString("E120_DEVICE_HOURS");
    result[E120_LAMP_HOURS] = QString("E120_LAMP_HOURS");
    result[E120_LAMP_STRIKES] = QString("E120_LAMP_STRIKES");
    result[E120_LAMP_STATE] = QString("E120_LAMP_STATE");
    result[E120_LAMP_ON_MODE] = QString("E120_LAMP_ON_MODE");
    result[E120_DEVICE_POWER_CYCLES] = QString("E120_DEVICE_POWER_CYCLES");
    result[E137_1_BURN_IN] = QString("E137_1_BURN_IN");
    result[E120_DISPLAY_INVERT] = QString("E120_DISPLAY_INVERT");
    result[E120_DISPLAY_LEVEL] = QString("E120_DISPLAY_LEVEL");
    result[E120_PAN_INVERT] = QString("E120_PAN_INVERT");
    result[E120_TILT_INVERT] = QString("E120_TILT_INVERT");
    result[E120_PAN_TILT_SWAP] = QString("E120_PAN_TILT_SWAP");
    result[E120_REAL_TIME_CLOCK] = QString("E120_REAL_TIME_CLOCK");
    result[E137_1_LOCK_PIN] = QString("E137_1_LOCK_PIN");
    result[E137_1_LOCK_STATE] = QString("E137_1_LOCK_STATE");
    result[E137_1_LOCK_STATE_DESCRIPTION] = QString("E137_1_LOCK_STATE_DESCRIPTION");
    result[E120_IDENTIFY_DEVICE] = QString("E120_IDENTIFY_DEVICE");
    result[E120_RESET_DEVICE] = QString("E120_RESET_DEVICE");
    result[E120_POWER_STATE] = QString("E120_POWER_STATE");
    result[E120_PERFORM_SELFTEST] = QString("E120_PERFORM_SELFTEST");
    result[E120_SELF_TEST_DESCRIPTION] = QString("E120_SELF_TEST_DESCRIPTION");
    result[E120_CAPTURE_PRESET] = QString("E120_CAPTURE_PRESET");
    result[E120_PRESET_PLAYBACK] = QString("E120_PRESET_PLAYBACK");
    result[E137_1_IDENTIFY_MODE] = QString("E137_1_IDENTIFY_MODE");
    result[E137_1_PRESET_INFO] = QString("E137_1_PRESET_INFO");
    result[E137_1_PRESET_STATUS] = QString("E137_1_PRESET_STATUS");
    result[E137_1_PRESET_MERGEMODE] = QString("E137_1_PRESET_MERGEMODE");
    result[E137_1_POWER_ON_SELF_TEST] = QString("E137_1_POWER_ON_SELF_TEST");

    return result;

}
