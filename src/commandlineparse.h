#ifndef COMMANDLINEPARSE_H
#define COMMANDLINEPARSE_H

#include <QObject>
#include <QCommandLineParser>
#include <QFile>
#include "stdout.h"
#include "packettable.h"

class commandLineParse : public QObject
{
    Q_OBJECT
public:
    explicit commandLineParse(const QCoreApplication &app, QObject *parent = nullptr);
    ~commandLineParse();
    enum parseResult_t {
        ShowGui,
        HideGui,
        ExitApp
    };
    parseResult_t getResult() { return parseResult; }

signals:

public slots:

private:
    parseResult_t parseResult = ShowGui;
    QCommandLineParser parser;

    PacketTable packetTable;

    const QCommandLineOption optSniff = {{"s", "sniff"}, "Sniff directly into <file>.", "file"};
    const QCommandLineOption optCompress = {"compress", "Compress file output"};
    void doSniff();
};

#endif // COMMANDLINEPARSE_H
