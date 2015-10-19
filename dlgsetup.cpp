#include "dlgsetup.h"
#include "ui_dlgsetup.h"

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

void dlgSetup::refresh()
{
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
