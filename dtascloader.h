#ifndef DTASCLOADER_H
#define DTASCLOADER_H

#include <QObject>
#include <QFile>
#include <QStringList>
#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QApplication>

#include "dtascnode.h"

class DtAscLoader : public QObject
{
    Q_OBJECT
public:
    explicit DtAscLoader(QObject *parent = 0);
    DtAscLoader(QString str, QObject *parent = 0);
    DtAscLoader(QFile *f, QObject *parent = 0);
    QList<DtAscNode *> load();

signals:
    void loadProgress(int val);

public slots:

private:
    QString s;
    int maxprogress;
    int progress;

    QList<DtAscNode *> loadPriv(QString s);
    QStringList parseLine(QString line);


};

#endif // DTASCLOADER_H
