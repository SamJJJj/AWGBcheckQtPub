#include <QtWidgets/QApplication>
#include <QProcess>
#include <iostream>
#include "AWQueue.h"
#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include "main.h"

using namespace std;

int main(int argc, char *argv[])
{
    pGBStart_s GB = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    QProcess* pro = new QProcess();
    int handle;
    int ret;
    pro->start("beanstalkd.exe -l 127.0.0.1 -p 11300");
    ret = AW_BSQueue_Init(&handle, nullptr, 0, (unsigned char *)"server",(unsigned char *)"server");
    if(ret == 0)
        cout << "init OK" << endl;
	QApplication a(argc, argv);
    ConfigureGuide w(NULL, GB, handle);
	w.show();
    a.exec();
    pro->kill();
    AW_BSQueue_Destroy(handle);
    cout << "process killed" << endl;
    return 0;
}
