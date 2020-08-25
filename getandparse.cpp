#pragma execution_character_set(“utf-8”)
#include "AWGBCheckTool.h"
#include "AWQueue.h"
#include <iostream>
#include <QTextStream>
#include <QtXml/QDomComment>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>
#include <QTextCodec>
extern "C"{
#include <iconv.h>
}

using namespace std;

static int code_convert(char *from_charset, char *to_charset, unsigned char *inbuf, unsigned int inlen, unsigned char *outbuf, unsigned int *outlen)
{
    iconv_t cd;
    unsigned char **pin = &inbuf;
    unsigned char **pout = &outbuf;
    cd = iconv_open(to_charset, from_charset);
    if (cd == 0)
        return -1;

    memset(outbuf, 0, *outlen);

    if (iconv(cd, (char **)pin, &inlen, (char **)pout, outlen) == -1)
        return -1;

    iconv_close(cd);

    return 0;
}

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

void GetAndParseThread::init(QStandardItemModel* tree, QStandardItemModel *table, int h, QString* sipMessage, QString* i, QString* p, QString *pm, QString *id,QStandardItemModel* file)
{
    treeModel = tree;
    checkResModel = table;
    handle = h;
    tableId = 0;
    fileId = 0;
    sipMessages = sipMessage;
    ip = i;
    port = p;
    pushMethod = pm;
    pushId = id;
    recordFileModel = file;
}



