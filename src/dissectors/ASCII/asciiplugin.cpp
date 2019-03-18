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

#include "asciiplugin.h"
#include "e110_startcodes.h"
#include "packetbuffer.h"
#include "util.h"
#include <QList>
#include <QTreeWidgetItem>
#include <QString>

enum slotIdx
{
    idxStartCode = 0,
    idxPage = 1,
    idxCharLines = 2,
    idxText = 3,
    idxMinLength = idxText
};

QVariant ASCIIPlugin::getProtocolName()
{
    return "E1.11-2008 ASCII Text Packet";
}

QList<quint8> ASCIIPlugin::getStartCodes()
{
    QList<quint8> startCodes;
    startCodes << E110_SC::ASCII_TEXT;
    return startCodes;
}

QVariant ASCIIPlugin::getSource(const Packet &p)
{
    Q_UNUSED(p);
    return QString();
}

QVariant ASCIIPlugin::getDestination(const Packet &p)
{
    Q_UNUSED(p);
    return QString("Broadcast");
}

QVariant ASCIIPlugin::getInfo(const Packet &p)
{
    switch (static_cast<quint8>(p.at(0)))
    {
        case E110_SC::ASCII_TEXT:
            return QString::fromLatin1(p.data() + idxText, p.size() - idxText);

        default:
            return tr("Unknown");
    }
}

int ASCIIPlugin::preprocessPacket(const Packet &p, QList<Packet> &list)
{
    list.append(p);
    return 1;
}

void ASCIIPlugin::dissectPacket(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = new QTreeWidgetItem();

    parent->setText(0, getProtocolName().toString());
    Util::setPacketByteHighlight(parent, 0, p.size());

    if (p.size() < idxMinLength)
    {
        QTreeWidgetItem *i = new QTreeWidgetItem();
        i->setText(0, "Too Short");
        i->setText(1, QString::number(p.size()));
        Util::setPacketByteHighlight(i, 0, p.size());
        parent->addChild(i);
        return;
    }

    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Start Code"));
    switch (static_cast<quint8>(p.at(idxStartCode)))
    {
        case E110_SC::ASCII_TEXT:
            i->setText(1, QObject::tr("ASCII Text Packet"));
            break;

        default:
            i->setText(1, QObject::tr("Unknown"));
            break;

    }
    Util::setPacketByteHighlight(i, idxStartCode, 1);
    parent->addChild(i);

    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Page"));
    i->setText(1, QString::number(p.at(idxPage)));
    Util::setPacketByteHighlight(i, idxPage, 1);
    parent->addChild(i);

    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("Characters per Line"));
    if (p.at(2) == 0)
        i->setText(1, QObject::tr(("Ignore")));
    else
        i->setText(1, QString::number(p.at(idxCharLines)));
    Util::setPacketByteHighlight(i, idxCharLines, 1);
    parent->addChild(i);

    i = new QTreeWidgetItem();
    i->setText(0, QObject::tr("String"));
    QString strValue = "";
    switch (static_cast<quint8>(p.at(idxStartCode)))
    {
        case E110_SC::ASCII_TEXT:
            strValue = QString::fromLatin1(p.data() + idxText, p.size() - idxText);
            break;

        default:
            break;
    }
    i->setText(1, strValue);
    Util::setPacketByteHighlight(i, 3, std::min(strValue.length(), p.size() - idxText));
    parent->addChild(i);
}


