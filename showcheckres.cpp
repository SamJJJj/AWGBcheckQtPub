#include "AWGBCheckTool.h"

ShowCheckResThread::ShowCheckResThread(QObject * parent)
{
    ;
}

ShowCheckResThread::~ShowCheckResThread()
{
    ;
}

void ShowCheckResThread::setTableView(QTableView * table)
{
    tableView = table;
}

void ShowCheckResThread::run()
{
    QStandardItemModel *checkResModel = new QStandardItemModel(tableView);
    checkResModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("设备ID")));
    checkResModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("设备类型")));
    checkResModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("信令类型")));
    checkResModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("状态")));
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i = 0; i < 10; ++i)
    {
        tableView->setModel(checkResModel);
        checkResModel->setItem(i,0, new QStandardItem("ss"));
        Sleep(500);
    }
    //取数据显示。。
}
