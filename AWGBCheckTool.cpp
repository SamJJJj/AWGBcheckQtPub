#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include "AWQueue.h"
#include <QListWidget>
#include <QComboBox>
#include <iostream>
#include <regex>
#include <QMessageBox>
#include <QThread>
#include <QTextBrowser>
#include <QTextBlock>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QProcess>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMetaType>
#include <QCoreApplication>
using namespace std;


AWGBCheckTool::AWGBCheckTool(QWidget *parent, pGBStart_s param, int h)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    seq = 0;
    AWGBCheckTool::SetList(param);
    currGBInfo = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    memcpy(currGBInfo, param, sizeof(GBStart_s));
    if(currGBInfo->modeType <= 3 || currGBInfo->modeType == 6)
    {
        ui.pushButton_8->setEnabled(false);
        ui.stackedWidget_2->setCurrentIndex(0);
    }
    handle = h;
    sipMessage = new QString;
    getThread = new GetAndParseThread;
    udpReceiver = new RtpReciever;
    tcpListener = new TcpListener;
    treeModel = new QStandardItemModel(ui.treeView);
    checkResModel = new QStandardItemModel(ui.tableView);
    fileModel = new QStandardItemModel(ui.tableView_3);//都显示哪些字段？？
    ip = new QString;
    port = new QString;
    pushMethod = new QString;
    pushId = new QString;
    video = new ShowVideo(ui.openGLWidget);
    recordVideo = new ShowVideo(ui.openGLWidget_2);
    isRecordPlay = false;
    fileModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("视频编号")));
    fileModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("通道ID")));
    fileModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("录像开始时间")));
    fileModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("录像结束时间")));
    checkResModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("设备ID")));
    checkResModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("设备类型")));
    checkResModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("信令类型")));
    checkResModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("状态")));
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableView->setModel(checkResModel);
    ui.tableView_3->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableView_3->setModel(fileModel);
    ui.tableView_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    treeModel->setHorizontalHeaderLabels(QStringList() << QStringLiteral("Id(设备名)"));
    ui.treeView->setModel(treeModel);
    ui.dateEdit->setCalendarPopup(true);
    ui.dateEdit_2->setCalendarPopup(true);
    ui.dateEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui.dateEdit_2->setDateTime(QDateTime::currentDateTime());
    ui.timeEdit_3->setTime(QTime::currentTime());
    ui.timeEdit_4->setTime(QTime::currentTime());
    getThread->init(treeModel, checkResModel, h, sipMessage, ip, port, pushMethod, pushId, fileModel);
    getThread->start();
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui.tableView_2->model(), &QStandardItemModel::dataChanged, this, &AWGBCheckTool::dataChangedSlot);
    connect(ui.pushButton_8, &QPushButton::clicked, this, &AWGBCheckTool::deviceRegister);
    connect(ui.pushButton_6, &QPushButton::clicked, this, &AWGBCheckTool::deviceCatalog);
    connect(ui.pushButton_14, &QPushButton::clicked, this, &AWGBCheckTool::deviceCatalog);
    connect(ui.pushButton_23, &QPushButton::clicked, this, &AWGBCheckTool::toControlPage);
    connect(ui.pushButton_13, &QPushButton::clicked, this, &AWGBCheckTool::toListPage);
    connect(ui.pushButton, &QPushButton::clicked, this, &AWGBCheckTool::toVideoPage);
    connect(ui.pushButton_9, &QPushButton::clicked, this, &AWGBCheckTool::toSipPage);
    connect(getThread, &GetAndParseThread::toTree, this, &AWGBCheckTool::setTree);
    connect(getThread, &GetAndParseThread::toTable, this, &AWGBCheckTool::setCheckRes);
    connect(getThread, &GetAndParseThread::toText, this, &AWGBCheckTool::setTextBrowser);
    connect(ui.pushButton_7, &QPushButton::clicked, this, &AWGBCheckTool::prePage);
    connect(ui.pushButton_5, &QPushButton::clicked, this, &AWGBCheckTool::nextPage);
    connect(ui.pushButton_39, &QPushButton::clicked, this, &AWGBCheckTool::prePage);
    connect(ui.pushButton_38, &QPushButton::clicked, this, &AWGBCheckTool::nextPage);
    connect(ui.pushButton_24, &QPushButton::clicked, this, &AWGBCheckTool::playVideo);
    connect(ui.tableView, &QTableView::clicked, this, &AWGBCheckTool::gotoMatchBuffer);
    connect(getThread, &GetAndParseThread::push, this, &AWGBCheckTool::pushStream);
    connect(ui.pushButton_25, &QPushButton::clicked, this, &AWGBCheckTool::stopVideo);
    connect(getThread, &GetAndParseThread::UDP, this, &AWGBCheckTool::UDPPlay);
    connect(getThread, &GetAndParseThread::TCP, this, &AWGBCheckTool::TCPPlay);
    connect(getThread, &GetAndParseThread::TCPActive, this, &AWGBCheckTool::TCPActivePlay);
    connect(ui.pushButton_11, &QPushButton::clicked, this, &AWGBCheckTool::clearInfo);
    connect(ui.pushButton_12, &QPushButton::clicked, this, &AWGBCheckTool::clearList);
    connect(ui.pushButton_36, &QPushButton::clicked, this, &AWGBCheckTool::reMain);
    connect(ui.pushButton_2, &QPushButton::clicked, this, &AWGBCheckTool::recordInfo);
    connect(ui.pushButton_4, &QPushButton::clicked, this, &AWGBCheckTool::recordPlay);
    connect(ui.pushButton_20, &QPushButton::clicked, this, &AWGBCheckTool::recordStop);
    connect(getThread, &GetAndParseThread::registerOk, this, &AWGBCheckTool::changeRegisterStatus);
    connect(getThread, &GetAndParseThread::unRegisterOk, this, &AWGBCheckTool::changeRegisterStatus);
    connect(ui.pushButton_15, &QPushButton::clicked, this, &AWGBCheckTool::subscribe);
    connect(ui.pushButton_16, &QPushButton::clicked, this, &AWGBCheckTool::unSubscribe);
    connect(getThread, &GetAndParseThread::stopPush, this, &AWGBCheckTool::stopPush);
}

