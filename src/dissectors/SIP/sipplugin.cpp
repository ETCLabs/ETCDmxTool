// Copyright (c) 2018 Electronic Theatre Controls, Inc., http://www.etcconnect.com
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

#include "sipplugin.h"
#include "e110_startcodes.h"
#include "e110_mfctrIDs.h"
#include "packetbuffer.h"
#include "util.h"
#include <QList>
#include <QTreeWidgetItem>
#include <QString>
#include <QBitArray>

enum slotIdx
{
    idxStartCode = 0,
    idxByteCount = 1,
    idxControlBit = 2,
    idxChecksumPrevMSB = 3,
    idxChecksumPrevLSB = 4,
    idxSequence = 5,
    idxDMXUniverse = 6,
    idxDMXProcessingLevel = 7,
    idxVersion = 8,
    idxDMXPacketLengthMSB = 9,
    idxDMXPacketLengthLSB = 10,
    idxPacketCountMSB = 11,
    idxPacketCountLSB = 12,
    idxManufacturerIDMSB = 13,
    idxManufacturerIDLSB = 14,
    idxManufacturerIDMSB_2 = 15,
    idxManufacturerIDLSB_2 = 16,
    idxManufacturerIDMSB_3 = 17,
    idxManufacturerIDLSB_3 = 18,
    idxManufacturerIDMSB_4 = 19,
    idxManufacturerIDLSB_4 = 20,
    idxManufacturerIDMSB_5 = 21,
    idxManufacturerIDLSB_5 = 22,
    idxManufacturerIDMSB_START = idxManufacturerIDMSB_2,
    idxManufacturerIDLSB_START = idxManufacturerIDLSB_2,
    idxManufacturerIDMSB_END = idxManufacturerIDMSB_5,
    idxManufacturerIDLSB_END = idxManufacturerIDLSB_5,
    idxReserved0 = 23,
    idxReserved1 = 24,
    idxMinLength = idxReserved1
};

enum ControlBits {
    kControlBitSubsequent = 1 << 0,
    kControlBitPrevious = 1 << 1
};

enum PacketLengh {
    kPacketLenghFixed = 0x0000,
    kPacketLenghDynamic = 0x8000,
    kPacketLenghMask = 0x03FF
};

enum ManufIds {
    kManufIdsNotDeclared = 0x0000,
    kManufIdsDevelopment = 0xffff
};

QVariant SIPPlugin::getProtocolName()
{
    return tr("E1.11-2008 System Information Packet (SIP)");
}

bool SIPPlugin::enableByDefault()
{
    return true;
}

QList<quint8> SIPPlugin::getStartCodes()
{
    QList<quint8> startCodes;
    startCodes << E110_SC::SIP;
    return startCodes;
}

QVariant SIPPlugin::getSource(const Packet &p)
{
    if (p.size() > idxDMXUniverse) {
        auto universeNum = static_cast<uint8_t>(p.at(idxDMXUniverse));
        return QString("Universe %1").arg(universeNum);
    }
    return QString();
}

QVariant SIPPlugin::getDestination(const Packet &p)
{
    Q_UNUSED(p);
    return QString("Broadcast");
}

QVariant SIPPlugin::getInfo(const Packet &p, int role)
{
    switch (static_cast<quint8>(p.at(0)))
    {
        case E110_SC::SIP:
            if (p.length() >= idxMinLength) {
                auto sequenceNum = static_cast<uint8_t>(p.at(idxSequence));
                bool checksumOk = validateChecksum(p);
                if (role == Qt::DisplayRole) {
                    return QString("Sequence %1 %2")
                            .arg(
                                QString::number(sequenceNum),
                                checksumOk ? "" : "[Checksum error]");
                } else if (role == Qt::BackgroundColorRole) {
                    if (!checksumOk)
                        return Packet::Invalid::INVALID_PACKET_BACKGROUND;
                    else
                        return QVariant();
                }
            } else {
                if (role == Qt::DisplayRole)
                    return tr("Invalid");
                else if (role == Qt::BackgroundColorRole)
                    return Packet::Invalid::INVALID_PACKET_BACKGROUND;
            }

        default:
            if (role == Qt::DisplayRole)
                return tr("Unknown");
            else if (role == Qt::BackgroundColorRole)
                return Packet::Invalid::INVALID_PACKET_BACKGROUND;
    }

    return QVariant();
}

int SIPPlugin::preprocessPacket(const Packet &p, QList<Packet> &list)
{
    list.append(p);
    return 1;
}

