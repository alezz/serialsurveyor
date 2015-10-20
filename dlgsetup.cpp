#include "dlgsetup.h"
#include "ui_dlgsetup.h"
#include <QFileDialog>

dlgSetup::dlgSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgSetup)
{
    ui->setupUi(this);

    ui->comboBaud->insertItems(0,QStringList()
                               <<"300"
                               <<"1200"
                               <<"2400"
                               <<"4800"
                               <<"9600"
                               <<"19200"
                               <<"38400"
                               <<"57600"
                               <<"115200"
                               <<"230400");
    ui->comboBaud->setCurrentIndex(4);  // 9600
    refresh();
    connect(ui->btnRefresh,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(ui->btnClose,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->editTerminator,SIGNAL(textEdited(QString)),this,SLOT(calcHex(QString)));
    connect(ui->editTermHex,SIGNAL(textEdited(QString)),this,SLOT(calcAscii(QString)));
    connect(ui->btnBrowseLogAll,SIGNAL(clicked()),this,SLOT(browseLogAll()));
    connect(ui->btnBrowseLogTemp,SIGNAL(clicked()),this,SLOT(browseLogTemp()));

    //default packet term
    ui->editTermHex->setText("0d");
    this->calcAscii(ui->editTermHex->text());
}

dlgSetup::~dlgSetup()
{
    delete ui;
}

QStringList dlgSetup::getSelectedDevices()
{
    QStringList devList;
    for (int i =0; i<ui->list->count();i++)
        if (ui->list->item(i)->checkState()==Qt::Checked)
            devList.append(ui->list->item(i)->text());
    return devList;
}

int dlgSetup::getTimeout()
{
    return ui->spinTimeout->value();
}

QByteArray dlgSetup::getTerminator()
{
    return ui->editTerminator->text().toLatin1();
}

qint32 dlgSetup::getBaudrate()
{
    return (qint32)ui->comboBaud->currentText().toInt();
}

QString dlgSetup::getLogAllFileName()
{
    if (ui->checkLogAll->isChecked())
        return ui->editLogAllFileName->text();
    else
        return QString();
}

QString dlgSetup::getLogTempFileName()
{
    if (ui->checkLogTemp->isChecked())
        return ui->editLogTempFileName->text();
    else
        return QString();
}

int dlgSetup::getLogWriteInterval()
{
    return ui->spinLogWriteInterval->value();
}

void dlgSetup::setupBaseFileNames(QString basePath)
{
    ui->editLogAllFileName->setText(basePath + "/all.csv");
    ui->editLogTempFileName->setText(basePath + "/temp.csv");
}

void dlgSetup::refresh()
{
    ui->list->clear();
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        //QListWidgetItem * item = new QListWidgetItem( info.portName(),this->ui->list);
        QListWidgetItem * item = new QListWidgetItem( info.portName());
        this->ui->list->addItem(item);
        item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        item->setCheckState(Qt::Checked);

    }
}

void dlgSetup::calcHex(QString ascii)
{
    ui->editTermHex->setText(ascii.toLatin1().toHex());
}

void dlgSetup::calcAscii(QByteArray hex)
{
    ui->editTerminator->setText(QByteArray::fromHex(hex));
}

void dlgSetup::calcAscii(QString hex)
{
    this->calcAscii(hex.toLatin1());
}

void dlgSetup::browseLogAll()
{
    QString file = chooseFile();
    if (!file.isEmpty())
        ui->editLogAllFileName->setText( file);
}

void dlgSetup::browseLogTemp()
{
    QString file = chooseFile();
    if (!file.isEmpty())
        ui->editLogTempFileName->setText( file);
}

QString dlgSetup::chooseFile()
{
    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setDefaultSuffix("csv");
    fd.setNameFilter("*.csv");
    fd.setAcceptMode(QFileDialog::AcceptSave);
    if (fd.exec()==QDialog::Rejected) return QString();
    if (fd.selectedFiles().isEmpty()) return QString();
    return fd.selectedFiles().first();
}


