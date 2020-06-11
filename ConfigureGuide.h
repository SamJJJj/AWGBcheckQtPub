#pragma once

#include <QWidget>
#include <QFileDialog>
#include "ui_ConfigureGuide.h"
#include "AWGBCheckTool.h"

class ConfigureGuide : public QWidget
{
	Q_OBJECT

public:
	ConfigureGuide(QWidget *parent = Q_NULLPTR, void *param = NULL);
	~ConfigureGuide();

private:
    Ui::ConfigureGuide ui;
    void ProtocolSwitch();
    void GetCertPath();
    void SetConfigure(void *);
    void localIdCheck();
    void localIpCheck();
    void localPwdCheck();
    void LineEditChange();
    int CheackLen(QString sttr);
    void initConf();
    void exConf();
    void inConf();
};

void strcpy(uc * dst, char * src);
