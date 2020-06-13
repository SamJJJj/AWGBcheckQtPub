#include "AWGBCheckTool.h"

ShowTree::ShowTree(QObject * parent)
{
    ;
}

ShowTree::~ShowTree()
{

}

void ShowTree::setTreeView(QTreeView * tree)
{
    treeView = tree;
}

void ShowTree::run()
{
    QStandardItemModel *treeModel = new QStandardItemModel(treeView);    //设置不可修改
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //不设置会默显示一个1，需要修改
    treeModel->setHorizontalHeaderLabels(QStringList() << QStringLiteral("Id(设备名)"));
    QStandardItem* it1=new QStandardItem(QString("00000000000000000000"));
    QStandardItem* it2=new QStandardItem(QString("11111111111111111111"));
    QStandardItem* it3=new QStandardItem(QString("22222222222222222222"));
    treeModel->appendRow(it1);
    it1->appendRow(it2);
    it1->appendRow(it3);
    treeView->setModel(treeModel);
}


