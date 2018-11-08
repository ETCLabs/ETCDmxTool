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

#include "rdm_dimmermsg.h"

#include "rdm/estardm.h"
#include "util.h"

#include <QTreeWidgetItem>

bool RDM_DimmerMsg::dissectMsg(const Packet &p, QTreeWidgetItem *parent, quint8 cc, quint16 paramId, int offset, int pdl)
{
    switch (paramId)
    {
    case E137_1_IDENTIFY_MODE :
        identifyMode(p, parent, cc, offset, pdl);
        break;
    case E137_1_DMX_BLOCK_ADDRESS :
        blockAddress(p, parent, cc, offset, pdl);
        break;
    case E137_1_DMX_FAIL_MODE : // Same as Startup
    case E137_1_DMX_STARTUP_MODE :
        dmxFailStartMode(p, parent, cc, offset, pdl);
        break;
    case E137_1_POWER_ON_SELF_TEST :
        powerOnTest(p, parent, cc, offset, pdl);
        break;
    case E137_1_LOCK_STATE :
        lockState(p, parent, cc, offset, pdl);
        break;
    case E137_1_LOCK_STATE_DESCRIPTION :
        lockStateDescription(p, parent, cc, offset, pdl);
        break;
    case E137_1_LOCK_PIN :
        lockPin(p, parent, cc, offset, pdl);
        break;
    case E137_1_BURN_IN :
        burnIn(p, parent, cc, offset, pdl);
        break;
    case E137_1_DIMMER_INFO :
        dimmerInfo(p, parent, cc, offset, pdl);
        break;
    case E137_1_MINIMUM_LEVEL :
        dimmerMinimum(p, parent, cc, offset, pdl);
        break;
    case E137_1_MAXIMUM_LEVEL :
        dimmerMaximum(p, parent, cc, offset, pdl);
        break;
    case E137_1_CURVE :
    case E137_1_OUTPUT_RESPONSE_TIME :
    case E137_1_MODULATION_FREQUENCY :
        optionList(p, parent, cc, offset, pdl);
        break;
    case E137_1_CURVE_DESCRIPTION :
        optionListDescription(p, parent, cc, offset, pdl, "Curve");
        break;
    case E137_1_OUTPUT_RESPONSE_TIME_DESCRIPTION :
        optionListDescription(p, parent, cc, offset, pdl, "Response Time");
        break;
    case E137_1_MODULATION_FREQUENCY_DESCRIPTION :
        modulationFrequencyDescription(p, parent, cc, offset, pdl);
        break;
    case E137_1_PRESET_INFO :
        presetInfo(p, parent, cc, offset, pdl);
        break;
    case E137_1_PRESET_STATUS :
        presetStatus(p, parent, cc, offset, pdl);
        break;
    case E137_1_PRESET_MERGEMODE :
        presetMergeMode(p, parent, cc, offset, pdl);
        break;
    default : // Not an E1.37-1 PID
        return false;
    }
    return true;
}

