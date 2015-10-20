#ifndef DLGSETUP_H
#define DLGSETUP_H

#include <QDialog>
#include <QSerialPortInfo>

namespace Ui {
class dlgSetup;
}

class dlgSetup : public QDialog
{
    Q_OBJECT

public:
    explicit dlgSetup(QWidget *parent = 0);
    ~dlgSetup();

    QStringList getSelectedDevices();
    int getTimeout();
    QByteArray getTerminator();
    qint32 getBaudrate();

    QString getLogAllFileName();
    QString getLogTempFileName();
    int getLogWriteInterval();

    void setupBaseFileNames(QString basePath);

private:
    Ui::dlgSetup *ui;

private slots:
    void refresh();
    void calcHex(QString ascii);
    void calcAscii(QByteArray hex);
    void calcAscii(QString hex);
    void browseLogAll();
    void browseLogTemp();
    QString chooseFile();
};

#endif // DLGSETUP_H
