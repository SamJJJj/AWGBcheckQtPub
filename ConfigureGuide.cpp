#include "ConfigureGuide.h"
#include "main.h"
#include <qvalidator.h>
#include <qlineedit.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <QMessageBox>
#include <regex>

using namespace std;

ConfigureGuide::ConfigureGuide(QWidget *parent, void *param)
	: QWidget(parent)
{
    ui.setupUi(this);
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
//    ([0-9]|[1-9]\d|[1-9]\d{2}|[1-9]\d{3}|[1-5]\d{4}|6[0-4]\d{3}|65[0-4]\d{2}|655[0-2]\d|6553[0-5])
    connect(ui.localPort, &QLineEdit::editingFinished, this, &ConfigureGuide::localPwdCheck);
    connect(ui.mediaPort, &QLineEdit::editingFinished, this, &ConfigureGuide::localPwdCheck);


    connect(ui.protoType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ConfigureGuide::ProtocolSwitch);

    connect(ui.PushButton_1, &QPushButton::clicked, this, &ConfigureGuide::GetCertPath);
    connect(ui.LineEdit_9, &QLineEdit::textEdited, this, &ConfigureGuide::LineEditChange);

    connect(ui.startButton, &QPushButton::clicked, this, [=]{ConfigureGuide::SetConfigure(param);});
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
    cout << length << endl;
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
    this->close();
}