AWGBCheckTool::~AWGBCheckTool()
{
    getThread->terminate();
    udpReceiver->stop();
    delete video;
    delete getThread;
    delete udpReceiver;
    delete sipMessage;
    delete tcpListener;
    delete currGBInfo;
    delete ip;
    delete port;
    delete pushMethod;
    delete pushId;
}

inline QString chooseModeType(int i)
{
    switch (i) {
    case 0: return "IPC";
    case 1: return "NVR/DVR";
    case 2: return "解码器";
    case 3: return "下级平台（作为上级）";
    case 4: return "下级平台（作为IPC）";
    case 5: return "上级平台";
    }
}

inline int mode2Int(QString s)
{
    if(s == "IPC")
        return 0;
    if(s == "NVR/DVR")
        return 1;
    if(s == "解码器")
        return 2;
    if(s == "下级平台（作为上级）")
        return 3;
    if(s == "下级平台（作为IPC）")
        return 4;
    if(s == "上级平台")
        return 5;
    else
        return -1;
}
inline int proto2Int(QString s)
{
    if(s == "双向")
        return 0;
    if(s == "单向")
        return 1;
    else
        return -1;
}
void AWGBCheckTool::EndList(){

    ui.textBrowser->moveCursor(QTextCursor::End);
}

inline QString chooseAuthMode(int i)
{
    switch (i) {
    case 0: return "双向";
    case 1: return "单向";
    }
}

void AWGBCheckTool::SetList(pGBStart_s param)
{
    QStandardItemModel *student_model = new QStandardItemModel();
    student_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("设备类型")));
    student_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("认证方式")));
    student_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("设备ID")));
    student_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("设备IP")));
    student_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("设备端口")));
    student_model->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("设备域")));
    student_model->setHorizontalHeaderItem(6, new QStandardItem(QObject::tr("有效期")));
    student_model->setHorizontalHeaderItem(7, new QStandardItem(QObject::tr("心跳间隔")));
    student_model->setHorizontalHeaderItem(8, new QStandardItem(QObject::tr("心跳次数")));
    student_model->setHorizontalHeaderItem(9, new QStandardItem(QObject::tr("媒体端口")));
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("IPC");
    comboBox->addItem("NVR/DVR");
    //student_model->setItem(0, 1, QStandardItem(comboBox));
    if (param->protoType == 0){
        student_model->setHorizontalHeaderItem(10, new QStandardItem(QObject::tr("文件路径")));
        student_model->setHorizontalHeaderItem(11, new QStandardItem(QObject::tr("选择路径")));
    }else{
        student_model->setHorizontalHeaderItem(10, new QStandardItem(QObject::tr("密码")));
    }
    //利用setModel()方法将数据模型与QTableView绑定;
    for(int i = 0; i < 5; ++i)
    {
        student_model->setItem(i, 0, new QStandardItem(chooseModeType(i)));
        student_model->setItem(i, 2, new QStandardItem(QString("34020000002000000001")));
        student_model->setItem(i, 3, new QStandardItem(QString("192.168.0.108")));
        student_model->setItem(i, 4, new QStandardItem(QString("5060")));
        student_model->setItem(i, 5, new QStandardItem(QString("3402000000")));
        student_model->setItem(i, 6, new QStandardItem(QString("60")));
        student_model->setItem(i, 7, new QStandardItem(QString("10")));
        student_model->setItem(i, 8, new QStandardItem(QString("6")));
        student_model->setItem(i, 9, new QStandardItem(QString("10002")));
        if (param->protoType == 0){
            student_model->setItem(i, 1, new QStandardItem(chooseAuthMode(0)));
            student_model->setItem(i, 10, new QStandardItem(QString("path")));
        }else{
            student_model->setItem(i, 10, new QStandardItem(QString("passwd")));
        }
    }
    //    ui.tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableView_2->setModel(student_model);
    if (param->protoType == 0){
        for(int i = 0; i < 5; ++i)
        {
            QPushButton* c_button = new QPushButton("path");
            ui.tableView_2->setIndexWidget(student_model->index(i, 11), c_button);
            connect(c_button, &QPushButton::clicked, this, [=]{ButtonCli(param);student_model->setItem(i, 10, new QStandardItem(QString(QString::fromStdString(string((char*)param->path)))));
                ui.tableView_2->setModel(student_model);});
        }
    }
    connect(ui.tableView_2, &QTableView::clicked, this, &AWGBCheckTool::showInCurrentInterface);  //选中列表时显示到当前通道
}

void AWGBCheckTool::showInCurrentInterface()
{
    if(currGBInfo->modeType >3)
        ui.pushButton_8->setEnabled(true);
    int curRow=ui.tableView_2->currentIndex().row();
    QAbstractItemModel *modessl = ui.tableView_2->model();
    QModelIndex indextemp = modessl->index(curRow, 2);
    QVariant datatemp = modessl->data(indextemp);
    ui.lineEdit->setText(datatemp.toString());
}

void AWGBCheckTool::ButtonCli(pGBStart_s param)
{
    QString filePath = QFileDialog::getExistingDirectory(this,"֤���ļ���·��", "./");
    strcpy(param->path, filePath.toLatin1().data());
    param->pathLen = filePath.length();
}

int reverseModeType(QString s)
{
    if(s == QString("IPC"))
    {
        return 0;
    }
    if(s == QString("NVR/DVR"))
    {
        return 1;
    }
    if(s == QString("解码器"))
    {
        return 2;
    }
    if(s == QString("上级平台"))
    {
        return 3;
    }
    if(s == QString("下级平台"))
    {
        return 4;
    }
    else
    {
        return -1;
    }
}