void SIPPlugin::dissectPacket(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = nullptr;

    parent->setText(0, getProtocolName().toString());
    Util::setPacketByteHighlight(parent, 0, p.size());

    if (p.size() < idxMinLength)
    {
        i = new QTreeWidgetItem();
        i->setText(0, QObject::tr("Too Short"));
        i->setText(1, QString::number(p.size()));
        Util::setPacketByteHighlight(i, 0, p.size());
        parent->addChild(i);
        return;
    }

    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Start Code"));
    switch (static_cast<quint8>(p.at(idxStartCode)))
    {
        case E110_SC::SIP:
            i->setText(1, QObject::tr("SIP Packet"));
            break;

        default:
            i->setText(1, QObject::tr("Unknown"));
            break;

    }
    Util::setPacketByteHighlight(i, idxStartCode, 1);
    parent->addChild(i);

    // Group SIP packet subitems
    dissectSIPInfo(p, parent);

    // Grouped details about NULL start codes
    dissectDMXInfo(p, parent);
}

void SIPPlugin::dissectDMXInfo(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = nullptr;

    // Grouped NULL Code subitems
    auto dmxItem = new QTreeWidgetItem();
    dmxItem->setText(0, QObject::tr("DMX512"));
    parent->addChild(dmxItem);

    // D5.7 Originating universe
    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Universe"));
    auto universe = static_cast<uint8_t>(p.at(idxDMXUniverse));
    if (universe == 0) {
        i->setText(1, QObject::tr("Not Used"));
    } else {
        i->setText(1, QString::number(universe));
    }
    Util::setPacketByteHighlight(i, idxDMXUniverse, 1);
    dmxItem->addChild(i);

    // D5.10 Packet lengths
    uint16_t packetLen = static_cast<uint8_t>(p.at(idxDMXPacketLengthMSB)) << 8;
    packetLen += static_cast<uint8_t>(p.at(idxDMXPacketLengthLSB));
    // Dynamic Length?
    {
        auto subItem = new QTreeWidgetItem();
        subItem->setText(0, QObject::tr("Dynamic slot count?"));
        subItem->setText(1, packetLen & kPacketLenghDynamic ? tr("True") : tr("False"));
        Util::setPacketByteHighlight(subItem, idxDMXPacketLengthMSB, 1);
        dmxItem->addChild(subItem);
    }
    // Length
    {
        auto subItem = new QTreeWidgetItem();
        subItem->setText(0, QObject::tr("Slot count"));
        subItem->setText(1, QString::number(packetLen & kPacketLenghMask));
        Util::setPacketByteHighlight(subItem, idxDMXPacketLengthMSB, 2);
        dmxItem->addChild(subItem);
    }

    // D5.5 Checksums
    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Checksum of previous"));
    uint16_t checksum = static_cast<uint8_t>(p.at(idxChecksumPrevMSB)) << 8;
    checksum += static_cast<uint8_t>(p.at(idxChecksumPrevLSB));
    if (checksum == 0) {
        i->setText(1, QObject::tr("Not Used"));
    } else {
        i->setText(1, QString::number(checksum));
    }
    Util::setPacketByteHighlight(i, idxChecksumPrevMSB, 2);
    dmxItem->addChild(i);

    // Grouped details about packet history
    dissectHistoryInfo(p, dmxItem);
}

void SIPPlugin::dissectSIPInfo(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = nullptr;

    // Grouped details about SIP
    auto sipItem = new QTreeWidgetItem();
    sipItem->setText(0, QObject::tr("SIP Details"));
    parent->addChild(sipItem);

    // D5.3 SIP checksum pointer
    i = new QTreeWidgetItem();
    auto byteCount = static_cast<uint8_t>(p.at(idxByteCount));
    i->setText(0, QObject::tr("Byte count"));
    if (byteCount != p.size() - 1) {
        i->setText(1, QString("Size mismatch (%1)")
                   .arg(QString::number(byteCount)));
        Util::setItemInvalid(i);
    } else {
        i->setText(1, QString::number(byteCount));
    }
    Util::setPacketByteHighlight(i, idxByteCount, 1);
    sipItem->addChild(i);

    // D5.6 SIP Sequence number
    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Sequence number"));
    i->setText(1, QString::number(static_cast<uint8_t>(p.at(idxSequence))));
    Util::setPacketByteHighlight(i, idxSequence, 1);
    sipItem->addChild(i);

    // D5.11 Number of packets
    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Packets since last"));
    uint16_t packetCount = static_cast<uint8_t>(p.at(idxPacketCountMSB)) << 8;
    packetCount += static_cast<uint8_t>(p.at(idxPacketCountLSB));
    i->setText(1, QString::number(packetCount));
    Util::setPacketByteHighlight(i, idxPacketCountMSB, 2);
    sipItem->addChild(i);

    // D5.4 Control bit field
    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Control Bits"));
    auto controlBits = static_cast<uint8_t>(p.at(idxControlBit));
    Util::setPacketByteHighlight(i, idxControlBit, 1);
    sipItem->addChild(i);

    // D5.4.1 Subsequent NULL packet hold control bit d0
    {
        auto subItem = new QTreeWidgetItem();
        subItem->setText(0, QObject::tr("Hold subsequent NULL packet?"));
        subItem->setText(1, controlBits & kControlBitSubsequent ? tr("True") : tr("False"));
        Util::setPacketByteHighlight(subItem, idxControlBit, 1);
        i->addChild(subItem);
    }

    // D5.4.2 Previous packet bit d1
    {
        auto subItem = new QTreeWidgetItem();
        subItem->setText(0, QObject::tr("Previous packet was an Alternate Start Code?"));
        subItem->setText(1, controlBits & kControlBitPrevious ? tr("True") : tr("False"));
        Util::setPacketByteHighlight(subItem, idxControlBit, 1);
        i->addChild(subItem);
    }

    // D5.14 SIP Checksum
    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Checksum"));
    if (byteCount != p.size() - 1) {
        i->setText(1, QString("Missing"));
        Util::setItemInvalid(i);
    } else {
        uint8_t checksum = static_cast<uint8_t>(p.at(byteCount));
        bool valid = validateChecksum(p);
        i->setText(1, QString("%1 (0x%2)")
                   .arg(
                       valid ? tr("Valid") : tr("Invalid"),
                       QString::number(checksum,16)));
        Util::setPacketByteHighlight(i, byteCount, 1);
        if (!valid)
            Util::setItemInvalid(i);
    }
    sipItem->addChild(i);
}

