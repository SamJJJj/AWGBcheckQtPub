#ifndef VIDEOCHANNEL_H
#define VIDEOCHANNEL_H

#include <stdint.h>
#include <stdio.h>
#include <list>
#include <QString>

#include "mutex.h"
#include "DataListThread.h"

#include <string>
#include "savc/SvacPlayer.h"
#include "savc/ZxSvacDecLib.h"
#include "savc/svac_dec.h"
#include "savc/utils.h"
#define TEST_FRAME_NUM (-5)

extern "C"
{
//    #include "savc/d3doffscreenrender.h"
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/time.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
    #include "libavutil/imgutils.h"
}

class VideoChannelEventHandle;

struct RtpDataNode
{
    uint8_t * buffer;
    int size;
    long time;
    bool isLostPacket; //是否丢帧了

    RtpDataNode()
    {
        buffer = NULL;
        size = 0;
        time = 0;
        isLostPacket = false;
    }
};

///通道数据，这个数据需要通过catalog后获取到的，基本上就是相机的一个通道
class VideoChannel : public DataListThread<RtpDataNode>
{
public:
    VideoChannel();
    ~VideoChannel();

    /**
     * @brief setEventHandle 设置回调函数
     * @param handle
     */
    void setEventHandle(const VideoChannelEventHandle *handle){mVideoChannelEventHandle = (VideoChannelEventHandle *)handle;}

    ///塞入rtp数据等待处理
    void inputRtpBuffer(uint8_t *buffer, int size, uint32_t sequenceNumber, bool isLastPacket);

    void setCallId(int callId){mCallId = callId;}
    void setDId(int did) { this->did = did; }
    int getCallId(){return (this!=NULL)?mCallId:-1;}
    int64_t getTimeStamp_MilliSecond();

    int m_PsBufLen;
    bool m_bStop;
    int m_nNeedSnapCnt;
    int m_nPlayCount;
    int m_port;
    int m_nTotalDecodeFrameCnt;
    int m_nFileFrameCnt;
    unsigned long long m_nFileDecodeTime;
    void *handle;
    int id = 0;
    int frameLen, outFrameLen;
    int filelen = 0;
    int ret;
    char *in_buff, *out_buff;
    QByteArray vkek;
    QString keyVersion;
    QByteArray pubkey;

//    char vkek[16];
//    char vkek_version[32];
    struct auth_info info;
    int videotype;

protected:
    void threadStart();
    void threadStop();
    void dealwithDataNode(const RtpDataNode &node);

public:
    int RtpSSRC; //rtp身份标志，用于invite请求，用来区分多个rtp流。
    std::string DeviceName;
    std::string DeviceID;
    std::string IPAddress;
    int Port;
    int did;
    std::string Status;

private:
    int mCallId; //发送invite的call id, 此id不为-1 说明正在发送invite请求，发送invite过程中，不能再次发送invite请求，否则会有问题。

    uint32_t mReceiveRtpTime;     //接收到rtp数据的时间(用来判断rtp数据接收是否超时了)

    bool mIsCurrentFrameLostPacket;
    uint32_t mLastSequenceNumber; //上一次的rtp序号

    uint8_t *mRtpBuffer; //存放RTP收到的PS流
    int mRtpBufferSize;

    ///ffmpeg解码相关参数
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;
    AVFrame * pFrameRGB;    //存放解码得到的RGB帧
    AVFrame * pFrame;
    uint8_t * rgbBuffer;
    AVPacket packet;
    int getContext = 0;

    SwsContext * img_convert_ctx;
    uint8_t *out_buffer_rgb;
    int numBytes_rgb;

    char *mH264buf;

    bool mIsLastKeyFrameLostPacket; //上一个I帧是否丢包了，是的话，接下来的帧都有可能花屏 接下来所有的帧， 都不传入检测
    bool mIsKeyFrameGetted; //用来记录I帧是否获取到了 否则丢弃得到的h264帧

    bool openH264Decoder();
    void closeH264Decoder();

    void decodeH264Buffer(uint8_t *buffer, int size, bool isLostPacket);



    void decodeSVACBuffer(uint8_t *buffer, int size, int tag);
    HANDLE m_SvacHandle;
    unsigned char *m_pBufDecode;				//临时存储裸码流。
    unsigned char m_psAudioType;
    unsigned char* m_psbuf;
    bool m_FindIFrame;
    SVAC_PREFETCH_PARAM m_param;
    int m_BytePerSample;
    unsigned char* m_buf;
    unsigned char* m_svc_buf;

    bool m_bCanPreview;
//    D3DOffscreenRender* m_render;
    Callback_AddText m_cbAddText;
//    HWND m_hwnd;
    int m_threadnum;
    int m_coreId;
    //CString m_ukey_name;
    //CString m_ukey_pin;
    //CString m_filename;
    //CString m_password;
    //CString m_cert_file_root;
    //CString m_cert_file_self;
    //CString m_crypt_key_version;
    //CString m_crypt_key_for_video;
    int m_decodeCount;


    char* m_ukey_name;
    char* m_ukey_pin;
    char* m_filename;
    char* m_password;
    char* m_cert_file_root;
    char* m_cert_file_self;
    char* m_crypt_key_version;
    char* m_crypt_key_for_video;
    ///用于输出到界面上的回调函数
private:
    VideoChannelEventHandle *mVideoChannelEventHandle; //回调函数 用于传递信息给界面

    void dealWithRgb32Frame(const uint8_t *rgb32Buffer, const int bufferSize,  const int width, const int height, const bool isLostPacket, const bool isKeyFrame); //显示视频数据

};

#endif // VIDEOCHANNEL_H
