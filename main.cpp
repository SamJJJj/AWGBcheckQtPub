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

void test()
{
    char * buf = "<Request><CmdType>SipShowBuffer</CmdType></Request>";
    QString str;
    QTextStream stream(&str);
    QDomDocument doc;
    doc.setContent(QString((char*)buf));
    QDomNodeList list = doc.elementsByTagName("CmdType");
    if(list.at(0).toElement().childNodes().at(0).toText().save(stream, 0), str == QString("SipShowBuffer"))
    {
        cout << "parseOK" << endl;
    }
    cout << str.toStdString() << endl;

}

int main(int argc, char *argv[])
{
    pGBStart_s GB = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    QProcess* pro = new QProcess();
    int handle;
    int ret;
//    pro->start("beanstalkd.exe -l 127.0.0.1 -p 11300");
    ret = AW_BSQueue_Init(&handle, (unsigned char *)"127.0.0.1", 0, (unsigned char *)"Server",(unsigned char *)"Client");
    if(ret == 0)
        cout << "init OK" << endl;
	QApplication a(argc, argv);
    ConfigureGuide w(NULL, GB, handle);
//    test();
	w.show();
    a.exec();
//    pro->kill();
    AW_BSQueue_Destroy(handle);
    cout << "process killed" << endl;
    return 0;
}
