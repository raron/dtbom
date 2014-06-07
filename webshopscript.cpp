#include "webshopscript.h"

WebShopScript::WebShopScript(QString filename) :
    name("<UNDEF>")
{
    qDebug() << "WebShopScript loading:" << filename;

    QSettings s(filename, QSettings::IniFormat);
    name = s.value("name", "<UNDEF>").toString();
    version = s.value("version", "<UNDEF>").toString();
    searchUrlScript = s.value("searchUrlScript").toString();
    searchIdScript = s.value("searchIdScript").toString();
    searchscript = s.value("searchScript").toString();

    qDebug() << "WebShopScript  loaded:" << name << "ver.:" << version;

    /*
    QFile f(filename);
    f.open(QFile::ReadOnly);
    QTextStream s(&f);
    while(!s.atEnd())
    {
        QString line = s.readLine();

        if (line.startsWith("//name:"))
        {
            name = line.remove("//name:").trimmed();
        }
        else if (line.startsWith("//searchUrl:"))
        {
            searchurl = line.remove("//searchUrl:").trimmed();
            if (idurl.isEmpty())
            {
                idurl = searchurl;
                idurl.replace("%t", "%id");
            }
        }
        else if (line.startsWith("//idUrl:"))
        {
            idurl = line.remove("//idUrl:").trimmed();
        }
    }
    s.seek(0);
    searchscript = s.readAll();*/
}

WebShopScript::~WebShopScript()
{
}

QString WebShopScript::getName()
{
    return name;
}

QString WebShopScript::getVersion()
{
    return version;
}

QString WebShopScript::getSearchUrl(QString value, QString type, QString pattern)
{
    QScriptEngine e;
    e.globalObject().setProperty("dtBomLocale", QLocale::system().name());
    e.globalObject().setProperty("dtBomValue", value);
    e.globalObject().setProperty("dtBomType", type);
    e.globalObject().setProperty("dtBomPattern", pattern);

    e.evaluate(searchUrlScript);

    QString ret = e.globalObject().property("ret").toString();
    qDebug() << "WebShopScript::getSearchUrl:" << ret;

    return ret;
}

QString WebShopScript::getIdUrl(QString id)
{
    QScriptEngine e;
    e.globalObject().setProperty("dtBomLocale", QLocale::system().name());
    e.globalObject().setProperty("dtBomId", id);

    e.evaluate(searchIdScript);

    QString ret = e.globalObject().property("ret").toString();
    qDebug() << "WebShopScript::getIdUrl:" << ret;

    return ret;
}

QString WebShopScript::getSearchScript()
{
    return searchscript;
}

QList<WebShopScript> WebShopScript::fromDir(QString dirname)
{
    QList<WebShopScript> scriptlist;

    QDir dir(dirname);
    dir.setFilter(QDir::Files | QDir::Readable);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        if (list[i].fileName().endsWith(".dtboms"))
        scriptlist << WebShopScript(list[i].filePath());
    }
    return scriptlist;
}

