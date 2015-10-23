/*
 * (c) 2015 Alessandro Mauro <www.maetech.it>
 *
 *
 * */

#include "countermodel.h"

CounterModel::CounterModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    // maybe unnecessary
    subCounterNames.clear();
    counters.clear();
}

QVariant CounterModel::data(const QModelIndex &index, int role) const
{
    if (role!=Qt::DisplayRole) return QVariant();
    switch(index.column())
    {
    case 0: return QString("%1 [%2]")
                .arg(QString(counters.at(index.row()).pattern))
                .arg(QString(counters.at(index.row()).pattern.toHex())); break;
    case 1: return counters.at(index.row()).total; break;
    default: return counters.at(index.row()).subCounters.at(index.column()-2);
    }
}

QVariant CounterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role!=Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Vertical)
        return QVariant();
    switch(section)
    {
    case 0: return tr("pattern");
    case 1: return tr("total");
    default: return subCounterNames.at(section-2);
    }
}

int CounterModel::rowCount(const QModelIndex &parent) const
{
    return this->counters.count();
}

int CounterModel::columnCount(const QModelIndex &parent) const
{
    return 2 + subCounterNames.count();
}

void CounterModel::addCounter(QByteArray pattern, int init)
{
    beginInsertRows(QModelIndex(),counters.count(),counters.count());
    CounterEntry entry;
    entry.total=init;
    entry.pattern=pattern;
    entry.subCounters.clear();
    for (int i=0; i< subCounterNames.count();i++)
        entry.subCounters.append(init);
    counters.append(entry);
    endInsertRows();
}

QModelIndex CounterModel::checkForMatch(QByteArray data, QString subCounterName)
{
    QModelIndex index;
    for (int i=0; i<counters.count();i++)
    {
        //count
        int c=0;    //la seguente aggira un "bug", se il pattern di ricerca
                    //e' vuoto, si ottiene un blocco di 10-15 secondi !
        if (!counters.at(i).pattern.isEmpty())
            c = data.count(counters.at(i).pattern);
        if (c>0)
        {
            //total counter
            counters[i].total += c;
            //subCounters
            if (!subCounterName.isEmpty())
            {
                if (!subCounterNames.contains(subCounterName))
                    addSubCounter(subCounterName);
                for (int j=0; j< subCounterNames.count(); j++)
                    if (subCounterNames.at(j) == subCounterName )
                    {
                        counters[i].subCounters[j] += c;
                        index = this->index(i,j+2);
                    }
            }
            else
                index = this->index(i,1);
        }
    }
    emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
    return index;
}

void CounterModel::clearCounters()
{
    for(int i = 0; i< this->counters.count();i++)
    {
        this->counters[i].total=0;
        for (int j=0; j<this->counters[i].subCounters[j];j++)
            this->counters[i].subCounters[j]=0;
    }
    emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
}

void CounterModel::addSubCounter(QString name, int init)
{
    //add sub counter to both subCounterNames (headers) and to the subCounter list of
    //  each counterEntry (row) . [subcounters are columns]
    beginInsertColumns(QModelIndex(),this->columnCount(),this->columnCount());
    this->subCounterNames.append(name);
    for (int i=0; i<counters.count();i++)
        counters[i].subCounters.append(init);
    //done
    endInsertColumns();
}
