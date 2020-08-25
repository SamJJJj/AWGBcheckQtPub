#ifndef RTPRECEIVER_H
#define RTPRECEIVER_H
#include <stdint.h>
#include <string.h>
#include <iostream>
#include "videochannel.h"

#define MEDIASERVER_RTP_PORT 9000

class RtpReciever
{
public:
    RtpReciever();
    ~RtpReciever();

public:
    void start();
    void setPort(int);
    void stop(bool isWait = false);
    VideoChannel *channel;  //后面不应该是这种方式
    bool mIsStoped;
signals:
    void downLoadOK();
protected:
    bool mIsThreadRunning;
    int port;
    void run();

public:
    /**
     * @brief GetH264FromPs   rtp解析h264视频信息
     * @param buffer          输入rtp数据（国标的ps封装）
     * @param length          长度
     * @param h264Buffer      输出的h264数据
     * @param h264length_out
     * @return
     */
    static int GetH264FromPs(char* buffer, int length, char *h264Buffer, int *h264length_out);

};

#endif // RTPRECEIVER_H
