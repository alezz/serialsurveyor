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
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setupDlg = new dlgSetup();
    this->exportDlg = new dlgExport();

    this->setupDlg->setupBaseFileNames(QApplication::applicationDirPath());


    QSplitter * splitter = new QSplitter(Qt::Vertical, this);
    ui->centralWidget->layout()->addWidget(splitter);
    splitter->addWidget(ui->frameLog);
    splitter->addWidget(ui->frameTemperature);
    splitter->addWidget(ui->frameCounters);
    splitter->addWidget(ui->frameSyslog);
    splitter->setHandleWidth(14);
    splitter->setStretchFactor(0,4);
    splitter->setStretchFactor(1,2);
    splitter->setStretchFactor(2,1);
    splitter->setStretchFactor(3,1);


    // !! Important !! Actions are accessed by index in some parts of the code
    //                 so the order with which are added is important...
    //--- from here
    ui->toolBar->addAction(QIcon(":/icon/gear"),"setup",this->setupDlg,SLOT(exec()));
    ui->toolBar->addAction(QIcon(":/icon/start"),"go",this,SLOT(start()));
    ui->toolBar->addAction(QIcon(":/icon/stop"),"stop",this,SLOT(stop()));
    ui->toolBar->actions().last()->setEnabled(false);
    //--- till here

    //ui->toolBar->addAction(QIcon(":/icon/disk"),"save",this,SLOT(save()));


    logModel = new LogModel(this);
    logProxyModel = new QSortFilterProxyModel(this);
    logProxyModel->setSourceModel(logModel);
    ui->logView->setModel(logProxyModel);
    ui->logView->setColumnWidth(0,90);
    ui->logView->setColumnWidth(1,50);
    ui->logView->setColumnWidth(2,200);

    counterModel = new CounterModel(this);
    ui->counterView->setModel(counterModel);

    temperatureModel = new temperatureMonitorModel(this);
    ui->temperatureView->setModel(this->temperatureModel);

    connect(ui->btnAddCounter,SIGNAL(clicked()),this,SLOT(addCounter()));
    ui->statusBar->showMessage("Ready.");

    connect(ui->editPattern,SIGNAL(textEdited(QString)),this,SLOT(calcPatHex(QString)));
    connect(ui->editPatternHex,SIGNAL(textEdited(QString)),this,SLOT(calcPatAscii(QString)));

    connect(ui->editFilterAscii,SIGNAL(textEdited(QString)),this,SLOT(applyLogFilterAscii(QString)));
    connect(ui->editFilterHex,SIGNAL(textEdited(QString)),this,SLOT(applyLogFilterHex(QString)));
    connect(ui->btnClearFilter,SIGNAL(clicked()),this,SLOT(removeLogFilter()));
    connect(ui->btnClear,SIGNAL(clicked()),this->logModel,SLOT(clear()));
    connect(ui->btnRestartCounter,SIGNAL(clicked()),this->counterModel,SLOT(clearCounters()));

    ui->logView->horizontalHeader()->setStretchLastSection(true);

    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()),this,SLOT(timerTimeout()));

    lblLed=new QLabel(this);
    ui->statusBar->addPermanentWidget(this->lblLed);
    lblLed->setPixmap(QPixmap(":/icon/red"));
    lblLed->setVisible(false);
    lblDisk=new QLabel(this);
    ui->statusBar->addPermanentWidget(this->lblDisk);
    lblDisk->setPixmap(QPixmap(":/icon/disk"));
    lblDisk->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete logProxyModel;
    delete logModel;
    delete counterModel;
    delete temperatureModel;
    delete setupDlg;
    delete exportDlg;
    delete lblLed;
    delete lblDisk;
    delete ui;

}

QByteArray MainWindow::bytefromHex(QString hex_rapresentation)
{
    return QByteArray::fromHex(hex_rapresentation.toLatin1());
}

void MainWindow::start()
{
    //logfiles
    if (!openLogFiles()) return;

    //ui
    ui->toolBar->actions().at(0)->setEnabled(false);
    ui->toolBar->actions().at(1)->setEnabled(false);
    ui->toolBar->actions().at(2)->setEnabled(true);

    //serialports
    openSerialPorts();

    //general purpose timer (log files flushing, status led blinking)
    newdata=false;
    this->timer->start(this->setupDlg->getLogWriteInterval());

    //status bar message
    ui->statusBar->showMessage(tr("Running: listening to %1 serial ports, timeout %2, %3")
                               .arg(serialPorts.count())
                               .arg(this->setupDlg->getTimeout())
                               .arg(this->setupDlg->getTerminator().isEmpty() ? tr("no terminator") : tr("0x%1").arg(
                               QString(this->setupDlg->getTerminator().toHex())))
                               );

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

}

