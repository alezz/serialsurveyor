#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>

class LogModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LogModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

signals:

public slots:

    int addData(QString serial, QByteArray data);
    void clear();

private:
    struct LogEntry {
        QByteArray data;
        QString serial;
        QDateTime timestamp;
    };
    QList<LogEntry> log;
};

#endif // LOGMODEL_H
