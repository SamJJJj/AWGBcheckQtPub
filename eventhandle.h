#ifndef EVENTHANDLE_H
#define EVENTHANDLE_H

/*
*   此头文件系输出到界面的回调方法类
*/
#include <sys/types.h>
#include <stdint.h>

class VideoChannelEventHandle
{
    ///用于输出到界面上的回调函数
public:
    virtual void onDisPlayVideoData(const uint8_t *rgb32Buffer, const int bufferSize,  const int width, const int height) = 0; //显示视频数据

};

#endif // EVENTHANDLE_H