void makeDeviceInviteXml(char *xmlBuffer, QString method, QString deviceid)
{
    QDomDocument doc;
    QDomElement root=doc.createElement("Request");
    doc.appendChild(root);

    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);

    QDomElement deviceId = doc.createElement("DeviceId");
    QDomText deviceIdText = doc.createTextNode(deviceid);
    deviceId.appendChild(deviceIdText);
    root.appendChild(deviceId);

    QDomElement devicePort = doc.createElement("SipCmd");
    QDomText devicePortText = doc.createTextNode("Invite");
    devicePort.appendChild(devicePortText);
    root.appendChild(devicePort);

    QDomElement playMet = doc.createElement("Method");
    QDomText playMetText = doc.createTextNode(method);
    playMet.appendChild(playMetText);
    root.appendChild(playMet);


    memcpy(xmlBuffer, doc.toString().toStdString().c_str(), doc.toString().length());
}


void AWGBCheckTool::dataChangedSlot()
{
    pGBStart_s param = (pGBStart_s)calloc(1, sizeof(pGBStart_S));
    int curRow=ui.tableView_2->currentIndex().row();
    int curColumn=ui.tableView_2->currentIndex().column();
    QAbstractItemModel *modessl = ui.tableView_2->model();
    QModelIndex indextemp = modessl->index(curRow,curColumn);
    QVariant datatemp = modessl->data(indextemp);
    int type;
    switch(curColumn)
    {
    case 0 :
        type = mode2Int(datatemp.toString());
        if ( type == -1)
        {
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp, chooseModeType(curRow));
        }else{
            //            param->modeType = type;
        }
        break;
    case 1 :
        if(param->protoType == 1)
        {
            modessl->setData(indextemp, "");
            break;
        }
        type = proto2Int(datatemp.toString());
        if ( type == -1)
        {
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp, chooseAuthMode(0));
        }else{
            //            param->authMethod = type;
        }
        break;
    case 2 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{18}|\\d{20}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("00000000000000000000"));
        }else{
            //            strcpy(param->localId,datatemp.toString().toLatin1().data());
            //            param->localIdLen = datatemp.toString().length();
        }
        //        cout << param->localId << endl;
        break;
    case 3 :
        if (!regex_match(datatemp.toString().toStdString(),regex("[0-9\\.]{1,15}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("192.168.0.1"));
        }else{
            //        strcpy(param->localIp,datatemp.toString().toLatin1().data());
            //        param->localIpLen = datatemp.toString().length();
        }
        break;
    case 4 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("5060"));
        }else{
            //        cout << datatemp.toString().toLatin1().data() << endl;
            //        strcpy(param->localPort,datatemp.toString().toLatin1().data());
            //        param->localPortLen = datatemp.toString().length();
            //        cout << param->localPort << endl;
        }
        break;
    case 5 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("0000000000"));
        }else{
            /*strcpy(param->localArea,datatemp.toString().toLatin1().data());
        param->localAreaLen = datatemp.toString().length();*/
        }
        break;
    case 6 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("60"));
        }else{
            /*strcpy(param->validTime,datatemp.toString().toLatin1().data());
        param->validTimeLen = datatemp.toString().length();*/
        }
        break;
    case 7 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("10"));
        }else{
            //        strcpy(param->beatTime,datatemp.toString().toLatin1().data());
            //        param->beatTimeLen = datatemp.toString().length();
        }
        break;
    case 8 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("6"));
        }else{
            //        strcpy(param->beatCnt,datatemp.toString().toLatin1().data());
            //        param->beatCntLen = datatemp.toString().length();
        }
        break;
    case 9 :
        if (param->protoType == 0){
            QString a = "不允许在此处修改";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString("path"));
        }else{
            if (!regex_match(datatemp.toString().toStdString(),regex(".{1,32}"))){
                QString a = "数据格式错误";
                QMessageBox::information(this, "information", a);
                modessl->setData(indextemp,QString("passwd"));
            }else{
                //            strcpy(param->passwd,datatemp.toString().toLatin1().data());
                //            param->passwdLen = datatemp.toString().length();
            }
        }
        break;
    }
    free(param);
}

void AWGBCheckTool::showST()
{
    //    showT->setTextBrowser(ui.textBrowser);
    //    showT->start();
}


void AWGBCheckTool::showCheckRes()
{
    //    showCheckResT->setTableView(checkResModel, handle);
    //    showCheckResT->start();
}

void AWGBCheckTool::showTreeView()
{
    //    showTreeT->setTreeView(treeModel);
    //    showTreeT->start();
    connect(ui.treeView, &QTreeView::clicked, this, &AWGBCheckTool::showTreeInCurrentInterface);
    connect(ui.treeView, &QTreeView::clicked, this, [=]{ui.pushButton_8->setEnabled(false);});
}

void AWGBCheckTool::showTreeInCurrentInterface()
{
    QModelIndex selected = ui.treeView->currentIndex();
    selected = selected.sibling(selected.row(),0);
    ui.lineEdit->setText(ui.treeView->model()->itemData(selected).values()[0].toString().split("(")[0]);
    ui.lineEdit_3->setText(ui.treeView->model()->itemData(selected).values()[0].toString().split("(")[0]);
}