void MainWindow::stop()
{
    //ui
    ui->toolBar->actions().at(0)->setEnabled(true);
    ui->toolBar->actions().at(1)->setEnabled(true);
    ui->toolBar->actions().at(2)->setEnabled(false);

    //serialports
    closeSerialPorts();

    //logfiles
    this->timer->stop();
    closeLogFiles();

    //status bar message
    ui->statusBar->showMessage("Stopped - Ready.");
    this->lblDisk->setVisible(false);
    this->lblLed->setVisible(false);

}

void MainWindow::closeSerialPorts()
{
    foreach (serialDevice * dev, serialPorts)
        delete dev;
    serialPorts.clear();
    this->syslog(tr("All serial ports closed."),1);

}

void MainWindow::handleData(QByteArray data)
{
    if (sender()==NULL) return;
    serialDevice * dev =((serialDevice *)sender());

    if (this->fileLogAll!=0)
    {
        this->fileLogAll->write(QString("%1,%2,'%3','%4'\n")
                            .arg(QDateTime::currentDateTime().toString("dd/MM/yy"))
                            .arg(QDateTime::currentDateTime().toString("hh.mm.ss.zzz"))
                            .arg(dev->portName())
                            .arg(QString(data.trimmed()))
                            .toLocal8Bit());
        newdata=true;
    }

    ui->logView->selectRow(
                this->logModel->addData(dev->portName(),data));

    //temperature

    if (data.startsWith(this->setupDlg->getTempPrefix()))
    {
        int temp = qRound(data.right(data.count()-1).trimmed().toFloat()*100.0);

        if (this->fileLogTemp!=0)
        {
            this->fileLogTemp->write(QString("%1,%2,'%3',%4,'%5'\n")
                                 .arg(QDateTime::currentDateTime().toString("dd/MM/yy"))
                                 .arg(QDateTime::currentDateTime().toString("hh.mm.ss.zzz"))
                                 .arg(dev->portName())
                                 .arg(temp)
                                 .arg(QString(data.trimmed()))
                                 .toLocal8Bit());
            newdata=true;
        }
        this->temperatureModel->refreshTemp(temp,dev->portName());
    }

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

bool MainWindow::openLogFiles()
{
    QString existingfiles=QString(), fn;
    fn = this->setupDlg->getLogAllFileName();
    if (!fn.isEmpty() )
    {
        this->fileLogAll = new QFile(fn);
        if (this->fileLogAll->exists() )
            existingfiles.append("\n"+fileLogAll->fileName());
    }
    else
        this->fileLogAll = 0;
    fn = this->setupDlg->getLogTempFileName();
    if (!fn.isEmpty())
    {
        this->fileLogTemp = new QFile(fn);
        if (this->fileLogTemp->exists() )
            existingfiles.append("\n"+fileLogTemp->fileName());
    }
    else
        this->fileLogTemp=0;

    if (!existingfiles.isEmpty())
        if (QMessageBox::warning(this,"File overwriting confirm",
                             tr("One or more log files do exist, continuing means OVERWRITING them! Proceed anyway??\n%1")
                                 .arg(existingfiles),
                             QMessageBox::Yes | QMessageBox::No,QMessageBox::No) != QMessageBox::Yes  ) return false;

    //TODO check open okay
    if (this->fileLogAll!=0)
    {
        if (this->fileLogAll->open(QIODevice::WriteOnly | QIODevice::Text))
            syslog(tr("AllLog file %1 succesfully opened")
                   .arg(fileLogAll->fileName()),1);
        else
            syslog(tr("Cannot open AllLog file %1")
                   .arg(fileLogAll->fileName()),2);
    }
    if (this->fileLogTemp!=0)
    {
        if (this->fileLogTemp->open(QIODevice::WriteOnly | QIODevice::Text))
            syslog(tr("TempLog file %1 succesfully opened")
                   .arg(fileLogTemp->fileName()),1);
        else
            syslog(tr("Cannot open TempLog file %1")
                   .arg(fileLogTemp->fileName()),2);
    }
    return true;

}

void MainWindow::closeLogFiles()
{
    this->timerTimeout();
    if (this->fileLogAll!=0)
    {
        this->fileLogAll->close();
        delete this->fileLogAll;
        this->fileLogAll=0;
    }
    if (this->fileLogTemp!=0)
    {
        this->fileLogTemp->close();
        delete this->fileLogTemp;
        this->fileLogTemp=0;
    }
    syslog("all log files closed",1);
}

void MainWindow::timerTimeout()
{
    //running "led"
    lblLed->setVisible(!lblLed->isVisible());

    //file flushing (only if new data is waiting...)
    if (newdata)
    {
        newdata=false;
        //ui->statusBar->showMessage("Writing files...",this->timer->interval()/2);
        lblDisk->setVisible(true);
        if (this->fileLogAll!=0) this->fileLogAll->flush();
        if (this->fileLogTemp!=0) this->fileLogTemp->flush();
    }
}

void MainWindow::addCounter()
{
    QByteArray byte = bytefromHex(ui->editPatternHex->text());
    if (byte.isEmpty()) return;
    this->counterModel->addCounter(byte);
    ui->editPattern->clear();
    ui->editPatternHex->clear();
}

