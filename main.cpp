/**
    DtBom
    Copyright (C) 2014  RADICS Áron

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("DtBom");
    QApplication::setApplicationVersion(APP_VERSION);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    bool quit = false;

    do
    {
        QFileDialog fd;
        fd.setNameFilter("*.dtbom");
        fd.setDefaultSuffix("dtbom");
        fd.setAcceptMode(QFileDialog::AcceptSave);
        fd.setConfirmOverwrite(false);

        if (fd.exec())
        {
            db.setDatabaseName(fd.selectedFiles()[0]);
        }
        if (!db.open())
        {
            QMessageBox mbox;
            mbox.setWindowTitle(QMessageBox::trUtf8("File open / create failed."));
            mbox.setText(QMessageBox::trUtf8("File open / create failed.\nTry again?"));
            mbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            quit = mbox.exec() == QMessageBox::No;
        }
    }
    while( (!db.isOpen()) && (!quit) );


    if (!quit)
    {
        QSqlQuery query(db);
        query.exec("create table if not exists parts (id TEXT PRIMARY KEY, refdes TEXT, part TEXT, type TEXT, value TEXT, pattern TEXT, shop TEXT, shopid TEXT);");
        query.exec("create table if not exists info (key TEXT PRIMARY KEY, val TEXT)");
        query.exec("insert or replace into info (key, val) VALUES ('last open', CURRENT_TIMESTAMP);");
        query.exec("insert or ignore into info (key, val) VALUES ('created', CURRENT_TIMESTAMP);");
        query.exec("insert or ignore into info (key, val) VALUES ('db version', '" + QString::number(DB_VERSION) + "');");

        query.exec("select val from info where key = 'db version'");
        if (query.first())
        {
            if (query.value(0).toInt() < DB_VERSION)
            {
                // TODO
                qCritical("old db version");
            }
            if (query.value(0).toInt() > DB_VERSION)
            {
                // TODO
                qCritical("new db version");
            }
        }

        MainWindow w(db);
        w.show();
        return a.exec();
    }
    return EXIT_FAILURE;
}
