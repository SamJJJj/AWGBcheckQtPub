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
        Sleep(500);
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
                sipMessages->append(text);
                text.clear();
                str.clear();
                list = doc.elementsByTagName("DeviceId");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                checkResModel->setItem(tableId, 0, new QStandardItem(text));
                text.clear();
                list = doc.elementsByTagName("DeviceType");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                checkResModel->setItem(tableId, 1, new QStandardItem(text));
                text.clear();
                list = doc.elementsByTagName("SipType");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                checkResModel->setItem(tableId, 2, new QStandardItem(text));
                text.clear();
                list = doc.elementsByTagName("State");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                checkResModel->setItem(tableId, 3, new QStandardItem(text));
                ++tableId;
                emit toText();
            }
            else if(str == QString("DeviceTree"))
            {
                emit toTree();
            }
            memset(buf, 0, 2048);
            len = INT_MAX;
        }
    }
}
