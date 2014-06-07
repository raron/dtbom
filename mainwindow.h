#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QUrl>
#include <QModelIndex>
#include <QDebug>
#include <QWebPage>
#include <QDesktopServices>
#include <QWebFrame>
#include <QProgressBar>
#include <QFileDialog>
#include <QFile>
#include <QList>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>

#include "dtascloader.h"
#include "webshopscript.h"
#include "loadform.h"
#include "importcsv.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlDatabase _db, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void linkClicked(QUrl url);
    void loaded(bool ok);

    void on_actionImport_from_DipTrace_ASCII_triggered();

    void on_webShopSelect_activated(int index);

    void on_partList_activated(const QModelIndex &index);

    void on_partListSelect_activated(int index);

    void on_actionParts_group_by_shop_triggered();

    void on_actionFile_info_triggered();

    void on_actionExport_to_CSV_triggered();

    void on_actionExport_to_CSV_grouped_triggered();

    void on_actionExport_to_CSV_by_shop_triggered();

    void on_actionImport_from_CSV_triggered();

private:
    Ui::MainWindow *ui;
    QList<WebShopScript> webShopScripts;
    QSqlDatabase db;
    QSqlQuery partListQuery;
    QSqlQueryModel partListQueryModel;

    void updatePartList();
    void exportToCsv(QString queryString);
    void mergeImport();
    bool checkImportDate(QString fn);
};

#endif // MAINWINDOW_H
