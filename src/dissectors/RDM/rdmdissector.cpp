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

#include "RDMDissector.h"

#include <QTreeWidgetItem>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <QStringBuilder>

// E1.20 - 2010
void dissectProductDetailIdListReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectSupportedParametersReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectParameterDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);

void dissectDmxPersonalityReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectDmxPersonalityDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectSlotInfoReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectSlotDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectDefaultSlotValueReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);

void dissectLampState(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectLampOnMode(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);

void dissectDisplayInvert(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectResetDeviceCommand(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectPowerState(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);

void dissectCommsStatusReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectStatusMessageReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectSubDeviceReportThreshold(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectSensorDefinitionReply(const Packet &p, QTreeWidgetItem *parent, int offset);
void dissectSensorValueReply(const Packet &p, QTreeWidgetItem *parent, int offset);
void dissectSelfTestDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectCapturePreset(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectPresetPlayback(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);
void dissectRealTimeClock(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl);


/* Does the real checksum calculation on given buffer. */
/* Also grabs the message size */
quint16 calcChecksum(quint8 &msgsize, const Packet &buffer)
{
	quint16 sum = 0;
	msgsize = buffer[RDM_LENGTH];
	if (buffer.size() > msgsize)
	{
		for(quint8 i=0; i < msgsize; ++i)
		{
			sum = sum + buffer[i];
		}
	}
	return sum;
}

quint64 unpackRdmId(const Packet &p, int start)
{
    // Unpack 48-bit (6 byte) RDM UID to little endian
	quint64 result = 0;
	result |= (quint64) (0xff&p[start])		<< 40;
	result |= (quint64) (0xff&p[start+1])	<< 32;
	result |= (quint64) (0xff&p[start+2])	<< 24;
	result |= (quint64) (0xff&p[start+3])	<< 16;
	result |= (quint64) (0xff&p[start+4])	<< 8;
	result |= (quint64) (0xff&p[start+5]);

	return result;
}

void loadManufIdsFromFile(QIODevice *file, QMap<quint16, QString> &manufMap)
{
    QTextStream stream(file);
    QString line = stream.readLine();
    while (!line.isNull())
    {
        bool ok;
        quint16 tmpId = line.trimmed().left(4).toUInt(&ok, 16);
        if (ok)
        {
            QString tmpName = line.section(',', 1, 1, QString::SectionSkipEmpty);
            if (tmpName.startsWith('"'))
            {
                tmpName.remove('"');
            };
            manufMap.insert(tmpId, tmpName.trimmed());
        }
        line = stream.readLine();
    }
}

QString manufNameFromId(quint16 manuf)
{
    static QMap<quint16, QString> manufMap;
    if (manufMap.isEmpty())
    {
        QFile resFile(":/estaid.csv");
        if (resFile.open(QIODevice::ReadOnly))
        {
            loadManufIdsFromFile(&resFile, manufMap);
        }
        QFile file("estaid.csv");
        if (file.open(QIODevice::ReadOnly))
        {
            loadManufIdsFromFile(&file, manufMap);
        }
        manufMap.insert(0x0000, "ILLEGAL");
        manufMap.insert(0xFFFF, "");
    }

    return manufMap.value(manuf);
}

QString formatRdmUid(quint64 value)
{
	quint16 manuf;
	quint32 deviceid;

	manuf = 0xFFFF & (value >> 32);
	deviceid = 0xFFFFFFFF & value;

    QString result = QString("%1:%2")
            .arg(manuf, 4, 16, QChar('0'))
            .arg(deviceid, 8, 16, QChar('0'))
            .toUpper();

    QString manufName = manufNameFromId(manuf);

    if (manufName.isEmpty()) return result;

    return result % " (" % manufName % ")";
}

void dissectDeviceInfoReply(const Packet &p, QTreeWidgetItem *parent, int offset)
{
	// RDM Protocol Version
	quint8 pvHi = p[offset];
	quint8 pvLo = p[offset+1] ;
	QTreeWidgetItem *i = new QTreeWidgetItem();
	i->setText(0, "RDM Protocol Version");
	i->setText(1, QString::number(pvHi) + QString(".") + QString::number(pvLo));
    Util::setPacketByteHighlight(i, offset, 2);
	parent->addChild(i);
	offset += 2;
	// Model ID
	quint16 modelId = p[offset] << 8 | p[offset+1] ;
	i = new QTreeWidgetItem();
	i->setText(0, "Model ID");
	i->setText(1, QString::number(modelId));
    Util::setPacketByteHighlight(i, offset, 2);
	parent->addChild(i);
	offset += 2;
	// Product Category
	quint16 prodCat = p[offset] << 8 | p[offset+1] ;
	i = new QTreeWidgetItem();
	i->setText(0, "Product Category");
    i->setText(1, RDM_PIDString::productCategoryToString(prodCat) + QString(" (%1)").arg(prodCat));
    Util::setPacketByteHighlight(i, offset, 2);
	parent->addChild(i);
	offset += 2;
	// SW Version ID
	quint32 swVer = p[offset] << 24 | p[offset+1] << 16 | p[offset+2] << 8 | p[offset+3] ;
	i = new QTreeWidgetItem();
	i->setText(0, "Software Version ID");
	i->setText(1, QString("%1").arg(swVer));
    Util::setPacketByteHighlight(i, offset, 4);
	parent->addChild(i);
	offset += 4;
	// DMX Footprint
	quint16 dmxFoot = p[offset] << 8 | p[offset+1] ;
	i = new QTreeWidgetItem();
	i->setText(0, "DMX Footprint");
	i->setText(1, QString("%1").arg(dmxFoot));
    Util::setPacketByteHighlight(i, offset, 2);
	parent->addChild(i);
	offset += 2;
	// DMX Personality
	quint8 dmxPers = p[offset];
	i = new QTreeWidgetItem();
	i->setText(0, "DMX Personality Number");
	i->setText(1, QString("%1").arg(dmxPers));
    if (dmxPers == 0) Util::setItemInvalid(i);
    Util::setPacketByteHighlight(i, offset, 1);
	parent->addChild(i);
	offset += 1;
	quint8 dmxPersCount = p[offset];
	i = new QTreeWidgetItem();
	i->setText(0, "DMX Personality Count");
	i->setText(1, QString("%1").arg(dmxPersCount));
    Util::setPacketByteHighlight(i, offset, 1);
	parent->addChild(i);
	offset += 1;
	// DMX Start
	quint16 dmxStart = p[offset] << 8 | p[offset+1] ;
	i = new QTreeWidgetItem();
	i->setText(0, "DMX Start Address");
	i->setText(1, QString("%1").arg(dmxStart));
    Util::setPacketByteHighlight(i, offset, 2);
	parent->addChild(i);
	offset += 2;
	// Subdevice Count
	quint16 subDevCount = p[offset] << 8 | p[offset+1] ;
	i = new QTreeWidgetItem();
	i->setText(0, "Subdevice Count");
	i->setText(1, QString("%1").arg(subDevCount));
    Util::setPacketByteHighlight(i, offset, 2);
	parent->addChild(i);
	offset += 2;
}

void dissectDiscUniqueBranch(const Packet &p, QTreeWidgetItem *parent)
{
	quint64 lowerBound = unpackRdmId(p, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA);
	QTreeWidgetItem *i = new QTreeWidgetItem();
	i->setText(0, "Lower Bound UID");
	i->setText(1, formatRdmUid(lowerBound));
    Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, RDM_UID_LENGTH);
	parent->addChild(i);

	
	quint64 upperBound = unpackRdmId(p, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA + RDM_UID_LENGTH);
	i = new QTreeWidgetItem();
	i->setText(0, "Upper Bound UID");
	i->setText(1, formatRdmUid(upperBound));
    Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA + RDM_UID_LENGTH, RDM_UID_LENGTH);
	parent->addChild(i);
}

void dissectDiscMuteUnMute(const Packet &p, QTreeWidgetItem *parent, quint8 pdl)
{
	switch(pdl)
	{
	default:
        Util::setItemInvalid(parent);
		return;
	case 0: return;
	case 8:
		{
			quint64 bindingUid = unpackRdmId(p, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA + RDM_MUTE_BINDING_UID);
			QTreeWidgetItem *i = new QTreeWidgetItem();
			i->setText(0, "Binding UID");
			i->setText(1, formatRdmUid(bindingUid));
            Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA + RDM_MUTE_BINDING_UID, RDM_UID_LENGTH);
			parent->addChild(i);
		}
	case 2:
		{
            quint16 control = Util::unpackU16(p, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA + RDM_MUTE_CONTROL_FIELD);

			QTreeWidgetItem *ctrl = new QTreeWidgetItem();
			ctrl->setText(0, "Control Field");
            Util::setPacketByteHighlight(ctrl, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA + RDM_MUTE_CONTROL_FIELD, 2);

			if(control & 0xFFF0)
			{
				QTreeWidgetItem *i = new QTreeWidgetItem();
				i->setText(0, "Reserved");
                Util::setItemInvalid(i);
				i->setText(1, QString("Invalid (%1)").arg(control & 0xFFF0, 4, 16, QChar('0')));
                Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA + RDM_MUTE_CONTROL_FIELD, 2);
				ctrl->addChild(i);
			}

			QString ctrlValue;
			if(control & E120_CONTROL_PROXIED_DEVICE) ctrlValue += "Proxied ";
			if(control & E120_CONTROL_BOOT_LOADER) ctrlValue += "BootLoader ";
			if(control & E120_CONTROL_SUB_DEVICE) ctrlValue += "SubDevice ";
			if(control & E120_CONTROL_MANAGED_PROXY) ctrlValue += "Proxy ";
			ctrl->setText(1, ctrlValue);

			parent->addChild(ctrl);
		} break;
	}
}

void dissectMessageBlock(const Packet &p, QTreeWidgetItem *parent)
{
	
   // Command Class
    const quint8 cc = p[RDM_MESSAGE_BLOCK + RDM_CC];
	QTreeWidgetItem *i = new QTreeWidgetItem();
	i->setText(0, "Command Class");
	i->setText(1, QString("%1 (%2)")
        .arg(RDM_PIDString::commandClassToString(cc))
		.arg(cc, 2, 16, QChar('0')));
    Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_CC, 1);
	parent->addChild(i);
	
   // Parameter Id
	quint16 paramId = p[RDM_MESSAGE_BLOCK + RDM_PARAMETER_ID] << 8 | p[RDM_MESSAGE_BLOCK + RDM_PARAMETER_ID + 1] ;
	i = new QTreeWidgetItem();
	i->setText(0, "Parameter Id");
	i->setText(1, QString("%1 (%2)")
        .arg(Util::paramIdToString(paramId))
		.arg(paramId, 2, 16, QChar('0')));
    Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PARAMETER_ID, 2);
	parent->addChild(i);

   // Param Data Length
	quint8 pdl = p[RDM_MESSAGE_BLOCK + RDM_PDL];
	i = new QTreeWidgetItem();
	i->setText(0, "Param Data Length");
	i->setText(1, QString::number(pdl));
    Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PDL, 1);
	parent->addChild(i);

	// Any data to dissect?
	if(pdl == 0) return;

    QTreeWidgetItem *pidItem = new QTreeWidgetItem();
    pidItem->setText(0, Util::paramIdToString(paramId));
    Util::setPacketByteHighlight(pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
    parent->addChild(pidItem);

    // Fallback generic pid data dissector selection
    quint8 genericDataType = Util::GenericDataNumber | Util::GenericDataText | Util::GenericDataBitset;

	switch(paramId)
	{
    // RDM:2010 7.5 Discovery Messages
	case E120_DISC_UNIQUE_BRANCH:
        dissectDiscUniqueBranch(p, pidItem);
        return;
	case E120_DISC_MUTE:
	case E120_DISC_UN_MUTE:
		if(cc == E120_DISCOVERY_COMMAND_RESPONSE)
		{
            dissectDiscMuteUnMute(p, pidItem, pdl);
            return;
		}
        break;

        // RDM:2010 10.2 Network Management Messages
    case E120_COMMS_STATUS:
        if (cc == E120_GET_COMMAND_RESPONSE)
        {
            dissectCommsStatusReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        }
        break;

        // RDM:2010 10.3 Collection of Queued and Status Messages
    case E120_QUEUED_MESSAGE:
        if ((cc == E120_GET_COMMAND_RESPONSE) || (pdl != 1) )
        {   // Not allowed
            Util::setItemInvalid(pidItem);
        }
        else
        {
            pidItem->setText(1, RDM_PIDString::statusTypeToString(p[RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA]));
            return;
        }
        break;
    case E120_STATUS_MESSAGES:
    {
        switch (cc)
        {
        case E120_GET_COMMAND :
            pidItem->setText(1, RDM_PIDString::statusTypeToString(p[RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA]));
            return;
        case E120_GET_COMMAND_RESPONSE :
            dissectStatusMessageReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_SUB_DEVICE_STATUS_REPORT_THRESHOLD:
    {
        dissectSubDeviceReportThreshold(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
        return;
    }

        // RDM:2010 10.4 RDM Information Messages
    case E120_SUPPORTED_PARAMETERS:
        if (cc == E120_GET_COMMAND_RESPONSE)
        {
            dissectSupportedParametersReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        }
        break;
    case E120_PARAMETER_DESCRIPTION:
        if (cc == E120_GET_COMMAND_RESPONSE)
        {
            dissectParameterDescriptionReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        }
        break;

        // RDM:2010 10.5 Product Information Messages
	case E120_DEVICE_INFO:
		if(cc==E120_GET_COMMAND_RESPONSE)
		{
            dissectDeviceInfoReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA);
            return;
		}
		break;
    case E120_PRODUCT_DETAIL_ID_LIST:
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE:
            dissectProductDetailIdListReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        case E120_GET_COMMAND: return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_DEVICE_MODEL_DESCRIPTION:
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE:
            genericDataType = Util::GenericDataText;
            break;
        case E120_GET_COMMAND:
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_MANUFACTURER_LABEL :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataText;
            break;
        case E120_GET_COMMAND :
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_DEVICE_LABEL :
    {
        switch (cc)
        {
        case E120_SET_COMMAND :
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataText;
            break;
        case E120_SET_COMMAND_RESPONSE :
        case E120_GET_COMMAND :
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_FACTORY_DEFAULTS :
    {
        switch (cc)
        {
        case E120_SET_COMMAND :
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataBool;
            break;
        case E120_SET_COMMAND_RESPONSE :
        case E120_GET_COMMAND :
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
        // TODO: E120_LANGUAGE_CAPABILITIES
        // TODO: E120_LANGUAGE
    case E120_SOFTWARE_VERSION_LABEL :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataText;
            break;
        case E120_GET_COMMAND :
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_BOOT_SOFTWARE_VERSION_ID :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataNumber;
            break;
        case E120_GET_COMMAND :
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_BOOT_SOFTWARE_VERSION_LABEL :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataText;
            break;
        case E120_GET_COMMAND :
        default: Util::setItemInvalid(pidItem);
        }
    } break;

        // RDM:2010 10.6 DMX512 Setup Messages
    case E120_DMX_PERSONALITY :
    {
        switch (cc)
        {
        case E120_SET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        case E120_SET_COMMAND_RESPONSE :
            return;
        case E120_GET_COMMAND_RESPONSE :
            dissectDmxPersonalityReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        case E120_GET_COMMAND :
            return;
        }
    } break;
    case E120_DMX_PERSONALITY_DESCRIPTION :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            dissectDmxPersonalityDescriptionReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        case E120_GET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_DMX_START_ADDRESS :
    {
        switch (cc)
        {
        case E120_SET_COMMAND :
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_SLOT_INFO :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            dissectSlotInfoReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        case E120_GET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_SLOT_DESCRIPTION :
    {
        switch (cc)
        {
        case E120_GET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        case E120_GET_COMMAND_RESPONSE :
            dissectSlotDescriptionReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_DEFAULT_SLOT_VALUE :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            dissectDefaultSlotValueReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        default: Util::setItemInvalid(pidItem);
        }
    } break;

        // RDM:2010 10.7 Sensor Parameter Messages
    case E120_SENSOR_DEFINITION:
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            dissectSensorDefinitionReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA);
            return;
        case E120_GET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_SENSOR_VALUE:
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND_RESPONSE :
            dissectSensorValueReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA);
            return;
        case E120_GET_COMMAND :
        case E120_SET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        }
    } break;
    case E120_RECORD_SENSORS :
    {
        switch (cc)
        {
        case E120_GET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);
        }
    } break;

        // RDM:2010 10.8 Power/Lamp Setting Parameter Messages
    case E120_DEVICE_HOURS :
    case E120_LAMP_HOURS :
    case E120_LAMP_STRIKES :
    case E120_DEVICE_POWER_CYCLES :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;
    case E120_LAMP_STATE :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            dissectLampState(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;
    case E120_LAMP_ON_MODE :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            dissectLampOnMode(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;

        // RDM:2010 10.9 Display Setting Parameter Messages
    case E120_DISPLAY_INVERT :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            dissectDisplayInvert(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;
    case E120_DISPLAY_LEVEL :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;

        // RDM:2010 10.10 Device Configuration Parameter Messages
    case E120_PAN_INVERT :
    case E120_TILT_INVERT :
    case E120_PAN_TILT_SWAP :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            genericDataType = Util::GenericDataBool;
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;

    case E120_REAL_TIME_CLOCK:
        dissectRealTimeClock(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
        return;

        // RDM:2010 10.11 Device Control Parameter Messages
    case E120_IDENTIFY_DEVICE :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            genericDataType = Util::GenericDataBool;
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;
    case E120_RESET_DEVICE :
    {
        switch (cc)
        {
        case E120_SET_COMMAND :
            dissectResetDeviceCommand(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        default: Util::setItemInvalid(pidItem);        }
    } break;
    case E120_POWER_STATE :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            dissectPowerState(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;
    case E120_PERFORM_SELFTEST :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            genericDataType = Util::GenericDataBool;
            break;
        case E120_SET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default: Util::setItemInvalid(pidItem);        }
    } break;
    case E120_SELF_TEST_DESCRIPTION :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
            dissectSelfTestDescriptionReply(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        case E120_GET_COMMAND :
            genericDataType = Util::GenericDataNumber;
            break;
        default : Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_CAPTURE_PRESET :
    {
        switch (cc)
        {
        case E120_SET_COMMAND :
            dissectCapturePreset(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        case E120_SET_COMMAND_RESPONSE :
            break;
        default : Util::setItemInvalid(pidItem);
        }
    } break;
    case E120_PRESET_PLAYBACK :
    {
        switch (cc)
        {
        case E120_GET_COMMAND_RESPONSE :
        case E120_SET_COMMAND :
            dissectPresetPlayback(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl);
            return;
        default : Util::setItemInvalid(pidItem);
        }
    } break;
    default:
        // E1.37-1
        if (RDM_DimmerMsg::dissectMsg(p, pidItem, cc, paramId, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl))
            return;
        break;
    }

    // Generic PID dissector
    pidItem->setText(0, "Data");
    Util::dissectGenericData(p, pidItem, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, pdl, genericDataType);
}

void dissectRdm(const Packet &p, QTreeWidgetItem *parent)
{
	// Within min/max RDM message size
	if(p.size() < RDM_PACKET_MIN_BYTES)
	{
		QTreeWidgetItem *i = new QTreeWidgetItem();
		i->setText(0, "Too Short");
		i->setText(1, QString::number(p.size()));
        Util::setPacketByteHighlight(i, 0, p.size());
		parent->addChild(i);
		return;
	}

	if(p.size() > RDM_PACKET_MAX_BYTES)
	{
		QTreeWidgetItem *i = new QTreeWidgetItem();
		i->setText(0, "Too Long");
		i->setText(1, QString::number(p.size()));
        Util::setPacketByteHighlight(i, 0, RDM_PACKET_MAX_BYTES);
		parent->addChild(i);
		return;
	}

	// Sub Start Code
	quint8 subStartCode = p[RDM_SUB_START_CODE];
	QTreeWidgetItem *i = new QTreeWidgetItem();
	i->setText(0, "Sub-Start Code");
	i->setText(1, QString::number(subStartCode));
    Util::setPacketByteHighlight(i, RDM_SUB_START_CODE, 1);
	parent->addChild(i);
   
	// Length and calculated checksum
	quint8 length;
	quint16 actualChecksum = calcChecksum(length, p);
	
	bool haveMessageBlock = !(p.size() < length + 2);

	i = new QTreeWidgetItem();
	i->setText(0, "Length");
	if (haveMessageBlock)
	{
		i->setText(1, QString::number(length));
	}
	else
	{
        Util::setItemInvalid(i);
		i->setText(1, QString("Short: (%1) expected (%2)").arg(p.size()-2).arg(length));
	}

    Util::setPacketByteHighlight(i, RDM_LENGTH, 1);
	parent->addChild(i);

	// Destination UID   
	quint64 destination = unpackRdmId(p, RDM_DEST_UID);
	i = new QTreeWidgetItem();
	i->setText(0, "Destination UID");
	i->setText(1, formatRdmUid(destination));
    Util::setPacketByteHighlight(i, RDM_DEST_UID, RDM_UID_LENGTH);
	parent->addChild(i);
		
	// Source UID   
	quint64 source = unpackRdmId(p, RDM_SOURCE_UID);
	i = new QTreeWidgetItem();
	i->setText(0, "Source UID");
	i->setText(1, formatRdmUid(source));
    Util::setPacketByteHighlight(i, RDM_SOURCE_UID, RDM_UID_LENGTH);
	parent->addChild(i);

	// Transaction
	quint8 transaction = p[RDM_TRANSACTION_NR];
	i = new QTreeWidgetItem();
	i->setText(0, "Transaction Nr");
	i->setText(1, QString::number(transaction));
    Util::setPacketByteHighlight(i, RDM_TRANSACTION_NR, 1);
	parent->addChild(i);
	
	// Response Type or Port Id
	quint8 cc = p[RDM_MESSAGE_BLOCK + RDM_CC];
	quint8 ackNack = E120_RESPONSE_TYPE_ACK;
	switch(cc)
	{
	case E120_DISCOVERY_COMMAND:
	case E120_GET_COMMAND:
	case E120_SET_COMMAND:
		{
			//Port Id
			quint8 port = p[RDM_PORT_ID];
			i = new QTreeWidgetItem();
			i->setText(0, "Port");
			i->setText(1, QString::number(port));
            Util::setPacketByteHighlight(i, RDM_PORT_ID, 1);
			parent->addChild(i);
		} break;
		
	case E120_DISCOVERY_COMMAND_RESPONSE:
	case E120_GET_COMMAND_RESPONSE:
	case E120_SET_COMMAND_RESPONSE:
		{
			//Response type
			ackNack = p[RDM_RESPONSE_TYPE];
			i = new QTreeWidgetItem();
			i->setText(0, "Response Type");
            i->setText(1, RDM_PIDString::responseTypeToString(ackNack));
            Util::setPacketByteHighlight(i, RDM_RESPONSE_TYPE, 1);
			parent->addChild(i);
		} break;
	}

	// Message Count
	quint8 msg_count = p[RDM_MESSAGE_COUNT];
	i = new QTreeWidgetItem();
	i->setText(0, "Message Count");
	i->setText(1, QString::number(msg_count));
    Util::setPacketByteHighlight(i, RDM_MESSAGE_COUNT, 1);
	parent->addChild(i);

	// Sub device
    quint16 subdevice = Util::unpackU16(p, RDM_SUBDEVICE);
	i = new QTreeWidgetItem();
	i->setText(0, "Subdevice");
	i->setText(1, QString::number(subdevice));
    Util::setPacketByteHighlight(i, RDM_SUBDEVICE, 2);
	parent->addChild(i);

	if(haveMessageBlock)
	{
		switch(ackNack)
		{
		case E120_RESPONSE_TYPE_ACK:
		case E120_RESPONSE_TYPE_ACK_OVERFLOW:
			{
				// Message Block
				i = new QTreeWidgetItem();
				i->setText(0, "Message Block");
                Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK, length+1);
				parent->addChild(i);
				dissectMessageBlock(p, i);
			} break;
		case E120_RESPONSE_TYPE_ACK_TIMER:
			{
				// Timeout
				i = new QTreeWidgetItem();
				i->setText(0, "Ack Timeout");
                quint16 timeout = Util::unpackU16(p, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA);
				i->setText(1, QString::number(float(timeout) / 10, 'f', 1) + " sec");
                Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, 2);
				parent->addChild(i);
			} break;
		case E120_RESPONSE_TYPE_NACK_REASON:
			{
				// Reason
				i = new QTreeWidgetItem();
				i->setText(0, "Nack Reason");
                quint16 reasonCode = Util::unpackU16(p, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA);
                i->setText(1, RDM_PIDString::nackReasonToString(reasonCode));
                Util::setPacketByteHighlight(i, RDM_MESSAGE_BLOCK + RDM_PARAMETER_DATA, 2);
				parent->addChild(i);
			} break;
		}
	}
	
	// Checksum
	quint16 checksum = p[length] << 8 | p[length+1];
	i = new QTreeWidgetItem();
	i->setText(0, "Checksum");
	if (actualChecksum == checksum)
	{
		i->setText(1, QString("Ok (%1)")
			.arg(checksum, 4, 16, QChar('0')));
	}
	else
	{
        Util::setItemInvalid(i);
		i->setText(1, QString("Invalid (%1 != %2)")
			.arg(checksum, 4, 16, QChar('0'))
			.arg(actualChecksum, 4, 16, QChar('0')));
	}
    Util::setPacketByteHighlight(i, length, 2);
	parent->addChild(i);
}


void dissectRdmDiscResponse(const Packet &p, QTreeWidgetItem *parent)
{
	int index=0;
	while(p[index]==0xfe && index<p.length())
		index++;
	// Preamble
	QTreeWidgetItem *i = new QTreeWidgetItem();
	i->setText(0, "Preamble");
    Util::setPacketByteHighlight(i, 0, index);
	parent->addChild(i);

	if(p.length() - index != 17)
	{
		i = new QTreeWidgetItem();
		i->setText(0, "Invalid Response - wrong length");
        Util::setPacketByteHighlight(i, index, p.length());
		parent->addChild(i);
		return;
	}

	quint8 preambleSep = p[index];
	i = new QTreeWidgetItem();
	i->setText(0, "Preamble Separator");
	QString text;
	text = QString("%1 - ").arg(preambleSep, 2, 16, QChar('0'));
	text = text.toUpper();
	if(preambleSep==0xAA)
		text.append("OK");
	else
		text.append("Incorrect");
	i->setText(1, text);
    Util::setPacketByteHighlight(i, index, 1);
	parent->addChild(i);
	index++;
	if(preambleSep!=0xAA)
		return;

	// EUID
	i = new QTreeWidgetItem();
	i->setText(0, "Encoded UID");
    Util::setPacketByteHighlight(i, index, 12);
    parent->addChild(i);

	quint8 euid[12];
	for(int pos=11; pos>=0; pos--)
	{
		euid[pos] = p[index];
		index++;
	}
	
	quint64 rdmId = 
		quint64(euid[11] & euid[10]) << 40 |
		quint64(euid[9] & euid[8]) << 32 |
		quint64(euid[7] & euid[6]) << 24 |
		quint64(euid[5] & euid[4]) << 16 |
		quint64(euid[3] & euid[2]) << 8 |
		quint64(euid[1] & euid[0]);

	i->setText(1, formatRdmUid(rdmId));

	// Checksum
	
	i = new QTreeWidgetItem();
	i->setText(0, "Encoded Checksum");
    Util::setPacketByteHighlight(i, index, 4);
	parent->addChild(i);

	quint8 encChecksum[4];
	for(int pos=3; pos>=0; pos--)
	{
		encChecksum[pos] = p[index];
		index++;
	}
	quint16 checksum = 
		(quint16)(encChecksum[3] & encChecksum[2]) << 8 |
		(quint16)(encChecksum[1] & encChecksum[0]);

	quint16 localChecksum = 0;
	for(int pos=0; pos<12; pos++)
		localChecksum += (quint16)euid[pos];

	text = QString("%1 : ").arg(checksum, 4, 16, QChar('0'));
	if(localChecksum==checksum)
		text.append("Correct");
	else
    {
        text.append(QString("Incorrect - should be %1").arg(localChecksum, 4, 16, QChar('0')));
        Util::setItemInvalid(i);
    }
	i->setText(1, text);
}

void dissectProductDetailIdListReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl == 0) return;   // No data
    int detailsCount = pdl / 2;
    for(int i=0; i < detailsCount; ++i)
    {
        quint16 detailId = Util::unpackU16(p, offset + (i*2));
        QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Product Detail");
        tmpItem->setText(1, QString("%1 (%2)")
                         .arg(RDM_PIDString::detailIdToString(detailId)).arg(detailId));
        Util::setPacketByteHighlight(tmpItem, offset + (i*2), 2);
        parent->addChild(tmpItem);
    }
}

void dissectSupportedParametersReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl == 0) return;   // No data
    int paramsCount = pdl / 2;
    for(int i=0; i < paramsCount; ++i)
    {
        quint16 pid = Util::unpackU16(p, offset + (i*2));
        QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Parameter ID");
        tmpItem->setText(1, QString("%1 (%2)")
                         .arg(Util::paramIdToString(pid)).arg(pid));
        Util::setPacketByteHighlight(tmpItem, offset + (i*2), 2);
        parent->addChild(tmpItem);
    }
}

enum ParameterDescriptionReply
{
    PARAMDESC_PID_OFFSET = 0,
    PARAMDESC_PDLSIZE_OFFSET = 2,
    PARAMDESC_DATA_TYPE_OFFSET = 3,
    PARAMDESC_CC_OFFSET = 4,
    PARAMDESC_TYPE_OFFSET = 5,
    PARAMDESC_UNIT_OFFSET = 6,
    PARAMDESC_PREFIX_OFFSET = 7,
    PARAMDESC_MINVALUE_OFFSET = 8,
    PARAMDESC_MAXVALUE_OFFSET = 12,
    PARAMDESC_DEFAULTVALUE_OFFSET = 16,
    PARAMDESC_DESCRIPTION_OFFSET = 20,
};

void dissectParameterDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl < 0x14)
    {   // Shorter than the minimum length
        Util::setItemInvalid(parent);
        return;
    }

    quint16 pid = Util::unpackU16(p, offset + PARAMDESC_PID_OFFSET);

    QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Parameter ID");
    tmpItem->setText(1, QString("%1 (%2)")
                     .arg(Util::paramIdToString(pid)).arg(pid));
    Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_PID_OFFSET, 2);
    parent->addChild(tmpItem);

    quint8 pdlSize = p[offset + PARAMDESC_PDLSIZE_OFFSET];

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "PDL Size");
    tmpItem->setText(1, QString::number(pdlSize));
    Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_PDLSIZE_OFFSET, 1);
    parent->addChild(tmpItem);

    quint8 dataType = p[offset + PARAMDESC_DATA_TYPE_OFFSET];

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Data Type");
    tmpItem->setText(1, RDM_PIDString::dataTypeToString(dataType));
    Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_DATA_TYPE_OFFSET, 1);
    parent->addChild(tmpItem);

    quint8 cc = p[offset + PARAMDESC_CC_OFFSET];

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Command Class");
    switch (cc)
    {
    case E120_CC_GET : tmpItem->setText(1, "Get Only"); break;
    case E120_CC_SET : tmpItem->setText(1, "Set Only"); break;
    case E120_CC_GET_SET : tmpItem->setText(1, "Get and Set"); break;
    default :
        tmpItem->setText(1, "Invalid: " + QString::number(cc));
        Util::setItemInvalid(tmpItem);
    }
    Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_CC_OFFSET, 1);
    parent->addChild(tmpItem);

    quint8 type = p[offset + PARAMDESC_TYPE_OFFSET];

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Type (Obs.)");
    tmpItem->setText(1, QString::number(type));
    Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_TYPE_OFFSET, 1);
    if (type != 0) Util::setItemInvalid(tmpItem);
    parent->addChild(tmpItem);

    quint8 unit = p[offset + PARAMDESC_UNIT_OFFSET];

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Unit");
    tmpItem->setText(1,Util::sensorUnitToString(unit));
    Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_UNIT_OFFSET, 1);
    parent->addChild(tmpItem);

    quint8 prefix = p[offset + PARAMDESC_PREFIX_OFFSET];

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Prefix");
    tmpItem->setText(1, QString("10^%1").arg(Util::sensorUnitPrefixPower(prefix)));
    Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_PREFIX_OFFSET, 1);
    parent->addChild(tmpItem);

    // Min/Max/Default Values
    switch (dataType)
    {
    // Signed types
    case E120_DS_SIGNED_BYTE :
    case E120_DS_SIGNED_WORD :
    case E120_DS_SIGNED_DWORD :
    {
        qint32 minVal = Util::unpack32(p, offset + PARAMDESC_MINVALUE_OFFSET);
        qint32 maxVal = Util::unpack32(p, offset + PARAMDESC_MAXVALUE_OFFSET);
        qint32 defVal = Util::unpack32(p, offset + PARAMDESC_DEFAULTVALUE_OFFSET);

        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Minimum Value");
        tmpItem->setText(1, QString::number(minVal));
        Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_MINVALUE_OFFSET, 4);
        parent->addChild(tmpItem);
        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Maximum Value");
        tmpItem->setText(1, QString::number(maxVal));
        Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_MAXVALUE_OFFSET, 4);
        parent->addChild(tmpItem);
        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Default Value");
        tmpItem->setText(1, QString::number(defVal));
        Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_DEFAULTVALUE_OFFSET, 4);
        parent->addChild(tmpItem);
    } break;
    // Unsigned types
    case E120_DS_UNSIGNED_BYTE :
    case E120_DS_UNSIGNED_WORD :
    case E120_DS_UNSIGNED_DWORD :
    {
        quint32 minVal = Util::unpackU32(p, offset + PARAMDESC_MINVALUE_OFFSET);
        quint32 maxVal = Util::unpackU32(p, offset + PARAMDESC_MAXVALUE_OFFSET);
        quint32 defVal = Util::unpackU32(p, offset + PARAMDESC_DEFAULTVALUE_OFFSET);

        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Minimum Value");
        tmpItem->setText(1, QString::number(minVal));
        Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_MINVALUE_OFFSET, 4);
        parent->addChild(tmpItem);
        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Maximum Value");
        tmpItem->setText(1, QString::number(maxVal));
        Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_MAXVALUE_OFFSET, 4);
        parent->addChild(tmpItem);
        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Default Value");
        tmpItem->setText(1, QString::number(defVal));
        Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_DEFAULTVALUE_OFFSET, 4);
        parent->addChild(tmpItem);
    } break;
    // Non-numeric types
    case E120_DS_NOT_DEFINED :
    case E120_DS_BIT_FIELD :
    case E120_DS_ASCII :
    default :
    {
        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Min/Max/Default");
        tmpItem->setText(1, "Not supported");
        Util::setPacketByteHighlight(tmpItem, offset + PARAMDESC_MINVALUE_OFFSET, 12);
        parent->addChild(tmpItem);
    } break;
    }

    // Parameter description text
    Util::dissectGenericData(p, parent, offset + PARAMDESC_DESCRIPTION_OFFSET, pdl - PARAMDESC_DESCRIPTION_OFFSET, Util::GenericDataText);
}

void dissectDmxPersonalityReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 2)
    {
        Util::setItemInvalid(parent);
        return;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "DMX Personality Number");
    item->setText(1, QString::number(p[offset]));
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);

    item = new QTreeWidgetItem();
    item->setText(0, "DMX Personality Count");
    item->setText(1, QString::number(p[offset+1]));
    Util::setPacketByteHighlight(item, offset+1, 1);
    parent->addChild(item);
}

enum DmxPersonalityDescriptionReply
{
    DMXPERSDESC_PERSNUM = 0,
    DMXPERSDESC_DMXFOOTPRINT = 1,
    DMXPERSDESC_PERSTEXT = 3
};

void dissectDmxPersonalityDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl < DMXPERSDESC_PERSTEXT)
    {
        Util::setItemInvalid(parent);
        return;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "DMX Personality Number");
    item->setText(1, QString::number(p[offset + DMXPERSDESC_PERSNUM]));
    Util::setPacketByteHighlight(item, offset + DMXPERSDESC_PERSNUM, 1);
    parent->addChild(item);

    item = new QTreeWidgetItem();
    item->setText(0, "DMX Footprint");
    item->setText(1, QString::number(Util::unpackU16(p, offset+DMXPERSDESC_DMXFOOTPRINT)));
    Util::setPacketByteHighlight(item, offset+DMXPERSDESC_DMXFOOTPRINT, 2);
    parent->addChild(item);

    Util::dissectGenericData(p, parent, offset+DMXPERSDESC_PERSTEXT, pdl-DMXPERSDESC_PERSTEXT, Util::GenericDataText);
}

