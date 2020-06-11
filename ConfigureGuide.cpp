#include "ConfigureGuide.h"
#include "main.h"
#include <qvalidator.h>
#include <qlineedit.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <QMessageBox>
#include <regex>
#include <QTextStream>
#include <QInputDialog>

using namespace std;

ConfigureGuide::ConfigureGuide(QWidget *parent, void *param)
	: QWidget(parent)
{
    ui.setupUi(this);
    ConfigureGuide::initConf();
    QRegExp regExp("\\d{1,16}");
    QRegExp regID("\\d{1,20}");
    QRegExp regAxp("\\d{10}");
    QRegExp rxIp("[\\d\\.]{1,16}");
    QRegExp url("");
    QRegExp rxpwd(".{1,32}");
    ui.localIp->setValidator(new QRegExpValidator(rxIp, ui.localIp));
    ui.localPort->setValidator(new QRegExpValidator(regExp, ui.localPort));
    ui.localId->setValidator(new QRegExpValidator(regID, ui.localId));
    ui.localArea->setValidator(new QRegExpValidator(regAxp, ui.localArea));
    ui.mediaPort->setValidator(new QRegExpValidator(regExp, ui.mediaPort));
    ui.validTime->setValidator(new QRegExpValidator(regExp, ui.validTime));
    ui.beatCnt->setValidator(new QRegExpValidator(regExp, ui.beatCnt));
    ui.beatTime->setValidator(new QRegExpValidator(regExp, ui.beatTime));
    ui.LineEdit_9->setValidator(new QRegExpValidator(url, ui.LineEdit_9));
    ui.LineEdit_10->setValidator(new QRegExpValidator(rxpwd, ui.LineEdit_10));
    connect(ui.localId, &QLineEdit::editingFinished, this, &ConfigureGuide::localIdCheck);
    connect(ui.localIp, &QLineEdit::editingFinished, this, &ConfigureGuide::localIpCheck);
    connect(ui.localPort, &QLineEdit::editingFinished, this, &ConfigureGuide::localPwdCheck);
    connect(ui.mediaPort, &QLineEdit::editingFinished, this, &ConfigureGuide::localPwdCheck);
    connect(ui.protoType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ConfigureGuide::ProtocolSwitch);
    connect(ui.PushButton_1, &QPushButton::clicked, this, &ConfigureGuide::GetCertPath);
    connect(ui.LineEdit_9, &QLineEdit::textEdited, this, &ConfigureGuide::LineEditChange);
    connect(ui.startButton, &QPushButton::clicked, this, [=]{ConfigureGuide::SetConfigure(param);});
    connect(ui.exButton, &QPushButton::clicked, this, &ConfigureGuide::exConf);
    connect(ui.inButton, &QPushButton::clicked, this, &ConfigureGuide::inConf);
}
void ConfigureGuide::inConf(){
    int in = ui.boBox_1->currentIndex();
    QString f = ui.boBox_1->itemText(in);
    QFile file(f+".txt");
    file.open(( QIODevice::ReadOnly));
    QString data;
    data = file.readLine().data();
    int flag;
    if (data.length() != 0){
        if (data.trimmed() == '0'){flag = 0;ui.protoType->setCurrentIndex(0);}
        if (data.trimmed() == '1'){flag = 1;ui.protoType->setCurrentIndex(1);}
    }
    data = file.readLine().data();
    if (data.length() != 0){
        if (data.trimmed() == '0'){ui.modelType->setCurrentIndex(0);}
        if (data.trimmed() == '1'){ui.modelType->setCurrentIndex(1);}
        if (data.trimmed() == '2'){ui.modelType->setCurrentIndex(2);}
        if (data.trimmed() == '3'){ui.modelType->setCurrentIndex(3);}
        if (data.trimmed() == '4'){ui.modelType->setCurrentIndex(4);}
    }
    data = file.readLine().data();
    ui.localId->setText(data);
    data = file.readLine().data();
    ui.validTime->setText(data);
    data = file.readLine().data();
    ui.localIp->setText(data);
    data = file.readLine().data();
    ui.beatTime->setText(data);
    data = file.readLine().data();
    ui.localPort->setText(data);
    data = file.readLine().data();
    ui.beatCnt->setText(data);
    data = file.readLine().data();
    ui.localArea->setText(data);
    data = file.readLine().data();
    ui.mediaPort->setText(data);
    if (flag == 0){
        data = file.readLine().data();
        if (data.trimmed() == '0'){ui.authMethod->setCurrentIndex(0);}
        if (data.trimmed() == '1'){ui.authMethod->setCurrentIndex(1);}
        data = file.readLine().data();
        ui.LineEdit_9->setText(data);
    }else{
        data = file.readLine().data();
        ui.LineEdit_10->setText(data);
    }
}
void ConfigureGuide::initConf(){
    QFile file("list.txt");//括号内为文件路径
    file.open(( QIODevice::ReadOnly));
    QVector<QString> list;
    ui.boBox_1->addItem("");
    QString data;
    while (data = file.readLine().data(), data.length() != 0) {
        if(list.indexOf(data.trimmed()) == -1)
        {
            ui.boBox_1->addItem(data.trimmed());
            list.push_back(data.trimmed());
        }
    }

    file.close();
}

