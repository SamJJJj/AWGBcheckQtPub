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
    while(i < 30000){
        browser->append(QString::number(i));
//        QApplication::processEvents();
        i++;
        browser->moveCursor(QTextCursor::End);
        Sleep(1000);

    }
    emit isDone();
}


void showThread::setTextBrowser(QTextBrowser *textBrowser)
{
    browser = textBrowser;
}

