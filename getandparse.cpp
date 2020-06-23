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
//            if(1)
        {
//            cout << len << endl;
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
//                else if(1)
            {
                QString strID;
                QTextStream streamID(&strID);
                QString strName;
                QTextStream streamName(&strName);
                QDomNodeList list2 = doc.elementsByTagName("DeviceID");
                int deCutBf = list2.size();
                int deCount =0;
                while (deCutBf < deCount) {
                QDomElement list3 = list2.at(deCutBf).toElement();
                QDomNodeList list4 = list3.elementsByTagName("NAME");
                QDomNodeList list5 = list3.elementsByTagName("ID");
                list5.at(0).toElement().childNodes().at(0).toText().save(streamID, 0);
                list4.at(0).toElement().childNodes().at(0).toText().save(streamName, 0);
                QString item = strID+"("+strName+")";
                QStandardItem* itemProject = new QStandardItem((item));
                treeModel->appendRow(itemProject);
                strID.clear();
                strName.clear();
                item.clear();
                QDomNodeList deviceChannel = doc.elementsByTagName("ChannelID");
                int deCut = deviceChannel.size();
                int count =0;
                while (count<deCut){
                    QDomElement deviceCh = deviceChannel.at(count).toElement();
                    QDomNodeList list9 = deviceCh.elementsByTagName("NAME");
                    QDomNodeList list10 = deviceCh.elementsByTagName("ID");
                    list10.at(0).toElement().childNodes().at(0).toText().save(streamID, 0);
                    list9.at(0).toElement().childNodes().at(0).toText().save(streamName, 0);
                    item = strID+"("+strName+")";
                    QStandardItem* itemProject = new QStandardItem((item));
                    itemProject->appendRow(itemProject);
                    strID.clear();
                    strName.clear();
                    item.clear();
                    count ++;
                }
                deCutBf ++;

                }
                emit toTree();
                str.clear();
            }

//            sipMessages->append();
//            checkResModel->setItemData();
            memset(buf, 0, 2048);
            len = INT_MAX;
        }
    }
}
