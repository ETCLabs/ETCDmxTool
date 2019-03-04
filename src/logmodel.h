#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>
#include "GadgetDLL.h"

class LogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    static LogModel *getInstance();
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void logData(const char* data);
private:
    static LogModel *m_instance;
    explicit LogModel(QObject *parent = nullptr);
    QStringList m_logStrings;
};

void __stdcall GadgetLogCallback(const char* logData);


#endif // LOGMODEL_H