void GetAndParseThread::run()
{
    unsigned char getBuf[2048*4] = {0};
    unsigned char utfBuf[2048*4] = {0};
    unsigned int utfLen = 2048*4;
    int ret = -1;
    unsigned int len = INT_MAX;
    QString str;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextStream streamJudge(&str);
    while(1)
    {
        bool flag = true;
        Sleep(100);
        ret = AW_BSQueue_GetBuffer(handle, NULL, &len);
        if(!ret)
        {
            //解析发来的数据，放进结构体, 发信号
            ret = AW_BSQueue_GetBuffer(handle, getBuf, &len);
            code_convert("gb2312", "utf-8", getBuf, len, utfBuf, &utfLen);
            QDomDocument doc;
            doc.setContent(QString((char*)utfBuf));
            QDomNodeList list = doc.elementsByTagName("CmdType");
            if(list.at(0).toElement().childNodes().at(0).toText().save(streamJudge, 0), str == QString("SipShowBuffer"))
            {
                QString text;
                QTextStream streamText(&text);
                list = doc.elementsByTagName("Buffer");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                *sipMessages = text;
                sipMessages->replace("&lt;", "<");
                sipMessages->replace("&#xd;", "");
                *sipMessages = sipMessages->toLocal8Bit();
                //                *sipMessages = codec->toUnicode(sipMessages->toLocal8Bit());//转成utf-8
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
                qInfo() << "Get sipshowbuffer";
                emit toText();
            }
            else if(str == QString("DeviceTree"))
            {
                treeModel->clear();
                treeModel->setHorizontalHeaderLabels(QStringList() << QStringLiteral("Id(设备名)"));

                QDomNodeList devices = doc.elementsByTagName("layer1");
                int devNum = devices.size();
                int devCur = 0;
                while(devCur < devNum)
                {
                    QDomElement devElem = devices.at(devCur).toElement();
                    QDomNodeList devId = devElem.elementsByTagName("ID");
                    QDomNodeList devName = devElem.elementsByTagName("Name");
                    QStandardItem* devItem = new QStandardItem(devId.at(0).toText().data() + "(" +devName.at(0).toText().data() + ")");
                    QDomNodeList province =doc.elementsByTagName("layer2");
                    int provNum = province.size();
                    int provCur = 0;
                    if(provNum == 0)
                        break;
                    while(provCur < provNum)
                    {
                        QDomElement provElem = province.at(provCur).toElement();
                        QDomNodeList provCode = provElem.elementsByTagName("Code");
                        QDomNodeList provName = provElem.elementsByTagName("Name");
                        QDomNodeList city = provElem.elementsByTagName("layer3");
                        QStandardItem* provItem = new QStandardItem(provCode.at(0).toText().data() + "(" + provName.at(0).toText().data() + ")");
                        int cityNum = city.size();
                        int cityCur = 0;
                        if(!cityNum)
                            break;
                        while(cityCur < cityNum)
                        {
                            QDomElement cityElem = city.at(cityCur).toElement();
                            QDomNodeList cityCode = cityElem.elementsByTagName("Code");
                            QDomNodeList cityName = cityElem.elementsByTagName("Name");
                            QDomNodeList county = cityElem.elementsByTagName("layer4");
                            QStandardItem* cityItem = new QStandardItem(cityCode.at(0).toText().data()+ "(" + cityName.at(0).toText().data() + ")");
                            int countyNum = county.size();
                            if(!countyNum)
                                break;
                            int countyCur = 0;
                            while(countyCur < countyNum)
                            {
                                QDomElement countyElem = county.at(countyCur).toElement();
                                QDomNodeList countyCode = countyElem.elementsByTagName("Code");
                                QDomNodeList countyName = countyElem.elementsByTagName("Name");
                                QDomNodeList unit = countyElem.elementsByTagName("layer5");
                                QStandardItem* countyItem = new QStandardItem(countyCode.at(0).toText().data()+ "(" + countyName.at(0).toText().data() + ")");
                                int unitNum = unit.size();
                                int unitCur = 0;
                                while(unitCur < unitNum){
                                    QDomElement unitElem = unit.at(unitCur).toElement();
                                    QDomNodeList unitCode = unitElem.elementsByTagName("Code");
                                    QDomNodeList unitName = unitElem.elementsByTagName("Name");
                                    QDomNodeList channel = unitElem.elementsByTagName("layer6");
                                    QStandardItem* unitItem = new QStandardItem(unitCode.at(0).toText().data() + "("+ unitName.at(0).toText().data()+")");
                                    int chNum = channel.size();
                                    int chCur = 0;
                                    while(chCur < chNum)
                                    {
                                        QDomElement chElem = channel.at(chCur).toElement();
                                        QDomNodeList chId = chElem.elementsByTagName("ID");
                                        QDomNodeList chName = chElem.elementsByTagName("Name");
                                        QStandardItem* channelItem = new QStandardItem(chId.at(0).toText().data() + "(" + chName.at(0).toText().data() + ")");
                                        ++chCur;
                                        unitItem->appendRow(channelItem);
                                    }
                                    ++unitCur;
                                    countyItem->appendRow(unitItem);
                                }
                                ++countyCur;
                                cityItem->appendRow(countyItem);
                            }
                            ++cityCur;
                            provItem->appendRow(cityItem);
                        }
                        ++provCur;
                        devItem->appendRow(provItem);
                    }
                    ++devCur;
                    treeModel->appendRow(devItem);
                }


//                                QString strID;
//                                QTextStream streamID(&strID);
//                                QString strName;
//                                QTextStream streamName(&strName);
//                                QDomNodeList list2 = doc.elementsByTagName("DeviceID");
//                                int deCutBf = list2.size();
//                                int deCount = 0;
//                                while (deCount < deCutBf) {
//                                    QDomElement list3 = list2.at(deCount).toElement();
//                                    QDomNodeList list5 = list3.elementsByTagName("ID");
//                                    list5.at(0).toElement().childNodes().at(0).toText().save(streamID, 0);
//                                    QString item = strID;
//                                    QStandardItem* itemC = new QStandardItem((item.toLocal8Bit().data()));
//                                    strID.clear();
//                                    strName.clear();
//                                    item.clear();
//                                    QDomNodeList deviceChannel = list3.elementsByTagName("ChannelID");
//                                    int deCut = deviceChannel.size();
//                                    int count =0;
//                                    while (count<deCut){
//                                        QDomElement deviceCh = deviceChannel.at(count).toElement();
//                                        QDomNodeList list9 = deviceCh.elementsByTagName("Name");
//                                        QDomNodeList list10 = deviceCh.elementsByTagName("ID");
//                                        list10.at(0).toElement().childNodes().at(0).toText().save(streamID, 0);
//                                        list9.at(0).toElement().childNodes().at(0).toText().save(streamName, 0);
//                                        strName = list9.at(0).toElement().childNodes().at(0).toText().data();
//                                        item = strID+"("+strName+")";
//                                        QStandardItem* itemProject = new QStandardItem(QString(item.toLocal8Bit()));
//                                        itemC->appendRow(itemProject);
//                                        strID.clear();
//                                        strName.clear();
//                                        item.clear();
//                                        count ++;
//                                    }
//                                    deCount ++;
//                                    treeModel->appendRow(itemC);
//                                }
                qInfo() << "Get DeviceTree";
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
                qInfo() << "Get PushStream";
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

                list = doc.elementsByTagName("Bvkek");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                bvkek = text;
                text.clear();

                list = doc.elementsByTagName("Keyversion");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                keyVersion = text;
                text.clear();

                list = doc.elementsByTagName("Bpubkey");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                bPubKey = text;
                //                std::cout << bPubKey.toStdString() << std::endl;
                text.clear();

                list = doc.elementsByTagName("Method");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                qInfo() << "Get PullStream";
                if(method == QString("UDP"))
                    emit UDP();
                else if(method == QString("TCP"))
                {
                    emit TCP();

                }
                else
                    emit TCPActive();
                text.clear();
                str.clear();
            }
            else if(str == "RecordInfo"){
                QString text;
                QTextStream streamText(&text);
                list = doc.elementsByTagName("FilePath");
                QDomNodeList list1 = doc.elementsByTagName("StartTime");
                QDomNodeList list2 = doc.elementsByTagName("EndTime");
                QDomNodeList list3 = doc.elementsByTagName("DeviceID");
                list.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                recordFileModel->setItem(fileId, 0, new QStandardItem(text));
                text.clear();
                list3.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                recordFileModel->setItem(fileId, 1, new QStandardItem(text));
                text.clear();
                list1.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                recordFileModel->setItem(fileId, 2, new QStandardItem(text));
                text.clear();
                list2.at(0).toElement().childNodes().at(0).toText().save(streamText, 0);
                recordFileModel->setItem(fileId, 3, new QStandardItem(text));
                ++fileId;

                text.clear();
                str.clear();
            }
            else if(str == "RegisterOK")
            {
                emit registerOk();
            }
            else if(str == "UnRegisterOK")
            {
                emit unRegisterOk();
            }
            else if(str == "StopPush")
            {
                emit stopPush();
            }

            memset(getBuf, 0, 2048);
            memset(utfBuf, 0, 2048);
            len = INT_MAX;
            utfLen = 2048;
        }
    }
}

void GetAndParseThread::clearList()
{
    tableId = 0;
}

void GetAndParseThread::clearRecordFile()
{
    fileId = 0;
}