enum SlotInfoReply
{
    SLOTINFO_OFFSET = 0,
    SLOTINFO_TYPE = 2,
    SLOTINFO_LABEL_ID = 3,

    SLOTINFO_SIZE = 5
};

void dissectSlotInfoReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if ((pdl % SLOTINFO_SIZE) != 0)
    {
        Util::setItemInvalid(parent);
        // One of the slot infos was not the right size
    }

    for(int pos = 0; pos+SLOTINFO_SIZE <= pdl; pos +=SLOTINFO_SIZE)
    {
        QTreeWidgetItem *slotItem = new QTreeWidgetItem();
        slotItem->setText(0, "DMX Slot Offset");
        slotItem->setText(1, QString::number(Util::unpackU16(p, offset + pos + SLOTINFO_OFFSET)));
        parent->addChild(slotItem);
        Util::setPacketByteHighlight(slotItem, offset + pos+SLOTINFO_OFFSET, 2);

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, "Type");
        item->setText(1, QString::number(p[offset + pos + SLOTINFO_TYPE]));
        slotItem->addChild(item);
        Util::setPacketByteHighlight(item, offset + pos+SLOTINFO_TYPE, 1);

        item = new QTreeWidgetItem();
        item->setText(0, "Label ID");
        item->setText(1, QString::number(Util::unpackU16(p, offset + pos + SLOTINFO_LABEL_ID)));
        Util::setPacketByteHighlight(item, offset + pos+SLOTINFO_LABEL_ID, 2);
        slotItem->addChild(item);
    }
}

