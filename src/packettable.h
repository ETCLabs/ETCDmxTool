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

#ifndef PACKETTABLE_H
#define PACKETTABLE_H

#include <QAbstractTableModel>

#include "packetbuffer.h"
#include "dissectors.h"

class PacketDissector;

class PacketTable : public QAbstractTableModel
{
	Q_OBJECT

public:
    PacketTable(QObject *parent = Q_NULLPTR);
	virtual ~PacketTable();

    enum COLUMNS
    {
        Timestamp,
        Source,
        Destination,
        Protocol,
        Info,
        COLCOUNT
    };

    enum TIMEFORMAT
    {
        DATE_AND_TIME,
        TIME_OF_DAY,
        SECONDS_SINCE_CAPTURE_START,
        SECONDS_SINCE_PREVIOUS_PACKET
    };

	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	int columnCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Raw data access
    void clearAll();
    void appendPacket(Packet &packet);

    const Packet &getPacket(int row) const;
    Packet takePacket(int row);

    // Register dissectors
    void registerProtocolDissectors(dissectors *dissectors);

    // Time Format
    void setTimeFormat(TIMEFORMAT format) {
        m_timeFormat = format;
        emit timeFormatChange();
    }
    TIMEFORMAT timeFormat() const { return m_timeFormat;}

signals:
    void newPacket();
    void timeFormatChange();

private:
    QList<Packet> m_packets;
    dissectors *m_dissectors = Q_NULLPTR;
    TIMEFORMAT m_timeFormat;
    QString formatTime(quint64 time, quint64 previous, quint64 start) const;
};

#endif // PACKETTABLE_H
