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

#include "fileopen.h"

FileOpen::FileOpen(PacketTable &packetTable, QString fileName, QObject *parent) : QObject(parent),
    m_packetTable(&packetTable)
{
    // Check and open file
    m_file = new QFile(fileName);
    bool ok = m_file->open(QIODevice::ReadOnly);
    if(!ok)
        return;

    m_stream = new QTextStream(m_file);

    // Thread off reading
    m_thread = new QThread;
    m_thread->setObjectName(QString("File Opener"));
    this->moveToThread(m_thread);
    connect(m_thread, SIGNAL(started()), this, SLOT(doRead()));
    connect(this, SIGNAL(Finished()), this, SLOT(deleteLater()));
    m_thread->start();
}

FileOpen::~FileOpen()
{
    m_thread->exit();
    m_thread->deleteLater();
    m_file->close();
    delete m_stream;
    delete m_file;
}

void FileOpen::doRead()
{
    emit Started();

    QString line = m_stream->readLine();

    // Try to import alternative file formats
    if (loadGoddardDesigns(line))
    {
        emit Finished();
        return;
    }

    while(!line.isNull())
    {
        QTextStream lineStream(&line);
        QString chunk;
        Packet p;
        lineStream >> chunk;
        p.timestamp = chunk.toDouble();
        while(!lineStream.atEnd())
        {
            lineStream >> chunk;
            bool ok;
            unsigned char byte = chunk.toInt(&ok, 16);
            if(ok)
            {
                p.append(byte);
            }
        }

        if(p.size()>0)
        {
            m_packetTable->appendPacket(p);
        }
        line = m_stream->readLine();
    }

    qDebug("Done");
    emit Finished();
}

bool FileOpen::loadGoddardDesigns(const QString firstLine)
{
    if (!firstLine.startsWith("Timestamp")) return false;
    // Find data column
    int dataCol = -1;
    QStringList header = firstLine.split(',');
    for (int i=0; i<header.count(); ++i)
    {
        if (header.at(i).contains("Data")) dataCol = i;
    }
    if (dataCol == -1) return false;

    // Now read all Timestamp and Data
    QString line = m_stream->readLine();
    while(!line.isNull())
    {
        Packet p = QByteArray::fromHex(line.section(',', dataCol, dataCol).toLatin1());
        p.timestamp = line.section(',', 0, 0).toFloat() / 1000000;  // Microseconds

        if (p.size() > 0)
        {
            m_packetTable->appendPacket(p);
        }
        line = m_stream->readLine();
    }
    return true;
}