void prochange(int a){
    ;
}
void ConfigureGuide::exConf(){
    bool isOK;//QInputDialog 是否成功得到输入
    QString text = QInputDialog::getText(NULL,"新建配置项","输入要保存的配置名 ",QLineEdit::Normal,"",&isOK);
//    cout << text.toLatin1().data() << endl;
    if (text.length()){
        int protoType = ui.protoType->currentIndex();
        int devType = ui.modelType->currentIndex();
        QFile file("list.txt");//括号内为文件路径
        file.open(( QIODevice::Append));
        file.write(text.toLatin1().data());
        file.write("\r\n");
        file.close();
        QFile file2(text+".txt");//括号内为文件路径
        file2.open(( QIODevice::WriteOnly));
        file2.write(QString::number(protoType).toLatin1().data());
        file2.write("\r\n");
        file2.write(QString::number(devType).toLatin1().data());
        file2.write("\r\n");
        QString localId = ui.localId->text();
        file2.write(localId.toLatin1().data());
        file2.write("\r\n");
        QString validTime = ui.validTime->text();
        file2.write(validTime.toLatin1().data());
        file2.write("\r\n");
        QString localIp = ui.localIp->text();
        file2.write(localIp.toLatin1().data());
        file2.write("\r\n");
        QString beatTime = ui.beatTime->text();
        file2.write(beatTime.toLatin1().data());
        file2.write("\r\n");
        QString localPort = ui.localPort->text();
        file2.write(localPort.toLatin1().data());
        file2.write("\r\n");
        QString beatCnt = ui.beatCnt->text();
        file2.write(beatCnt.toLatin1().data());
        file2.write("\r\n");
        QString localArea = ui.localArea->text();
        file2.write(localArea.toLatin1().data());
        file2.write("\r\n");
        QString mediaPort = ui.mediaPort->text();
        file2.write(mediaPort.toLatin1().data());
        file2.write("\r\n");
        if (protoType == 0){
            int autType = ui.authMethod->currentIndex();
            QString path = ui.LineEdit_9->text();
            file2.write(QString::number(autType).toLatin1().data());
            file2.write("\r\n");
            file2.write(path.toLatin1().data());
            file2.write("\r\n");
        }else{
            QString passwd = ui.LineEdit_10->text();
            file2.write(passwd.toLatin1().data());
            file2.write("\r\n");
        }
        file2.close();
    }

}

int ConfigureGuide::CheackLen(QString sttr)
{
    int length = sttr.length();
    if(length != 0 )
    {
        return 1;
    }else{
        QString a = "数据不完整";
        QMessageBox::information(this, "information", a);
        ui.localId->setText(tr(""));
        return 0;
    }
}
void ConfigureGuide::LineEditChange(){
    ui.LineEdit_9->setText(tr(""));
}

void ConfigureGuide::localIdCheck()
{
    int length = ui.localId->text().length();
    if(length == 18 || length == 20)
    {
        ;
    }else{
        QString a = "本地ID输入错误";
        QMessageBox::information(this, "information", a);
        ui.localId->setText(tr(""));
    }
    return;
}
void ConfigureGuide::localIpCheck()
{
    if (regex_match(ui.localIp->text().toStdString(),regex("((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})(\\.((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})){3}"))){
        ;
    }else{
        QString a = "本地IP输入错误";
        QMessageBox::information(this, "information", a);
        ui.localIp->setText(tr(""));
    }
    return;
}

