#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QStandardItemModel>
#include "rtpreceiver.h"
#include "ui_AWGBCheckTool.h"
#include "main.h"

class AWGBCheckTool;
class RtpReciever;


class GetAndParseThread : public QThread
{
    Q_OBJECT
public:
    explicit GetAndParseThread(QObject * parent = 0);
    ~GetAndParseThread();
    void init(QStandardItemModel*, QStandardItemModel*, int, QVector<QString> *);
protected:
    void run();
signals:
    void toTree();
    void toTable();
    void toText();
private:
    int handle;
    int tableId;
    QStandardItemModel *checkResModel;
    QStandardItemModel *treeModel;
    QVector<QString> *sipMessages;

};

//class ShowTree: public QThread
//{
//    friend class AWGBCheckTool;
//    Q_OBJECT
//public:
//    explicit ShowTree(QObject * parent = 0);
//    ~ShowTree();
//    void setTreeView(QStandardItemModel *);
//protected:
//    void run();
//signals:
//    void showTree();
//private:
//    QStandardItemModel * treeModel;
//};

//class ShowCheckResThread: public QThread
//{
//    friend class AWGBCheckTool;
//    Q_OBJECT
//public:
//    explicit ShowCheckResThread(QObject * parent = 0);
//    ~ShowCheckResThread();
//    void setTableView(QStandardItemModel *, int handle);
//protected:
//    void run();
//signals:
//    void showCheck();
//private:
//    QStandardItemModel *checkResModel;
//    int handle;
//};

//class ShowThread: public QThread
//{
//    friend class AWGBCheckTool;
//    Q_OBJECT
//public:
//    explicit ShowThread(QObject * parent = 0);
//    ~ShowThread();
//    void setTextBrowser(QTextBrowser * );
//protected:
//    void run();
//signals:
//    void isDone();
//    void showSip();
//private:
//    QTextBrowser * browser;
//};

class AWGBCheckTool : public QMainWindow
{
    friend class showThread;
    friend class ShowCheckResThread;
	Q_OBJECT

public:
    AWGBCheckTool(QWidget *parent = Q_NULLPTR, pGBStart_s param = Q_NULLPTR, int handle = 0);
    ~AWGBCheckTool();
    void showST();
    void showTreeView();
    void showCheckRes();
private:
	Ui::AWGBCheckToolClass ui;
    GetAndParseThread *getThread;
    RtpReciever *udpReceiver;
    QStandardItemModel *treeModel;
    QStandardItemModel *checkResModel;
//    QHash<int, QString> sipMessage;
    QVector<QString> sipMessage;
    int seq;
    int handle;
    void SetList(pGBStart_s);
    void EndList();
private slots:
    void dataChangedSlot();
    void ButtonCli(pGBStart_s);
    void showInCurrentInterface();
    void showTreeInCurrentInterface();
    void deviceRegister();
    void deviceCatalog();
    void toControlPage();
    void toListPage();
    void toVideoPage();
    void toSipPage();
    void setTree();
    void setCheckRes();
    void prePage();
    void nextPage();
    void setTextBrowser();
    void playVideo();
};
