#include<QTextCursor>
#include <QMetaType>
#include <QTextCursor>
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
        browser->append(QString("pppppppppppppppp"));
//        QApplication::processEvents();
        i++;
        browser->moveCursor(QTextCursor::End);
        Sleep(500);

    }
    emit isDone();
}


void showThread::setTextBrowser(QTextBrowser *textBrowser)
{
    browser = textBrowser;
}

