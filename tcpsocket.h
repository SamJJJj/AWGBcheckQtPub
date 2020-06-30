#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <vector>
#include <QDebug>
#include "jrtplib3/rtptcpaddress.h"
#include "videochannel.h"
using namespace jrtplib;
class TcpListener{
public:
    TcpListener();
    ~TcpListener();
    void start();
    void stop(bool isWait = false);
    void tcpListenerStart();
    int connectType;
    int sign;
    int port;
    QString ip;
    int activePort;
    char *activeIp;
    VideoChannel *channel;  //后面不应该是这种方式
protected:
    bool mIsStop;
    bool mIsThreadRunning;
    SocketType sClient;
    void run();
};

#endif
