#include "temperaturemonitormodel.h"

temperatureMonitorModel::temperatureMonitorModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    averageT=0;
}

QVariant temperatureMonitorModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    switch (index.column())
    {
        case 0:
        {
        int a = averageT-((averageT/100)*100);
        return QString ("%1,%2").arg(averageT/100).arg(QString("%1").arg(a),2,'0');
        }
        default:
        {
            int t = deviceT.at(index.column()-1);
            return QString("%1,%2").arg(t/100).arg(QString("%1").arg(t-((t/100)*100)),2,'0');
        }
    }
}

QVariant temperatureMonitorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();
    switch (section)
    {
    case 0: return tr("average");
    default: return deviceNames.at(section-1);
    }
}

int temperatureMonitorModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int temperatureMonitorModel::columnCount(const QModelIndex &parent) const
{
    return deviceNames.count() +1;
}

void temperatureMonitorModel::refreshTemp(int temp, QString deviceName)
{
    int sum=0;
    if (!deviceNames.contains(deviceName))
    {
        this->beginInsertColumns(QModelIndex(),this->columnCount(),this->columnCount());
        deviceNames.append(deviceName);
        deviceT.append(0);
        this->endInsertColumns();
    }
    for (int i=0; i<deviceNames.count(); i++)
    {
        if (deviceNames.at(i) == deviceName)
        {
            deviceT[i] = temp;
            emit dataChanged(this->index(0,i+1),this->index(0,i+1));
        }
        sum += deviceT.at(i);
    }

    //calculate average
    this->averageT = sum / deviceNames.count();
    emit dataChanged(this->index(0,0),this->index(0,0));
}

//bool temperatureMonitorModel::checkData(QByteArray data, QString deviceName)
//{
//    float temp;
//    float sum=0;
//    if (!data.startsWith('T')) return 0;
//    temp = data.right(data.count()-1).toFloat();
//    if (!deviceNames.contains(deviceName))
//    {
//        deviceNames.append(deviceName);
//        deviceT.append(0);
//    }
//    for (int i=0; i<deviceNames.count(); i++)
//    {
//        if (deviceNames.at(i) == deviceName)
//            deviceT[i] = temp;
//        sum += deviceT.at(i);
//    }
//    //calculate average
//    average = sum / deviceNames.count();
//}
