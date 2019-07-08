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
        GuiOnly,
        ConsoleOnly,
        ExitApp
    };
    parseResult_t getResult() { return parseResult; }

signals:

public slots:

private:
    void showConsole();
    #if defined(Q_OS_WIN)
        FILE * pStdout = Q_NULLPTR;
        FILE * pStderr = Q_NULLPTR;
    #endif

    parseResult_t parseResult = GuiOnly;
    QCommandLineParser parser;

    PacketTable packetTable;

    const QCommandLineOption optSniff = {{"s", "sniff"}, "Sniff directly into <file>.", "file"};
    const QCommandLineOption optCompress = {"compress", "Compress file output"};
    void doSniff();
};

#endif // COMMANDLINEPARSE_H
