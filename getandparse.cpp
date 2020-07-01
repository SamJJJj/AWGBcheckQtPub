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

void GetAndParseThread::setMethod(const QString & m)
{
    method = m;
}

void GetAndParseThread::init(QStandardItemModel* tree, QStandardItemModel *table, int h, QString* sipMessage, QString* i, QString* p, QString *pm, QString *id)
{
    treeModel = tree;
    checkResModel = table;
    handle = h;
    tableId = 0;
    sipMessages = sipMessage;
    ip = i;
    port = p;
    pushMethod = pm;
    pushId = id;
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
        bool flag = true;
        Sleep(100);
        ret = AW_BSQueue_GetBuffer(handle, buf, &len);
        if(!ret)
        {
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
                *sipMessages = text;
                text.clear();
                str.clear();
                list = doc.elementsByTagName("DeviceId");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                if(text.isEmpty())
                {
                    flag = false;
                }
                checkResModel->setItem(tableId, 0, new QStandardItem(text));
                text.clear();
                list = doc.elementsByTagName("DeviceType");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                if(text.isEmpty())
                {
                    flag = false;
                }
                checkResModel->setItem(tableId, 1, new QStandardItem(text));
                text.clear();
                list = doc.elementsByTagName("SipType");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                if(text.isEmpty())
                {
                    flag = false;
                }
                checkResModel->setItem(tableId, 2, new QStandardItem(text));
                text.clear();
                list = doc.elementsByTagName("State");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                if(text.isEmpty())
                {
                    flag = false;
                }
                checkResModel->setItem(tableId, 3, new QStandardItem(text));
                if(!flag)
                {
//                    emit toTable();
                    checkResModel->setItem(tableId, 0, new QStandardItem(""));
                    checkResModel->setItem(tableId, 1, new QStandardItem(""));
                    checkResModel->setItem(tableId, 2, new QStandardItem(""));
                    checkResModel->setItem(tableId, 3, new QStandardItem(""));
//                    ++tableId;
                    //cout << tableId << endl;
                }else{
                    ++tableId;
                    //cout << tableId << endl;
                }

                emit toText();
            }
            else if(str == QString("DeviceTree"))
            {
                treeModel->clear();
                treeModel->setHorizontalHeaderLabels(QStringList() << QStringLiteral("Id(设备名)"));
                QString strID;
                QTextStream streamID(&strID);
                QString strName;
                QTextStream streamName(&strName);
                QDomNodeList list2 = doc.elementsByTagName("DeviceID");
                int deCutBf = list2.size();
                int deCount =0;
                while (deCount < deCutBf) {
                QDomElement list3 = list2.at(deCount).toElement();
//                QDomNodeList list4 = list3.elementsByTagName("NAME");
                QDomNodeList list5 = list3.elementsByTagName("ID");
                list5.at(0).toElement().childNodes().at(0).toText().save(streamID, 0);
//                list4.at(0).toElement().childNodes().at(0).toText().save(streamName, 0);
                QString item = strID;
//                QString item = strID+"("+strName+")";

                QStandardItem* itemC = new QStandardItem((item));
//                treeModel->appendRow(itemProject);
                strID.clear();
                strName.clear();
                item.clear();
                QDomNodeList deviceChannel = list3.elementsByTagName("ChannelID");
                int deCut = deviceChannel.size();
                int count =0;
                while (count<deCut){
                    QDomElement deviceCh = deviceChannel.at(count).toElement();
                    QDomNodeList list9 = deviceCh.elementsByTagName("Name");
                    QDomNodeList list10 = deviceCh.elementsByTagName("ID");
                    list10.at(0).toElement().childNodes().at(0).toText().save(streamID, 0);
                    list9.at(0).toElement().childNodes().at(0).toText().save(streamName, 0);
                    item = strID+"("+strName+")";
                    QStandardItem* itemProject = new QStandardItem((item));
                    itemC->appendRow(itemProject);
                    strID.clear();
                    strName.clear();
                    item.clear();
                    count ++;
                }
                deCount ++;
                treeModel->appendRow(itemC);
                }
                emit toTree();
                str.clear();

            }
            else if(str == QString("PushStream"))
            {
                QString text;
                QTextStream streamText(&text);
                list = doc.elementsByTagName("DeviceID");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                *pushId = text;
                text.clear();

                list = doc.elementsByTagName("DevicePort");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                *port = text;
                text.clear();

                list = doc.elementsByTagName("DeviceIP");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                *ip = text;
                text.clear();

                list = doc.elementsByTagName("PushMethod");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                *pushMethod = text;
                text.clear();
                emit push();
                str.clear();
            }
            else if(str == QString("PullStream"))
            {
                QString text;
                QTextStream streamText(&text);
                list = doc.elementsByTagName("DeviceID");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                text.clear();

                list = doc.elementsByTagName("DeviceIP");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                *ip = text;
                text.clear();

                list = doc.elementsByTagName("DevicePort");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                *port = text;
                text.clear();

                list = doc.elementsByTagName("Method");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                if(method == QString("UDP"))
                    emit UDP();
                else if(method == QString("TCP"))
                {
                    emit TCP();
                    cout << "TCP play" << endl;
                }
                else
                    emit TCPActive();
                text.clear();
                str.clear();
            }
            cout << "AW_BSQueue_GetBuffer"<< endl;
            cout << len << endl;
            memset(buf, 0, 2048);
            len = INT_MAX;
        }
    }
}

void GetAndParseThread::clearList()
{
    tableId = 0;
}
