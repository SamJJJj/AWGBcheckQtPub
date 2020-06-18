#include "AWGBCheckTool.h"
#include "AWQueue.h"
#include <iostream>
#include <QTextStream>
#include <QtXml/QDomComment>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>

using namespace std;
GetAndParseThread::GetAndParseThread(QObject *)
{

}

GetAndParseThread::~GetAndParseThread()
{

}

void GetAndParseThread::init(QStandardItemModel* tree, QStandardItemModel *table, int h, QVector<QString>* sipMessage)
{
    treeModel = tree;
    checkResModel = table;
    handle = h;
    tableId = 1;
    sipMessages = sipMessage;
}



void GetAndParseThread::run()
{
    unsigned char buf[2048] = {0};
    int ret = -1;
    unsigned int len = INT_MAX;
    QString str;
    QTextStream streamJudge(&str);
    while(1)
    {
        Sleep(1000);
        ret = AW_BSQueue_GetBuffer(handle, buf, &len);
        if(!ret)
        {
            cout << len << endl;
            //解析发来的数据，放进结构体，setModel, 发信号
            QDomDocument doc;
            doc.setContent(QString((char*)buf));
            QDomNodeList list = doc.elementsByTagName("CmdType");
            if(list.at(0).toElement().childNodes().at(0).toText().save(streamJudge, 0), str == QString("SipShowBuffer"))
            {
                QString text;
                QTextStream streamText(&text);
                list = doc.elementsByTagName("Buffer");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
//                QDomNodeList list1 = doc.elementsByTagName("CheckKey");
//                QString textKey;
//                QTextStream streamKey(&textKey);
//                list1.at(0).toElement().childNodes().at(0).toText().save(streamKey, 0);
                sipMessages->append(text);
                str.clear();
                emit toText();
//                emit toTable();
            }
            else if(str == QString("DeviceTree"))
            {
                emit toTree();
            }

//            sipMessages->append();
//            checkResModel->setItemData();
            memset(buf, 0, 2048);
            len = INT_MAX;
        }
    }
}