void RDM_DimmerMsg::identifyMode(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND_RESPONSE :
    case E120_SET_COMMAND :
        if (pdl != 1) Util::setItemInvalid(parent);
        else
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Identify Mode");
            switch (p[offset])
            {
            case 0x00 : item->setText(1, "Quiet");
                break;
            case 0xFF : item->setText(1, "Loud");
                break;
            default : item->setText(1, "Undefined");
                break;
            }
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::blockAddress(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND:
    case E120_SET_COMMAND_RESPONSE:
        if (pdl != 0)
        {
            Util::setItemInvalid(parent);
        }
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 4)
        {
            Util::setItemInvalid(parent);
        }
        else
        {
            quint16 footprint = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Total Footprint");
            item->setText(1, QString::number(footprint));
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);

            quint16 start = Util::unpackU16(p, offset+2);
            item = new QTreeWidgetItem();
            item->setText(0, "Start Addr");
            if (start == 0xFFFF) item->setText(1, "Split");
            else
            {
                item->setText(1, QString::number(start));
                // Cannot overflow universe
                if ((start + footprint) > 513) Util::setItemInvalid(parent);
            }
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);
        }
        return;
    case E120_SET_COMMAND :
        if (pdl != 2) Util::setItemInvalid(parent);
        else
        {
            quint16 start = Util::unpackU16(p, offset+2);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Block Start Addr");
            item->setText(1, QString::number(start));
            // Cannot overflow universe
            if ((start > 512) || (start == 0)) Util::setItemInvalid(parent);
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::dmxFailStartMode(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_SET_COMMAND :
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x07) Util::setItemInvalid(parent);
        else
        {
            quint16 scene = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Scene #");
            switch (scene)
            {
            case E120_PRESET_PLAYBACK_OFF :
                item->setText(1, "To Level");
                break;
            default:
                item->setText(1, QString::number(scene));
            }
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);

            quint16 delay = Util::unpackU16(p, offset+2);
            item = new QTreeWidgetItem();
            item->setText(0, "Hold Last Look");
            if (delay == 0xFFFF) item->setText(1, "Forever");
            else item->setText(1, Util::strTimeFromTenths(delay));
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);

            quint16 hold = Util::unpackU16(p, offset+4);
            item = new QTreeWidgetItem();
            item->setText(0, "Scene Hold");
            if (hold == 0xFFFF) item->setText(1, "Forever");
            else item->setText(1, Util::strTimeFromTenths(hold));
            Util::setPacketByteHighlight(item, offset+4, 2);
            parent->addChild(item);

            quint8 level = p[offset+6];
            item = new QTreeWidgetItem();
            item->setText(0, "Level");
            item->setText(1, QString("%1 (%2%)")
                          .arg(level)
                          .arg((double(level) / 2.55), 0, 'f', 1));
            Util::setPacketByteHighlight(item, offset+6, 1);
            parent->addChild(item);
        }
    }
}

void RDM_DimmerMsg::powerOnTest(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND_RESPONSE :
    case E120_SET_COMMAND :
        if (pdl != 1) Util::setItemInvalid(parent);
        else
        {
            Util::dissectGenericData(p, parent, offset, pdl, Util::GenericDataBool);
        }
    }
}

void RDM_DimmerMsg::lockState(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x02) Util::setItemInvalid(parent);
        else
        {
            quint8 lockState = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Current Lock state");
            if (lockState == 0x00) item->setText(1, "Unlocked");
            else item->setText(1, QString::number(lockState));
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);

            quint8 lockCount = p[offset+1];
            item = new QTreeWidgetItem();
            item->setText(0, "Lock State Count");
            item->setText(1, QString::number(lockCount));
            Util::setPacketByteHighlight(item, offset+1, 1);
            parent->addChild(item);
        }
        return;
    case E120_SET_COMMAND :
        if (pdl != 0x03) Util::setItemInvalid(parent);
        else
        {
            quint16 pin = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Pin Code");
            item->setText(1, QString::number(pin));
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);

            quint8 lockState = p[offset+2];
            item = new QTreeWidgetItem();
            item->setText(0, "Desired Lock state");
            if (lockState == 0x00) item->setText(1, "Unlocked");
            else item->setText(1, QString::number(lockState));
            Util::setPacketByteHighlight(item, offset+2, 1);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::lockStateDescription(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_SET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND :
        if (pdl != 1) Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl == 0) Util::setItemInvalid(parent);
        else
        {
            quint8 lockState = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Lock State Requested");
            if (lockState == 0x00)
            {
                item->setText(1, "Unlocked");
                Util::setItemInvalid(parent);
            }
            else item->setText(1, QString::number(lockState));
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);

            Util::dissectGenericData(p, parent, offset+1, pdl, Util::GenericDataText);
        }
        return;
    }
}

