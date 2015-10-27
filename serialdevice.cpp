/*
 * (c) 2015 Alessandro Mauro <www.maetech.it>
 *
 *
 * */

#include "serialdevice.h"

serialDevice::serialDevice(QString portName, QObject *parent) :
    QTimer(parent)
{
    this->setSingleShot(true);
    this->buffer.clear();
    this->serialPort=new QSerialPort(parent);
    this->serialPort->setPortName(portName);
    connect(this,SIGNAL(timeout()),this,SLOT(timerTimeout()));
}

bool serialDevice::openDevice(qint32 baud, int timeout, QByteArray terminator )
{
    if (this->serialPort->open(QIODevice::ReadWrite))
    {
        this->serialPort->setBaudRate(baud);
        connect(this->serialPort,SIGNAL(readyRead()),this,SLOT(handleReadyRead()));
        this->timeout=timeout;
        this->terminator = terminator;
        return true;
    }
    return false;
}

serialDevice::~serialDevice()
{
    if (this->serialPort->isOpen())
        this->serialPort->close();
    delete this->serialPort;
}

QString serialDevice::portName()
{
    return this->serialPort->portName();
}

void serialDevice::write_data(const QByteArray data)
{
    this->serialPort->write(data);
    this->serialPort->flush();
}

void serialDevice::handleReadyRead()
{
    buffer.append(this->serialPort->readAll());
    //buffer.split(terminator)
    if ((!terminator.isEmpty()) && buffer.contains(terminator))
        this->timerTimeout();
    else if (timeout>0)
        this->start(this->timeout);
    else if (terminator.isEmpty())
        this->timerTimeout();
}

void serialDevice::timerTimeout()
{
    emit dataReady(buffer);
    this->stop();
    buffer.clear();
}