enum SlotDescriptionReply
{
    SLOTDESC_OFFSET = 0,
    SLOTDESC_TEXT = 2
};

void dissectSlotDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl < SLOTDESC_TEXT)
    {
        Util::setItemInvalid(parent);
        return;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Slot Offset");
    item->setText(1, QString::number(Util::unpackU16(p, offset+SLOTDESC_OFFSET)));
    Util::setPacketByteHighlight(item, offset+SLOTDESC_OFFSET, 2);
    parent->addChild(item);

    Util::dissectGenericData(p, parent, offset+SLOTDESC_TEXT, pdl-SLOTDESC_TEXT, Util::GenericDataText);
}

enum DefaultSlotValueReply
{
    DEFSLOTVAL_OFFSET = 0,
    DEFSLOTVAL_VALUE = 2,

    DEFSLOTVAL_SIZE = 3
};

void dissectDefaultSlotValueReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if ((pdl % DEFSLOTVAL_SIZE) != 0)
    {
        Util::setItemInvalid(parent);
        // One of the slot infos was not the right size
    }

    for(int pos = offset; pos+DEFSLOTVAL_SIZE <= pdl; pos +=DEFSLOTVAL_SIZE)
    {
        QTreeWidgetItem *slotItem = new QTreeWidgetItem();
        slotItem->setText(0, "DMX Slot Offset");
        slotItem->setText(1, QString::number(Util::unpackU16(p, pos + DEFSLOTVAL_OFFSET)));
        Util::setPacketByteHighlight(slotItem, pos+DEFSLOTVAL_OFFSET, 2);
        parent->addChild(slotItem);

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, "Type");
        item->setText(1, QString::number(p[pos + DEFSLOTVAL_VALUE]));
        Util::setPacketByteHighlight(item, pos+DEFSLOTVAL_VALUE, 1);
        slotItem->addChild(item);
    }
}