void RDM_DimmerMsg::lockPin(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x02) Util::setItemInvalid(parent);
        else
        {
            quint16 pin = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Current Pin Code");
            item->setText(1, QString::number(pin));
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);
        }
        return;
    case E120_SET_COMMAND :
        if (pdl != 0x04) Util::setItemInvalid(parent);
        else
        {
            quint16 newPin = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "New Pin Code");
            item->setText(1, QString::number(newPin));
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);

            quint16 oldPin = Util::unpackU16(p, offset+2);
            item = new QTreeWidgetItem();
            item->setText(0, "Old Pin Code");
            item->setText(1, QString::number(oldPin));
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::burnIn(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 1) Util::setItemInvalid(parent);
        else {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Hours remaining");
            item->setText(1, QString::number(p[offset]));
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);
        }
        return;
    case E120_SET_COMMAND :
        if (pdl != 1) Util::setItemInvalid(parent);
        else {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Hours To Burn");
            item->setText(1, QString::number(p[offset]));
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::dimmerInfo(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_SET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;

    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x08) Util::setItemInvalid(parent);
        else {
            quint16 level = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Min. Level Lower Limit");
            item->setText(1, QString::number(level));
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);

            level = Util::unpackU16(p, offset+2);
            item = new QTreeWidgetItem();
            item->setText(0, "Min. Level Upper Limit");
            item->setText(1, QString::number(level));
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);

            level = Util::unpackU16(p, offset+4);
            item = new QTreeWidgetItem();
            item->setText(0, "Max. Level Lower Limit");
            item->setText(1, QString::number(level));
            Util::setPacketByteHighlight(item, offset+4, 2);
            parent->addChild(item);

            level = Util::unpackU16(p, offset+6);
            item = new QTreeWidgetItem();
            item->setText(0, "Max. Level Upper Limit");
            item->setText(1, QString::number(level));
            Util::setPacketByteHighlight(item, offset+6, 2);
            parent->addChild(item);

            quint8 curves = p[offset+8];
            item = new QTreeWidgetItem();
            item->setText(0, "Curve Count");
            item->setText(1, QString::number(curves));
            Util::setPacketByteHighlight(item, offset+8, 1);
            parent->addChild(item);

            quint8 resolution = p[offset+9];
            item = new QTreeWidgetItem();
            item->setText(0, "Resolution");
            item->setText(1, QString::number(resolution) + " bits");
            Util::setPacketByteHighlight(item, offset+9, 1);
            parent->addChild(item);

            quint8 splitLevel = p[offset+10];
            item = new QTreeWidgetItem();
            item->setText(0, "Split Levels");
            switch (splitLevel)
            {
            case 0x00 : item->setText(1, "Not Supported");
                break;
            case 0x01 : item->setText(1, "Supported");
                break;
            default: item->setText(1, "Invalid");
                Util::setItemInvalid(item);
                break;
            }
            Util::setPacketByteHighlight(item, offset+10, 1);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::dimmerMinimum(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_SET_COMMAND :
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x05) Util::setItemInvalid(parent);
        else
        {
            quint16 level = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Min. Level Increasing");
            item->setText(1, QString::number(level));
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);

            level = Util::unpackU16(p, offset+2);
            item = new QTreeWidgetItem();
            item->setText(0, "Min. Level Decreasing");
            item->setText(1, QString::number(level));
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);

            quint8 onMin = p[offset+4];
            item = new QTreeWidgetItem();
            item->setText(0, "On Below Minimum");
            switch (onMin)
            {
            case 0x00 : item->setText(1, "Off");
                break;
            case 0x01 : item->setText(1, "On");
                break;
            default: item->setText(1, "Invalid");
            }
            Util::setPacketByteHighlight(item, offset+4, 1);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::dimmerMaximum(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_SET_COMMAND :
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x02) Util::setItemInvalid(parent);
        else
        {
            quint16 level = Util::unpackU16(p, offset);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Max. Level");
            item->setText(1, QString::number(level));
            Util::setPacketByteHighlight(item, offset, 2);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::optionList(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_SET_COMMAND :
        if (pdl != 0x01) Util::setItemInvalid(parent);
        else
        {
            quint8 curve = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Option #");
            item->setText(1, QString::number(curve));
            Util::setPacketByteHighlight(item, offset, 1);
            if (curve == 0) Util::setItemInvalid(item);
            parent->addChild(item);
        }
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x02) Util::setItemInvalid(parent);
        else
        {
            quint8 curve = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Option #");
            item->setText(1, QString::number(curve));
            Util::setPacketByteHighlight(item, offset, 1);
            if (curve == 0) Util::setItemInvalid(item);
            parent->addChild(item);

            quint8 curveCount = p[offset+1];
            item = new QTreeWidgetItem();
            item->setText(0, "Option Count");
            item->setText(1, QString::number(curveCount));
            Util::setPacketByteHighlight(item, offset+1, 1);
            if (curveCount == 0) Util::setItemInvalid(item);
            parent->addChild(item);

        }
        return;
    }
}

