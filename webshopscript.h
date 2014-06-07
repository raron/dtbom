#ifndef WEBSHOPSCRIPT_H
#define WEBSHOPSCRIPT_H

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QList>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QScriptEngine>
#include <QMainWindow>
#include <QLocale>
#include <QUrl>

class WebShopScript
{
public:
    WebShopScript(QString filename);
    ~WebShopScript();
    QString getName();
    QString getVersion();
    QString getSearchUrl(QString value, QString type, QString pattern);
    QString getIdUrl(QString id);
    QString getSearchScript();

    static QList<WebShopScript> fromDir(QString dirname);
private:
    QString name;
    QString version;
    QString searchUrlScript;
    QString searchIdScript;
    QString searchscript;
};

#endif // WEBSHOPSCRIPT_H
