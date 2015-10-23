#ifndef COUNTERMODEL_H
#define COUNTERMODEL_H

#include <QAbstractTableModel>

class CounterModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CounterModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:

public slots:
    void addCounter(QByteArray pattern, int init=0);
    //void incrementCounter(QByteArray pattern);
    QModelIndex checkForMatch(QByteArray data, QString subCounterName = QString());
    //bool addSubCounter(QString name);
    void clearCounters();

private:
    struct CounterEntry
    {
        QByteArray pattern;
        int total;
        QList<int> subCounters;
    };
    QList<CounterEntry> counters;
    QStringList subCounterNames;

private slots:
    void addSubCounter(QString name, int init=0);
};

#endif // COUNTERMODEL_H
