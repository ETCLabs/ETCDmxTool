#include "logmodel.h"
#include "stdout.h"

#include <QDateTime>
#include <QSettings>
#include <QTextStream>
#include <QThread>

LogModel *LogModel::getInstance()
{
    static LogModel instance;
    return &instance;
}

LogModel::LogModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Gadget2_SetLogCallback(GadgetLogCallback);
    QSettings settings(this);
    m_verbosity = settings.value("LogVerbosity", QVariant(1)).toInt();
    m_category = settings.value("LogCategory", QVariant(CDL_CAT_ALL)).toInt();
    m_severity = settings.value("LogSeverity", QVariant(CDL_SEV_ALL)).toInt();
}

LogModel::~LogModel()
{
    QSettings settings(this);
    settings.setValue("LogVerbosity", QVariant(m_verbosity));
    settings.setValue("LogCategory", QVariant(m_category));
    settings.setValue("LogSeverity", QVariant(m_severity));
    settings.sync();
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_logStrings.count();
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==Qt::DisplayRole)
        return QVariant(m_logStrings.at(index.row()));

    return QVariant();
}

void LogModel::doLog(const QString &message, quint32 severity)
{
    if(this->thread()==QThread::currentThread())
    {
        emit beginInsertRows(QModelIndex(), m_logStrings.length(), m_logStrings.length());
        QString data = QDateTime::currentDateTime().toString(Qt::ISODate) + QString("\t") + message;
        if (severity == CDL_SEV_ERR) {
            qStdErr() << data << endl;
        } else {
            qStdOut() << data << endl;
        }
        m_logStrings << data;
        emit endInsertRows();
    }
    else {
        QMetaObject::invokeMethod(this,
                                  "doLog",
                                  Q_ARG(QString, message)
                                 );
    }
}

void LogModel::doLog(const QString &message, quint32 severity, int verbosity)
{
    if(severity>=m_severity && verbosity>=m_verbosity)
        return;

    if(this->thread()==QThread::currentThread())
    {
        if(severity<=m_severity && verbosity<=m_verbosity)
        {
            doLog(message, severity);
        }
    }
    else {
        QMetaObject::invokeMethod(this,
                                  "doLog",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, message),
                                  Q_ARG(quint32, severity),
                                  Q_ARG(int, verbosity)
                                  );
    }
}

void __stdcall GadgetLogCallback(const char* logData)
{
    LogModel::getInstance()->log(QString(logData));
}

QString LogModel::severityToString(int severity)
{
    QString result;
    if(severity & CDL_SEV_ERR)
        result.append(tr("Error"));

    if(severity & CDL_SEV_WRN)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("Warning"));
    }

    if(severity & CDL_SEV_INF)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("Information"));
    }

    if(severity & CDL_SEV_UKN)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("Unknown"));
    }

    return result;
}

QString LogModel::categoryToString(int category)
{
    QString result;
    if(category & CDL_CAT_DBG)
        result.append(tr("Debug"));

    if(category & CDL_CAT_SEC)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("Security"));
    }

    if(category & CDL_CAT_LIB)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("Library"));
    }

    if(category & CDL_CAT_SYS)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("System"));
    }

    if(category & CDL_CAT_APP)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("Application"));
    }

    if(category & CDL_CAT_UKN)
    {
        if(!result.isEmpty()) result.append(" ¦ ");
        result.append(tr("Unknown"));
    }

    return result;
}



int LogModel::getSeverityFilter() const
{
    return m_severity;
}

void LogModel::setSeverity(int severity)
{
    m_severity = severity;
    Gadget2_SetLogFilter(m_verbosity, m_category, m_severity);
}

int LogModel::getCategoryFilter() const
{
    return m_category;
}

void LogModel::setCategoryFilter(int category)
{
    m_category = category;
    Gadget2_SetLogFilter(m_verbosity, m_category, m_severity);
}

int LogModel::getVerbosityFilter() const
{
    return m_verbosity;
}

void LogModel::setVerbosityFilter(int value)
{
    m_verbosity = value;
    Gadget2_SetLogFilter(m_verbosity, m_category, m_severity);
}

void LogModel::saveFile(QFile *file)
{
    QTextStream stream(file);
    foreach(QString s, m_logStrings)
    {
        stream << s;
        stream << "\r\n";
    }
}
