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

#include "rdm_appendix_b.h"

QString RDM_2010::statusMessageToDescription(quint16 statusMessageId, quint16 data1, quint16 data2)
{
    switch(statusMessageId)
    {
    case STS_CAL_FAIL       :
        return QString("%1 failed calibration").arg(data1);
    case STS_SENS_NOT_FOUND :
        return QString("%1 sensor not found").arg(data1);
    case STS_SENS_ALWAYS_ON :
        return QString("%1 sensor always on").arg(data1);
    case STS_LAMP_DOUSED    :
        return QString("Lamp doused");
    case STS_LAMP_STRIKE    :
        return QString("Lamp failed to strike");
    case STS_OVERTEMP       :
        return QString("Sensor %1 over temp at %2 degrees C")
                .arg(data1).arg(data2);
    case STS_UNDERTEMP      :
        return QString("Sensor %1 under temp at %2 degrees C")
                .arg(data1).arg(data2);
    case STS_SENS_OUT_RANGE  :
        return QString("Sensor %1 out of range").arg(data1);
    case STS_OVERVOLTAGE_PHASE :
        return QString("Phase %1 over voltage at %2 V")
                .arg(data1)
                .arg(data2);
    case STS_UNDERVOLTAGE_PHASE :
        return QString("Phase %1 under voltage at %2 V")
                .arg(data1)
                .arg(data2);
    case STS_OVERCURRENT     :
        return QString("Phase %1 over current at %2 A")
                .arg(data1)
                .arg(data2);
    case STS_UNDERCURRENT    :
        return QString("Phase %1 under current at %2 A")
                .arg(data1)
                .arg(data2);
    case STS_PHASE           :
        return QString("Phase %1 is at %2 degrees")
                .arg(data1)
                .arg(data2);
    case STS_PHASE_ERROR     :
        return QString("Phase %1 error").arg(data1);
    case STS_AMPS            :
        return QString("%1 Amps").arg(data1);
    case STS_VOLTS           :
        return QString("%1 Volts").arg(data1);
    case STS_DIMSLOT_OCCUPIED :
        return QString("No Dimmer");
    case STS_BREAKER_TRIP    :
        return QString("Tripped Breaker");
    case STS_WATTS           :
        return QString("%1 Watts").arg(data1);
    case STS_DIM_FAILURE     :
        return QString("Dimmer Failure");
    case STS_DIM_PANIC       :
        return QString("Panic Mode");
    case STS_READY           :
        return QString("%1 ready").arg(data1);
    case STS_NOT_READY       :
        return QString("%1 not ready").arg(data1);
    case STS_LOW_FLUID       :
        return QString("%1 low fluid").arg(data1);
    default:
        if (statusMessageId > STS_MAX_STD)
            return "Manufacturer-Specific";
        else return "Unknown Standard";
    }
}
