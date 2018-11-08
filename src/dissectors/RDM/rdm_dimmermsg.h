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

#ifndef RDM_DIMMERMSG_H
#define RDM_DIMMERMSG_H

#include "packetbuffer.h"

// Dissect E1.37-1 2012

class QTreeWidgetItem;

class RDM_DimmerMsg
{
public:
    static bool dissectMsg(const Packet &p, QTreeWidgetItem *parent, quint8 cc, quint16 paramId, int offset, int pdl);

private:
    // Supported messages
    // General Parameter Messages
    static void identifyMode(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void blockAddress(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void dmxFailStartMode(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void powerOnTest(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void lockState(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void lockStateDescription(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void lockPin(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void burnIn(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);

    // Dimmer Parameter Messages
    static void dimmerInfo(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void dimmerMinimum(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void dimmerMaximum(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);

    static void optionList(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void optionListDescription(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl, const QString &optionLabel);
    static void modulationFrequencyDescription(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);

    static void presetInfo(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void presetStatus(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);
    static void presetMergeMode(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl);

};

#endif // RDM_DIMMERMSG_H
