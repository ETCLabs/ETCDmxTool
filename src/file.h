#ifndef FILE_H
#define FILE_H

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
    void loadOriginal(QFile* file);
    bool loadCompressed(QFile* file);
    bool loadGoddardDesigns(QFile* file);

    QThread *m_thread;
    PacketTable *m_packetTable;
    QFile *m_file;
};

class FileSave : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        original,
        compressed
    } format_t;

    typedef enum {
        CopyAndSave,
        StreamToFile
    } operatingMode_t;

    explicit FileSave(PacketTable &packetTable, QString fileName, format_t fileFormat = original, operatingMode_t mode = CopyAndSave, QObject *parent = nullptr);
    ~FileSave();

signals:
    void Started();
    void Finished();

public slots:

private slots:
    void doSave(); // Thread
private:
    void writeTable();
    void writePacket(QTextStream &fileStream, const Packet &packet);
    void writePacket(QDataStream &fileStream, const Packet &packet);
    QThread *m_thread;
    PacketTable *m_packetTable;
    QFile *m_file;
    QTextStream *m_stream;
    operatingMode_t m_mode;
    format_t m_format;
};

#endif // FILESAVE_H