void dissectLampState(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 1)
    {
        Util::setItemInvalid(parent);
        return;
    }

    // RDM:2010 Table A-8
    quint8 stateVal = p[offset];
    QString label;
    switch(stateVal)
    {
    case E120_LAMP_OFF :
        label = "OFF: ";
        break;
    case E120_LAMP_ON :
        label = "ON: ";
        break;
    case E120_LAMP_STRIKE :
        label = "Striking: ";
        break;
    case E120_LAMP_STANDBY :
        label = "Standby: ";
        break;
    case E120_LAMP_NOT_PRESENT :
        label = "Not Present: ";
        break;
    case E120_LAMP_ERROR :
        label = "Error: ";
        break;
    default:
        label = "Custom: ";
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Lamp State");
    item->setText(1, label + QString::number(stateVal));
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);
}

void dissectLampOnMode(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 1)
    {
        Util::setItemInvalid(parent);
        return;
    }

    // RDM:2010 Table A-9
    quint8 stateVal = p[offset];
    QString label;
    switch(stateVal)
    {
    case E120_LAMP_ON_MODE_OFF :
        label = "Lamp Stays off until directly instructed to Strike: ";
        break;
    case E120_LAMP_ON_MODE_DMX :
        label = "Lamp Strikes upon receiving a DMX512 signal: ";
        break;
    case  E120_LAMP_ON_MODE_ON :
        label = "Lamp Strikes automatically at Power-up: ";
        break;
    case  E120_LAMP_ON_MODE_AFTER_CAL :
        label = "Lamp Strikes after Calibration or Homing procedure: ";
        break;
    default:
        label = "Custom: ";
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Lamp On Mode");
    item->setText(1, label + QString::number(stateVal));
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);
}

