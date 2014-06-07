#include "dtascnode.h"

DtAscNode::DtAscNode(QObject *parent)
{
}

DtAscNode::DtAscNode(QStringList sl, QObject *parent)
{
    if (sl.size() > 0)
    {
        type = sl[0];
        sl.removeFirst();
        parameters = sl;
    }
}

DtAscNode::~DtAscNode()
{
    while (subnodes.size() > 0)
    {
        delete subnodes.first();
        subnodes.removeFirst();
    }
}

DtAscNode *DtAscNode::fromSubNodes(QList<DtAscNode *> sl)
{
    DtAscNode *node = new DtAscNode;
    node->setType("<UNDEF>");
    node->addSubNodes(sl);
    return node;
}

QString DtAscNode::getType()
{
    return type;
}

QStringList DtAscNode::getParameters()
{
    return parameters;
}

QList<DtAscNode *> DtAscNode::getSubNodes()
{
    return subnodes;
}

void DtAscNode::setType(QString t)
{
    type = t;
}

void DtAscNode::setParameters(QStringList l)
{
    parameters = l;
}

void DtAscNode::addSubNode(DtAscNode *n)
{
    subnodes << n;
}

void DtAscNode::addSubNodes(QList<DtAscNode *> n)
{
    foreach (DtAscNode *node, n)
    {
        subnodes << node;
    }
}

QList<DtAscNode *> DtAscNode::findSubNodeByType(QString type)
{
    QList<DtAscNode *> list;
    foreach (DtAscNode *node, subnodes)
    {
        if (node->type == type)
            list << node;
    }
    return list;
}

QTreeWidgetItem *DtAscNode::toTreeWidgetItem()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << type << parameters);
    foreach (DtAscNode *node, subnodes)
    {
        item->addChild(node->toTreeWidgetItem());
    }
    return item;
}

QDebug operator<<(QDebug dbg, DtAscNode* n)
{
    dbg.nospace() << "("
                  << n->getType() << ": "
                  << n->getSubNodes().size() << ")";

    return dbg.space();
}
