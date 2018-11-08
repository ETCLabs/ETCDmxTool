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

#include <packetbuffer.h>

Packet::Packet()
    : QByteArray()
    , timestamp(0)
    , isRdmCollision(false)
{
}

Packet::Packet(const char* data, int size)
    : QByteArray(data, size)
    , timestamp(0)
    , isRdmCollision(false)
{
}

Packet::Packet(const QByteArray &data)
    : QByteArray(data)
    , timestamp(0)
    , isRdmCollision(false)
{
}

Packet::Packet(const Packet &other, int start)
    : QByteArray()
    , timestamp(other.timestamp)
    , isRdmCollision(other.isRdmCollision)
{
    append(other.mid(start));
}

unsigned char Packet::operator[](int i) const
{
    if(i<this->count())
        return (unsigned char)this->at(i);

    return 0;
    // TODO : better handling of invalid access/packets with exceptions
    //throw PacketAccessException();
}
