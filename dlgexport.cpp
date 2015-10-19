#include "dlgexport.h"
#include "ui_dlgexport.h"
#include <QFileDialog>

dlgExport::dlgExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgExport)
{
    ui->setupUi(this);
    connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->btnOk,SIGNAL(clicked()),this,SLOT(accept()));
}

dlgExport::~dlgExport()
{
    delete ui;
}

QString dlgExport::exec(int dummy)
{
    if (QDialog::exec()==QDialog::Accepted)
    {
        QFileDialog fd(this);
        fd.setFileMode(QFileDialog::AnyFile);
        fd.setDefaultSuffix(getSuffix());
        fd.setNameFilter(getFilter());
        fd.setAcceptMode(QFileDialog::AcceptSave);
        if (fd.exec()==QDialog::Rejected) return QString();
        if (fd.selectedFiles().isEmpty()) return QString();
        return fd.selectedFiles().first();
    }
    else
        return QString();
}

dlgExport::FileFormat dlgExport::getFormat()
{
    if (ui->formatCsv->isChecked()) return FORMAT_CSV;
    if (ui->formatXml->isChecked()) return FORMAT_XML;
    return FORMAT_TXT;
}

dlgExport::Source dlgExport::getSource()
{
    if (ui->sourceFiltered->isChecked()) return SOURCE_FILTERED;
    if (ui->sourceCounters->isChecked()) return SOURCE_COUNTERS;
    return SOURCE_LOG;
}

bool dlgExport::includeDate()
{
    return ui->fieldDate->isChecked();
}

bool dlgExport::includeTime()
{
    return ui->fieldTime->isChecked();
}

bool dlgExport::includeDevice()
{
    return ui->fieldDevice->isChecked();
}

bool dlgExport::includeAscii()
{
    return ui->fieldAscii->isChecked();
}

bool dlgExport::includeTotalCounts()
{
    return ui->fieldDate->isChecked();
}

bool dlgExport::includeSubCounts()
{
    return ui->fieldDate->isChecked();
}

QString dlgExport::getComment()
{
    return ui->editComment->toPlainText();
}

QString dlgExport::getFilter()
{
    switch (getFormat())
    {
    case FORMAT_TXT: return "*.txt *.TXT";
    case FORMAT_CSV: return "*.csv *.CSV";
    case FORMAT_XML: return "*.xml *.XML";
    }
}

QString dlgExport::getSuffix()
{
    switch (getFormat())
    {
    case FORMAT_TXT: return "txt";
    case FORMAT_CSV: return "csv";
    case FORMAT_XML: return "xml";
    }
}