void ConfigureGuide::localPwdCheck()
{
    if (regex_match(ui.localPort->text().toStdString(),regex("[0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5]"))){
        ;
    }else{
        QString a = "本地PORT输入错误";
        QMessageBox::information(this, "information", a);
        ui.localPort->setText(tr(""));
        ui.mediaPort->setText(tr(""));
    }
    return;
}

ConfigureGuide::~ConfigureGuide()
{
}

void ConfigureGuide::ProtocolSwitch()
{
    if (QObject::sender() == ui.protoType)
	{
        if (ui.protoType->currentIndex() == 0)
		{
			if (ui.StackedWidget->currentIndex() == 0) return;
			ui.StackedWidget->setCurrentIndex(0);
            ui.stackedWidget_1->setCurrentIndex(0);
		}
        else if (ui.protoType->currentIndex() == 1)
		{
			if (ui.StackedWidget->currentIndex() == 1) return;
			ui.StackedWidget->setCurrentIndex(1);
            ui.stackedWidget_1->setCurrentIndex(1);
		}
		return;
	}
}

void ConfigureGuide::GetCertPath()
{
	ui.LineEdit_9->setText(QFileDialog::getExistingDirectory(this,"֤���ļ���·��", "./"));
}

void strcpy(uc * dst, char * src)
{
    while( (*dst++ = (uc)(*src++)) != '\0' );
}

void ConfigureGuide::SetConfigure(void* param)
{

    int modelType = ui.modelType->currentIndex();
    int protoType = ui.protoType->currentIndex();
    int authMethod = ui.authMethod->currentIndex();
    int charSet = ui.charSet->currentIndex();
    bool gb28281 = false;
    QString localId = ui.localId->text(); //û������᷵�ؿ��ַ���
    if (CheackLen(localId) == 0) return;
    QString validTime = ui.validTime->text();
    if (CheackLen(validTime) == 0) return;
    QString localIp = ui.localIp->text();
    if (CheackLen(localIp) == 0) return;
    QString beatTime = ui.beatTime->text();
    if (CheackLen(beatTime) == 0) return;
    QString localPort = ui.localPort->text();
    if (CheackLen(localPort) == 0) return;
    QString beatCnt = ui.beatCnt->text();
    if (CheackLen(beatCnt) == 0) return;
    QString localArea = ui.localArea->text();
    if (CheackLen(localArea) == 0) return;
    QString mediaPort = ui.mediaPort->text();   //����������һ��Ϊ�վͲ���ͨ�������и�ʽ��������
    if (CheackLen(mediaPort) == 0) return;
    QString path;
    QString passwd;                             //path��passwd������Э����Ҫ���Ĳ���
    pGBStart_s res = static_cast<pGBStart_s>(param);
    res->modeType = modelType;
    res->protoType = protoType;
    res->authMethod = authMethod;
    strcpy(res->localId, localId.toUtf8().data());
    res->localIdLen = localId.length();
    strcpy(res->validTime, validTime.toUtf8().data());
    res->validTimeLen = validTime.length();
    strcpy(res->localIp, localIp.toUtf8().data());
    res->localIpLen = localIp.length();
    strcpy(res->beatTime, beatTime.toUtf8().data());
    res->beatTimeLen = beatTime.length();
    strcpy(res->localPort, localPort.toUtf8().data());
    res->localPortLen = localPort.length();
    strcpy(res->beatCnt, beatCnt.toUtf8().data());
    res->beatCntLen = beatCnt.length();
    strcpy(res->localArea, localArea.toUtf8().data());
    res->localAreaLen = localArea.length();
    strcpy(res->mediaPort, mediaPort.toUtf8().data());
    res->mediaPortLen= mediaPort.length();
    if(protoType == 1)
    {
        gb28281 = true;
    }
    if(gb28281)
    {
        passwd = ui.LineEdit_10->text();
        if (CheackLen(passwd) == 0) return;
        strcpy(res->passwd, passwd.toUtf8().data());
    }
    else
    {
        path = ui.LineEdit_9->text();
        if (CheackLen(path) == 0) return;
        strcpy(res->path, path.toUtf8().data());
    }
   // free(param);    //��ʱ��������free

    AWGBCheckTool *IC = new AWGBCheckTool(Q_NULLPTR, res);
    IC->setAttribute(Qt::WA_DeleteOnClose);
    IC->show();
    IC->showST();
    IC->showTreeView();
    IC->showCheckRes();
    this->close();
}
