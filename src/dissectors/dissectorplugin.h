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

// The interface for dissector plugins

#ifndef DISSECTORPLUGIN_H
#define DISSECTORPLUGIN_H

#include <QString>
#include <QTreeWidgetItem>

class Packet;

class DissectorPlugin
{
public:
    /*
     * @return Protocol friendly name
     */
    virtual QVariant getProtocolName() = 0;

    /*
     * @return Should this protocol be enabled by default
     * Generally disable for 'greedy' protocols that absorb
     * multiple start codes
     */
    virtual bool enableByDefault() = 0;

    /*
     * @return Accepted startcodes
     */
    virtual QList<quint8> getStartCodes() = 0;

    /*
     * @return Packet source address
     */
    virtual QVariant getSource(const Packet &p) = 0;

    /*
     * @return Packet destination address
     */
    virtual QVariant getDestination(const Packet &p) = 0;

    /*
     * @return Get packet info
     */
    virtual QVariant getInfo(const Packet &p, int role = Qt::DisplayRole) = 0;

    /*
     * Process packet and add to list
     * @return number of packets added to list
     */
    virtual int preprocessPacket(const Packet &p, QList<Packet> &list) = 0;

    /*
     * Dissect and create tree
     */
    virtual void dissectPacket(const Packet &p, QTreeWidgetItem *parent) = 0;
};

#define DissectorPlugin_iid "org.etc.EtcDmxTool.Plugins.Dissector"

Q_DECLARE_INTERFACE(DissectorPlugin, DissectorPlugin_iid)

#endif // DISSECTORPLUGIN_H
