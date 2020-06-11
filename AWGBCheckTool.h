#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QStandardItemModel>
#include "ui_AWGBCheckTool.h"
#include "main.h"

class AWGBCheckTool;

class ShowTree: public QThread
{
    friend class AWGBCheckTool;
public:
    explicit ShowTree(QObject * parent = 0);
    ~ShowTree();
    void setTreeView(QTreeView *);
protected:
    void run();
private:
    QTreeView * treeView;
};

class ShowCheckResThread: public QThread
{
    friend class AWGBCheckTool;
public:
    explicit ShowCheckResThread(QObject * parent = 0);
    ~ShowCheckResThread();
    void setTableView(QTableView *, int handle);
protected:
    void run();
private:
    QTableView *tableView;
    QStandardItemModel *checkResModel;
    int handle;
};

class showThread: public QThread
{
    friend class AWGBCheckTool;
    Q_OBJECT
public:
    explicit showThread(QObject * parent = 0);
    ~showThread();
    void setTextBrowser(QTextBrowser * );
protected:
    void run();
signals:
    void isDone();
    void showSip();
private:
    QTextBrowser * browser;
};

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
    showThread *showT;
    ShowCheckResThread *showCheckResT;
    ShowTree *showTreeT;
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
};
