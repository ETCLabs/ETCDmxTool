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

#include "dmxplugin.h"
#include "e110_startcodes.h"
#include "packetbuffer.h"
#include "util.h"
#include <QList>
#include <QTreeWidgetItem>

QVariant DmxPlugin::getProtocolName()
{
    return "DMX";
}

bool DmxPlugin::enableByDefault()
{
    return true;
}

QList<quint8> DmxPlugin::getStartCodes()
{
    QList<quint8> startCodes;
    startCodes << E110_SC::DIMMER_DATA;
    return startCodes;
}

QVariant DmxPlugin::getSource(const Packet &p)
{
    Q_UNUSED(p);
    return QString();
}

QVariant DmxPlugin::getDestination(const Packet &p)
{
    Q_UNUSED(p);
    return QString("Broadcast");
}

QVariant DmxPlugin::getInfo(const Packet &p)
{
    return QString("Slots %1").arg(p.size() - 1);
}

int DmxPlugin::preprocessPacket(const Packet &p, QList<Packet> &list)
{
    list.append(p);
    return 1;
}

void DmxPlugin::dissectPacket(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = new QTreeWidgetItem();

    parent->setText(0, getProtocolName().toString());
    Util::setPacketByteHighlight(parent, 0, p.size());

    if (p.size() > 1)
    {
        i = new QTreeWidgetItem();
        i->setText(0, QObject::tr("Start Code"));
        if ((quint8)p.at(0) == E110_SC::DIMMER_DATA)
            i->setText(1, QObject::tr("Default"));
        else
            i->setText(1, QObject::tr("Unknown"));
        Util::setPacketByteHighlight(i, 0, 1);
        parent->addChild(i);

        i = new QTreeWidgetItem();
        i->setText(0, QObject::tr("Slot levels"));
        i->setText(1, QString::number(p.size() - 1));
        Util::setPacketByteHighlight(i, 1, p.size() - 1);
        parent->addChild(i);
    }
    return;
}


