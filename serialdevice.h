#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <QTimer>
#include <QSerialPort>

class serialDevice : public QTimer
{
    Q_OBJECT
public:
    explicit serialDevice(QString portName,QObject *parent = 0);
    ~serialDevice();
    bool openDevice(qint32 baud,
                    int timeout=0,
                    QByteArray terminator = QByteArray());
    QString portName();

signals:
    void dataReady(QByteArray data);

public slots:

private:
    QSerialPort * serialPort;
    QByteArray buffer, terminator;
    int timeout;

private slots:
    void handleReadyRead();
    void timerTimeout();

};

#endif // SERIALDEVICE_H
