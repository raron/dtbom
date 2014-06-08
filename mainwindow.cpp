#include "mainwindow.h"
#include "ui_mainwindow.h"

/*

select 'torolt' m, * from parts where id not in (select parts.id from parts inner join import on (parts.id = import.id))
union
select 'uj' m, * from import where id not in (select parts.id from parts inner join import on (parts.id = import.id))
union
select 'valtozott' m, * from import where id not in (select parts.id from parts inner join import on (parts.id = import.id and parts.value = import.value and parts.pattern = import.pattern))
except
select 'valtozott' m, * from import where id not in (select parts.id from parts inner join import on (parts.id = import.id or parts.value = import.value or parts.pattern = import.pattern))



update b set asd = refdes || '.:.' || part



  */


MainWindow::MainWindow(QSqlDatabase _db, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    db(_db),
    partListQuery(db)
{
    ui->setupUi(this);

    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    connect(ui->webView, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));
    connect(ui->webView, SIGNAL(loadProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(loaded(bool)));

    webShopScripts << WebShopScript::fromDir(QApplication::applicationDirPath() + "/scripts");
    for(int i = 0; i < webShopScripts.size(); i++)
        ui->webShopSelect->addItem(webShopScripts[i].getName());

    updatePartList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::linkClicked(QUrl url)
{
    qDebug() << "link:" << url;
    if (url.toString().endsWith("pdf", Qt::CaseInsensitive))
    {
        QDesktopServices::openUrl(url);
    }
    else if (url.toString().startsWith("selectid:"))
    {
        if (ui->partList->currentIndex().isValid())
        {
            QSqlRecord record = partListQueryModel.record(ui->partList->currentIndex().row());

            QString id = url.toString().remove("selectid:");
            qDebug() << "selected:" << id;
            QSqlQuery query(db);
            query.prepare("update parts set shop = :shop, shopid = :shopid where value = :value and type = :type;");
            query.bindValue(":shop", webShopScripts[ui->webShopSelect->currentIndex()].getName());
            query.bindValue(":shopid", id);
            query.bindValue(":value", record.value("value"));
            query.bindValue(":type", record.value("type"));
            query.exec();

            updatePartList();
        }
    }
    else
    {
        ui->webView->load(url);
    }
}

void MainWindow::loaded(bool ok)
{
    if (ok)
        ui->webView->page()->currentFrame()->evaluateJavaScript(webShopScripts[ui->webShopSelect->currentIndex()].getSearchScript());
}

void MainWindow::on_actionImport_from_DipTrace_ASCII_triggered()
{
    QSqlQuery query(db);

    QString fn = QFileDialog::getOpenFileName(this, trUtf8("Import from DipTrace ASCII"), QString(), "*.asc");
    if (!fn.isEmpty())
    {
        if (checkImportDate(fn))
            return;


        QFile f(fn);
        f.open(QFile::ReadOnly);
        DtAscLoader loader(&f);
        connect(&loader, SIGNAL(loadProgress(int)), ui->progressBar, SLOT(setValue(int)));
        DtAscNode *root = DtAscNode::fromSubNodes(loader.load());

        bool sqlok = true;

        sqlok &= query.exec("create table if not exists import (id TEXT, refdes TEXT, part TEXT, type TEXT, value TEXT, pattern TEXT, shop TEXT, shopid TEXT);");
        qDebug() << query.lastError();
        sqlok &= query.exec("delete from import;");
        qDebug() << query.lastError();

        auto parts = root->findSubNodeByType("Schematic")[0]->findSubNodeByType("Components")[0]->findSubNodeByType("Part");

        for(int i = 0; i < parts.size(); i++)
        {
            QString refdes = parts[i]->getParameters()[1];
            QString value  = parts[i]->findSubNodeByType("Value")[0]->getParameters()[0];
            QString type   = parts[i]->findSubNodeByType("BaseName")[0]->getParameters()[0];
            QString part   = parts[i]->findSubNodeByType("PartName")[0]->getParameters()[0];
            QString pattern= parts[i]->findSubNodeByType("Pattern")[0]->getParameters()[0];
            QString shop;
            QString shopid;

            auto fields = parts[i]->findSubNodeByType("UserFields")[0]->findSubNodeByType("UserField");
            foreach (auto field, fields)
            {
                if ((field->getParameters().size() >= 2) && (field->getParameters()[0].toLower() == "shop"))
                {
                    shop = field->getParameters()[1];
                }
                if ((field->getParameters().size() >= 2) && (field->getParameters()[0].toLower() == "shopid"))
                {
                    shopid = field->getParameters()[1];
                }
                for(int j = 0; j < webShopScripts.size(); j++)
                {
                    if ((field->getParameters().size() >= 2) && (field->getParameters()[0].toLower() == webShopScripts[j].getName().toLower()))
                    {
                        shop = webShopScripts[j].getName();
                        shopid = field->getParameters()[1];
                    }
                }
            }

            sqlok &= query.exec(QString("insert into import (refdes, value, type, part, pattern, shop, shopid) VALUES (\"%1\",\"%2\",\"%3\",\"%4\",\"%5\", nullif(\"%6\", \"\"), nullif(\"%7\", \"\"));")
                       .arg(refdes)
                       .arg(value)
                       .arg(type)
                       .arg(part)
                       .arg(pattern)
                       .arg(shop)
                       .arg(shopid)
                       );

        }

        sqlok &= query.exec("update import set id = refdes || '.:.' || part");

        if (sqlok)
        {
            mergeImport();
        }
        else
        {
            QMessageBox::critical(this, trUtf8("Import error"), trUtf8("Import error"), QMessageBox::Ok);
        }

        updatePartList();

    }
}

void MainWindow::on_webShopSelect_activated(int index)
{
    Q_UNUSED(index);
    if (ui->partList->currentIndex().isValid())
    {
        QString url;
        QSqlRecord record = partListQueryModel.record(ui->partList->currentIndex().row());
        url = webShopScripts[ui->webShopSelect->currentIndex()].getSearchUrl(record.value("value").toString(), record.value("type").toString(), record.value("pattern").toString());
        ui->webView->load(QUrl(url));
    }
}

void MainWindow::on_partList_activated(const QModelIndex &index)
{
    QString url;
    QSqlRecord record = partListQueryModel.record(index.row());
    if (index.column() == record.indexOf("shopid"))
    {
        foreach (WebShopScript item, webShopScripts)
        {
            if (item.getName() == record.value("shop"))
            {
                url = item.getIdUrl(record.value("shopid").toString());
            }
        }
    }
    else
    {
        url = webShopScripts[ui->webShopSelect->currentIndex()].getSearchUrl(record.value("value").toString(), record.value("type").toString(), record.value("pattern").toString());
    }
    ui->webView->load(QUrl(url));
}

void MainWindow::updatePartList()
{
    if (ui->partListSelect->currentIndex() == 1)
    {
        partListQuery.exec("select distinct group_concat(refdes) RefDes, count(refdes) Quantity, type, pattern, value, shop, shopid from parts where refdes not like 'NetPort%' and part = (select asd.part from parts asd where asd.refdes = parts.refdes order by asd.part limit 1)  and (shopid == '' or shopid is null) group by type,value order by refdes, type, value;");
    }
    else if (ui->partListSelect->currentIndex() == 2)
    {
        partListQuery.exec("select 'deleted' Info, refdes, part, type, value, pattern from parts where id not in (select parts.id from parts inner join import on (parts.id = import.id)) "
                           "union "
                           "select 'new' Info, refdes, part, type, value, pattern from import where id not in (select parts.id from parts inner join import on (parts.id = import.id)) "
                           "union "
                           "select 'updated' Info, refdes, part, type, value, pattern from import where id not in (select parts.id from parts inner join import on (parts.id = import.id and parts.value = import.value and parts.pattern = import.pattern)) "
                           "except "
                           "select'updated' Info, refdes, part, type, value, pattern from import where id not in (select parts.id from parts inner join import on (parts.id = import.id or parts.value = import.value or parts.pattern = import.pattern)) ");
    }
    else
    {
        //partListQuery.exec("select distinct group_concat(refdes) RefDes, count(refdes) Quantity, type, pattern, value, shop, shopid from parts where refdes not like 'NetPort%' and part = (select asd.part from parts asd where asd.refdes = parts.refdes order by asd.part limit 1) group by type,value order by refdes, type, value;");
        partListQuery.exec("select distinct group_concat(refdes) RefDes, pattern, type, value, shopid, shop from parts where refdes not like 'NetPort%' and part = (select asd.part from parts asd where asd.refdes = parts.refdes order by asd.part limit 1) group by type,value order by refdes, type, value;");
    }

    partListQueryModel.setQuery(partListQuery);
    ui->partList->setModel(&partListQueryModel);
}

void MainWindow::on_partListSelect_activated(int index)
{
    Q_UNUSED(index);
    updatePartList();
}

void MainWindow::on_actionParts_group_by_shop_triggered()
{
    LoadForm lf(db);
    lf.setWindowTitle(trUtf8("Parts group by shop"));
    lf.setStandardButtons(QDialogButtonBox::NoButton);
    lf.setQuery("select shop, count(*) Quantity, group_concat(refdes) RefDes from parts where shop != '' and part = (select asd.part from parts asd where asd.refdes = parts.refdes order by asd.part limit 1) group by shop");
    lf.exec();
}

void MainWindow::on_actionFile_info_triggered()
{
    LoadForm lf(db);
    lf.setWindowTitle(trUtf8("File info"));
    lf.setStandardButtons(QDialogButtonBox::NoButton);
    lf.setQuery("select * from info");
    lf.exec();
}

void MainWindow::on_actionExport_to_CSV_triggered()
{
    exportToCsv("select * from parts");
}

void MainWindow::on_actionExport_to_CSV_grouped_triggered()
{
    exportToCsv("select distinct group_concat(refdes) RefDes, count(refdes) Quantity, type, pattern, value, shop, shopid from parts where refdes not like 'NetPort%' and part = (select asd.part from parts asd where asd.refdes = parts.refdes order by asd.part limit 1) group by type,value order by refdes, type, value;");
}

void MainWindow::exportToCsv(QString queryString)
{
    QFileDialog fd;
    fd.setWindowTitle(trUtf8("Export to CSV"));
    fd.setNameFilter("*.csv");
    fd.setDefaultSuffix("csv");
    fd.setAcceptMode(QFileDialog::AcceptSave);
    if (fd.exec())
    {
        QFile f(fd.selectedFiles()[0]);
        f.open(QFile::WriteOnly | QFile::Truncate);
        if (f.isOpen())
        {
            QTextStream s(&f);

            QSqlQuery query(db);
            query.exec(queryString);
            if (query.first())
            {
                for(int i = 0; i < query.record().count() - 1; i++)
                {
                    s << "\"" << query.record().fieldName(i) << "\", ";
                }
                s << "\"" << query.record().fieldName(query.record().count() - 1) << "\"" << endl;

                do
                {
                    for(int i = 0; i < query.record().count() - 1; i++)
                    {
                        s << "\"" << query.record().value(i).toString() << "\", ";
                    }
                    s << "\"" << query.record().value(query.record().count() - 1).toString() << "\"" << endl;
                }
                while (query.next());
            }

        }
        else
        {
            QMessageBox::warning(this, trUtf8("File error"), f.errorString(), QMessageBox::Ok);
        }
    }
}

void MainWindow::mergeImport()
{
    bool sqlok = true;
    QSqlQuery query(db);

    query.exec("BEGIN");
    LoadForm lf(db);
    lf.setWindowTitle("Import");
    lf.setQuery("select 'DELETE' Info, refdes, part, type, value, pattern from parts where id not in (select parts.id from parts inner join import on (parts.id = import.id)) "
                "union "
                "select 'INSERT' Info, refdes, part, type, value, pattern from import where id not in (select parts.id from parts inner join import on (parts.id = import.id)) "
                "union "
                "select 'UPDATE' Info, refdes, part, type, value, pattern from import where id not in (select parts.id from parts inner join import on (parts.id = import.id and parts.value = import.value and parts.pattern = import.pattern)) "
                "except "
                "select 'UPDATE' Info, refdes, part, type, value, pattern from import where id not in (select parts.id from parts inner join import on (parts.id = import.id or parts.value = import.value or parts.pattern = import.pattern)) "
                );
    if (lf.exec() == LoadForm::Accepted)
    {
        //TODO: osszefesulni nem csak beleirni
        sqlok &= query.exec("insert into parts select * from import where id not in (select parts.id from parts inner join import on (parts.id = import.id))");
        sqlok &= query.exec("delete from parts where id not in (select parts.id from parts inner join import on (parts.id = import.id))");
        sqlok &= query.exec("update parts set shop = NULL, shopid = NULL, type = (select type from import where id = parts.id), value = (select value from import where id = parts.id), pattern = (select pattern from import where id = parts.id) where id in (select id from import where id not in (select parts.id from parts inner join import on (parts.id = import.id and parts.value = import.value and parts.pattern = import.pattern)) except select id from import where id not in (select parts.id from parts inner join import on (parts.id = import.id or parts.value = import.value or parts.pattern = import.pattern)))");

        if (sqlok)
        {
            query.exec("COMMIT");
            query.exec("insert or replace into info (key, val) VALUES ('last load', CURRENT_TIMESTAMP);");
            QMessageBox::information(this, trUtf8("Import finished"), trUtf8("Import finished"), QMessageBox::Ok);
        }
        else
        {
            query.exec("ROLLBACK");
            QMessageBox::critical(this, trUtf8("Merge error"), trUtf8("Merge error"), QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::information(this, trUtf8("Import cancelled"), trUtf8("Import cancelled"), QMessageBox::Ok);
    }

}

bool MainWindow::checkImportDate(QString fn)
{
    QSqlQuery query(db);
    query.exec("select datetime(val, 'localtime') from info where key = 'last load';");
    if (query.first())
    {
        QDateTime lastload = query.value(0).toDateTime();
        QFileInfo fi(fn);
        if (lastload > fi.lastModified())
        {
            if (QMessageBox::warning(this, trUtf8("Import older file"), trUtf8("file last modified < last load\nLoad?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
            {
                return true;
            }
        }
    }
    return false;
}


void MainWindow::on_actionExport_to_CSV_by_shop_triggered()
{
    QSqlQuery query(db);
    query.exec("create table if not exists webshopscriptlist (name TEXT);");
    query.exec("delete from webshopscriptlist;");
    for(int i = 0; i < webShopScripts.size(); i++)
    {
        query.prepare("insert into webshopscriptlist (name) VALUES (:val)");
        query.bindValue(":val", webShopScripts[i].getName());
        query.exec();
    }
    query.exec("select shop from parts where shop != '' group by shop intersect select * from webshopscriptlist");

    QStringList shoplist;
    while (query.next())
    {
        shoplist << query.value(0).toString();
    }

    bool ok;
    QString shopname = QInputDialog::getItem(this, trUtf8("Shop"), trUtf8("Shop"), shoplist, 0, false, &ok);
    if (ok)
    {
        exportToCsv("select distinct group_concat(refdes) RefDes, count(refdes) Quantity, type, pattern, value, shop, shopid from parts where shop = '" + shopname + "' and refdes not like 'NetPort%' and part = (select asd.part from parts asd where asd.refdes = parts.refdes order by asd.part limit 1) group by type,value order by refdes, type, value;");
    }
}

void MainWindow::on_actionImport_from_CSV_triggered()
{
    QString fn = QFileDialog::getOpenFileName(this, trUtf8("Import from CSV"), QString(), "CSV (*.csv);;All files (*)");
    if (!fn.isEmpty())
    {
        if (checkImportDate(fn))
            return;

        QSqlQuery query(db);

        QFile f(fn);
        f.open(QFile::ReadOnly);

        qDebug() << f.isOpen();

        ImportCSV icsv(&f);
        if (icsv.exec() == ImportCSV::Accepted)
        {
            bool sqlok = true;

            sqlok &= query.exec("create table if not exists import (id TEXT, refdes TEXT, part TEXT, type TEXT, value TEXT, pattern TEXT, shop TEXT, shopid TEXT);");
            sqlok &= query.exec("delete from import;");

            for(int i = 0; i < icsv.getDataCount(); i++)
            {
                ImportCSV::Info info = icsv.getData(i);
                sqlok &= query.exec(QString("insert into import (refdes, value, type, part, pattern, shop, shopid) VALUES (\"%1\",\"%2\",\"%3\",\"%4\",\"%5\", nullif(\"%6\", \"\"), nullif(\"%7\", \"\"));")
                           .arg(info.refdes)
                           .arg(info.value)
                           .arg(info.type)
                           .arg(info.part)
                           .arg(info.pattern)
                           .arg(info.shop)
                           .arg(info.shopid)
                           );
            }

            sqlok &= query.exec("update import set id = refdes || '.:.' || part");

            if (sqlok)
            {
                mergeImport();
            }
            else
            {
                QMessageBox::critical(this, trUtf8("Import error"), trUtf8("Import error"), QMessageBox::Ok);
            }

            updatePartList();
        }

    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, trUtf8("About"),
                             trUtf8("<h3>About DtBom</h3><br/>"
                                    "Version: %1<br/>"
                                    "Created by RADICS Áron (raron)<br/>"
                                    "<a href=\"https://github.com/raron/dtbom/blob/master/LICENSE.md\">License: GNU GPLv2</a><br/>"
                                    "<a href=\"https://github.com/raron/dtbom\">Webpage: github.com/raron/dtbom</a><br/>"
                                    )
                             .arg(QApplication::applicationVersion()));
}
