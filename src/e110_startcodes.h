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

#ifndef E120_STARTCODES_H
#define E120_STARTCODES_H

namespace E110_SC {
    enum  {
        DIMMER_DATA = 0x00,
        ASCII_TEXT = 0x17,
        RDM = 0xCC,
        SIP = 0xCF,
        RDM_DSC_RESPONSE = 0xFE,

        // Electronic Theatre Controls
        NONOFFICIAL_ETC_FIXLINK = 0x45,

        // Goldern Sea/Highend Systems
        NONOFFICIAL_HIGHEND_NOOP = 0xA8,
        NONOFFICIAL_HIGHEND_UPDATE_HEADER = 0xA9,
        NONOFFICIAL_HIGHEND_UPDATE_DATA = 0xAA
    };
}

#endif // E120_STARTCODES_H