void AWGBCheckTool::deviceRegister()
{
    int curRow=ui.tableView_2->currentIndex().row();
    ui.pushButton_6->setEnabled(true);
    ui.pushButton_7->setEnabled(true);
    ui.label_3->setText("注册");
    if(ui.pushButton_8->text() == "注册")
    {
        pGBStart_s content = (pGBStart_s)calloc(1, sizeof(pGBStart_S));
        QAbstractItemModel *modessl = ui.tableView_2->model();
        QModelIndex indextemp = modessl->index(curRow, 0);
        QVariant datatemp = modessl->data(indextemp);
        content->modeType = mode2Int(datatemp.toString());

        indextemp = modessl->index(curRow, 1);
        datatemp = modessl->data(indextemp);
        content->authMethod = proto2Int(datatemp.toString());
        indextemp = modessl->index(curRow, 2);
        datatemp = modessl->data(indextemp);
        if(datatemp.toString().length() != 0)
        {
            strcpy(content->localId, datatemp.toString().toLatin1().data());
            content->localIdLen = datatemp.toString().length();
        }
        indextemp = modessl->index(curRow, 3);
        datatemp = modessl->data(indextemp);
        strcpy(content->localIp, datatemp.toString().toLatin1().data());
        content->localIpLen = datatemp.toString().length();

        indextemp = modessl->index(curRow, 4);
        datatemp = modessl->data(indextemp);
        strcpy(content->localPort, datatemp.toString().toLatin1().data());
        content->localPortLen = datatemp.toString().length();

        indextemp = modessl->index(curRow, 5);
        datatemp = modessl->data(indextemp);
        strcpy(content->localArea, datatemp.toString().toLatin1().data());
        content->localAreaLen = datatemp.toString().length();

        indextemp = modessl->index(curRow, 6);
        datatemp = modessl->data(indextemp);
        strcpy(content->validTime, datatemp.toString().toLatin1().data());
        content->validTimeLen = datatemp.toString().length();

        indextemp = modessl->index(curRow, 7);
        datatemp = modessl->data(indextemp);
        strcpy(content->beatTime, datatemp.toString().toLatin1().data());
        content->beatTimeLen = datatemp.toString().length();

        indextemp = modessl->index(curRow, 8);
        datatemp = modessl->data(indextemp);
        strcpy(content->beatCnt, datatemp.toString().toLatin1().data());
        content->beatCntLen = datatemp.toString().length();

        indextemp = modessl->index(curRow, 9);
        datatemp = modessl->data(indextemp);
        strcpy(content->mediaPort, datatemp.toString().toLatin1().data());
        content->mediaPortLen = datatemp.toString().length();

        indextemp = modessl->index(curRow, 10);
        datatemp = modessl->data(indextemp);
        if(datatemp.toString().length() != 0)
        {
            strcpy(content->path, datatemp.toString().toLatin1().data());
            content->pathLen = datatemp.toString().length();
        }
        else {
            strcpy(content->passwd, datatemp.toString().toLatin1().data());
            content->passwdLen = datatemp.toString().length();
        }
        cout << "copy ok" << endl;
        char buf[1024] = {0};
        int ret = 0;
        makeDeviceInfoXml(content, buf, false);
        ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)buf, strlen(buf));
        if(!ret)
        {
            qInfo() << "Send devInfo(before register) success";
        }
        else {
            qWarning() << "Send devInfo(before register) failed";
        }
        QDomDocument doc;
        QDomElement root=doc.createElement("Request");
        doc.appendChild(root);

        QDomElement cmdType = doc.createElement("CmdType");
        QDomText cmdTypeText = doc.createTextNode("SipCmd");
        cmdType.appendChild(cmdTypeText);
        root.appendChild(cmdType);

        QDomElement sipCmd = doc.createElement("SipCmd");
        QDomText sipCmdText = doc.createTextNode("Register");
        sipCmd.appendChild(sipCmdText);
        root.appendChild(sipCmd);

        QDomElement deviceId = doc.createElement("DeviceId");
        if(ui.lineEdit->text().isEmpty())
        {
            //弹messageBox，返回
            QString a = "请选择一个设备";
            QMessageBox::information(this, "information", a);
            free(content);
            return ;
        }
        QDomText deviceIdText = doc.createTextNode(ui.lineEdit->text());
        deviceId.appendChild(deviceIdText);
        root.appendChild(deviceId);

        ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().size());
        //    cout << ret << endl;
        free(content);
    }
    else {
        QDomDocument doc;
        QDomElement root=doc.createElement("Request");
        doc.appendChild(root);

        QDomElement cmdType = doc.createElement("CmdType");
        QDomText cmdTypeText = doc.createTextNode("SipCmd");
        cmdType.appendChild(cmdTypeText);
        root.appendChild(cmdType);

        QDomElement sipCmd = doc.createElement("SipCmd");
        QDomText sipCmdText = doc.createTextNode("UnRegister");
        sipCmd.appendChild(sipCmdText);
        root.appendChild(sipCmd);

        QDomElement deviceId = doc.createElement("DeviceId");
        if(ui.lineEdit->text().isEmpty())
        {
            //弹messageBox，返回
            QString a = "请选择一个设备";
            QMessageBox::information(this, "information", a);
            return ;
        }
        QDomText deviceIdText = doc.createTextNode(ui.lineEdit->text());
        deviceId.appendChild(deviceIdText);
        root.appendChild(deviceId);
        AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().size());
    }
}

void AWGBCheckTool::deviceCatalog()
{
    ui.label_3->setText("目录查询");
    QString currentChannel = ui.lineEdit->text();
    qInfo() << "send Catalog for: " << currentChannel;
    QDomDocument doc;
    QDomElement root = doc.createElement("Request");
    doc.appendChild(root);
    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);
    if(!currentChannel.isEmpty())
    {
        ;
    }
    QDomElement sipCmd = doc.createElement("SipCmd");
    QDomText sipCmdText = doc.createTextNode("Catalog");
    sipCmd.appendChild(sipCmdText);
    root.appendChild(sipCmd);

    int ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().length());
    if(!ret)
        qInfo() << "Send catalog success";
}


void AWGBCheckTool::toControlPage()
{
    ui.stackedWidget_2->setCurrentIndex(0);
}

