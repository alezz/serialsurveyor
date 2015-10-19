#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTime>
#include "logmodel.h"
#include "countermodel.h"
#include "serialdevice.h"
#include "dlgsetup.h"
#include "dlgexport.h"

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
    dlgSetup * setupDlg;
    dlgExport * exportDlg;
    QByteArray bytefromHex(QString hex_rapresentation);

private slots:
    void start();
    void stop();

    void openSerialPorts();
    void closeSerialPorts();

    void addCounter();

    void handleData(QByteArray data);

    void syslog(QString message, int level = 0);

    void calcPatAscii(QString hex);
    void calcPatHex(QString ascii);

    void save();

    void applyLogFilterAscii(QString ascii);
    void applyLogFilterHex(QString hex);
    void removeLogFilter();

};

#endif // MAINWINDOW_H