void RDM_DimmerMsg::optionListDescription(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl, const QString &optionLabel)
{
    switch (cc)
    {
    case E120_SET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND :
        if (pdl != 1) Util::setItemInvalid(parent);
        else
        {
            quint8 option = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, optionLabel);
            item->setText(1, QString::number(option));
            Util::setPacketByteHighlight(item, offset, 1);
            if (option == 0) Util::setItemInvalid(item);
            parent->addChild(item);
        }
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl == 0) Util::setItemInvalid(parent);
        else
        {
            quint8 option = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, optionLabel);
            item->setText(1, QString::number(option));
            Util::setPacketByteHighlight(item, offset, 1);
            if (option == 0) Util::setItemInvalid(item);
            parent->addChild(item);

            Util::dissectGenericData(p, parent, offset+1, pdl-1, Util::GenericDataText);
        }
        return;
    }
}

void RDM_DimmerMsg::modulationFrequencyDescription(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_SET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND :
        if (pdl != 1) Util::setItemInvalid(parent);
        else
        {
            quint8 freqOption = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Option #");
            item->setText(1, QString::number(freqOption));
            if (freqOption == 0) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);
        }
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        else
        {
            quint8 freqOption = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Option #");
            item->setText(1, QString::number(freqOption));
            if (freqOption == 0) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);

            quint32 freq = Util::unpackU32(p, offset+1);
            item = new QTreeWidgetItem();
            item->setText(0, "Frequency");
            if (freq == 0xFFFFFFFF) item->setText(1, "Unknown");
            else item->setText(1, QString::number(freq) + "Hz");
            Util::setPacketByteHighlight(item, offset+1, 4);
            parent->addChild(item);

            Util::dissectGenericData(p, parent, offset+5, pdl, Util::GenericDataText);
        }
        return;
    }
}