void AWGBCheckTool::toListPage()
{
    ui.stackedWidget_2->setCurrentIndex(1);
}

void AWGBCheckTool::toVideoPage()
{
    ui.stackedWidget->setCurrentIndex(1);
    ui.stackedWidget_2->setCurrentIndex(2);
}

void AWGBCheckTool::toSipPage()
{
    ui.stackedWidget->setCurrentIndex(0);
    ui.stackedWidget_2->setCurrentIndex(1);
}

void AWGBCheckTool::setTree()
{
    ui.treeView->setModel(treeModel);
}

void AWGBCheckTool::setCheckRes()
{
    //    ui.tableView->setModel(checkResModel);
    cout << "setCheckRes" << endl;
}

void AWGBCheckTool::prePage(){
    int cur = ui.stackedWidget->currentIndex();
    if (cur == 0){
        ui.stackedWidget->setCurrentIndex(2);
        ui.stackedWidget_3->setMaximumHeight(330);
        ui.stackedWidget_3->setCurrentIndex(1);
        ui.stackedWidget_2->setCurrentIndex(3);
    }else if (cur == 1){
        ui.stackedWidget->setCurrentIndex(0);
        ui.stackedWidget_3->setMaximumHeight(112);
        ui.stackedWidget_3->setCurrentIndex(0);
        ui.stackedWidget_2->setCurrentIndex(1);
    }else{
        ui.stackedWidget->setCurrentIndex(1);
        ui.stackedWidget_3->setMaximumHeight(112);
        ui.stackedWidget_3->setCurrentIndex(0);
        ui.stackedWidget_2->setCurrentIndex(2);
    }
}
void AWGBCheckTool::nextPage(){
    int cur = ui.stackedWidget->currentIndex();
    if (cur == 0){
        ui.stackedWidget->setCurrentIndex(1);
        ui.stackedWidget_3->setMaximumHeight(112);
        ui.stackedWidget_3->setCurrentIndex(0);
        ui.stackedWidget_2->setCurrentIndex(2);
    }else if (cur == 1){
        ui.stackedWidget->setCurrentIndex(2);
        ui.stackedWidget_3->setMaximumHeight(330);
        ui.stackedWidget_3->setCurrentIndex(1);
        ui.stackedWidget_2->setCurrentIndex(3);
    }else{
        ui.stackedWidget->setCurrentIndex(0);
        ui.stackedWidget_3->setMaximumHeight(112);
        ui.stackedWidget_3->setCurrentIndex(0);
        ui.stackedWidget_2->setCurrentIndex(1);
    }
}

void AWGBCheckTool::setTextBrowser()
{
    ui.textBrowser->append(*sipMessage);
    ui.textBrowser->moveCursor(QTextCursor::End);
    //    QTextCursor cursor = ui.textBrowser->textCursor();
    //    cursor.movePosition(QTextCursor::End);
    //    cursor.insertBlock();
}

void AWGBCheckTool::playVideo()
{
    //先发给服务端参数，播放视频 根据类型启动线程。
    //解码器
    if(ui.lineEdit->text().isEmpty())
    {
        QString a = "请选择一个通道/设备";
        QMessageBox::information(this, "information", a);
        return ;
    }
    if(currGBInfo->modeType == 2)
    {
        //后端发invite
        QDomDocument doc;
        QDomElement root = doc.createElement("Request");
        doc.appendChild(root);
        QDomElement cmdType = doc.createElement("CmdType");
        QDomText cmdTypeText = doc.createTextNode("SipCmd");
        cmdType.appendChild(cmdTypeText);
        root.appendChild(cmdType);
        QDomElement deviceId = doc.createElement("DeviceId");
        QDomText deviceIdText = doc.createTextNode(ui.lineEdit->text());
        deviceId.appendChild(deviceIdText);
        root.appendChild(deviceId);
        QDomElement sipCmd = doc.createElement("SipCmd");
        QDomText sipCmdText = doc.createTextNode("Invite");
        sipCmd.appendChild(sipCmdText);
        root.appendChild(sipCmd);
        int ret = AW_BSQueue_PutBuffer(handle, (unsigned char*)doc.toString().toStdString().c_str(), doc.toString().length());
        if(!ret)
            qInfo() << "decode invite send to backend!";
        ui.pushButton_24->setEnabled(false);
    }
    else
    {
        //      不同选项不同模式
        char sendBuf[2048] = {0};
        QString deId = ui.lineEdit->text();
        int ret;
        switch (ui.comboBox_3->currentIndex()) {
        case 0:
            //            cout << "udp" << endl;
            makeDeviceInviteXml(sendBuf,  "UDP", deId);
            ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)sendBuf, strlen(sendBuf));
            getThread->setMethod("UDP");
            if(!ret)
                qInfo() << "UDP mode invite send to backend";
            break;
        case 1:
            makeDeviceInviteXml(sendBuf, "TCP_Active", deId);
            ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)sendBuf, strlen(sendBuf));
            //            cout << "TCP 主动" << endl;
            getThread->setMethod("TCP_Active");
            if(!ret)
                qInfo() << "TCP_Active mode invite send to backend";
            break;
        case 2:
            makeDeviceInviteXml(sendBuf, "TCP", deId);
            ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)sendBuf, strlen(sendBuf));
            //            cout << "TCP 被动" << endl;
            getThread->setMethod("TCP");
            if(!ret)
                qInfo() << "TCP mode invite send to backend";
            break;
            }
     }
        ui.comboBox_3->setEnabled(false);
        ui.pushButton_24->setEnabled(false);

}

void AWGBCheckTool::gotoMatchBuffer()
{
    //    int row = ui.tableView->currentIndex().row();
    //    QTextCursor tc = ui.textBrowser->textCursor();
    //    int position = ui.textBrowser->document()->findBlockByNumber ( row-1 ).position();
    //    tc.setPosition(position,QTextCursor::MoveAnchor);
    //    ui.textBrowser->setTextCursor(tc);
}

