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

#ifndef FILEOPEN_H
#define FILEOPEN_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include "packettable.h"

class FileOpen : public QObject
{
    Q_OBJECT
public:
    explicit FileOpen(PacketTable &packetTable, QString fileName, QObject *parent = nullptr);
    ~FileOpen();

signals:
    void Started();
    void Finished();

public slots:

private slots:
    void doRead();
private:
    bool loadGoddardDesigns(const QString firstLine);

    QThread *m_thread;
    PacketTable *m_packetTable;
    QFile *m_file;
    QTextStream *m_stream;
};

#endif // FILEOPEN_H
