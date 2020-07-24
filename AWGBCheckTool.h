#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QStandardItemModel>
#include <QProcess>
#include "rtpreceiver.h"
#include "tcpsocket.h"
#include "ui_AWGBCheckTool.h"
#include "main.h"
#include "showvideo.h"

class AWGBCheckTool;
class RtpReciever;


class GetAndParseThread : public QThread
{
    Q_OBJECT
public:
    explicit GetAndParseThread(QObject * parent = 0);
    ~GetAndParseThread();
    void init(QStandardItemModel*, QStandardItemModel*, int, QString *, QString *, QString *, QString *, QString *, QStandardItemModel*);
    void setMethod(const QString & );
    void clearList();
    void clearRecordFile();
    QString bvkek;
    QString keyVersion;
    QString bPubKey;
protected:
    void run();
signals:
    void toTree();
    void toTable();
    void toText();
    void push();
    void UDP();
    void TCP();
    void TCPActive();
    void svacKey();
    void registerOk();
    void unRegisterOk();
    void stopPush();
private:
    int handle;
    int tableId;
    int fileId;
    QString method;
    QString *ip;
    QString *port;
    QString *pushMethod;
    QString *pushId;
    QStandardItemModel *checkResModel;
    QStandardItemModel *treeModel;
    QString *sipMessages;
    QStandardItemModel *recordFileModel;
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
    TcpListener *tcpListener;
    QStandardItemModel *treeModel;
    ShowVideo* video;
    ShowVideo* recordVideo;
    QStandardItemModel *checkResModel;
    QStandardItemModel *fileModel;
    QString *sipMessage;
    pGBStart_s currGBInfo;
    QString *ip;
    QString *port;
    QString *pushMethod;
    QString *pushId;
    QProcess *pro;
    bool isRecordPlay;
    int seq;
    int handle;
    void SetList(pGBStart_s);
    void EndList();
    void stopPush();
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
    void gotoMatchBuffer();
    void pushStream();
    void stopVideo();
    void UDPPlay();
    void TCPPlay();
    void TCPActivePlay();
    void clearInfo();
    void clearList();
    void reMain();
    void recordInfo();
    void changeRegisterStatus();
    void recordPlay();
    void recordStop();
    void subscribe();
    void unSubscribe();
};
