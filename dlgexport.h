#ifndef DLGEXPORT_H
#define DLGEXPORT_H

#include <QDialog>

namespace Ui {
class dlgExport;
}

class dlgExport : public QDialog
{
    Q_OBJECT

public:
    explicit dlgExport(QWidget *parent = 0);
    ~dlgExport();
    QString exec(int dummy=0);
    enum FileFormat {FORMAT_TXT , FORMAT_CSV, FORMAT_XML };
    FileFormat getFormat();
    enum Source {SOURCE_LOG , SOURCE_FILTERED, SOURCE_COUNTERS };
    Source getSource();
    bool includeDate();
    bool includeTime();
    bool includeDevice();
    bool includeAscii();
    bool includeHex();
    bool includeTotalCounts();
    bool includeSubCounts();
    QString getComment();

private:
    Ui::dlgExport *ui;
    QString getFilter();
    QString getSuffix();


};

#endif // DLGEXPORT_H
