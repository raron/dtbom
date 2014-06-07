#ifndef DTASCNODE_H
#define DTASCNODE_H

#include <QObject>
#include <QStringList>
#include <QDebug>
#include <QTreeWidgetItem>

//class DtAscNode : public QObject
class DtAscNode
{
    //Q_OBJECT
public:
    explicit DtAscNode(QObject *parent = 0);
    explicit DtAscNode(QStringList sl, QObject *parent = 0);
    ~DtAscNode();
    static DtAscNode* fromSubNodes(QList<DtAscNode *> sl);

    QString getType();
    QStringList getParameters();
    QList<DtAscNode *> getSubNodes();

    void print(int m = 0);

    void setType(QString t);
    void setParameters(QStringList l);

    void addSubNode(DtAscNode *n);
    void addSubNodes(QList<DtAscNode *> n);

    QList<DtAscNode *> findSubNodeByType(QString type);

    QTreeWidgetItem * toTreeWidgetItem();

signals:

public slots:

private:
    QList<DtAscNode *> subnodes;
    QString type;
    QStringList parameters;

};

QDebug operator<<(QDebug dbg, DtAscNode* n);

#endif // DTASCNODE_H
