#pragma optimize("", off)
#include "rtpreceiver.h"
#include "videochannel.h"

#include <iostream>
#include <thread>
#include <QDebug>

#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtppacket.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"
#include "jrtplib3/rtpsourcedata.h"
#include "jrtplib3/rtpmemoryobject.h"


using namespace jrtplib;


#pragma pack (1)
typedef union littel_endian_size_s {
    unsigned short int	length;
    unsigned char		m_byte[2];
} littel_endian_size;

typedef struct pack_start_code_s {
    unsigned char start_code[3];
    unsigned char stream_id[1];
} pack_start_code;

typedef struct program_stream_pack_header_s {
    pack_start_code PackStart;// 4
    unsigned char Buf[9];
    unsigned char stuffinglen;
} program_stream_pack_header;

typedef struct program_stream_map_s {
    pack_start_code PackStart;
    littel_endian_size PackLength;//we mast do exchange
} program_stream_map;

typedef struct program_stream_e_s {
    pack_start_code		PackStart;
    littel_endian_size	PackLength;//we mast do exchange
    char				PackInfo1[2];
    unsigned char		stuffing_length;
} program_stream_e;


#define H264_FRAME_SIZE_MAX (1024*1024*2)

int inline ProgramStreamPackHeader(char* Pack, int length, char **NextPack, int *leftlength)
{
    //printf("[%s]%x %x %x %x\n", __FUNCTION__, Pack[0], Pack[1], Pack[2], Pack[3]);
    //通过 00 00 01 ba头的第14个字节的最后3位来确定头部填充了多少字节
    program_stream_pack_header *PsHead = (program_stream_pack_header *)Pack;
    unsigned char pack_stuffing_length = PsHead->stuffinglen & '\x07';

    *leftlength = length - sizeof(program_stream_pack_header)-pack_stuffing_length;//减去头和填充的字节
    *NextPack = Pack + sizeof(program_stream_pack_header)+pack_stuffing_length;
    if (*leftlength<4)
        return 0;

    return *leftlength;
}

inline int ProgramStreamMap(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen)
{
    program_stream_map* PSMPack = (program_stream_map*)Pack;

    //no payload
    *PayloadData = 0;
    *PayloadDataLen = 0;

    if ((unsigned int)length < sizeof(program_stream_map)) return 0;

    littel_endian_size psm_length;
    psm_length.m_byte[0] = PSMPack->PackLength.m_byte[1];
    psm_length.m_byte[1] = PSMPack->PackLength.m_byte[0];

    *leftlength = length - psm_length.length - sizeof(program_stream_map);
    if (*leftlength <= 0) return 0;

    *NextPack = Pack + psm_length.length + sizeof(program_stream_map);

    return *leftlength;
}

inline int ProgramShHead(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen)
{
    program_stream_map* PSMPack = (program_stream_map*)Pack;

    //no payload
    *PayloadData = 0;
    *PayloadDataLen = 0;

    if ((unsigned int)length < sizeof(program_stream_map)) return 0;

    littel_endian_size psm_length;
    psm_length.m_byte[0] = PSMPack->PackLength.m_byte[1];
    psm_length.m_byte[1] = PSMPack->PackLength.m_byte[0];

    *leftlength = length - psm_length.length - sizeof(program_stream_map);
    if (*leftlength <= 0)
        return 0;

    *NextPack = Pack + psm_length.length + sizeof(program_stream_map);

    return *leftlength;
}

inline int Pes(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen)
{
    program_stream_e* PSEPack = (program_stream_e*)Pack;

    *PayloadData = 0;
    *PayloadDataLen = 0;

    if ((unsigned int)length < sizeof(program_stream_e)) return 0;

    littel_endian_size pse_length;
    pse_length.m_byte[0] = PSEPack->PackLength.m_byte[1];
    pse_length.m_byte[1] = PSEPack->PackLength.m_byte[0];

    *PayloadDataLen = pse_length.length - 2 - 1 - PSEPack->stuffing_length;
    if (*PayloadDataLen>0)
        *PayloadData = Pack + sizeof(program_stream_e)+PSEPack->stuffing_length;

    *leftlength = length - pse_length.length - sizeof(pack_start_code)-sizeof(littel_endian_size);
    if (*leftlength <= 0) return 0;

    *NextPack = Pack + sizeof(pack_start_code)+sizeof(littel_endian_size)+pse_length.length;

    return *leftlength;
}

//查错
static void checkerror(int rtperr)
{
    if (rtperr < 0)
    {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        exit(-1);
    }
}

