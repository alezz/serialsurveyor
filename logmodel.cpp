/*
 * (c) 2015 Alessandro Mauro <www.maetech.it>
 *
 *
 * */

#include "logmodel.h"

LogModel::LogModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (role!=Qt::DisplayRole) return QVariant();
    switch(index.column())
    {
    case 0: return log.at(index.row()).timestamp.toString("yy-MM-dd\nhh:mm:ss.zzz"); break;
    case 1: return log.at(index.row()).serial; break;
    case 2: return log.at(index.row()).data; break;
    case 3: return log.at(index.row()).data.toHex(); break;
    default: return QVariant();
    }
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role!=Qt::DisplayRole) return QVariant();
    if (orientation==Qt::Vertical)
        return tr("%1").arg(section);
    switch (section)
    {
    case 0: return tr("time");
    case 1: return tr("serial");
    case 2: return tr("data Ascii");
    case 3: return tr("data Hex");
    default: return QVariant();
    }
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    return log.count();
}

int LogModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

int LogModel::addData(QString serial, QByteArray data)
{
    beginInsertRows(QModelIndex(),log.count(),log.count());
    LogEntry entry;
    entry.data = data;
    entry.serial = serial;
    entry.timestamp = QDateTime::currentDateTime();
    log.append(entry);
    endInsertRows();
    return log.count()-1;
}
