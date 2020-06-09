#include <QtWidgets/QApplication>
#include <iostream>
#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include "main.h"

int main(int argc, char *argv[])
{
    pGBStart_s GB = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    // 开启子线程，先阻塞
	QApplication a(argc, argv);
    ConfigureGuide w(NULL, GB);
	w.show();
	return a.exec();
}
