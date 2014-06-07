#ifndef IMPORTCSV_H
#define IMPORTCSV_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QList>
#include <QKeyEvent>
#include <QMessageBox>

namespace Ui {
class ImportCSV;
}

class ImportCSV : public QDialog
{
    Q_OBJECT

public:
    explicit ImportCSV(QFile *file, QWidget *parent = 0);
    ~ImportCSV();

    struct Info
    {
        QString refdes, part, type, value, pattern, shop, shopid;
    };

    int getDataCount() const;
    Info getData(int i);


private slots:
    void on_reread_clicked();
    void reRead();
    void updateHeader();
    void done(int r);

private:
    Ui::ImportCSV *ui;
    QTextStream stream;
    QList<QStringList> data;
    QStringList header;
    int maxFileldNo;

    QStringList parseLine(QString line, QString fieldsep, QString stringsep);
    void keyPressEvent(QKeyEvent *event);
};

#endif // IMPORTCSV_H
