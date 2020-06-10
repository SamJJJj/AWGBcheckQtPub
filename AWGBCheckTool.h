#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include "ui_AWGBCheckTool.h"
#include "main.h"

class AWGBCheckTool;

class showThread: public QThread
{
    friend class AWGBCheckTool;
    Q_OBJECT
public:
    explicit showThread(QObject * parent = 0);
    ~showThread();
    void setIc(AWGBCheckTool * );
protected:
    void run();
signals:
    void isDone();
private:
    AWGBCheckTool * IC;
};

class AWGBCheckTool : public QMainWindow
{
    friend class showThread;
	Q_OBJECT

public:
    AWGBCheckTool(QWidget *parent = Q_NULLPTR, pGBStart_s param = Q_NULLPTR);
    void showST();
private:
	Ui::AWGBCheckToolClass ui;
    showThread showT;
    void SetList(pGBStart_s);
    void EndList();
private slots:
    void dataChangedSlot(pGBStart_s);
    void ButtonCli(pGBStart_s);
};
