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

#ifndef UTIL_H
#define UTIL_H

#include "packetbuffer.h"

class QTreeWidgetItem;

class Util
{
public:
    static void setItemInvalid(QTreeWidgetItem *item);
    static void setPacketByteHighlight(QTreeWidgetItem *item, int start, int length);

    static quint16 unpackU16(const Packet &p, int start);
    static qint16 unpack16(const Packet &p, int start);
    static unsigned int unpackU24(const Packet &p, int start);
    static signed int unpack24(const Packet &p, int start);
    static quint32 unpackU32(const Packet &p, int start);
    static qint32 unpack32(const Packet &p, int start);

    static QString strTimeFromTenths(quint16 tenths);

    static QString displayBits(const Packet &p, int offset, int pdLength);

    enum GenericParamDataTypeFlags
    {
        GenericDataNumber   = 0x01,
        GenericDataText     = 0x02,
        GenericDataBitset   = 0x04,
        GenericDataBool     = 0x08
    };

    static void dissectGenericData(const Packet &p, QTreeWidgetItem *parent, int offset, int pdLength, quint8 dataTypeFlags = GenericDataNumber | GenericDataText | GenericDataBitset);

    static QString formatRdmUid(quint16 manufacturer, quint32 devId);
    static QString formatRdmUid(quint64 combinedId);
    static QString paramIdToString(quint16 paramId);

    static QString sensorUnitToString(quint8 unit);
    static QString sensorUnitToShortString(quint8 unit);
    static int sensorUnitPrefixPower(quint8 prefix);
    static QString sensorTypeToString(quint8 type);
    static QString formatSensorValue(quint8 unit, quint8 prefix, quint16 value);

    /* Gives all RDM param IDs which can be meaningfully set or get
     * excludes some status/discovery params */
    static QMap<quint16, QString> getAllRdmParameterIds();
};

#endif // UTIL_H
