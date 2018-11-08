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

#include <QtGlobal>
#include "util.h"
#include "rdm/rdmEtcConsts.h"
#include "rdm/estardm.h"
#include "rdm_appendix_b.h"
#include "rdm_dimmermsg.h"
#include "rdm/rdmpidstrings.h"

// Length in bytes
#define RDM_UID_LENGTH 6

enum RDM_MESSAGEBLOCK
{
	RDM_CC,
	RDM_PARAMETER_ID,
	RDM_PDL = 3,
	RDM_PARAMETER_DATA
};

enum RDM_MUTE_PARAMETER_DATA
{
	RDM_MUTE_CONTROL_FIELD = 0,
	RDM_MUTE_BINDING_UID   = 2
};

enum RDM_PACKET_STRUCT
{
	RDM_START_CODE = 0,
	RDM_SUB_START_CODE = 1,
	RDM_LENGTH     = 2,
	RDM_DEST_UID   = 3,
	RDM_SOURCE_UID = 9,
	RDM_TRANSACTION_NR = 15,
	RDM_PORT_ID       = 16,
	RDM_RESPONSE_TYPE = 16,
	RDM_MESSAGE_COUNT = 17,
	RDM_SUBDEVICE     = 18,
	RDM_MESSAGE_BLOCK = 20,

	// Packet overall sizes
	RDM_PACKET_MIN_BYTES = 26,
    RDM_PACKET_MAX_BYTES = 257
};

void dissectRdmDiscResponse(const Packet &p, QTreeWidgetItem *parent);

void dissectRdm(const Packet &p, QTreeWidgetItem *parent);

quint64 unpackRdmId(const Packet &p, int start);



