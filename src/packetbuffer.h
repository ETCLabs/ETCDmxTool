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

#pragma once

#include <QObject>
#include <exception>
#include <QColor>

struct PacketAccessException : public std::exception {
   const char * what () const throw () {
      return "Packet Invalid Access";
   }
};


class Packet : public QByteArray
{
public:
	Packet();
	Packet(const char *data, int size);
	Packet(const QByteArray &data);
    Packet(const Packet &other, int start = 0);

    qint64 timestamp;
    unsigned char operator[] ( int i ) const;

    class Invalid
    {
    public:
       static const QColor INVALID_PACKET_BACKGROUND;
       static const QColor INVALID_PARAMETER_BACKGROUND;
    };
};