void RDM_DimmerMsg::presetInfo(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_SET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x20) Util::setItemInvalid(parent);
        else
        {
            quint8 levelSupported = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Level Supported");
            item->setText(1, QString::number(levelSupported));
            if (levelSupported > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);

            quint8 sequenceSupported = p[offset+1];
            item = new QTreeWidgetItem();
            item->setText(0, "Sequence Supported");
            item->setText(1, QString::number(sequenceSupported));
            if (sequenceSupported > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset+1, 1);
            parent->addChild(item);

            quint8 splitSupported = p[offset+2];
            item = new QTreeWidgetItem();
            item->setText(0, "Split Times Supported");
            item->setText(1, QString::number(splitSupported));
            if (splitSupported > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset+2, 1);
            parent->addChild(item);

            quint8 infDelaySupported = p[offset+3];
            item = new QTreeWidgetItem();
            item->setText(0, "DMX HLLF Supported");
            item->setText(1, QString::number(infDelaySupported));
            if (infDelaySupported > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset+3, 1);
            parent->addChild(item);

            quint8 infHoldSupported = p[offset+4];
            item = new QTreeWidgetItem();
            item->setText(0, "Preset Hold Forever Supported");
            item->setText(1, QString::number(infHoldSupported));
            if (infHoldSupported > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset+4, 1);
            parent->addChild(item);

            quint8 startupInfSupported = p[offset+5];
            item = new QTreeWidgetItem();
            item->setText(0, "Startup Hold Forever Supported");
            item->setText(1, QString::number(startupInfSupported));
            if (startupInfSupported > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset+5, 1);
            parent->addChild(item);

            quint16 sceneCount = Util::unpackU16(p, offset+6);
            item = new QTreeWidgetItem();
            item->setText(0, "Scene Count");
            item->setText(1, QString::number(sceneCount));
            if (startupInfSupported > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset+6, 2);
            parent->addChild(item);

            quint16 time = Util::unpackU16(p, offset+8);
            item = new QTreeWidgetItem();
            item->setText(0, "Min Preset Fade Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+8, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+10);
            item = new QTreeWidgetItem();
            item->setText(0, "Max Preset Fade Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+10, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+12);
            item = new QTreeWidgetItem();
            item->setText(0, "Min Preset Wait Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+12, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+14);
            item = new QTreeWidgetItem();
            item->setText(0, "Max Preset Wait Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+14, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+16);
            item = new QTreeWidgetItem();
            item->setText(0, "Min DMX Fail Delay Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+16, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+18);
            item = new QTreeWidgetItem();
            item->setText(0, "Max DMX Fail Delay Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+18, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+20);
            item = new QTreeWidgetItem();
            item->setText(0, "Min DMX Fail Hold Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+20, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+22);
            item = new QTreeWidgetItem();
            item->setText(0, "Max DMX Fail Hold Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+22, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+24);
            item = new QTreeWidgetItem();
            item->setText(0, "Min Startup Delay Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+24, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+26);
            item = new QTreeWidgetItem();
            item->setText(0, "Max Startup Delay Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+26, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+28);
            item = new QTreeWidgetItem();
            item->setText(0, "Min Startup Hold Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+28, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+30);
            item = new QTreeWidgetItem();
            item->setText(0, "Max Startup Hold Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+30, 2);
            parent->addChild(item);
        }
        return;
    }
}

void RDM_DimmerMsg::presetStatus(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_GET_COMMAND :
        if (pdl != 1) Util::setItemInvalid(parent);
        else
        {
            quint8 option = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Scene #");
            item->setText(1, QString::number(option));
            if (option == 0) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);
        }
        return;

    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 0x09) Util::setItemInvalid(parent);
        else
        {
            quint8 option = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Scene #");
            item->setText(1, QString::number(option));
            if (option == 0) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);

            quint16 time = Util::unpackU16(p, offset+2);
            item = new QTreeWidgetItem();
            item->setText(0, "Upfade Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+4);
            item = new QTreeWidgetItem();
            item->setText(0, "Downfade Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+4, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+6);
            item = new QTreeWidgetItem();
            item->setText(0, "Wait Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+6, 2);
            parent->addChild(item);

            quint8 programmed = p[offset+8];
            item = new QTreeWidgetItem();
            item->setText(0, "Programmed");
            switch(programmed)
            {
            case E137_1_PRESET_NOT_PROGRAMMED :
                item->setText(1, "Blank");
                break;
            case E137_1_PRESET_PROGRAMMED :
                item->setText(1, "Programmed");
                break;
            case E137_1_PRESET_PROGRAMMED_READ_ONLY :
                item->setText(1, "Read Only");
                break;
            default :
                item->setText(1, QString("Invalid: %1").arg(programmed));
                Util::setItemInvalid(item);
            }
            Util::setPacketByteHighlight(item, offset+8, 1);
            parent->addChild(item);
        }
        return;
    case E120_SET_COMMAND :
        if (pdl != 0x09) Util::setItemInvalid(parent);
        else
        {
            quint8 option = p[offset];
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, "Scene #");
            item->setText(1, QString::number(option));
            if (option == 0) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset, 1);
            parent->addChild(item);

            quint16 time = Util::unpackU16(p, offset+2);
            item = new QTreeWidgetItem();
            item->setText(0, "Upfade Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+2, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+4);
            item = new QTreeWidgetItem();
            item->setText(0, "Downfade Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+4, 2);
            parent->addChild(item);

            time = Util::unpackU16(p, offset+6);
            item = new QTreeWidgetItem();
            item->setText(0, "Wait Time");
            item->setText(1, Util::strTimeFromTenths(time));
            Util::setPacketByteHighlight(item, offset+6, 2);
            parent->addChild(item);

            quint8 programmed = p[offset+8];
            item = new QTreeWidgetItem();
            item->setText(0, "Erase");
            item->setText(1, QString::number(programmed));
            if (programmed > 1) Util::setItemInvalid(item);
            Util::setPacketByteHighlight(item, offset+8, 1);
            parent->addChild(item);
        }
        return;
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    }
}

void RDM_DimmerMsg::presetMergeMode(const Packet &p, QTreeWidgetItem *parent, quint8 cc, int offset, int pdl)
{
    switch (cc)
    {
    case E120_SET_COMMAND :
    case E120_GET_COMMAND_RESPONSE :
        if (pdl != 1) Util::setItemInvalid(parent);
        else
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            Util::setPacketByteHighlight(item, offset, 1);
            item->setText(0, "Merge Mode");
            switch(p[offset])
            {
            case E137_1_MERGEMODE_DEFAULT :
                item->setText(1, "Default");
                break;
            case E137_1_MERGEMODE_HTP :
                item->setText(1, "HTP");
                break;
            case E137_1_MERGEMODE_LTP :
                item->setText(1, "LTP");
                break;
            case E137_1_MERGEMODE_DMX_ONLY :
                item->setText(1, "Use DMX Only");
                break;
            case E137_1_MERGEMODE_OTHER :
                item->setText(1, "Other");
                break;
            default :
                item->setText(1, "Unknown " + QString::number(p[offset]));
                Util::setItemInvalid(item);
                break;
            }
        }
        return;
    case E120_GET_COMMAND :
    case E120_SET_COMMAND_RESPONSE :
        if (pdl != 0) Util::setItemInvalid(parent);
        return;
    }
}
