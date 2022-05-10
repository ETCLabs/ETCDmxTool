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

#include "packettable.h"

#include <QDateTime>

PacketTable::PacketTable(QObject *parent)
    : QAbstractTableModel(parent),
      m_timeFormat(DATE_AND_TIME)
{
    emit timeFormatChange();
}

PacketTable::~PacketTable()
{
}

int PacketTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_packets.count();
}

int PacketTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return COLCOUNT;
}

QVariant PacketTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Vertical) return section;

    switch(section)
    {
    case Timestamp  : return tr("Timestamp");
    case Source     : return tr("Source");
    case Destination : return tr("Destination");
    case Protocol   : return tr("Protocol");
    case Info       : return tr("Information");
    }
    return QVariant();
}

QVariant PacketTable::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    if ((row >=0) && (row < rowCount()))
    {
        DissectorPlugin *dissector = m_dissectors->getDissector(m_packets.at(row));
        quint64 timestamp=0, previous=0, first=0;
        timestamp = m_packets.at(row).timestamp;
        if(row>0) previous = m_packets.at(row-1).timestamp;
        first = m_packets.at(0).timestamp;

        if (dissector == Q_NULLPTR)
        {
            // Fallthrough to handle unknown data
            switch (index.column())
            {
            case Timestamp  :
                if (role == Qt::DisplayRole) return formatTime(timestamp, previous, first);
                break;
            case Protocol   :
                if (role == Qt::DisplayRole && m_packets.at(row).length()>0) return tr("Unknown (0x%1)").arg(m_packets.at(row)[0], 2, 16, QLatin1Char('0'));
                if (role == Qt::DisplayRole && m_packets.at(row).length()==0) return tr("Empty");
                break;
            }
            return QVariant();
        }

        switch (index.column())
        {
        case Timestamp  :
            if (role == Qt::DisplayRole) return formatTime(timestamp, previous, first);
            break;
        case Source     :
            if (role == Qt::DisplayRole) return dissector->getSource(m_packets.at(row));
            break;
        case Destination :
            if (role == Qt::DisplayRole) return dissector->getDestination(m_packets.at(row));
            break;
        case Protocol   :
            if (role == Qt::DisplayRole) return dissector->getProtocolName();
            break;
        case Info       :
            return dissector->getInfo(m_packets.at(row), role);
            break;
        default:;
        }
    }
    return QVariant();
}

void PacketTable::clearAll()
{
    beginResetModel();
    m_packets.clear();
    endResetModel();
}

void PacketTable::appendPacket(Packet &packet)
{
    DissectorPlugin *dissector = Q_NULLPTR;
    if (m_dissectors)
        dissector = m_dissectors->getDissector(packet);

    decltype(m_packets) newPackets;
    int startRow = rowCount();
    int endRow = startRow;
    if(dissector)
        endRow = startRow - 1 + dissector->preprocessPacket(packet, newPackets);
    else
        newPackets << packet;

    if (endRow >= startRow) {
        beginInsertRows(QModelIndex(), startRow, endRow);
        m_packets.append(newPackets);
        endInsertRows();
    }

    emit newPacket();
}

const Packet &PacketTable::getPacket(int row) const
{
    return m_packets.at(row);
}

Packet PacketTable::takePacket(int row)
{
    return m_packets.takeAt(row);
}

void PacketTable::registerProtocolDissectors(dissectors *dissectors)
{
    m_dissectors = dissectors;
}

QString PacketTable::formatTime(quint64 time, quint64 previous, quint64 start) const
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(time);
    switch(m_timeFormat)
    {
    case DATE_AND_TIME:
        return dt.toString("yyyy-MM-dd hh:mm:ss.zzz");
    case TIME_OF_DAY:
        return dt.toString("hh:mm:ss.zzz");
    case SECONDS_SINCE_CAPTURE_START:
        return QString::number((time - start)/1000.0);
    case SECONDS_SINCE_PREVIOUS_PACKET:
        return QString::number((time - previous)/1000.0);
    }

    return QString::number(time);
}

void PacketTable::discardDmxData()
{
    beginResetModel();

    QMutableListIterator<Packet>i(m_packets);
    while(i.hasNext())
    {
        Packet p = i.next();
        if(p.at(0)==00)
            i.remove();
    }
    endResetModel();
}