enum DisplayInvertMode
{
    E120_DISPLAY_INVERT_OFF = 0,
    E120_DISPLAY_INVERT_ON  = 1,
    E120_DISPLAY_INVERT_AUTO = 2
};

void dissectDisplayInvert(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 1)
    {
        Util::setItemInvalid(parent);
        return;
    }

    // RDM:2010 10.9.1
    quint8 stateVal = p[offset];
    QString label;
    switch(stateVal)
    {
    case E120_DISPLAY_INVERT_OFF :
        label = "Normal";
        break;
    case E120_DISPLAY_INVERT_ON :
        label = "Inverted";
        break;
    case  E120_DISPLAY_INVERT_AUTO :
        label = "Automatic";
        break;
    default:
        label = "Invalid: " + QString::number(stateVal);
        Util::setItemInvalid(parent);
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Display Invert");
    item->setText(1, label);
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);
}

enum ResetDeviceCommand
{
    E120_RESET_DEVICE_WARM = 0x01,
    E120_RESET_DEVICE_COLD = 0xFF
};

void dissectResetDeviceCommand(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 1)
    {
        Util::setItemInvalid(parent);
        return;
    }

    // RDM:2010 10.11.2
    quint8 stateVal = p[offset];
    QString label;
    switch(stateVal)
    {
    case E120_RESET_DEVICE_WARM :
        label = "Warm Reset";
        break;
    case E120_RESET_DEVICE_COLD :
        label = "Cold Reset";
        break;
    default:
        label = "Invalid or Custom: " + QString::number(stateVal);
        Util::setItemInvalid(parent);
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Reset Device");
    item->setText(1, label);
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);
}

