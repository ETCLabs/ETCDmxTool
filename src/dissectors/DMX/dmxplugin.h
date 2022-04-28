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

#ifndef DMX_H
#define DMX_H

#include <QObject>
#include <QtPlugin>
#include "dissectors/dissectorplugin.h"

class DmxPlugin : public QObject, DissectorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DissectorPlugin_iid)
    Q_INTERFACES(DissectorPlugin)

public:
    QVariant getProtocolName() override;
    bool enableByDefault() override;
    QList<quint8> getStartCodes() override;
    QVariant getSource(const Packet &p) override;
    QVariant getDestination(const Packet &p) override;
    QVariant getInfo(const Packet &p, int role = Qt::DisplayRole) override;
    int preprocessPacket(const Packet &p, QList<Packet> &list) override;
    void dissectPacket(const Packet &p, QTreeWidgetItem *parent) override;
};

#endif // DMX_H
