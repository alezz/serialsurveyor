#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSortFilterProxyModel>
#include <QDateTime>
#include "logmodel.h"
#include "countermodel.h"
#include "temperaturemonitormodel.h"
#include "serialdevice.h"
#include "dlgsetup.h"
#include "dlgexport.h"
#include <QTimer>
#include <QFile>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<serialDevice *> serialPorts;
    LogModel * logModel;
    QSortFilterProxyModel * logProxyModel;
    CounterModel * counterModel;
    temperatureMonitorModel * temperatureModel;
    dlgSetup * setupDlg;
    dlgExport * exportDlg;
    QByteArray bytefromHex(QString hex_rapresentation);
    QTimer * timer;
    QFile * fileLogAll;
    QFile * fileLogTemp;
    QLineEdit * editSendData;
    QLabel * lblLed;
    QLabel * lblDisk;

    bool newdata;

private slots:
    void start();
    void stop();

    void _send();

    void openSerialPorts();
    void closeSerialPorts();

    void addCounter();

    void handleData(QByteArray data);

    void syslog(QString message, int level = 1);

    void calcPatAscii(QString hex);
    void calcPatHex(QString ascii);

    void save();

    void applyLogFilterAscii(QString ascii);
    void applyLogFilterHex(QString hex);
    void removeLogFilter();

    bool openLogFiles();
    void closeLogFiles();
    void timerTimeout();

};

#endif // MAINWINDOW_H