void AWGBCheckTool::pushStream()
{
    qInfo() << "Receive pushstream from backend";
    QString url = "";
    int choice;         //choice换成string放在最后当参数
    if(*pushMethod == QString("UDP")){
        choice = 0;
        url = "http://127.0.0.1/index/api/push_GB28181Rtp?schema=rtp&vhost=__defaultVhost__&app=rtp&stream=1&PushUrl=rtp://"+*ip+":"+*port+"&RTPtype=1";
    }else if(*pushMethod == QString("TCP")){
        choice = 1;
        url = "http://127.0.0.1/index/api/push_GB28181Rtp?schema=rtp&vhost=__defaultVhost__&app=rtp&stream=1&PushUrl=rtp://"+*ip+":"+*port+"&RTPtype=0";
    }else{
        choice = 2;
        url = "http://127.0.0.1/index/api/bind_GBRtpSsrc?client_ip="+*ip+"&client_port="+*port+"&GBrtpSSRC=1";
    }
    //注册MP4
    //http://127.0.0.1/index/api/push_MP4toStream?Mp4Path=D:\\test.mp4&app=rtp&stream=1
    //推(我的主动)
    //http://127.0.0.1/index/api/push_GB28181Rtp?schema=rtp&vhost=__defaultVhost__&app=rtp&stream=1&PushUrl=rtp://192.168.0.241:10000&RTPtype=0(0, tcp, 1, udp)
    //推（我的被动）
    //http://127.0.0.1/index/api/bind_GBRtpSsrc?client_ip=192.168.0.92&client_port=4001&GBrtpSSRC=1
    //关闭
    //http://127.0.0.1/index/api/close_pushGB28181Rtp?schema=rtp&vhost=__defaultVhost__&app=rtp&stream=1&PushUrl=rtp://192.168.0.92:8000
    QString url1 = "http://127.0.0.1/index/api/push_MP4toStream?Mp4Path="+ QDir::currentPath() +"/test.mp4&app=rtp&stream=1";
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request;
    request.setUrl(QUrl(url1));
    QNetworkReply *pReply = manager->get(request);
    // 开启一个局部的事件循环，等待页面响应结束
    QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    // 获取网页Body中的内容
    QByteArray bytes = pReply->readAll();
    cout << bytes.toStdString() << endl;
    if(bytes[14].operator!=('0'))
    {
        qCritical() << "Register mp4 file failed!";
        return;
    }
    request.setUrl((QUrl(url)));
    qInfo() << "Push used url:" << url;
    pReply = manager->get(request);
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    bytes = pReply->readAll();
    cout << bytes.toStdString() << endl;
    if(bytes[14].operator==('0'))
    {

        QDomDocument doc;
        QDomElement root = doc.createElement("Request");
        doc.appendChild(root);
        QDomElement cmdType = doc.createElement("CmdType");
        QDomText cmdTypeText = doc.createTextNode("SipCmd");
        cmdType.appendChild(cmdTypeText);
        root.appendChild(cmdType);
        QDomElement sipCmd = doc.createElement("SipCmd");
        QDomText sipCmdText = doc.createTextNode("Ack");
        sipCmd.appendChild(sipCmdText);
        root.appendChild(sipCmd);
        QDomElement deviceId = doc.createElement("DeviceId");
        QDomText deviceIdText = doc.createTextNode(*pushId);
        deviceId.appendChild(deviceIdText);
        root.appendChild(deviceId);
        int ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().length());
        if(!ret)
            cout << "ACk send OK!" << endl;
    }
}

void makeDevInfoAck(char * buf, QString deId)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("Request");
    doc.appendChild(root);
    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);
    QDomElement sipCmd = doc.createElement("SipCmd");
    QDomText sipCmdText = doc.createTextNode("Ack");
    sipCmd.appendChild(sipCmdText);
    root.appendChild(sipCmd);
    QDomElement deviceId = doc.createElement("DeviceId");
    QDomText deviceIdText = doc.createTextNode(deId);
    deviceId.appendChild(deviceIdText);
    root.appendChild(deviceId);
    memcpy(buf, doc.toString().toStdString().c_str(), doc.toString().length());
}


void AWGBCheckTool::stopVideo(){
    QDomDocument doc;
    QDomElement root = doc.createElement("Request");
    doc.appendChild(root);
    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);
    QDomElement sipCmd = doc.createElement("SipCmd");
    QDomText sipCmdText = doc.createTextNode("Bye");
    sipCmd.appendChild(sipCmdText);
    root.appendChild(sipCmd);
    QDomElement deviceId = doc.createElement("DeviceId");
    QDomText deviceIdText;
    if(!pushId->isEmpty())
        deviceIdText = doc.createTextNode(*pushId);
    else {
        deviceIdText = doc.createTextNode(ui.lineEdit->text());
    }
    deviceId.appendChild(deviceIdText);
    root.appendChild(deviceId);
    int ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().length());
    if(!ret)
        qInfo() << "Send Bye to backend";
    tcpListener->channel->stop();
    tcpListener->stop();
    udpReceiver->stop();
    if(!isRecordPlay)
        video->clear();
    else {
        recordVideo->clear();
    }
    qInfo() << "Video play stopped";
    ui.pushButton_24->setEnabled(true);
    ui.comboBox_3->setEnabled(true);
    if(currGBInfo->modeType == 2)
        stopPush();
}

void AWGBCheckTool::stopPush()
{
    QString deId = ui.lineEdit->text();
    QString urlS = "http://127.0.0.1/index/api/close_pushGB28181Rtp?schema=rtp&vhost=__defaultVhost__&app=rtp&stream=1&PushUrl=rtp://"+*ip+":" + *port;
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request;
    request.setUrl(QUrl(urlS));
    QNetworkReply *pReply = manager->get(request);
    // 开启一个局部的事件循环，等待页面响应结束
    QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    // 获取网页Body中的内容
    QByteArray bytes = pReply->readAll();
    cout << bytes.toStdString() << endl;
    if(bytes[14].operator!=('0'))
    {
        qInfo() << "stop push stream failed";
        ui.pushButton_24->setEnabled(true);
        return;
    }
}

