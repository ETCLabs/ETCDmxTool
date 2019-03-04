#include "logmodel.h"

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
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
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
    m_logStrings << QString::fromLatin1(data);
    emit endInsertRows();
}

void __stdcall GadgetLogCallback(const char* logData)
{
    LogModel::getInstance()->logData(logData);
}
