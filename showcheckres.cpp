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

void ShowCheckResThread::setTableView(QStandardItemModel * table, int h)
{
    checkResModel = table;
    handle = h;
}

void ShowCheckResThread::run()
{
//    QStandardItemModel *checkResModel = new QStandardItemModel();
//    checkResModel->moveToThread(tableView->thread());
//    checkResModel->setParent(tableView);
    checkResModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("设备ID")));
    checkResModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("设备类型")));
    checkResModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("信令类型")));
    checkResModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("状态")));
//    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    unsigned char * bufRecv = (unsigned char *)calloc(2048, sizeof(char));
    unsigned int len = 2048;
//    unsigned int len1 = 2048;
    int ret;
    while(1)
    {
        Sleep(500);
//        unsigned char *bufRecv = NULL;
//        unsigned int len;
//        AW_BSQueue_GetBuffer(handle, bufRecv, &len);
//        bufRecv = (unsigned char*)malloc(len + 1);
        ret = AW_BSQueue_GetBuffer(handle, bufRecv, &len);
//        memset(bufRecv, 0, 2048);
//        ret = AW_BSQueue_GetBuffer(handle, bufRecv, &len1);
        if(!ret)
            cout << "len" << len << endl;
//        cout << "len1" << len1 << endl;
        bufRecv[len] = 0;
        memset(bufRecv, 0, 2048);
        emit showCheck();
//        checkResModel->setItem(0,1, new QStandardItem(QString((char*)bufRecv)));
    }
    free(bufRecv);
//    tableView->setModel(checkResModel);
    //取数据显示。。
}
