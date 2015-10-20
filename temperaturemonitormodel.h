#ifndef TEMPERATUREMONITORMODEL_H
#define TEMPERATUREMONITORMODEL_H

#include <QAbstractTableModel>

class temperatureMonitorModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit temperatureMonitorModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:

public slots:
    //float checkData(QByteArray data, QString deviceName);
    void refreshTemp(int temp, QString deviceName);


private:
    int averageT;
    QList<int> deviceT;
    QStringList deviceNames;

};

#endif // TEMPERATUREMONITORMODEL_H