void AWGBCheckTool::UDPPlay()
{
    if(ui.comboBox_4->currentIndex() == 1)
    {
        udpReceiver->channel->videotype = 2;
        if(currGBInfo->protoType == 0)
        {
            udpReceiver->channel->pubkey = QByteArray::fromBase64(getThread->bPubKey.toLocal8Bit());
            udpReceiver->channel->vkek = QByteArray::fromBase64(getThread->bvkek.toLocal8Bit());
            udpReceiver->channel->keyVersion = getThread->keyVersion.split(".")[0];
            qInfo() << "Start play in udp, svac";
        }
    }
    else {
        udpReceiver->channel->videotype = 0;
        qInfo() << "Start play in udp, h264";
    }
    if(!isRecordPlay)
        udpReceiver->channel->setEventHandle(video);
    else {
        udpReceiver->channel->setEventHandle(recordVideo);
    }
    udpReceiver->setPort(atoi((char *)currGBInfo->mediaPort));
    udpReceiver->start();
    char * buf = (char*)calloc(1024, sizeof(char));
    makeDevInfoAck(buf, ui.lineEdit->text());
    int ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)buf, strlen(buf));
    cout << buf <<endl;
    if(!ret)
        qInfo() << "Send Ack to backend, UDP mode";
    if(!isRecordPlay)
        video->resize(ui.openGLWidget->width(), ui.openGLWidget->height());
    else {
        recordVideo->resize(ui.openGLWidget_2->width(), ui.openGLWidget_2->height());
    }
    free(buf);
}

void AWGBCheckTool::TCPPlay()
{
    if(ui.comboBox_4->currentIndex())
    {
        tcpListener->channel->videotype = 2;
    }
    else {
        tcpListener->channel->videotype = 0;
    }
    tcpListener->connectType = 2;
    tcpListener->ip = *ip;
    tcpListener->port = atoi((char *)currGBInfo->mediaPort);
    if(!isRecordPlay)
        tcpListener->channel->setEventHandle(video);
    else {
        tcpListener->channel->setEventHandle(recordVideo);
    }
    tcpListener->start();
    char * buf = (char*)calloc(1024, sizeof(char));
    makeDevInfoAck(buf, ui.lineEdit->text());
    int ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)buf, strlen(buf));
    if(!ret)
        qInfo() << "Send Ack to backend, TCP mode";
    if(!isRecordPlay)
        video->resize(ui.openGLWidget->width(), ui.openGLWidget->height());
    else {
        recordVideo->resize(ui.openGLWidget_2->width(), ui.openGLWidget_2->height());
    }
    free(buf);
}


void AWGBCheckTool::TCPActivePlay()
{
    if(ui.comboBox_4->currentIndex())
    {
        tcpListener->channel->videotype = 2;
    }
    else {
        tcpListener->channel->videotype = 0;
    }
    tcpListener->connectType = 1;
    tcpListener->ip = *ip;
//    tcpListener->port = (*port).toInt();
    tcpListener->port = atoi((char *)currGBInfo->mediaPort);
    tcpListener->channel->setEventHandle(video);
    tcpListener->start();
    char * buf = (char*)calloc(1024, sizeof(char));
    makeDevInfoAck(buf, ui.lineEdit->text());
    int ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)buf, strlen(buf));
    if(!ret)
        qInfo() << "Send Ack to backend, TCP Active mode";
    video->resize(ui.openGLWidget->width(), ui.openGLWidget->height());
    free(buf);
}

void AWGBCheckTool::clearInfo(){
    ui.textBrowser->clear();
}

void AWGBCheckTool::clearList(){
    checkResModel->clear();
    checkResModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("设备ID")));
    checkResModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("设备类型")));
    checkResModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("信令类型")));
    checkResModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("状态")));
    getThread->clearList();
}

void AWGBCheckTool::reMain(){
    qInfo() << "Reconfigure application";
    QString c = "taskkill /im MediaServer.exe /f";
    QProcess::execute(c);
    c = "taskkill /im beanstalkd.exe /f";
    QProcess::execute(c);
    c = "taskkill /im AWGBBase.exe /f";
    QProcess::execute(c);
    QProcess* proBeanstalk = new QProcess();
    QProcess* proMS = new QProcess();
    QProcess* proBase = new QProcess();
    proMS->start("MediaServer.exe");
    proBeanstalk->start("beanstalkd.exe -l 127.0.0.1 -p 11300");
    proBase->start("AWGBBase.exe");
    pGBStart_s papa = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    int haha=0;
    int ret = AW_BSQueue_Init(&haha, (unsigned char *)"127.0.0.1", 0, (unsigned char *)"Server",(unsigned char *)"Client");
    ConfigureGuide *ICB = new ConfigureGuide(Q_NULLPTR, papa, haha);
    ICB->setAttribute(Qt::WA_DeleteOnClose);
    ICB->show();
    this->close();
}