void SIPPlugin::dissectOrginatorInfo(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = nullptr;

    // Grouped details about frame originator
    auto orginatorItem = new QTreeWidgetItem();
    orginatorItem->setText(0, QObject::tr("Orginator"));
    parent->addChild(orginatorItem);

    // D5.12 Manufacturer ID
    orginatorItem->addChild(
                &manufacturerDetails(
                    p, idxManufacturerIDMSB, idxManufacturerIDLSB));

    // D5.9 Software version
    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Software version"));
    auto softwareVersion = static_cast<uint8_t>(p.at(idxVersion));
    if (softwareVersion == 0) {
        i->setText(1, QObject::tr("Not Implemented"));
    } else {
        i->setText(1, QString::number(softwareVersion));
    }
    Util::setPacketByteHighlight(i, idxVersion, 1);
    orginatorItem->addChild(i);
}

void SIPPlugin::dissectHistoryInfo(const Packet &p, QTreeWidgetItem *parent)
{
    // Grouped details about packet history
    auto historyItem = new QTreeWidgetItem();
    historyItem->setText(0, QObject::tr("Packet History"));
    parent->addChild(historyItem);

    // Grouped details about frame originator
    dissectOrginatorInfo(p, historyItem);

    // D5.8 DMX512 processing level
    auto i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Hop Count"));
    i->setText(1, QString::number(static_cast<uint8_t>(p.at(idxDMXProcessingLevel))));
    Util::setPacketByteHighlight(i, idxDMXProcessingLevel, 1);
    historyItem->addChild(i);

    // D5.13 Packet history
    uint8_t msbSlot = idxManufacturerIDMSB_START;
    uint8_t lsbSlot = idxManufacturerIDLSB_START;
    uint8_t hop = 0;
    while (msbSlot <= idxManufacturerIDMSB_END && lsbSlot <= idxManufacturerIDLSB_END) {
        auto hopItem = new QTreeWidgetItem();
        hopItem->setText(0, QObject::tr("Hop %1").arg(QString::number(hop++)));
        hopItem->addChild(
                    &manufacturerDetails(
                        p, msbSlot, lsbSlot));
        msbSlot += 2;
        lsbSlot += 2;
        historyItem->addChild(hopItem);
    }
}

QTreeWidgetItem& SIPPlugin::manufacturerDetails(const Packet &p, uint8_t slotMSB, uint8_t slotLSB)
{
    auto i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Manufacturer ID"));
    uint16_t manuId = static_cast<uint8_t>(p.at(slotMSB)) << 8;
    manuId += static_cast<uint8_t>(p.at(slotLSB));
    Util::setPacketByteHighlight(i, slotMSB, 2);

    QString manuStr = "Unknown";
    switch (manuId) {
    case kManufIdsNotDeclared:
        manuStr = QObject::tr("Not declared");
        break;
    case kManufIdsDevelopment:
        manuStr = QObject::tr("Development");
        break;
    default:
        if (E110_MFCTRIDS::mfctrIds.count(manuId))
          manuStr = E110_MFCTRIDS::mfctrIds.at(manuId).c_str();
    }

    i->setText(1, QString("%1 (0x%2)")
               .arg(manuStr, QString::number(manuId, 16)));

    return *i;
}

bool SIPPlugin::validateChecksum(const Packet &p)
{
    uint8_t calc = 0;
    uint8_t expected = p.at(p.size() - 1);

    /* 8 bit ones complement additive checksum
     * of the SIP START Code (CFh)
     * and all subsequent slots of SIP data.
    */
    for (size_t slot = 0; slot < static_cast<size_t>(p.size() - 1); ++slot)
        calc += p.at(slot);

    return calc == expected;
}
