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

#ifndef RDM_APPENDIX_B_H
#define RDM_APPENDIX_B_H

#include <QString>

namespace RDM_2010
{
    enum StatusMessageId
    {
        STS_CAL_FAIL        = 0x0001,
        STS_SENS_NOT_FOUND  = 0x0002,
        STS_SENS_ALWAYS_ON  = 0x0003,
        STS_LAMP_DOUSED     = 0x0011,
        STS_LAMP_STRIKE     = 0x0012,
        STS_OVERTEMP        = 0x0021,
        STS_UNDERTEMP       = 0x0022,
        STS_SENS_OUT_RANGE  = 0x0023,
        STS_OVERVOLTAGE_PHASE = 0x0031,
        STS_UNDERVOLTAGE_PHASE = 0x0032,
        STS_OVERCURRENT     = 0x0033,
        STS_UNDERCURRENT    = 0x0034,
        STS_PHASE           = 0x0035,
        STS_PHASE_ERROR     = 0x0036,
        STS_AMPS            = 0x0037,
        STS_VOLTS           = 0x0038,
        STS_DIMSLOT_OCCUPIED = 0x0041,
        STS_BREAKER_TRIP    = 0x0042,
        STS_WATTS           = 0x0043,
        STS_DIM_FAILURE     = 0x0044,
        STS_DIM_PANIC       = 0x0045,
        STS_READY           = 0x0050,
        STS_NOT_READY       = 0x0051,
        STS_LOW_FLUID       = 0x0052,

        STS_MAX_STD         = 0x7FFF
    };

    QString statusMessageToDescription(quint16 statusMessageId, quint16 data1, quint16 data2);

}

#endif // RDM_APPENDIX_B_H
