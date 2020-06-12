#include "AWGBCheckTool.h"
#include "AWQueue.h"
#include <iostream>

using namespace std;

ShowCheckResThread::ShowCheckResThread(QObject * parent)
{
    ;
}

ShowCheckResThread::~ShowCheckResThread()
{
    ;
}

void ShowCheckResThread::setTableView(QTableView * table, int h)
{
    tableView = table;
    handle = h;
}

void ShowCheckResThread::run()
{
    QStandardItemModel *checkResModel = new QStandardItemModel(tableView);
//    QStandardItemModel *checkResModel = new QStandardItemModel();
//    checkResModel->moveToThread(tableView->thread());
//    checkResModel->setParent(tableView);
    checkResModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("设备ID")));
    checkResModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("设备类型")));
    checkResModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("信令类型")));
    checkResModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("状态")));
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    while(1)
    {
        Sleep(1000);
        unsigned char *bufRecv = NULL;
        unsigned int len;
        AW_BSQueue_GetBuffer(handle, bufRecv, &len);
        cout << "get:" << len << endl;
//        checkResModel->setItem(0,1, new QStandardItem(QString((char*)bufRecv)));
    }
//    tableView->setModel(checkResModel);
    //取数据显示。。
}
