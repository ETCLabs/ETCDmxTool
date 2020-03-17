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

#include "file.h"
#include "logmodel.h"
#include <QByteArray>

namespace CompressedFile {
    static int DataStreamVersion = QDataStream::Qt_5_10;
    static QString MagicHeader = "ETCDMXTool";
    typedef enum : quint16 {
        Version_0
    } versions_t;
    QDataStream& operator >>(QDataStream& ds, versions_t& e)
    {
        ds >> reinterpret_cast<quint16&>(e);
        return ds;
    }
    QDataStream& operator <<(QDataStream& ds, const versions_t& e)
    {
        ds << reinterpret_cast<const quint16&>(e);
        return ds;
    }
}

/*
 * File Open
 */

FileOpen::FileOpen(PacketTable &packetTable, QString fileName, QObject *parent) : QObject(parent),
    m_packetTable(&packetTable),
    m_file(new QFile(fileName, this))
{
    // Check and open file
    LogModel::log(QString("Opening File %1").arg(m_file->fileName()), CDL_SEV_INF, 1);
    if (!m_file->open(QIODevice::ReadOnly))
    {
        LogModel::log(QString("Unable to open file %1 for read").arg(m_file->fileName()), CDL_SEV_ERR, 1);
        return;
    }
}

FileOpen::~FileOpen()
{
    m_file->close();
}

void FileOpen::doRead()
{
    emit Started();

    // Detect file format and load
    if (loadGoddardDesigns(m_file))
    {
        // Noop
    } else if (loadCompressed(m_file)) {
        // Noop
    } else {
        loadOriginal(m_file);
    }

    emit Finished();
    return;
}

void FileOpen::loadOriginal(QFile* file)
{
    file->seek(0);
    QTextStream fileStream(file);
    QString line = fileStream.readLine();

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

        line = fileStream.readLine();
    }
    LogModel::log(
                QString("Finished loading file, %1 packets").arg(m_packetTable->rowCount()),
                CDL_SEV_INF,
                1);
}

bool FileOpen::loadCompressed(QFile* file)
{
    file->seek(0);
    QDataStream fileStream(file);
    fileStream.setVersion(CompressedFile::DataStreamVersion);

    QString magic;
    CompressedFile::versions_t version;
    fileStream >> magic >> version;
    if (fileStream.status() != QDataStream::Ok)
        return false;
    if (magic.compare(CompressedFile::MagicHeader) !=0)
        return false;

    while (fileStream.status() == QDataStream::Ok)
    {
        switch (version)
        {
            case CompressedFile::Version_0:
            {
                Packet p;
                QByteArray compressedArr;
                fileStream >> p.timestamp >> p.isRdmCollision >> compressedArr;
                p.append(qUncompress(compressedArr));
                if (fileStream.status() == QDataStream::Ok)
                    m_packetTable->appendPacket(p);
            } break;

            default:
            {
                LogModel::log(
                    QString("Unknown file format version"),
                    CDL_SEV_WRN,
                    1);

                return false;
            }
        }
    }

    LogModel::log(
                QString("Finished loading compressed file, %1 packets").arg(m_packetTable->rowCount()),
                CDL_SEV_INF,
                1);

    return true;
}

bool FileOpen::loadGoddardDesigns(QFile* file)
{
    file->seek(0);
    QTextStream fileStream(file);

    QString headerLine = fileStream.readLine();

    if (!headerLine.startsWith("Timestamp", Qt::CaseInsensitive)) return false;

    // Find data column
    int dataCol = -1;
    QStringList header = headerLine.split(',');
    for (int i=0; i<header.count(); ++i)
    {
        if (header.at(i).contains("Data", Qt::CaseInsensitive))
        {
            dataCol = i;
            break;
        }
    }
    if (dataCol == -1) return false;

    // Now read all Timestamp and Data
    QString line = fileStream.readLine();
    while(!line.isNull())
    {
        Packet p = QByteArray::fromHex(line.section(',', dataCol, dataCol).toLatin1());

        if (p.size() > 0)
        {
            p.timestamp = static_cast<qint64>(line.section(',', 0, 0).toFloat() / 1000000.f);  // Microseconds
            m_packetTable->appendPacket(p);
        }
        line = fileStream.readLine();
    }
    return true;
}


/*
 * File Save
 */

FileSave::FileSave(PacketTable &packetTable, QString fileName, format_t fileFormat, operatingMode_t mode, QObject *parent) : QObject(parent),
    m_packetTable(&packetTable),
    m_file(new QFile(fileName,this)),
    m_mode(mode),
    m_format(fileFormat)
{
    // Open file handle
    LogModel::log(QString("Saving File %1").arg(m_file->fileName()), CDL_SEV_INF, 1);
    if (!m_file->open(QIODevice::WriteOnly))
    {
        LogModel::log(QString("Unable to open file %1 for write").arg(m_file->fileName()), CDL_SEV_ERR, 1);
        return;
    }

    // Compressed file header?
    if (m_format == compressed)
    {
        QDataStream fileStream(m_file);
        fileStream.setVersion(CompressedFile::DataStreamVersion);
        fileStream << CompressedFile::MagicHeader << CompressedFile::Version_0;
    }
}

FileSave::~FileSave()
{
    m_file->close();
}

void FileSave::writePacket(QTextStream &fileStream, const Packet &packet)
{
    QString line = QString("%1 ").arg(packet.timestamp);
    for(int i=0; i<packet.length(); i++)
    {
        line.append(QString("%1").arg(static_cast<unsigned char>(packet.at(i)), 2, 16, QChar('0')));
        if (i < packet.length()) line.append(QString(" "));
    }
    line.append(QString("\r\n"));
    fileStream << line;
}


void FileSave::writePacket(QDataStream &fileStream, const Packet &packet)
{
    fileStream << packet.timestamp << packet.isRdmCollision << qCompress(packet);
}

void FileSave::writeTable()
{
    switch (m_format) {
        case original:
        {
            QTextStream textStream(m_file);

            switch (m_mode)
            {
                case CopyAndSave:
                {
                    for(int i=0; i<m_packetTable->rowCount(); ++i)
                    {
                        auto packet = m_packetTable->getPacket(i);
                        writePacket(textStream, packet);
                    }
                } break;

                case StreamToFile:
                {
                    while (m_packetTable->rowCount())
                    {
                        auto packet = m_packetTable->takePacket(0);
                        writePacket(textStream, packet);
                    }
                }

            }
            textStream.flush();
        } break;

        case compressed:
        {
            QDataStream dataStream(m_file);
            dataStream.setVersion(CompressedFile::DataStreamVersion);

            switch (m_mode)
            {
                case CopyAndSave:
                {
                    for(int i=0; i<m_packetTable->rowCount(); ++i)
                    {
                        auto packet = m_packetTable->getPacket(i);
                        writePacket(dataStream, packet);
                    }
                } break;

                case StreamToFile:
                {
                    while (m_packetTable->rowCount())
                    {
                        auto packet = m_packetTable->takePacket(0);
                        writePacket(dataStream, packet);
                    }
                } break;
            }

            m_file->flush();

        } break;
    }
}

void FileSave::doSave()
{
    emit Started();

    switch (m_mode)
    {
        case CopyAndSave:
        {
            writeTable();
            LogModel::log(
                QString("Finished saving file, %1 packets").arg(m_packetTable->rowCount()),
                CDL_SEV_INF,
                1);

            emit Finished();
        } break;

        case StreamToFile:
        {
            writeTable();
            connect(m_packetTable, &PacketTable::newPacket, [=]() { writeTable(); });
        } break;
    }
}
