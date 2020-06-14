#include "AWGBCheckTool.h"
#include "AWQueue.h"
#include <iostream>

using namespace std;
GetAndParseThread::GetAndParseThread(QObject *)
{

}

GetAndParseThread::~GetAndParseThread()
{

}

void GetAndParseThread::init(QStandardItemModel* tree, QStandardItemModel *table, int h)
{
    treeModel = tree;
    checkResModel = table;
    handle = h;
    tableId = 1;
}



void GetAndParseThread::run()
{
    unsigned char buf[2048] = {0};
    int ret = -1;
    unsigned int len = INT_MAX;
    while(1)
    {
        Sleep(500);
        ret = AW_BSQueue_GetBuffer(handle, buf, &len);
        if(!ret)
        {
            cout << len << endl;
            //解析发来的数据，放进结构体，setModel, 发信号
        }
    }
}
