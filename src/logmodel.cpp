#include "logmodel.h"

#include <QDateTime>

LogModel *LogModel::m_instance = nullptr;

LogModel *LogModel::getInstance()
{
    if(!m_instance)
        m_instance = new LogModel();
    return m_instance;
}

LogModel::LogModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Gadget2_SetLogCallback(GadgetLogCallback);
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

void LogModel::logData(const char *data)
{
    emit beginInsertRows(QModelIndex(), m_logStrings.length(), m_logStrings.length());
    m_logStrings <<  QDateTime::currentDateTime().toString(Qt::ISODate) + QString("\t") + QString::fromLatin1(data);
    emit endInsertRows();
}

void LogModel::doLog(const QString &message, quint32 severity, int verbosity)
{
    if(severity<=m_severity && verbosity<=m_verbosity)
    {
        emit beginInsertRows(QModelIndex(), m_logStrings.length(), m_logStrings.length());
        QString data = QDateTime::currentDateTime().toString(Qt::ISODate) + QString("\t") + message;
        m_logStrings << data;
        emit endInsertRows();
    }
}

void LogModel::log(const QString &message, quint32 severity, int verbosity)
{
    LogModel::getInstance()->doLog(message, severity, verbosity);
}

void __stdcall GadgetLogCallback(const char* logData)
{
    LogModel::getInstance()->logData(logData);
}
