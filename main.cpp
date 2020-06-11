#include <QtWidgets/QApplication>
#include <QProcess>
#include <iostream>
#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include "main.h"

using namespace std;
int main(int argc, char *argv[])
{
    pGBStart_s GB = (pGBStart_s)calloc(1, sizeof(GBStart_s));
	QApplication a(argc, argv);
    ConfigureGuide w(NULL, GB);
    QProcess* pro = new QProcess();
    pro->start("beanstalkd.exe");
    if(!pro->FailedToStart)
    {
        cout << "started" << endl;
    }
    pro->kill();
	w.show();
    a.exec();
    return 0;
}