void AWGBCheckTool::recordInfo()
{
    fileModel->clear();
    fileModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("视频编号")));
    fileModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("通道ID")));
    fileModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("录像开始时间")));
    fileModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("录像结束时间")));
    getThread->clearRecordFile();
    QDomDocument doc;
    QDomElement root = doc.createElement("Request");
    doc.appendChild(root);

    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);

    QDomElement sipCmd = doc.createElement("SipCmd");
    QDomText sipCmdText = doc.createTextNode("RecordInfo");
    sipCmd.appendChild(sipCmdText);
    root.appendChild(sipCmd);
    if(ui.lineEdit->text().isEmpty())
    {
        QString a = "请选择一个设备/通道";
        QMessageBox::information(this, "information", a);
        return ;
    }
    QDomElement deviceId = doc.createElement("DeviceId");
    QDomText deviceIdText = doc.createTextNode(ui.lineEdit->text());
    deviceId.appendChild(deviceIdText);
    root.appendChild(deviceId);

    QDomElement start = doc.createElement("StartTime");
    QDomText startText = doc.createTextNode(ui.dateEdit->text().replace('/', '-') + "T" + ui.timeEdit_3->text() + ":00");
    start.appendChild(startText);
    root.appendChild(start);

    QDomElement end = doc.createElement("EndTime");
    QDomText endText = doc.createTextNode(ui.dateEdit_2->text().replace('/', '-') + "T" + ui.timeEdit_4->text() + ":00");
    end.appendChild(endText);
    root.appendChild(end);

    QDomElement type = doc.createElement("Type");
    QDomText typeText = doc.createTextNode(ui.comboBox->currentText());
    type.appendChild(typeText);
    root.appendChild(type);
    int ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().length());
    if(!ret)
        qInfo() << "Send RecordInfo to backend!";
}

void AWGBCheckTool::changeRegisterStatus()
{
    if(ui.pushButton_8->text() == "注册")
    {
        ui.pushButton_8->setText("注销");
    }
    else {
        ui.pushButton_8->setText("注册");
    }
}

void AWGBCheckTool::recordPlay()
{
    //给后端发invite，要包含录像文件名字
    int curRow=ui.tableView_3->currentIndex().row();
    QAbstractItemModel *modessl = ui.tableView_3->model();
    QModelIndex indextemp = modessl->index(curRow, 1);
    QVariant datatemp = modessl->data(indextemp);
    QString deviceID = datatemp.toString();
    if(deviceID.isEmpty())
    {
        QString a = "请选择一个录像文件";
        QMessageBox::information(this, "information", a);
        return ;
    }

    QDomDocument doc;
    QDomElement root=doc.createElement("Request");
    doc.appendChild(root);

    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);

    QDomElement deviceId = doc.createElement("DeviceId");
    QDomText deviceIdText = doc.createTextNode(deviceID);
    deviceId.appendChild(deviceIdText);
    root.appendChild(deviceId);

    QDomElement devicePort = doc.createElement("SipCmd");
    QDomText devicePortText = doc.createTextNode("Invite");//标识是录像的invite
    devicePort.appendChild(devicePortText);
    root.appendChild(devicePort);

    QDomElement playMet = doc.createElement("Method");
    QDomText playMetText = doc.createTextNode(ui.comboBox_2->currentText());
    playMet.appendChild(playMetText);
    root.appendChild(playMet);
    //跟filepath应该无关，主要是用时间戳？？
//    QDomElement filePathXml = doc.createElement("filePath");
//    QDomText filePathXmlText = doc.createTextNode(filePath.replace('_', ' '));
//    filePathXml.appendChild(filePathXmlText);
//    root.appendChild(filePathXml);
    indextemp = modessl->index(curRow, 2);
    datatemp = modessl->data(indextemp);
    QDomElement startTime = doc.createElement("StartTime");
    QDomText startTimeText = doc.createTextNode(datatemp.toString());
    startTime.appendChild(startTimeText);
    root.appendChild(startTime);

    indextemp = modessl->index(curRow, 3);
    datatemp = modessl->data(indextemp);
    QDomElement endTime = doc.createElement("EndTime");
    QDomText endTimeText = doc.createTextNode(datatemp.toString());
    endTime.appendChild(endTimeText);
    root.appendChild(endTime);

    int ret;
    ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().size());
    getThread->setMethod(ui.comboBox_2->currentText());
    if(!ret)
        qInfo() << "send record play invite to backend";
    isRecordPlay = true;
}

void AWGBCheckTool::recordStop()
{
    stopVideo();
    isRecordPlay = false;
}

void AWGBCheckTool::subscribe()
{
    QDomDocument doc;
    QDomElement root=doc.createElement("Request");
    doc.appendChild(root);

    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);

    QDomElement sipCmd = doc.createElement("SipCmd");
    QDomText sipCmdText = doc.createTextNode("Subscribe");
    sipCmd.appendChild(sipCmdText);
    root.appendChild(sipCmd);

    QDomElement deviceId = doc.createElement("DeviceId");
    QDomText deviceIdText = doc.createTextNode(ui.treeView->currentIndex().parent().data().toString());
    deviceId.appendChild(deviceIdText);
    root.appendChild(deviceId);

    int ret;
    ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().size());
    if(!ret)
        qInfo() << "send subscribe to backend";
    cout << doc.toString().toStdString() << endl;
}


void AWGBCheckTool::unSubscribe()
{
    QDomDocument doc;
    QDomElement root=doc.createElement("Request");
    doc.appendChild(root);

    QDomElement cmdType = doc.createElement("CmdType");
    QDomText cmdTypeText = doc.createTextNode("SipCmd");
    cmdType.appendChild(cmdTypeText);
    root.appendChild(cmdType);

    QDomElement sipCmd = doc.createElement("SipCmd");
    QDomText sipCmdText = doc.createTextNode("UnSubscribe");
    sipCmd.appendChild(sipCmdText);
    root.appendChild(sipCmd);

    QDomElement deviceId = doc.createElement("DeviceId");
    QDomText deviceIdText = doc.createTextNode(ui.treeView->currentIndex().parent().data().toString());
    deviceId.appendChild(deviceIdText);
    root.appendChild(deviceId);

    int ret;
    ret = AW_BSQueue_PutBuffer(handle, (unsigned char *)doc.toString().toStdString().c_str(), doc.toString().size());
    if(!ret)
        qInfo() << "send unSubscribe to backend";
    cout << doc.toString().toStdString() << endl;
}
