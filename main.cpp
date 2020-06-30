#include <QtWidgets/QApplication>
#include <QProcess>
#include <iostream>
#include <QTextStream>
#include <QtXml/QDomComment>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>
#include "AWQueue.h"
#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include "main.h"

using namespace std;

int main(int argc, char *argv[])
{
    QString c = "taskkill /im MediaServer.exe /f";
    QProcess::execute(c);
    c = "taskkill /im beanstalkd.exe /f";
    QProcess::execute(c);
    c = "taskkill /im AWGBBase.exe /f";
    QProcess::execute(c);
    pGBStart_s GB = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    QProcess* proBeanstalk = new QProcess();
    QProcess* proMS = new QProcess();
    QProcess* proBase = new QProcess();
    int handle;
    int ret;
    proMS->start("MediaServer.exe");
    proBeanstalk->start("beanstalkd.exe -l 127.0.0.1 -p 11300");
    proBase->start("AWGBBase.exe");
    ret = AW_BSQueue_Init(&handle, (unsigned char *)"127.0.0.1", 0, (unsigned char *)"Server",(unsigned char *)"Client");
    if(ret == 0)
        cout << "init OK" << endl;
	QApplication a(argc, argv);
    ConfigureGuide w(NULL, GB, handle);
//    test();
	w.show();
    a.exec();
    proBeanstalk->kill();
    proMS->kill();
    proBase->kill();
    AW_BSQueue_Destroy(handle);
    cout << "process killed" << endl;
    return 0;
}
