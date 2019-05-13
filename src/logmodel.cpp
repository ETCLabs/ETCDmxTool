#include "logmodel.h"

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
    #if defined (GADGET2)
    Gadget2_SetLogCallback(GadgetLogCallback);
    #endif
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

void LogModel::logWithoutFilter(const QString &string)
{
    if(this->thread()==QThread::currentThread())
    {
        emit beginInsertRows(QModelIndex(), m_logStrings.length(), m_logStrings.length());
        m_logStrings <<  QDateTime::currentDateTime().toString(Qt::ISODate) + QString("\t") + string;
        emit endInsertRows();
    }
    else {
        QMetaObject::invokeMethod(this,
                                  "logWithoutFilter",
                                  Q_ARG(QString, string)
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
            emit beginInsertRows(QModelIndex(), m_logStrings.length(), m_logStrings.length());
            QString data = QDateTime::currentDateTime().toString(Qt::ISODate) + QString("\t") + message;
            m_logStrings << data;
            emit endInsertRows();
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

void LogModel::log(const QString &message, quint32 severity, int verbosity)
{
    LogModel::getInstance()->doLog(message, severity, verbosity);
}

#if defined (GADGET2)
void __stdcall GadgetLogCallback(const char* logData)
{
    LogModel::getInstance()->logWithoutFilter(QString(logData));
}
#endif

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
    #if defined (GADGET2)
    Gadget2_SetLogFilter(m_verbosity, m_category, m_severity);
    #endif
}

int LogModel::getCategoryFilter() const
{
    return m_category;
}

void LogModel::setCategoryFilter(int category)
{
    m_category = category;
    #if defined (GADGET2)
    Gadget2_SetLogFilter(m_verbosity, m_category, m_severity);
    #endif
}

int LogModel::getVerbosityFilter() const
{
    return m_verbosity;
}

void LogModel::setVerbosityFilter(int value)
{
    m_verbosity = value;
    #if defined (GADGET2)
    Gadget2_SetLogFilter(m_verbosity, m_category, m_severity);
    #endif
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