void dissectPowerState(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 1)
    {
        Util::setItemInvalid(parent);
        return;
    }

    // RDM:2010 Table A-11
    quint8 stateVal = p[offset];
    QString label;
    switch(stateVal)
    {
    case E120_POWER_STATE_FULL_OFF :
        label = "Full Off. Device is unpowered and can no longer respond.";
        break;
    case E120_POWER_STATE_SHUTDOWN :
        label = "Shutdown. Device still responds but may require reset to return to normal operation.";
        break;
    case E120_POWER_STATE_STANDBY :
        label = "Standby. Device still responds and can return to NORMAL without reset.";
        break;
    case E120_POWER_STATE_NORMAL :
        label = "Normal";
        break;
    default:
        label = "Invalid: " + QString::number(stateVal);
        Util::setItemInvalid(parent);
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Power State");
    item->setText(1, label);
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);
}

void dissectCommsStatusReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 6)
    {
        Util::setItemInvalid(parent);
        return;
    }
    quint16 shortMsg = Util::unpackU16(p, offset);

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Short Message Count");
    item->setText(1, QString::number(shortMsg));
    Util::setPacketByteHighlight(item, offset, 2);

    quint16 lengthMismatch = Util::unpackU16(p, offset+2);

    item = new QTreeWidgetItem();
    item->setText(0, "Length Mismatch Count");
    item->setText(1, QString::number(lengthMismatch));
    Util::setPacketByteHighlight(item, offset+2, 2);

    quint16 checksumFail = Util::unpackU16(p, offset+4);

    item = new QTreeWidgetItem();
    item->setText(0, "Checksum Failure Count");
    item->setText(1, QString::number(checksumFail));
    Util::setPacketByteHighlight(item, offset+4, 2);
}

enum StatusMessageReply
{
    STATUS_SUBDEVICE_OFFSET = 0,
    STATUS_TYPE_OFFSET      = 2,
    STATUS_MESSAGE_ID_OFFSET = 3,
    STATUS_DATA_1_OFFSET    = 5,
    STATUS_DATA_2_OFFSET    = 7,

    STATUS_MESSAGE_SIZE = 9
};

void dissectStatusMessageReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl == 0) return;   // No data
    int statusCount = pdl / STATUS_MESSAGE_SIZE;
    int statusMsgNum = 0;

    for(int i=offset;
        i < (p.size() - STATUS_MESSAGE_SIZE);
        i += STATUS_MESSAGE_SIZE)
    {
        ++statusMsgNum;
        quint16 subDevice = Util::unpackU16(p, i + STATUS_SUBDEVICE_OFFSET);
        quint8 statusType = p[i + STATUS_TYPE_OFFSET];
        quint16 statusMessageId = Util::unpackU16(p, i + STATUS_MESSAGE_ID_OFFSET);
        qint16 data1 = Util::unpack16(p, i + STATUS_DATA_1_OFFSET);
        qint16 data2 = Util::unpack16(p, i + STATUS_DATA_2_OFFSET);

        QTreeWidgetItem *statusMessageItem = new QTreeWidgetItem();
        statusMessageItem->setText(0, "Status Message");
        statusMessageItem->setText(1, QString::number(statusMsgNum));
        Util::setPacketByteHighlight(statusMessageItem, i, STATUS_MESSAGE_SIZE);

        QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Subdevice");
        tmpItem->setText(1, QString::number(subDevice));
        Util::setPacketByteHighlight(tmpItem, i+STATUS_SUBDEVICE_OFFSET, 2);
        statusMessageItem->addChild(tmpItem);

        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Status Type");
        tmpItem->setText(1, RDM_PIDString::statusTypeToString(statusType));
        Util::setPacketByteHighlight(tmpItem, i+STATUS_TYPE_OFFSET, 1);
        statusMessageItem->addChild(tmpItem);

        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Description");
        tmpItem->setText(1, RDM_2010::statusMessageToDescription(statusMessageId, data1, data2));
        Util::setPacketByteHighlight(tmpItem, i+STATUS_MESSAGE_ID_OFFSET, 6);
        statusMessageItem->addChild(tmpItem);

        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Message ID");
        tmpItem->setText(1, QString("0x%1").arg(statusMessageId, 4, 16, QChar('0')));
        Util::setPacketByteHighlight(tmpItem, i+STATUS_MESSAGE_ID_OFFSET, 2);
        statusMessageItem->addChild(tmpItem);

        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Data 1");
        tmpItem->setText(1, QString::number(data1));
        Util::setPacketByteHighlight(tmpItem, i+STATUS_DATA_1_OFFSET, 2);
        statusMessageItem->addChild(tmpItem);

        tmpItem = new QTreeWidgetItem();
        tmpItem->setText(0, "Data 2");
        tmpItem->setText(1, QString::number(data2));
        Util::setPacketByteHighlight(tmpItem, i+STATUS_DATA_2_OFFSET, 2);
        statusMessageItem->addChild(tmpItem);

        // All done
        parent->addChild(statusMessageItem);
    }

    if (statusMsgNum != statusCount)
    {
        Util::setItemInvalid(parent);
        parent->setText(1, QString("Invalid PDL, expected %1 got %2 messages")
                        .arg(statusCount).arg(statusMsgNum));
    }
}

