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
    Sleep(500);
    QStandardItemModel *checkResModel = new QStandardItemModel(tableView);
    checkResModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("设备ID")));
    checkResModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("设备类型")));
    checkResModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("信令类型")));
    checkResModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("状态")));
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    unsigned char buf[1024] = {0};
    unsigned int len;
    AW_BSQueue_GetBuffer(handle, buf, &len);
    cout << "get:" << len << endl;
    checkResModel->setItem(0,1, new QStandardItem(QString((char*)buf)));
    tableView->setModel(checkResModel);
    //取数据显示。。
}
