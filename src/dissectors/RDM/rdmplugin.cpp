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

#include "rdmplugin.h"
#include "e110_startcodes.h"
#include "packetbuffer.h"
#include "util.h"
#include <QList>
#include <QTreeWidgetItem>

#include "rdmdissector.h"


QVariant RdmPlugin::getProtocolName()
{
    return "RDM";
}

bool RdmPlugin::enableByDefault()
{
    return true;
}

QList<quint8> RdmPlugin::getStartCodes()
{
    QList<quint8> startCodes;
    startCodes << E110_SC::RDM << E110_SC::RDM_DSC_RESPONSE;
    return startCodes;
}

QVariant RdmPlugin::getSource(const Packet &p)
{
    if (p.count() < RDM_PACKET_MIN_BYTES)
        return QVariant();
    return Util::formatRdmUid(unpackRdmId(p, RDM_SOURCE_UID));
}

QVariant RdmPlugin::getDestination(const Packet &p)
{
    if (p.count() < RDM_PACKET_MIN_BYTES)
        return QVariant();
    return Util::formatRdmUid(unpackRdmId(p, RDM_DEST_UID));
}

QVariant RdmPlugin::getInfo(const Packet &p)
{
    QString sInfo;

    if (p.length() < RDM_MIN_BYTES)
        sInfo = "TOO SHORT: " + QString::number(p.length()) + " bytes";
    else
    {

        switch(p[RDM_MESSAGE_BLOCK + RDM_CC])
        {
        case E120_DISCOVERY_COMMAND:
            sInfo = "DISCOVERY ";
            break;
        case E120_DISCOVERY_COMMAND_RESPONSE:
            sInfo = "DISCOVERY RESPONSE ";
            break;
        case E120_GET_COMMAND:
            sInfo = "GET ";
            break;
        case E120_GET_COMMAND_RESPONSE:
            sInfo = QString("GET %1 ").arg(RDM_PIDString::responseTypeToString(p[RDM_RESPONSE_TYPE]));
            break;
        case E120_SET_COMMAND:
            sInfo = "SET ";
            break;
        case E120_SET_COMMAND_RESPONSE:
            sInfo = QString("SET %1 ").arg(RDM_PIDString::responseTypeToString(p[RDM_RESPONSE_TYPE]));
            break;
        }

        quint16 paramId = p[RDM_MESSAGE_BLOCK + RDM_PARAMETER_ID] << 8 | p[RDM_MESSAGE_BLOCK + RDM_PARAMETER_ID + 1];
        sInfo.append(Util::paramIdToString(paramId));
    }
//        switch (role)
//        {
//        case Qt::DisplayRole :
//        {

//        } break;
//        case Qt::BackgroundRole :
//            if (p.isRdmCollision) return QBrush(Qt::yellow);
//            break;
//        default: return QVariant();
//        }
//        return QVariant();

    return sInfo;
}

int RdmPlugin::preprocessPacket(const Packet &p, QList<Packet> &list)
{
    // The whip captures RDM discovery responses as one packet combined
    // This splits out the discovery collision reply as it's own packet
    if (p.length() < RDM_PACKET_MIN_BYTES)
    {
        list.append(p);
        return 1;
    }

    int length = p[RDM_LENGTH];
    if(p.length() > length+2)
    {
        QByteArray discResponsePacket(p, length+2);
        QByteArray discPacket(p);
        discPacket.resize(length+2);

        list.append(p);

        list.append(discResponsePacket);

        return 2;
    }
    list.append(p);
    return 1;
}

void RdmPlugin::dissectPacket(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = new QTreeWidgetItem();

    parent->setText(0, getProtocolName().toString());
    Util::setPacketByteHighlight(parent, 0, p.size());

    if (p.size() > 1)
    {
        i = new QTreeWidgetItem();
        i->setText(0, QObject::tr("Start Code"));
        Util::setPacketByteHighlight(i, 0, 1);
        switch ((quint8)p.at(0)) {
        case E110_SC::RDM:
            i->setText(1, QObject::tr("RDM"));
            parent->addChild(i);
            dissectRdm(p, parent);
            break;
        case E110_SC::RDM_DSC_RESPONSE:
            i->setText(1, QObject::tr("RDM Discovery Response"));
            parent->addChild(i);
            dissectRdmDiscResponse(p, parent);
            break;
        default:
            i->setText(1, QObject::tr("Unknown"));
            parent->addChild(i);
            break;
        }
    }
}
