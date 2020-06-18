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
    int activePort;
    char *activeIp;
protected:
    bool mIsStop;
    bool mIsThreadRunning;
    SocketType sClient;
    void run();
};

#endif
