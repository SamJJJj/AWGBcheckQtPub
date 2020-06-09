#include "AWGBCheckTool.h"

showThread::showThread(QObject*)
{
    ;
}

showThread::~showThread()
{
    ;
}

void showThread::run()
{
    int i = 0;
    while(i < 30){
        IC->ui.textBrowser->append(QString("pppppppppppppppp"));
        QApplication::processEvents();
        i++;
        Sleep(500);
    }
    emit isDone();
}


void showThread::setIc(AWGBCheckTool *ic)
{
    IC = ic;
}

