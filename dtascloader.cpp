#include "dtascloader.h"

DtAscLoader::DtAscLoader(QObject *parent) :
    QObject(parent)
{
}

DtAscLoader::DtAscLoader(QString str, QObject *parent)
{
    s = str;
    load();
}

DtAscLoader::DtAscLoader(QFile *f, QObject *parent)
{
    s = QString::fromLocal8Bit(f->readAll());
    load();
}

QList<DtAscNode *> DtAscLoader::load()
{
    maxprogress = s.count("(");
    progress = 0;
    return loadPriv(s);
}

QList<DtAscNode *> DtAscLoader::loadPriv(QString s)
{
    QList<DtAscNode *> lista;

    int i = 0;
    while( ((i = s.indexOf("(", i)) != -1) && (i < s.length()))
    {
        emit loadProgress((double)progress/maxprogress*100);
        QApplication::processEvents();
        progress++;

        int j = i;
        int k = 0;
        bool inStr = false;
        while(!((s[j] == ')') && (k == 1)) && (j < s.length()))
        {
            if (!inStr)
            {
                if (s[j] == '(')
                    k++;
                if (s[j] == ')')
                    k--;
            }
            if (s[j] == '"')
                inStr = ! inStr;
            j++;
        }


        QString ss = s.mid(i+1, j-i-1);
        if (ss.contains("("))
        {
            DtAscNode *item = new DtAscNode(parseLine(ss.left(ss.indexOf("(")).simplified()));
            item->addSubNodes(loadPriv(ss));
            lista << item;
        }
        else
        {
            DtAscNode *item = new DtAscNode(parseLine(ss.simplified()));
            lista << item;
        }

        i = j;
    }

    return lista;
}

QStringList DtAscLoader::parseLine(QString line)
{
    QStringList list;
    QStringList tmpList = line.split(QRegExp("\"")); // Split by "
    bool inside = false;
    list.clear();
    foreach (QString s, tmpList) {
    if (inside) { // If 's' is inside quotes ...
    list.append(s); // ... get the whole string
    } else { // If 's' is outside quotes ...
    list.append(s.split(QRegExp("\\s+"), QString::SkipEmptyParts)); // ... get the splitted string
    }
    inside = !inside;
    }
    return list;
}