void dissectSubDeviceReportThreshold(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl == 0) return;   // No data - set command response

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Threshold Status Type");
    item->setText(1, RDM_PIDString::statusTypeToString(p[offset]));
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);
}

void dissectSensorDefinitionReply(const Packet &p, QTreeWidgetItem *parent, int offset)
{
    quint8 sensorNum = p[offset];

    QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Sensor");
    tmpItem->setText(1, QString::number(sensorNum));
    Util::setPacketByteHighlight(tmpItem, offset, 1);
    parent->addChild(tmpItem);

    ++offset;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Type");
    tmpItem->setText(1, Util::sensorTypeToString(p[offset]));
    Util::setPacketByteHighlight(tmpItem, offset, 1);
    parent->addChild(tmpItem);

    ++offset;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Unit");
    tmpItem->setText(1, Util::sensorUnitToString(p[offset]));
    Util::setPacketByteHighlight(tmpItem, offset, 1);
    parent->addChild(tmpItem);

    ++offset;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Prefix");
    tmpItem->setText(1, QString("10^%1").arg(Util::sensorUnitPrefixPower(p[offset])));
    Util::setPacketByteHighlight(tmpItem, offset, 1);
    parent->addChild(tmpItem);

    ++offset;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Range Minimum");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);

    offset+=2;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Range Maximum");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);

    offset+=2;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Normal Minimum");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);

    offset+=2;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Normal Maximum");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);

    offset+=2;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Recordable");

    quint8 recordableFlag = p[offset];
    QString recordFlags;
    if (recordableFlag & 0x01)
    {
        recordFlags.append("Recorded Value, ");
    }
    if (recordableFlag & 0x02)
    {
        recordFlags.append("Lowest & Highest, ");
    }
    if (recordableFlag & 0xFC)
    {
        recordFlags.append("Reserved Bit Set, ");
        Util::setItemInvalid(tmpItem);
    }

    recordFlags.append("(" + QString::number(recordableFlag) + ")");

    tmpItem->setText(1, recordFlags);
    Util::setPacketByteHighlight(tmpItem, offset, 1);
    parent->addChild(tmpItem);

    ++offset;

    Util::dissectGenericData(p, parent, offset, p.size()-2-offset, Util::GenericDataText);
}

void dissectSensorValueReply(const Packet &p, QTreeWidgetItem *parent, int offset)
{
    QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Sensor");
    tmpItem->setText(1, QString::number(p[offset]));
    Util::setPacketByteHighlight(tmpItem, offset, 1);
    parent->addChild(tmpItem);

    ++offset;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Present Value");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);

    offset+=2;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Minimum Detected");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);

    offset+=2;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Maximum Detected");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);

    offset+=2;

    tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Recorded Value");
    tmpItem->setText(1, QString::number(Util::unpack16(p, offset)));
    Util::setPacketByteHighlight(tmpItem, offset, 2);
    parent->addChild(tmpItem);
}

void dissectSelfTestDescriptionReply(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl == 0)
    {   // Empty replies are not allowed
        Util::setItemInvalid(parent);
        return;
    }
    quint8 selfTestId = p[offset];

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Self Test Number");
    item->setText(1, QString::number(selfTestId));
    Util::setPacketByteHighlight(item, offset, 1);
    parent->addChild(item);

    Util::dissectGenericData(p, parent, offset+1, pdl-1, Util::GenericDataText);
}

double timeFromTenths(const Packet &p, int offset)
{
    quint16 tenths = Util::unpackU16(p, offset);
    return double(tenths) / 10;
}

void dissectCapturePreset(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    switch (pdl)
    {
    case 2:
    {
        Util::dissectGenericData(p, parent, offset, pdl, Util::GenericDataNumber);
        return;
    } break;
    case 4:
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, "Scene Number");
        item->setText(1, QString::number(Util::unpackU16(p, offset)));
        Util::setPacketByteHighlight(item, offset, 2);
        parent->addChild(item);

        item = new QTreeWidgetItem();
        item->setText(0, "Up Fade");
        item->setText(1, QString("%1 sec").arg(timeFromTenths(p, offset+2), 0, 'f', 1));
        Util::setPacketByteHighlight(item, offset+2, 2);
        parent->addChild(item);

        item = new QTreeWidgetItem();
        item->setText(0, "Down Fade");
        item->setText(1, QString("%1 sec").arg(timeFromTenths(p, offset+4), 0, 'f', 1));
        Util::setPacketByteHighlight(item, offset+4, 2);
        parent->addChild(item);

        item = new QTreeWidgetItem();
        item->setText(0, "Wait Time");
        item->setText(1, QString("%1 sec").arg(timeFromTenths(p, offset+6), 0, 'f', 1));
        Util::setPacketByteHighlight(item, offset+6, 2);
        parent->addChild(item);
    } break;
    default: Util::setItemInvalid(parent);
    }
}

enum PresetPlayback
{
    PRESETPLAYBACK_MODE = 0,
    PRESETPLAYBACK_LEVEL = 2
};

void dissectPresetPlayback(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 3)
    {
        Util::setItemInvalid(parent);
        return;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "Playback Mode");
    quint16 mode = Util::unpackU16(p, offset+PRESETPLAYBACK_MODE);
    // Table A-7
    switch (mode)
    {
    case E120_PRESET_PLAYBACK_OFF :
        item->setText(1, "Off");
        break;
    case E120_PRESET_PLAYBACK_ALL :
        item->setText(1, "All as Sequence");
        break;
    default:
        item->setText(1, "Scene #" + QString::number(mode));
    }
    Util::setPacketByteHighlight(item, offset+PRESETPLAYBACK_MODE, 2);
    parent->addChild(item);

    item = new QTreeWidgetItem();
    item->setText(0, "Level");
    item->setText(1, QString::number(p[offset+PRESETPLAYBACK_LEVEL]));
    Util::setPacketByteHighlight(item, offset+PRESETPLAYBACK_LEVEL, 1);
    parent->addChild(item);
}

void dissectRealTimeClock(const Packet &p, QTreeWidgetItem *parent, int offset, int pdl)
{
    if (pdl != 7)
    {
        Util::setItemInvalid(parent);
        return;
    }
    quint16 year = Util::unpackU16(p, offset);
    quint8 month = p[offset+2];
    quint8 day = p[offset+3];
    quint8 hour = p[offset+4];
    quint8 minute = p[offset+5];
    quint8 second = p[offset+6];

    QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
    tmpItem->setText(0, "Real Time Clock");
    tmpItem->setText(1, QString("%1-%2-%3 %4:%5:%6")
                     .arg(year).arg(month).arg(day).arg(hour).arg(minute).arg(second));
    Util::setPacketByteHighlight(tmpItem, offset, 6);
    parent->addChild(tmpItem);
}
