#include <QtWidgets/QApplication>
#include <QProcess>
#include <iostream>
#include <QMutex>
#include <QTextStream>
#include <QtXml/QDomComment>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>
#include "AWQueue.h"
#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include "main.h"

using namespace std;

//nIw8h7rwsAdxwCov09XFqwxWiGHVoRVEWRWECxxUgbtrVnry18ODkEVZg+pDMlqDwbADtezTYJl1DxluH0P47g==

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file("frontLog.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

//void test()
//{
//    QString s = "nIw8h7rwsAdxwCov09XFqwxWiGHVoRVEWRWECxxUgbtrVnry18ODkEVZg+pDMlqDwbADtezTYJl1DxluH0P47g==";
//    QByteArray a = QByteArray::fromBase64(s.toLocal8Bit());
//    for(int i = 0; i < a.length(); ++i)
//    {
//        printf("%x,",a.data()[i]);
//    }
//    printf("\n");
//}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(outputMessage);
    int ret;
    QString c = "taskkill /im MediaServer.exe /f";
    ret = QProcess::execute(c);
    if(!ret)
    {
        qWarning() << "LastTime application aborted, MediaServer killed, ready to start MediaServer";
    }
    c = "taskkill /im beanstalkd.exe /f";
    ret = QProcess::execute(c);
    if(!ret)
    {
        qWarning() << "LastTime application aborted, MediaServer killed, ready to start beanstalkd";
    }
    c = "taskkill /im AWGBBase.exe /f";
    ret = QProcess::execute(c);
    if(!ret)
    {
        qWarning() << "LastTime application aborted, MediaServer killed, ready to start AWGBBase";
    }
    pGBStart_s GB = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    QProcess* proBeanstalk = new QProcess();
    QProcess* proMS = new QProcess();
    QProcess* proBase = new QProcess();
    int handle;
    proMS->start("MediaServer.exe");
    proBeanstalk->start("beanstalkd.exe -l 127.0.0.1 -p 11300");
    proBase->start("AWGBBase.exe");
    ret = AW_BSQueue_Init(&handle, (unsigned char *)"127.0.0.1", 0, (unsigned char *)"Server",(unsigned char *)"Client");
    if(ret == 0)
        qInfo() << "AWQueue init OK";
    else {
        qCritical() << "AWQueue init failed!!!";
    }
	QApplication a(argc, argv);
    ConfigureGuide w(NULL, GB, handle);
//    test();
	w.show();
    a.exec();
    c = "taskkill /im MediaServer.exe /f";
    ret = QProcess::execute(c);
    if(!ret)
    {
        qInfo() << "MediaServer exited";
    }
    else {
        qWarning() << "MediaServer aborted";
    }
    c = "taskkill /im beanstalkd.exe /f";
    QProcess::execute(c);
    if(!ret)
    {
        qInfo() << "beanstalkd exited";
    }
    else {
        qWarning() << "beanstalkd aborted";
    }
    c = "taskkill /im AWGBBase.exe /f";
    if(!ret)
    {
        qInfo() << "AWGBBase exited";
    }
    else {
        qWarning() << "AWGBBase aborted";
    }
    QProcess::execute(c);
    AW_BSQueue_Destroy(handle);
    qInfo() << "Application exited";
    return 0;
}
