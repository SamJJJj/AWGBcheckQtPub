#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include <QListWidget>
#include <QStandardItemModel>
#include <QComboBox>
#include <iostream>
#include <regex>
#include <QMessageBox>
#include <QThread>
#include <QTextBrowser>
using namespace std;
AWGBCheckTool::AWGBCheckTool(QWidget *parent, pGBStart_s param)
	: QMainWindow(parent)
{
    ui.setupUi(this);
    AWGBCheckTool::SetList(param);
    connect(ui.textBrowser, &QTextBrowser::textChanged, this, &AWGBCheckTool::EndList);
    connect(ui.tableView_2->model(), &QStandardItemModel::dataChanged, this, [=]{AWGBCheckTool::dataChangedSlot(param);});

}

inline QString chooseModeType(int i)
{
    switch (i) {
    case 0: return "IPC";
    case 1: return "NVR/DVR";
    case 2: return "解码器";
    case 3: return "上级平台";
    case 4: return "下级平台";
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
    if(s == "上级平台")
        return 3;
    if(s == "下级平台")
        return 4;
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
    student_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("本地ID")));
    student_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("本地IP")));
    student_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("本地端口")));
    student_model->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("本地域")));
    student_model->setHorizontalHeaderItem(6, new QStandardItem(QObject::tr("有效期")));
    student_model->setHorizontalHeaderItem(7, new QStandardItem(QObject::tr("心跳间隔")));
    student_model->setHorizontalHeaderItem(8, new QStandardItem(QObject::tr("心跳次数")));
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("IPC");
    comboBox->addItem("NVR/DVR");
    //student_model->setItem(0, 1, QStandardItem(comboBox));
    if (param->protoType == 0){
        student_model->setHorizontalHeaderItem(9, new QStandardItem(QObject::tr("文件路径")));
        student_model->setHorizontalHeaderItem(10, new QStandardItem(QObject::tr("选择路径")));
    }else{
        student_model->setHorizontalHeaderItem(9, new QStandardItem(QObject::tr("密码")));
    }
    //利用setModel()方法将数据模型与QTableView绑定;
    student_model->setItem(0, 0, new QStandardItem(chooseModeType(param->modeType)));
    student_model->setItem(0, 1, new QStandardItem(chooseAuthMode(param->authMethod)));
    student_model->setItem(0, 2, new QStandardItem(QString(QString::fromStdString(string((char*)param->localId)))));
    student_model->setItem(0, 3, new QStandardItem(QString(QString::fromStdString(string((char*)param->localIp)))));
    student_model->setItem(0, 4, new QStandardItem(QString(QString::fromStdString(string((char*)param->localPort)))));
    student_model->setItem(0, 5, new QStandardItem(QString(QString::fromStdString(string((char*)param->localArea)))));
    student_model->setItem(0, 6, new QStandardItem(QString(QString::fromStdString(string((char*)param->validTime)))));
    student_model->setItem(0, 7, new QStandardItem(QString(QString::fromStdString(string((char*)param->beatTime)))));
    student_model->setItem(0, 8, new QStandardItem(QString(QString::fromStdString(string((char*)param->beatCnt)))));
    if (param->protoType == 0){
        student_model->setItem(0, 9, new QStandardItem(QString(QString::fromStdString(string((char*)param->path)))));
    }else{
        student_model->setItem(0, 9, new QStandardItem(QString(QString::fromStdString(string((char*)param->passwd)))));
    }
//    ui.tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableView_2->setModel(student_model);
    if (param->protoType == 0){
    QPushButton* c_button = new QPushButton("path");
    ui.tableView_2->setIndexWidget(student_model->index(0, 10), c_button);
    connect(c_button, &QPushButton::clicked, this, [=]{ButtonCli(param);student_model->setItem(0, 9, new QStandardItem(QString(QString::fromStdString(string((char*)param->path)))));
        ui.tableView_2->setModel(student_model);});
    }
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


void AWGBCheckTool::dataChangedSlot(pGBStart_s param)
{
    int curRow=ui.tableView_2->currentIndex().row();
    int curColumn=ui.tableView_2->currentIndex().column();
//    int curColumn=ui.tableView_2->currentIndex().data()
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
            modessl->setData(indextemp, chooseModeType(param->modeType));
        }else{
            param->modeType = type;
        }
        break;
    case 1 :
        type = proto2Int(datatemp.toString());
        if ( type == -1)
        {
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp, chooseAuthMode(param->authMethod));
        }else{
            param->authMethod = type;
        }
        break;
    case 2 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{18}|\\d{20}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString(QLatin1String((char *)param->localId)));
        }else{
            strcpy(param->localId,datatemp.toString().toLatin1().data());
            param->localIdLen = datatemp.toString().length();
        }
//        cout << param->localId << endl;
        break;
    case 3 :
        if (!regex_match(datatemp.toString().toStdString(),regex("[0-9\\.]{1,15}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString(QLatin1String((char *)param->localIp)));
        }else{
        strcpy(param->localIp,datatemp.toString().toLatin1().data());
        param->localIpLen = datatemp.toString().length();}
        break;
    case 4 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString(QLatin1String((char *)param->localPort)));
        }else{
//        cout << datatemp.toString().toLatin1().data() << endl;
        strcpy(param->localPort,datatemp.toString().toLatin1().data());
        param->localPortLen = datatemp.toString().length();
//        cout << param->localPort << endl;
        }
        break;
    case 5 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString(QLatin1String((char *)param->localAreaLen)));
        }else{
        strcpy(param->localArea,datatemp.toString().toLatin1().data());
        param->localAreaLen = datatemp.toString().length();}
        break;
    case 6 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString(QLatin1String((char *)param->validTime)));
        }else{
        strcpy(param->validTime,datatemp.toString().toLatin1().data());
        param->validTimeLen = datatemp.toString().length();}
        break;
    case 7 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString(QLatin1String((char *)param->beatTime)));
        }else{
        strcpy(param->beatTime,datatemp.toString().toLatin1().data());
        param->beatTimeLen = datatemp.toString().length();}
        break;
    case 8 :
        if (!regex_match(datatemp.toString().toStdString(),regex("\\d{1,16}"))){
            QString a = "数据格式错误";
            QMessageBox::information(this, "information", a);
            modessl->setData(indextemp,QString(QLatin1String((char *)param->beatCnt)));
        }else{
        strcpy(param->beatCnt,datatemp.toString().toLatin1().data());
        param->beatCntLen = datatemp.toString().length();}
        break;
    case 9 :
        if (param->protoType == 0){
                QString a = "不允许在此处修改";
                QMessageBox::information(this, "information", a);
                modessl->setData(indextemp,QString(QLatin1String((char *)param->path)));
        }else{
            if (!regex_match(datatemp.toString().toStdString(),regex(".{1,32}"))){
                QString a = "数据格式错误";
                QMessageBox::information(this, "information", a);
                modessl->setData(indextemp,QString(QLatin1String((char *)param->passwd)));
            }else{
            strcpy(param->passwd,datatemp.toString().toLatin1().data());
            param->passwdLen = datatemp.toString().length();}
        }
        break;
    }
//    ui.tableView_2->setModel(modessl);
//    free(param);
}

void AWGBCheckTool::showST()
{
    showT.setIc(this);
    showT.start();
}


