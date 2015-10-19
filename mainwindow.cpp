/*
 * (c) 2015 Alessandro Mauro <www.maetech.it>
 *
 *
 * */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setupDlg = new dlgSetup();
    this->exportDlg = new dlgExport();

    QSplitter * splitter = new QSplitter(Qt::Vertical, this);
    ui->centralWidget->layout()->addWidget(splitter);
    splitter->addWidget(ui->frameLog);
    splitter->addWidget(ui->frameCounters);
    splitter->addWidget(ui->frameSyslog);
    splitter->setHandleWidth(14);
    //splitter->setSizes(QList<int>()<<100<<100<<20);
    splitter->setStretchFactor(0,3);
    splitter->setStretchFactor(1,2);
    splitter->setStretchFactor(2,1);

    // !! Important !! Actions are accessed by index in some parts of the code
    //                 so the order with which are added is important...
    //--- from here
    ui->toolBar->addAction(QIcon(":/icon/gear"),"setup",this->setupDlg,SLOT(exec()));
    ui->toolBar->addAction(QIcon(":/icon/start"),"go",this,SLOT(start()));
    ui->toolBar->addAction(QIcon(":/icon/stop"),"stop",this,SLOT(stop()));
    ui->toolBar->actions().last()->setEnabled(false);
    //--- till here

    ui->toolBar->addAction(QIcon(":/icon/disk"),"save",this,SLOT(save()));


    logModel = new LogModel(this);
    logProxyModel = new QSortFilterProxyModel(this);
    logProxyModel->setSourceModel(logModel);
    ui->logView->setModel(logProxyModel);
    ui->logView->setColumnWidth(0,90);
    ui->logView->setColumnWidth(1,50);
    ui->logView->setColumnWidth(2,200);

    counterModel = new CounterModel(this);
    ui->counterView->setModel(counterModel);

    connect(ui->btnAddCounter,SIGNAL(clicked()),this,SLOT(addCounter()));
    ui->statusBar->showMessage("Ready.");

    connect(ui->editPattern,SIGNAL(textEdited(QString)),this,SLOT(calcPatHex(QString)));
    connect(ui->editPatternHex,SIGNAL(textEdited(QString)),this,SLOT(calcPatAscii(QString)));

    connect(ui->editFilterAscii,SIGNAL(textEdited(QString)),this,SLOT(applyLogFilterAscii(QString)));
    connect(ui->editFilterHex,SIGNAL(textEdited(QString)),this,SLOT(applyLogFilterHex(QString)));
    connect(ui->btnClearFilter,SIGNAL(clicked()),this,SLOT(removeLogFilter()));

    ui->logView->horizontalHeader()->setStretchLastSection(true);

}

MainWindow::~MainWindow()
{
    delete logProxyModel;
    delete logModel;
    delete counterModel;
    delete setupDlg;
    delete exportDlg;
    delete ui;

}

QByteArray MainWindow::bytefromHex(QString hex_rapresentation)
{
    return QByteArray::fromHex(hex_rapresentation.toLatin1());
}

void MainWindow::start()
{
    ui->toolBar->actions().at(0)->setEnabled(false);
    ui->toolBar->actions().at(1)->setEnabled(false);
    ui->toolBar->actions().at(2)->setEnabled(true);
    openSerialPorts();
}

void MainWindow::openSerialPorts()
{
    foreach (QString dev, this->setupDlg->getSelectedDevices())
    {
        serialDevice * entry = new serialDevice(dev,this);
        if (entry->openDevice(this->setupDlg->getBaudrate(),
                              this->setupDlg->getTimeout(),
                              this->setupDlg->getTerminator()))
        {
            this->syslog(tr("Serial port %1 successfully opened.")
                    .arg(dev),1);
            connect(entry,SIGNAL(dataReady(QByteArray)),this,SLOT(handleData(QByteArray)));
            serialPorts.append(entry);
        }
        else
        {
            this->syslog(tr("Cannot open %1 serial port.")
                         .arg(dev),2);
            delete entry;
        }
    }
    ui->statusBar->showMessage(tr("Running: listening to %1 serial ports, timeout %2, %3")
                               .arg(serialPorts.count())
                               .arg(this->setupDlg->getTimeout())
                               .arg(this->setupDlg->getTerminator().isEmpty() ? tr("no terminator") : tr("0x%1").arg(
                               QString(this->setupDlg->getTerminator().toHex())))
                               );
}

void MainWindow::stop()
{
    ui->toolBar->actions().at(0)->setEnabled(true);
    ui->toolBar->actions().at(1)->setEnabled(true);
    ui->toolBar->actions().at(2)->setEnabled(false);
    closeSerialPorts();
}

void MainWindow::closeSerialPorts()
{
    foreach (serialDevice * dev, serialPorts)
        delete dev;
    serialPorts.clear();
    //ui->logText->appendPlainText(tr("All serial ports closed."));
    this->syslog(tr("All serial ports closed."),1);
    ui->statusBar->showMessage("Stopped - Ready.");
}

void MainWindow::handleData(QByteArray data)
{
    if (sender()==NULL) return;
    serialDevice * dev =((serialDevice *)sender());
    this->syslog(tr("from %1 received: %2")
                 .arg(dev->portName())
                 .arg(QString(data)));
    ui->logView->selectRow(
                this->logModel->addData(dev->portName(),data));
    ui->counterView->selectionModel()->select(
                this->counterModel->checkForMatch(data,dev->portName()),
                QItemSelectionModel::ClearAndSelect);
}

void MainWindow::syslog(QString message, int level)
{
#ifndef DEBUGLOG
    if (level<=0) return;
#endif
    QString lvl;
    switch (level)
    {
    case 1: lvl="info"; break;
    case 2: lvl="warning"; break;
    case 3: lvl="error"; break;
    default : lvl="debug";
    }
    ui->logText->appendPlainText(tr("[%1] %2: %3")
                                 .arg(QTime::currentTime().toString())
                                 .arg(lvl)
                                 .arg(message));
}

void MainWindow::calcPatAscii(QString hex)
{
    ui->editPattern->setText(bytefromHex(hex));
}

void MainWindow::calcPatHex(QString ascii)
{
    ui->editPatternHex->setText(ascii.toLatin1().toHex());
}

void MainWindow::save()
{
    QString file = this->exportDlg->exec();
    //if (file)
    qDebug()<<file;
}

void MainWindow::applyLogFilterAscii(QString ascii)
{
    ui->editFilterHex->clear();
    this->logProxyModel->setFilterKeyColumn(2);
    this->logProxyModel->setFilterFixedString(ascii);
    if (ascii.isEmpty())
        ui->logView->setStyleSheet(QString());
    else
        ui->logView->setStyleSheet(QString("background: #ffa;"));
}

void MainWindow::applyLogFilterHex(QString hex)
{
    ui->editFilterAscii->clear();
    this->logProxyModel->setFilterKeyColumn(3);
    this->logProxyModel->setFilterFixedString(hex);
    if (hex.isEmpty())
        ui->logView->setStyleSheet(QString());
    else
        ui->logView->setStyleSheet(QString("background: #ffa;"));
}

void MainWindow::removeLogFilter()
{
    //this->logProxyModel->invalidate();
    ui->editFilterHex->clear();
    this->applyLogFilterHex(QString());
}

void MainWindow::addCounter()
{
    QByteArray byte = bytefromHex(ui->editPatternHex->text());
    if (byte.isEmpty()) return;
    this->counterModel->addCounter(byte);
    ui->editPattern->clear();
    ui->editPatternHex->clear();
}

