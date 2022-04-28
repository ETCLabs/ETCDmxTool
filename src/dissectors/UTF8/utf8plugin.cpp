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

#include "utf8plugin.h"
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

QVariant UTF8Plugin::getProtocolName()
{
    return tr("E1.11-2008 UTF-8 Text Packet");
}

bool UTF8Plugin::enableByDefault()
{
    return true;
}

QList<quint8> UTF8Plugin::getStartCodes()
{
    QList<quint8> startCodes;
    startCodes << E110_SC::UTF8_TEXT;
    return startCodes;
}

QVariant UTF8Plugin::getSource(const Packet &p)
{
    Q_UNUSED(p);
    return QString();
}

QVariant UTF8Plugin::getDestination(const Packet &p)
{
    Q_UNUSED(p);
    return QObject::tr("Broadcast");
}

QVariant UTF8Plugin::getInfo(const Packet &p, int role)
{
    if (!p.size()) {
        if (role == Qt::DisplayRole)
            return tr("Invalid");
        else if (role == Qt::BackgroundColorRole)
            return Packet::Invalid::INVALID_PACKET_BACKGROUND;
    }

    if (role != Qt::DisplayRole)
        return QVariant();
    switch (static_cast<quint8>(p.at(0)))
    {
        case E110_SC::UTF8_TEXT:
            return getText(p);

        default:
            return tr("Unknown");
    }
}

int UTF8Plugin::preprocessPacket(const Packet &p, QList<Packet> &list)
{
    list.append(p);
    return 1;
}

void UTF8Plugin::dissectPacket(const Packet &p, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *i = nullptr;

    parent->setText(0, getProtocolName().toString());
    Util::setPacketByteHighlight(parent, 0, p.size());

    if (p.size() < idxMinLength)
    {
        i = new QTreeWidgetItem();
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
        case E110_SC::UTF8_TEXT:
            i->setText(1, QObject::tr("UTF-8 Text Packet"));
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
    i->setText(1, getText(p));
    Util::setPacketByteHighlight(i, idxText, getText(p).length());
    parent->addChild(i);
}

QString UTF8Plugin::getText(const Packet &p)
{
    if (p.size() < idxText)
        return QString();
    return QString::fromUtf8(p.data() + idxText, p.size() - idxText);
}

