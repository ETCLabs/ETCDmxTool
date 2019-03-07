#ifndef LOGMODEL_H
#define LOGMODEL_H


//Logging category bitflags to determine what class of object generated
//the log.  They can be OR'd together.
#define CDL_CAT_DBG 0x00000001
#define CDL_CAT_SEC 0x00000002
#define CDL_CAT_LIB 0x00000004
#define CDL_CAT_SYS 0x00000008
#define CDL_CAT_APP 0x00000010
#define CDL_CAT_UKN 0x00000020
#define CDL_CAT_ALL 0xffffffff

//Logging severity bitflags to determine the severity of the message.
//They can be OR'd together.
#define CDL_SEV_ERR 0x00000001
#define CDL_SEV_WRN 0x00000002
#define CDL_SEV_INF 0x00000004
#define CDL_SEV_UKN 0x00000008
#define CDL_SEV_ALL 0xffffffff

// Verbosity 1-5; 1 means least output
#define CDL_VERB_MIN 1
#define CDL_VERB_MAX 5


#include <QAbstractListModel>
#include "GadgetDLL.h"
#include <QMap>


static QMap<int, QString> CDL_CAT_STRINGS{
    {CDL_CAT_DBG, "Debug"},
    {CDL_CAT_SEC, "Security"},
    {CDL_CAT_LIB, "Library"},
    {CDL_CAT_SYS, "System"},
    {CDL_CAT_APP, "Application"},
    {CDL_CAT_UKN, "Unknown"},
    {CDL_CAT_ALL, "All"}
};


static QMap<int, QString> CDL_SEV_STRINGS{
    {CDL_SEV_ERR, "Error"},
    {CDL_SEV_WRN, "Warning"},
    {CDL_SEV_INF, "Information"},
    {CDL_SEV_UKN, "Unknown"},
    {CDL_SEV_ALL, "All"}
};


class LogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    static LogModel *getInstance();
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    static void log(const QString &message, quint32 severity, int verbosity);
    void logData(const char* data);
private:
    static LogModel *m_instance;
    explicit LogModel(QObject *parent = nullptr);
    void doLog(const QString &message, quint32 severity, int verbosity);
    QStringList m_logStrings;
    quint32 m_severity = CDL_SEV_ALL;
    int m_verbosity = 5;
};

void __stdcall GadgetLogCallback(const char* logData);


#endif // LOGMODEL_H