RtpReciever::RtpReciever()
{
    mIsStoped = true;
    mIsThreadRunning = false;
    channel = new VideoChannel;
}

RtpReciever::~RtpReciever()
{

}

void RtpReciever::start()
{
    mIsStoped = false;
    channel->start();
    qInfo() << "RtpReceiver Started!";
    //启动新的线程实现读取视频文件
    std::thread([&](RtpReciever *pointer)
    {
        pointer->run();

    }, this).detach();
}


void RtpReciever::stop(bool isWait)
{
    mIsStoped = true;
    channel->stop();
    qInfo() << "RtpReceiver stopped";
    if (isWait)
    {
        while(mIsThreadRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void RtpReciever::run()
{

#ifdef RTP_SOCKETTYPE_WINSOCK
       WSADATA dat;
       WSAStartup(MAKEWORD(2, 2), &dat);
#endif // RTP_SOCKETTYPE_WINSOCK

    mIsThreadRunning = true;

    RTPSession sess;
    uint16_t portbase;

    int status;
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    //解析帧数
    sessparams.SetOwnTimestampUnit(1.0 / 9000);
//    sessparams.SetUsePollThread(true);
    portbase = port;
    transparams.SetRTPReceiveBuffer(1024 * 1024 *30); //10M

    sessparams.SetAcceptOwnPackets(true);
    transparams.SetPortbase(portbase);
    status = sess.Create(sessparams, &transparams);
    if (status < 0)
    {
        std::cout << "ERROR: " << RTPGetErrorString(status) << std::endl;
        exit(1);
    }
    //开始接收流包
    uint32_t ssrcbay=0;
    qInfo() << "Ready to receive rtp packet";
    while(!mIsStoped)
//    while(1)
    {
        Sleep(100);

        sess.BeginDataAccess();
        // check incoming packets
        if (sess.GotoFirstSource())
        {
            do
            {
                RTPPacket *pack;

                while ((pack = sess.GetNextPacket()) != NULL)
                {
                    RTPSourceData *mRTPSourceData = sess.GetCurrentSourceInfo();
//                    std::cout << "get data!" << std::endl;
                    uint32_t ssrc = mRTPSourceData->GetSSRC();
                    if (ssrcbay == 0) {
                        ssrcbay = ssrc;
                    }
//                    ssrc不需要传
                    if (ssrc != ssrcbay) continue;
                    int cameraId = ssrc-100000000; //这里的ssrc就是cameraId 在invite的时候传给相机的

//                    VideoChannel* channel = AppConfig::gGB28181Server->getVideoChannel(cameraId);
//                      也要通过cameraId联系起来

//fprintf(stderr,"%s 111 ssrc=%d %d channel=%d\n", __FUNCTION__, ssrc, cameraId, channel);

//                    if (channel != nullptr)
//                    {
                        ///////////////////////////////////////
                   if (mIsStoped == 1) {
                        sess.DeletePacket(pack);
                        break;
                    }
                    ///////////////////////////////////////
                    //将buffer传给解析器，这里以后要传一个特殊的channel区分设备
                    //因为以后可能有多个channel同时接收
//                   std::cout<< "---- reach inputRtpBufer ! ----" << std::endl;
                     channel->inputRtpBuffer(pack->GetPayloadData(), pack->GetPayloadLength(), pack->GetSequenceNumber(), pack->HasMarker());
//                    }

                // we don't longer need the packet, so
                // we'll delete it
                sess.DeletePacket(pack);
            }
            if (mIsStoped == 1) {
                break;
            }
        } while (sess.GotoNextSource());
        //}
        sess.EndDataAccess();
//        RTPTime::Wait(RTPTime(0, 10));
    }
    }
    struct timeval Timeout;
    Timeout.tv_sec = 0;
    Timeout.tv_usec = 500;
//    std::cout << "------------- destroyed 1 ----------" << std::endl;
    sess.BYEDestroy(RTPTime(Timeout.tv_sec, Timeout.tv_usec), 0, 0);
//    std::cout << "------------- destroyed ----------" << std::endl;
    //推流结束应该在这里，表示下载完成！！
//    emit downLoadOK();
    qInfo() << "Rtp session destroyed";
#ifdef WIN32
    WSACleanup();
//    std::cout << "------------- destroyed end----------" << std::endl;
#endif
    mIsThreadRunning = false;
}

//void RtpReciever::run()
//{
//////#ifdef RTP_SOCKETTYPE_WINSOCK
//////       WSADATA dat;
//////       WSAStartup(MAKEWORD(2, 2), &dat);
//////#endif // RTP_SOCKETTYPE_WINSOCK

////    mIsThreadRunning = true;

////    RTPSession sess;
////    uint16_t portbase;

////    int status;
////    RTPUDPv4TransmissionParams transparams;
////    RTPSessionParams sessparams;
////    //解析帧数
////    sessparams.SetOwnTimestampUnit(1.0 / 9000);
//////    sessparams.SetUsePollThread(true);
////std::cout << "------------- reciver 0-1 ----------" << std::endl;
////    portbase = port;
//////    transparams.SetRTPReceiveBuffer(1024 * 1024 *10); //10M
//////    sessparams.SetAcceptOwnPackets(true);
//////    transparams.SetPortbase(portbase);
//////    status = sess.Create(sessparams, &transparams);
//////    std::cout << "status :" << status << std::endl;
//////    Sleep(2000);
//////    sess.BYEDestroy(RTPTime(10, 0), 0, 0);
////    std::cout << "------------- destroyed end----------" << std::endl;
////    mIsThreadRunning = false;
//}


int RtpReciever::GetH264FromPs(char* buffer, int length, char *h264Buffer, int *h264length)
{
    int leftlength = 0;
    char *NextPack = 0;

    *h264length = 0;

    if (ProgramStreamPackHeader(buffer, length, &NextPack, &leftlength) == 0)
        return 0;

    char *PayloadData = NULL;
    int PayloadDataLen = 0;

    while ((unsigned int)leftlength >= sizeof(pack_start_code))
    {
        PayloadData = NULL;
        PayloadDataLen = 0;

        if (NextPack
            && NextPack[0] == '\x00'
            && NextPack[1] == '\x00'
            && NextPack[2] == '\x01'
            && NextPack[3] == '\xE0')
        {
            //接着就是流包，说明是非i帧
            if (Pes(NextPack, leftlength, &NextPack, &leftlength, &PayloadData, &PayloadDataLen))
            {
                if (PayloadDataLen)
                {
                    if (PayloadDataLen + *h264length < H264_FRAME_SIZE_MAX)
                    {
                        int len = PayloadData - NextPack;
                        int len2 = PayloadData - buffer;

                        if ((PayloadDataLen+len2) > length)
                        {
                            PayloadDataLen = length - len2;
                        }

                        memcpy(h264Buffer, PayloadData, PayloadDataLen);
                        h264Buffer += PayloadDataLen;
                        *h264length += PayloadDataLen;
                    }
                    else
                    {
//                        qDebug("h264 frame size exception!! %d:%d", PayloadDataLen, *h264length);
                    }
                }
            }
            else
            {
                if (PayloadDataLen)
                {
                    if (PayloadDataLen + *h264length < H264_FRAME_SIZE_MAX)
                    {
                        int len = PayloadData - NextPack;
                        int len2 = PayloadData - buffer;

                        if ((PayloadDataLen+len2) > length)
                        {
                            PayloadDataLen = length - len2;
                        }

                        memcpy(h264Buffer, PayloadData, PayloadDataLen);
                        h264Buffer += PayloadDataLen;
                        *h264length += PayloadDataLen;
                    }
                    else
                    {
//                        qDebug("h264 frame size exception!! %d:%d", PayloadDataLen, *h264length);
                    }
                }
                break;
            }
        }
        else if (NextPack
            && NextPack[0] == '\x00'
            && NextPack[1] == '\x00'
            && NextPack[2] == '\x01'
            && NextPack[3] == '\xBB')
        {
            if (ProgramShHead(NextPack, leftlength, &NextPack, &leftlength, &PayloadData, &PayloadDataLen) == 0)
                break;
        }
        else if (NextPack
            && NextPack[0] == '\x00'
            && NextPack[1] == '\x00'
            && NextPack[2] == '\x01'
            && NextPack[3] == '\xBC')
        {
            if (ProgramStreamMap(NextPack, leftlength, &NextPack, &leftlength, &PayloadData, &PayloadDataLen) == 0)
                break;
        }
        else if (NextPack
            && NextPack[0] == '\x00'
            && NextPack[1] == '\x00'
            && NextPack[2] == '\x01'
            && (NextPack[3] == '\xC0' || NextPack[3] == '\xBD'))
        {
//            printf("audio ps frame, skip it\n");
            break;
        }
        else
        {
//            printf("[%s]no know %x %x %x %x\n", sipId, NextPack[0], NextPack[1], NextPack[2], NextPack[3]);
            break;
        }
    }

    return *h264length;
}

void RtpReciever::setPort(int p)
{
    port = p;
}
