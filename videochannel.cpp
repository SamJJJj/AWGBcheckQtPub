#include "videochannel.h"


#include "rtpreceiver.h"
#include "eventhandle.h"
#include <sys/time.h>
#include <iostream>

using namespace std;
#define MAX_PS_BUFFER_SIZE 2048000
#define H264_FRAME_SIZE_MAX (1024*1024*2)



char* g_libPath;
CritSec g_svac_critsec;
#define PSBUFLEN (2 * 1024 * 1024)

AUTH_FUN_INIT(ZXSVACDec_Init);
AUTH_FUN_INIT(ZXSVACDec_Open);
AUTH_FUN_INIT(ZXSVACDec_Close);
AUTH_FUN_INIT(ZXSVACDec_PrefetchParam);
AUTH_FUN_INIT(ZXSVACDec_Decode);
AUTH_FUN_INIT(ZXSVACDec_DecodeAudio);
AUTH_FUN_INIT(ZXSVACDec_SetPassWord);
AUTH_FUN_INIT(ZXSVACDec_SetTrustCerts);
AUTH_FUN_INIT(ZXSVACDec_SetRSAKey);
AUTH_FUN_INIT(ZXSVACDec_SetVKEKBase64);
AUTH_FUN_INIT(ZXSVACDec_SetCommonExtensionInfo);
AUTH_FUN_INIT(ZXSVACDec_GetSdkVersion);
AUTH_FUN_INIT(ZXSVACDec_GetSECInfo);
AUTH_FUN_INIT(ZXSVACDec_SetSecurityLib);
AUTH_FUN_INIT(ZXSVACDec_GetSupportUKey);
AUTH_FUN_INIT(ZXSVACDec_SetUKey);

//extern unsigned char *vkek_bak;
//extern unsigned int vkek_lenbak;
//extern unsigned char *pKeyversion_bak;
//extern unsigned char *Pubkey_bak;

//extern int videotype;



//int videotype = 0;

int loadsvacdll()
{
    g_svac_critsec.Lock();
    static int g_load_svac_dll = 0;
    if (g_load_svac_dll == 0)
    {

        char* dllname = "ZxSvacDecLib.dll";
        HINSTANCE m_hInst = NULL;
        char* csStr = g_libPath;
        m_hInst = LoadLibrary(L".\\ZxSvacDecLib.dll");
        if (m_hInst == NULL)
        {
            if (m_hInst == NULL)
            {
                int err = GetLastError();
                char te[128];
                sprintf_s(te, 128, "LoadLibrary %s failed! error = %d\n", dllname, err);
                return -1;
            }
        }


        AUTH_GET_FUN(ZXSVACDec_Init);
        AUTH_GET_FUN(ZXSVACDec_Open);
        AUTH_GET_FUN(ZXSVACDec_Close);
        AUTH_GET_FUN(ZXSVACDec_PrefetchParam);
        AUTH_GET_FUN(ZXSVACDec_Decode);
        AUTH_GET_FUN(ZXSVACDec_DecodeAudio);
        AUTH_GET_FUN(ZXSVACDec_SetPassWord);
        AUTH_GET_FUN(ZXSVACDec_SetTrustCerts);
        AUTH_GET_FUN(ZXSVACDec_SetRSAKey);
        AUTH_GET_FUN(ZXSVACDec_SetVKEKBase64);
        AUTH_GET_FUN(ZXSVACDec_SetCommonExtensionInfo);
        AUTH_GET_FUN(ZXSVACDec_GetSdkVersion);
        AUTH_GET_FUN(ZXSVACDec_GetSECInfo);
        AUTH_GET_FUN(ZXSVACDec_SetSecurityLib);
        AUTH_GET_FUN(ZXSVACDec_GetSupportUKey);
        AUTH_GET_FUN(ZXSVACDec_SetUKey);

        g_pfZXSVACDec_Init();

    }
    g_svac_critsec.UnLock();

    g_load_svac_dll = 1;
    return 0;
}
VideoChannel::VideoChannel()
{
    Port = 0;

    RtpSSRC = 1;
    did = -1;
    mCallId = -1;

    mLastSequenceNumber = 0;
    mIsCurrentFrameLostPacket = false;

    mReceiveRtpTime = 0;

    mRtpBuffer = (uint8_t*)malloc(MAX_PS_BUFFER_SIZE);
    mRtpBufferSize = 0;

    pCodecCtx = nullptr;
    pCodec = nullptr;

    pCodecCtx = nullptr;
    pFrameRGB = nullptr;
    pFrame = nullptr;

    out_buffer_rgb = nullptr;
    numBytes_rgb = 0;

    mH264buf = nullptr;
    mVideoChannelEventHandle = nullptr;
    av_register_all();
    m_FindIFrame = false;

    m_SvacHandle = HANDLE(-1);
    memset(&m_param, 0, sizeof(SVAC_PREFETCH_PARAM));
    m_buf = NULL;
    m_svc_buf = NULL;
    m_BytePerSample = 0;

    //m_pAudioDecodeBuf = NULL;
    m_psAudioType = 0;

    loadsvacdll();

    m_psbuf = new unsigned char[2 * 1024 * 1024];
    //m_nPsBufLen = 0;
    //m_port = port;
    //m_hwnd = hwnd;
//    m_render = NULL;
    //m_hThread = NULL;
    m_bStop = false;
    m_nPlayCount = 0;

    //m_nFrameGop = 0;
    //m_nFrameIndex = 0;


    //m_pPSbuf = NULL;
    m_buf = NULL;
    m_cbAddText = NULL;
    m_threadnum = 0;
    m_coreId = -1;


    m_nNeedSnapCnt = -1;
    //m_bSaveYUVFile = true;
    m_bCanPreview = true;

    m_nTotalDecodeFrameCnt = 0;
    m_nFileFrameCnt = 0;
    m_nFileDecodeTime = 0;

    //m_strMulticastIP = "";
    //m_nMulticastPort = 34444;
    m_pBufDecode = NULL;

    m_ukey_name = "";
    m_ukey_pin = "";

    in_buff = (char*)malloc(2 * 1024 * 1024);
    out_buff = (char*)malloc(2 * 1024 * 1024);
    handle = SvacDecCreate();

}

VideoChannel::~VideoChannel()
{
    if (mRtpBuffer != nullptr)
        free(mRtpBuffer);
    mRtpBuffer = nullptr;
    free(in_buff);
    free(out_buff);
    SvacDecRelease(handle);
}

void VideoChannel::threadStart()
{
    mH264buf = (char *)malloc(H264_FRAME_SIZE_MAX);

    mIsLastKeyFrameLostPacket = false; //上一个I帧是否丢包了，是的话，接下来的帧都有可能花屏 接下来所有的帧， 都不传入检测
    mIsKeyFrameGetted = false; //用来记录I帧是否获取到了 否则丢弃得到的h264帧

    openH264Decoder();
}

void VideoChannel::threadStop()
{
    closeH264Decoder();

    if (mH264buf != nullptr)
        free(mH264buf);
}


#if 1
void VideoChannel::dealwithDataNode(const RtpDataNode &node)
{
    unsigned char *vkek_bak=(unsigned char *) calloc(16,1);
    unsigned int vkek_lenbak=16;
    unsigned char *pKeyversion_bak=(unsigned char *) calloc(16,1);
    unsigned char *Pubkey_bak=(unsigned char *) calloc(16,1);
    unsigned char *ptr = node.buffer;
    int psLen = node.size;

    if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x01 && ptr[3] == 0xBA && psLen > 0)
    {
        int h264length = 0;

        RtpReciever::GetH264FromPs((char *)ptr, psLen, mH264buf, &h264length);
        if (videotype == 2)
        {
            if (h264length > 0)
            {

                memcpy(in_buff, mH264buf, h264length);
//                memcpy(vkek, vkek_bak, vkek_lenbak);
//                memcpy(vkek_version, pKeyversion_bak, sizeof(vkek_version));
//                vkek_version[19] = '\0';


//                SvacDecSetVkek(handle, vkek, vkek_version, 32);
//                SvacDecSetPubkey(handle, (char*)Pubkey_bak);
                if (id == TEST_FRAME_NUM)
                    return;

                ret = SvacDecProcess(handle, in_buff, h264length, out_buff, &outFrameLen, &info);
                if (ret)
                {
                    return;
                }
                else
                {
                    if (outFrameLen > 0)
                    {
                        memcpy(mH264buf, out_buff, outFrameLen);
                        h264length = outFrameLen;

                    }
                }

            }
        }


            bool isKeyFrame = false;
            int pos = 0;
            while (1)
            {
                if ((h264length - pos) < 4)
                {
                    break;
                }

                unsigned char *buf = (unsigned char *)(mH264buf + pos);

                int index = 0;

                if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x01)
                {
                    index = 3;
                }
                else if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x01)
                {
                    index = 4;
                }
                else
                {
                    pos++;
                    continue;
                }

                if (videotype == 0)
                {
                    int nal_unit_type = (buf[index]) & 0x1f;                   // 5 bit
                    if (nal_unit_type == 7)
                    {
                        mIsLastKeyFrameLostPacket = node.isLostPacket;
                        isKeyFrame = true;
                        mIsKeyFrameGetted = true;
                        break;
                    }
                }
                if (videotype == 2)
                {
                    int nal_unit_type = ((buf[index]) & 0x3c) >> 2;                   // 5 bit
                    if (nal_unit_type == 2 || nal_unit_type == 4)
                    {
                        mIsLastKeyFrameLostPacket = node.isLostPacket;
                        isKeyFrame = true;
                        int tag = 1;
                        mIsKeyFrameGetted = true;
                        break;
                    }
                    else if (nal_unit_type == 1 || nal_unit_type == 3)
                    {
                        isKeyFrame = 0;
                        break;
                    }
                }

                pos++;
            }

            if (mIsKeyFrameGetted)
            {
                bool isLostPacket = node.isLostPacket;

                if (node.isLostPacket)
                {
                    fprintf(stderr, "lost packet! isKeyFrame=%d\n", isKeyFrame);
                }

                if (mIsLastKeyFrameLostPacket) //I帧丢了，接下来的帧都不检测，直到遇到下一个I帧
                {
                    isLostPacket = true;
                }

                if (videotype == 0)
                {
                    decodeH264Buffer((uint8_t*)mH264buf, h264length, isLostPacket);
                }
                else if (videotype == 2)
                {
                    decodeSVACBuffer((uint8_t*)mH264buf, h264length, isKeyFrame);
                }
            }

    }

    free(node.buffer);
}
#endif

#if 0
void VideoChannel::dealwithDataNode(const RtpDataNode &node)
{
    unsigned char *ptr = node.buffer;
    int psLen = node.size;

    if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x01 && ptr[3] == 0xBA && psLen > 0)
    {
        int h264length = 0;

        RtpReciever::GetH264FromPs((char *)ptr, psLen, mH264buf, &h264length);

        if (h264length > 0)
        {
            bool isKeyFrame = false;

//                ///还没获取到关键帧 则先判断是否是关键帧 否则丢弃
//                if (!isKeyFrameGetted)
            {
                int pos = 0;
                while(1)
                {
                    if ((h264length - pos) < 4)
                    {
                        break;
                    }

                    unsigned char *buf = (unsigned char *)(mH264buf+pos);

                    int index = 0;

                    if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x01)
                    {
                        index = 3;
                    }
                    else if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x01)
                    {
                        index = 4;
                    }
                    else
                    {
                        pos++;
                        continue;
                    }

                    int nal_unit_type = (buf[index]) & 0x1f;                   // 5 bit

                    if (nal_unit_type == 7)
                    {
                        mIsLastKeyFrameLostPacket = node.isLostPacket;
                        isKeyFrame = true;
                        mIsKeyFrameGetted = true;
                        break;
                    }

                    pos++;
                }
            }

            if (mIsKeyFrameGetted)
            {
                bool isLostPacket = node.isLostPacket;

                if (node.isLostPacket)
                {
                    fprintf(stderr, "lost packet! isKeyFrame=%d\n",isKeyFrame);
                }

                if (mIsLastKeyFrameLostPacket) //I帧丢了，接下来的帧都不检测，直到遇到下一个I帧
                {
                    isLostPacket = true;
                }

                decodeH264Buffer((uint8_t*)mH264buf, h264length, isLostPacket);
            }
        }
    }
    free(node.buffer);
}
#endif

int64_t VideoChannel::getTimeStamp_MilliSecond()
{
    int mSecond = 0; //当前毫秒数

//#if defined(WIN32)

//    SYSTEMTIME sys;
//    GetLocalTime(&sys);
//    mSecond = sys.wMilliseconds;
//#else

    struct timeval    tv;
    struct timezone tz;

    struct tm         *p;

    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);

    mSecond = tv.tv_usec / 1000;


//#endif
    int64_t timeStamp = ((int64_t)time(NULL)) * 1000 + mSecond;

    return timeStamp;

}

///塞入rtp数据等待处理
void VideoChannel::inputRtpBuffer(uint8_t *buffer, int size, uint32_t sequenceNumber, bool isLastPacket)
{
    if ((sequenceNumber - mLastSequenceNumber ) != 1)
    {
        if (mLastSequenceNumber != 0 && sequenceNumber != 0)
        {
            mIsCurrentFrameLostPacket = true;

             fprintf(stderr,"sequenceNumber=%d %d\n", sequenceNumber, mLastSequenceNumber);
        }
    }

    mLastSequenceNumber = sequenceNumber;

    mReceiveRtpTime = getTimeStamp_MilliSecond();

    unsigned char *ptr = buffer;

    ///这是一个ps头 则处理上一包
    if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x01 && ptr[3] == 0xBA)
    {
        if (mRtpBufferSize > 0)
        {
            RtpDataNode node;
            node.isLostPacket = mIsCurrentFrameLostPacket;
            node.size = mRtpBufferSize;

            node.buffer = (uint8_t *)malloc(mRtpBufferSize+10);
            memcpy(node.buffer, mRtpBuffer, mRtpBufferSize);

            int listSize = inputData(node);

            mRtpBufferSize = 0;
            mIsCurrentFrameLostPacket = false;

            if (listSize > 20)
            {
                fprintf(stderr, "decode too slow! mRtpBufferList=%d RtpSSRC=%d\n", listSize, RtpSSRC);
            }
        }
    }

    if ((mRtpBufferSize + size) > MAX_PS_BUFFER_SIZE)
    {
        fprintf(stderr, "rtp packet out of range! mRtpBufferSize=%d size=%d isLastPacket=%d\n", mRtpBufferSize, size, isLastPacket);
        mRtpBufferSize = 0;
        return;
    }

    memcpy(mRtpBuffer + mRtpBufferSize, buffer, size);
    mRtpBufferSize += size;

}

bool VideoChannel::openH264Decoder()
{
    bool isOpenHardDecoderucceed = false;

    pCodecCtx = avcodec_alloc_context3(NULL);

    ///查找硬件解码器
    pCodec = avcodec_find_decoder_by_name("h264_cuvid");

    if (pCodec == NULL)
    {
        fprintf(stderr,"h264_cuvid Codec not found.\n");
    }
    else
    {
        ///打开解码器
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
        {
            avcodec_close(pCodecCtx);
            fprintf(stderr,"Could not open codec h264_cuvid\n");
        }
        else
        {
            isOpenHardDecoderucceed = true;
        }

    }


    if (!isOpenHardDecoderucceed)
    {
        fprintf(stderr,"\n\nOpen h264_cuvid failed! use AV_CODEC_ID_H264\n\n");

        pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);

        if (pCodec == NULL)
        {
    //        qDebug("Codec not found.\n");
            return false;
        }
        pCodecCtx->thread_count = 8;
        pCodecCtx->thread_type = FF_THREAD_FRAME;
//        pCodecCtx->time_base.num = 1;
//        pCodecCtx->time_base.den = 30;

        ///打开解码器
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
            printf("Could not open codec.\n");
            avcodec_close(pCodecCtx);
            return false;
        }
    }
    else
    {
        fprintf(stderr,"\n\nOpen h264_cuvid succeed !\n\n");
    }

    pFrame = av_frame_alloc();
    av_init_packet(&packet);


printf("open codec succeed!\n");
    return true;
}

void VideoChannel::closeH264Decoder()
{

    if (pCodecCtx == NULL) {
        avcodec_close(pCodecCtx);
    }

    av_frame_free(&pFrame);
    av_frame_free(&pFrameRGB);

    av_free(out_buffer_rgb);

    pCodecCtx = NULL;
    pCodec = NULL;

    out_buffer_rgb = NULL;

    pFrameRGB = NULL;
    pFrame = NULL;
}

void VideoChannel::decodeH264Buffer(uint8_t *buffer, int size, bool isLostPacket)
{
    packet.data = buffer;
    packet.size = size;
//    AVRational raw_time_rate = av_inv_q(pCodecCtx->framerate);
//    av_packet_rescale_ts(&packet, AVRational(),raw_time_rate);

    if (avcodec_send_packet(pCodecCtx, &packet) != 0)
    {
       fprintf(stderr, "input AVPacket to decoder failed!\n");
       return;
    }
    while (0 == avcodec_receive_frame(pCodecCtx, pFrame))
    {
        ///判断解码完毕的帧是否是关键帧
        bool isKeyFrame = false;

        if(pFrame->key_frame)
        {
            isKeyFrame = true;
        }

        if (pFrameRGB == NULL)
        {
            pFrameRGB = av_frame_alloc();

            ///将解码后的YUV数据转换成RGB32
            img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                    pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                    AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

            numBytes_rgb = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);
            out_buffer_rgb = (uint8_t *) av_malloc(numBytes_rgb * sizeof(uint8_t));

            avpicture_fill((AVPicture *) pFrameRGB, out_buffer_rgb, AV_PIX_FMT_RGB32,
                    pCodecCtx->width, pCodecCtx->height);
        }

        if (isLostPacket)
        {
            fprintf(stderr,"current frame lost packet! do not dealwith it! \n");
        }
        else
        {
            sws_scale(img_convert_ctx,
                    (uint8_t const * const *) pFrame->data,
                    pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                    pFrameRGB->linesize);

            dealWithRgb32Frame(out_buffer_rgb, numBytes_rgb, pCodecCtx->width, pCodecCtx->height, isLostPacket, isKeyFrame);
        }
    }

}


class CAutoFree
{
public:
    CAutoFree(void *pBuf)
    {
        m_pBuf = pBuf;
    };
    ~CAutoFree()
    {
        if (m_pBuf != NULL)
        {
            free(m_pBuf);
            m_pBuf = NULL;
        }
    }


    void *m_pBuf;
};
int GetBasicYSize(int nYSize, int nRadio)
{
    int nBasicYSize = nYSize;
    switch (nRadio)
    {
    case 0:
    {
        nBasicYSize = nYSize * 9 / 16;
        break;
    }
    case 1:
    {
        nBasicYSize = nYSize * 4 / 16;
        break;
    }
    case 2:
    {
        nBasicYSize = nYSize * 1 / 16;
        break;
    }
    case 3:
    {
        nBasicYSize = nYSize * 1 / 36;
        break;
    }
    case 4:
    {
        nBasicYSize = nYSize * 1 / 64;
    }
    default:
        break;
    }

    return nBasicYSize;

}


#define BITS_PER_PIXCEL 24
#define FORMAT_RGBA 4
#define FORMAT_RGB  3

/** must needed. pack */
#pragma pack(1)

typedef struct
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BMP_FILE_HEADER;

typedef struct {
    DWORD      biSize;
    LONG       biWidth;
    LONG       biHeight;
    WORD       biPlanes;
    WORD       biBitCount;
    DWORD      biCompression;
    DWORD      biSizeImage;
    LONG       biXPelsPerMeter;
    LONG       biYPelsPerMeter;
    DWORD      biClrUsed;
    DWORD      biClrImportant;
} BMP_INFO_HEADER;

#pragma pack()
int rgbaToBmpFile(char *pFileName, unsigned char* pRgbaData, int nWidth, int nHeight)
{
    BMP_FILE_HEADER bmpHeader;
    BMP_INFO_HEADER bmpInfo;

    FILE* fp = NULL;
    char* pBmpSource = NULL;
    char* pBmpData = NULL;

    int i = 0, j = 0;

    //4 bytes pack. must be 4 times per line。
    int bytesPerLine = (nWidth*BITS_PER_PIXCEL + 31) / 32 * 4;
    int pixcelBytes = bytesPerLine * nHeight;

    bmpHeader.bfType = 0x4D42;
    bmpHeader.bfReserved1 = 0;
    bmpHeader.bfReserved2 = 0;
    bmpHeader.bfOffBits = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER);
    bmpHeader.bfSize = bmpHeader.bfOffBits + pixcelBytes;

    bmpInfo.biSize = sizeof(BMP_INFO_HEADER);
    bmpInfo.biWidth = nWidth;
    /** 这样图片才不会倒置 */
    bmpInfo.biHeight = -nHeight;
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = BITS_PER_PIXCEL;
    bmpInfo.biCompression = 0;
    bmpInfo.biSizeImage = pixcelBytes;
    bmpInfo.biXPelsPerMeter = 100;
    bmpInfo.biYPelsPerMeter = 100;
    bmpInfo.biClrUsed = 0;
    bmpInfo.biClrImportant = 0;


    /** convert in memort, then write to file. */
    pBmpSource = (char*)malloc(pixcelBytes);
    if (!pBmpSource)
    {
        return -1;
    }

    /** open file */
    fp = fopen(pFileName, "wb+");
    if (!fp)
    {
        return -1;
    }

    fwrite(&bmpHeader, sizeof(BMP_FILE_HEADER), 1, fp);
    fwrite(&bmpInfo, sizeof(BMP_INFO_HEADER), 1, fp);
    /** Here you should consider color format. RGBA ? RGB? BGR?
        Param format is RGBA, format for file is BGR */
    pBmpData = pBmpSource;
    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            pBmpData[0] = pRgbaData[2];
            pBmpData[1] = pRgbaData[1];
            pBmpData[2] = pRgbaData[0];
            pRgbaData += FORMAT_RGB;
            //pRgbaData += FORMAT_RGBA;
            pBmpData += FORMAT_RGB;
        }
        //pack for 4 bytes
        pBmpData += (bytesPerLine - nWidth * 3);
    }
    fwrite(pBmpSource, pixcelBytes, 1, fp);

    /** close and release。 */
    fclose(fp);
    free(pBmpSource);

    return 0;
}

//BMP文件头（14字节）
typedef struct                       /**** BMP file header structure ****/
{
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
} MyBITMAPFILEHEADER;

//位图信息头（40字节）
typedef struct                       /**** BMP file info structure ****/
{
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
} MyBITMAPINFOHEADER;
void MySaveBmp(const char *filename, unsigned char *rgbbuf, int width, int height)
{
    MyBITMAPFILEHEADER bfh;
    MyBITMAPINFOHEADER bih;
    /* Magic number for file. It does not fit in the header structure due to alignment requirements, so put it outside */
    unsigned short bfType = 0x4d42;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfSize = 2 + sizeof(MyBITMAPFILEHEADER) + sizeof(MyBITMAPINFOHEADER) + width * height * 2;
    bfh.bfOffBits = 0x36;

    bih.biSize = sizeof(MyBITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 5000;
    bih.biYPelsPerMeter = 5000;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    printf("filename=%s \n", filename);
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        printf("Could not write file\n");
        return;
    }

    /*Write headers*/
    fwrite(&bfType, sizeof(bfType), 1, file);
    fwrite(&bfh, sizeof(bfh), 1, file);
    fwrite(&bih, sizeof(bih), 1, file);

    fwrite(rgbbuf, width*height * 3, 1, file);
    fclose(file);

}

typedef struct RGB32 {
    BYTE    rgbBlue;      // 蓝色分量
    BYTE    rgbGreen;     // 绿色分量
    BYTE    rgbRed;       // 红色分量
    BYTE    rgbReserved;  // 保留字节（用作Alpha通道或忽略）
} RGB32;
void Yuv420p2Rgb32(const BYTE *yuvBuffer_in, const BYTE *rgbBuffer_out, int width, int height)
{
    BYTE *yuvBuffer = (BYTE *)yuvBuffer_in;
    RGB32 *rgb32Buffer = (RGB32 *)rgbBuffer_out;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * width + x;

            int indexY = y * width + x;
            int indexU = width * height + y / 2 * width / 2 + x / 2;
            int indexV = width * height + width * height / 4 + y / 2 * width / 2 + x / 2;

            BYTE Y = yuvBuffer[indexY];
            BYTE U = yuvBuffer[indexU];
            BYTE V = yuvBuffer[indexV];

            RGB32 *rgbNode = &rgb32Buffer[index];

            ///这转换的公式 百度有好多 下面这个效果相对好一些

            rgbNode->rgbRed = Y + 1.402 * (V - 128);
            rgbNode->rgbGreen = Y - 0.34413 * (U - 128) - 0.71414*(V - 128);
            rgbNode->rgbBlue = Y + 1.772*(U - 128);
        }
    }
}





bool YV12ToBGR24_FFmpeg(unsigned char* pYUV, unsigned char* pBGR24, int width, int height)
{
    if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
        return false;
    //int srcNumBytes,dstNumBytes;
    //uint8_t *pSrc,*pDst;
    AVPicture pFrameYUV, pFrameBGR;

    //pFrameYUV = avpicture_alloc();
    //srcNumBytes = avpicture_get_size(PIX_FMT_YUV420P,width,height);
    //pSrc = (uint8_t *)malloc(sizeof(uint8_t) * srcNumBytes);
    avpicture_fill(&pFrameYUV, pYUV, AV_PIX_FMT_YUV420P, width, height);

    //U,V互换
    uint8_t * ptmp = pFrameYUV.data[1];
    pFrameYUV.data[1] = pFrameYUV.data[2];
    pFrameYUV.data[2] = ptmp;

    //pFrameBGR = avcodec_alloc_frame();
    //dstNumBytes = avpicture_get_size(PIX_FMT_BGR24,width,height);
    //pDst = (uint8_t *)malloc(sizeof(uint8_t) * dstNumBytes);
    avpicture_fill(&pFrameBGR, pBGR24, AV_PIX_FMT_BGR32, width, height);

    struct SwsContext* imgCtx = NULL;
    imgCtx = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_BGR32, SWS_BILINEAR, 0, 0, 0);

    if (imgCtx != NULL) {
        sws_scale(imgCtx, pFrameYUV.data, pFrameYUV.linesize, 0, height, pFrameBGR.data, pFrameBGR.linesize);
        if (imgCtx) {
            sws_freeContext(imgCtx);
            imgCtx = NULL;
        }
        return true;
    }
    else {
        sws_freeContext(imgCtx);
        imgCtx = NULL;
        return false;
    }
}

void yuv2rgb8_c2(unsigned char *py,
    unsigned char *pu,
    unsigned char *pv,
    unsigned char *dst,
    int width,
    int height)
{
    register unsigned int *pRGB = (unsigned int*)dst;
    register unsigned char* _py = py;
    register unsigned char* _pu = pu;
    register unsigned char* _pv = pv;

    register unsigned int tt = 0;
    register unsigned char Y = 0;
    register unsigned char U = 0;
    register unsigned char V = 0;
    register int R = 0;
    register int G = 0;
    register int B = 0;
    register int RR = 0;
    register int GG = 0;
    register int BB = 0;

    for (int i = 0; i < height; i++)
    {
        _pu = pu + (width / 2)*(i / 2);
        _pv = pv + (width / 2)*(i / 2);
        for (int j = 0; j < width; j++)
        {
            RR = BB = GG = 0;
            Y = *_py;
            U = *_pu;
            V = *_pv;
            _py++;
            _pu += (j & 1);
            _pv += (j & 1);

            R = Y + 1.403*(V - 128);
            G = Y - 0.715*(V - 128) - 0.344*(U - 128);
            B = Y + 1.773*(U - 128);

            RR = (unsigned int)(R < 0 ? 0 : (R > 255 ? 255 : R));
            GG = (unsigned int)(G < 0 ? 0 : (G > 255 ? 255 : G));
            BB = (unsigned int)(B < 0 ? 0 : (B > 255 ? 255 : B));

            tt = 0;
            tt |= (BB);
            tt |= (GG << 8);
            tt |= (RR << 16);

            *pRGB = tt;
            pRGB++;
        }
    }
}



#if 1
void VideoChannel::decodeSVACBuffer(uint8_t *pBuf, int len, int tag)
{
    /*packet.data = buffer;
    packet.size = size;*/

    //g_pfZXSVACDec_Init();

    /*char filelog[256];
    sprintf(filelog, "svac_log%d.log", m_port + 1);
    FILE* pflog = fopen(filelog, "a+");
    char temp[5000] = { 0 };
    int templen = 100;*/



    /*SYSTEMTIME sys;
    GetLocalTime(&sys);
    sprintf(temp, "1 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
    fwrite(temp, templen, 1, pflog);*/




    if (tag == I_frame)
    {
        m_FindIFrame = true;
        SVAC_PREFETCH_PARAM param;
        memset(&param, 0, sizeof(SVAC_PREFETCH_PARAM));

        //char filelog[256];
        //sprintf(filelog, "svac_log%d.log", m_port + 1);
        //FILE* pflog = fopen(filelog, "a+");
        //char temp[500000] = { 0 };
        //int templen = 0;
        //for(int i=0;i<len;i++)
        //	templen += sprintf(temp + templen, "%02x, ", pBuf[i]);
        //fwrite(temp, templen, 1, pflog);
        //fclose(pflog);
        //for (int i = 0; i < len; i++)
        //	printf("%02x", pBuf[i]);

        int ret = g_pfZXSVACDec_PrefetchParam(m_SvacHandle, pBuf, len, &param);
        if (ret == 0)
        {
            if (m_param.width != param.width ||
                m_param.height != param.height ||
                m_param.bit_depth_luma != param.bit_depth_luma ||
                m_param.svac_version != param.svac_version ||
                m_param.spatial_svc_flag != param.spatial_svc_flag ||
                m_param.temporal_svc_flag != param.temporal_svc_flag ||
                ((param.spatial_svc_flag == 1) && (m_param.spatial_svc_ratio != param.spatial_svc_ratio)) ||		//当开启增强层，空域为1时，基本层和增强层比例发生变化时，重新申请空间
                ((param.temporal_svc_flag == 1) && (m_param.spatial_svc_ratio != param.spatial_svc_ratio)))			//当开启增强层，时域为1时，基本层和增强层比例发生变化时，重新申请空间
            {

                memcpy(&m_param, &param, sizeof(SVAC_PREFETCH_PARAM));
                m_BytePerSample = m_param.bit_depth_luma == 8 ? 1 : 2;
                SAFE_DELETE_ARRAY(m_buf);
                SAFE_DELETE_ARRAY(m_svc_buf);

                int nYSize = m_param.width* m_param.height;
                if (m_param.spatial_svc_flag != 0)
                {
                    int nBasicYSize = GetBasicYSize(nYSize, m_param.spatial_svc_ratio);				//根据增强层空间大小和所占比，计算基本层的所占空间大小。
                    m_buf = new unsigned char[m_BytePerSample* nBasicYSize * 3 / 2 + 16];
                    m_svc_buf = new unsigned char[m_BytePerSample* nYSize * 3 / 2 + 16];
                }
                else
                {
                    m_buf = new unsigned char[m_BytePerSample* nYSize * 3 / 2 + 16];
                }

                if (m_buf == NULL)
                {
                    return;
                }

                if (m_SvacHandle != HANDLE(-1))
                {
                    g_pfZXSVACDec_Close(m_SvacHandle);
                    m_SvacHandle = HANDLE(-1);
                }

                int res = 0;

                res = g_pfZXSVACDec_Open(m_SvacHandle, m_threadnum, m_coreId, m_param.svac_version);		//ZXSVACDec_Open返回值非0为成功，非0的数字是授权剩余天数。当正式授权过期时，也可以进行解码（m_SvacHandle为有效值），但解码的YUV会有logo
                //试用授权过期时，不可以进行解码（m_SvacHandle为无效值）。
                //WriteError("ZXSVACDec_Open", res);
                //ShowError(res);
                if (m_SvacHandle == HANDLE(-1))
                {
                    return;
                }



                res = g_pfZXSVACDec_SetCommonExtensionInfo(m_SvacHandle, TRUE, TRUE);
            }
        }
    }
    else
    {
        if (m_FindIFrame == false)
        {
            return;
        }
        if (m_param.width == 0 || m_param.height == 0)
        {
            return;
        }
    }

    if (m_SvacHandle == HANDLE(-1))
    {
        return;
    }
    DEC_INPUT_PARAM		dec_in;
    memset(&dec_in, 0, sizeof(DEC_INPUT_PARAM));
    DEC_OUTPUT_PARAM	dec_out;
    memset(&dec_out, 0, sizeof(DEC_OUTPUT_PARAM));


    int offset = 16 - (int)m_buf % 16;
    int svc_offset = 16 - (int)m_svc_buf % 16;

    int nYSize = m_param.width* m_param.height;
    int nUVSize = nYSize / 4;

    if (m_param.spatial_svc_flag)					//当开启SVC时
    {

        int nBasicYSize = GetBasicYSize(nYSize, m_param.spatial_svc_ratio);		//转化获取基本层大小
        int nBasicUVSize = GetBasicYSize(nUVSize, m_param.spatial_svc_ratio);

        unsigned char* dest_buf = m_buf + (offset == 16 ? 0 : offset);
        dec_out.pY = dest_buf;
        dec_out.pU = dest_buf + m_BytePerSample * nBasicYSize;
        dec_out.pV = dest_buf + m_BytePerSample * (nBasicYSize + nBasicUVSize);

        unsigned char* m_pSvcBuf = m_svc_buf + (svc_offset == 16 ? 0 : svc_offset);
        dec_out.pY_SVC = m_pSvcBuf;
        dec_out.pU_SVC = m_pSvcBuf + m_BytePerSample * nYSize;
        dec_out.pV_SVC = m_pSvcBuf + m_BytePerSample * (nYSize + nUVSize);
    }
    else
    {
        unsigned char* dest_buf = m_buf + (offset == 16 ? 0 : offset);
        dec_out.pY = dest_buf;
        dec_out.pU = dest_buf + m_BytePerSample * nYSize;
        dec_out.pV = dest_buf + m_BytePerSample * (nYSize + nUVSize);


    }

    EXT_INFO_COMMON *_ext_info_common = (EXT_INFO_COMMON *)malloc(sizeof(EXT_INFO_COMMON));
    if (_ext_info_common != NULL)
    {
        memset(_ext_info_common, 0, sizeof(EXT_INFO_COMMON));
    }
    CAutoFree csAutoFree(_ext_info_common);

    m_nFileFrameCnt++;
    m_nTotalDecodeFrameCnt++;

    //unsigned long long nCurTime = timeGetTime();
    int ret = 0;
    int tryCnt = 0;
    if (m_pBufDecode == NULL)
    {
        m_pBufDecode = new unsigned char[PSBUFLEN];
        if (m_pBufDecode == NULL)
        {
            return;
        }
    }
    //memset(m_pBufDecode, 0, PSBUFLEN);			//不需要memset，影响性能

    while (tryCnt < 5)
    {
        dec_in.pBitstream = m_pBufDecode;
        dec_in.nLen = len;
        dec_in.chroma_format_idc = 1;
        dec_in.bSvcdec = 1;
        dec_in.bExtdecOnly = 0;
        dec_in.check_authentication_flag = 2;

        dec_out.nWidth = m_param.width;
        dec_out.nHeight = m_param.height;

        memcpy(m_pBufDecode, pBuf, len);

        ret = g_pfZXSVACDec_Decode(m_SvacHandle, &dec_in, &dec_out, _ext_info_common);

        if (tag == I_frame)
        {
            SVAC_SEC_INFO stInfo = { 0 };
            int reslut = g_pfZXSVACDec_GetSECInfo(m_SvacHandle, &stInfo);
            if (reslut == 0 && ret <= 0)
            {
                if (m_password == "")
                {
                    break;
                }
                if (stInfo.encrptFlag == 0)
                {
                    break;
                }
                if (stInfo.encrptFlag == SVAC_STRM_USER_ENC)
                {
                    g_pfZXSVACDec_SetPassWord(m_SvacHandle, (char *)m_password, strlen(m_password));
                }
            }
        }
        else {
            break;
        }

        if (ret > 0) {
            break;
        }

        tryCnt++;
    }

    //free(pBufDecode);

    m_decodeCount++;

    /*GetLocalTime(&sys);
    sprintf(temp, "2 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
    fwrite(temp, templen, 1, pflog);*/

    //if (ret > 0)
    //{
    //	if ((dec_out.nIsEffect & 0x1) == 0)
    //		return;

    //	if (!m_cbAddText) {

    //		//printf_Ext_Info_Common(_ext_info_common, &(dec_out.DecExtData));

    //		//if (m_bCanPreview)
    //		//{
    //		//	if (m_render != NULL)
    //		//	{
    //		//		int ret = -1;
    //		//		if (dec_out.spatial_svc_flag && (dec_out.SVAC_STATE & 1))		//有增强层时，渲染增强层
    //		//		{
    //		//			ret = m_render->render((unsigned char*)dec_out.pY_SVC, (unsigned char*)dec_out.pU_SVC, (unsigned char*)dec_out.pV_SVC, dec_out.nWidth_EL, dec_out.nHeight_EL, m_BytePerSample);
    //		//		}
    //		//		else {
    //		//			ret = m_render->render((unsigned char*)dec_out.pY, (unsigned char*)dec_out.pU, (unsigned char*)dec_out.pV, dec_out.nWidth, dec_out.nHeight, m_BytePerSample);
    //		//		}
    //		//		if (ret < 0 && (m_nPlayCount == 0))//渲染失败时，保存YUV文件
    //		//		{

    //		//		}
    //		//		//WriteError("m_render->render", ret);
    //		//		//ShowError(ret);
    //		//	}
    //		//}

    //	}

    /*char render_err_ex[256] = { 0 };
    sprintf(render_err_ex, "yuv(EL_%d_%d_%d_%d).yuv", m_port + 1, m_param.bit_depth_luma, dec_out.nWidth, dec_out.nHeight);
    FILE* pf_err = fopen(render_err_ex, "ab+");
    fwrite(dec_out.pY, m_BytePerSample* dec_out.nWidth*dec_out.nHeight, 1, pf_err);
    fwrite(dec_out.pU, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf_err);
    fwrite(dec_out.pV, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf_err);
    fclose(pf_err);*/


    //int ret = 0;
    //DEC_INPUT_PARAM		dec_in;
    //memset(&dec_in, 0, sizeof(DEC_INPUT_PARAM));
    //DEC_OUTPUT_PARAM	dec_out;
    //memset(&dec_out, 0, sizeof(DEC_OUTPUT_PARAM));
    //EXT_INFO_COMMON *_ext_info_common = (EXT_INFO_COMMON *)malloc(sizeof(EXT_INFO_COMMON));
    //if (m_pBufDecode == NULL)
    //{
    //	m_pBufDecode = new unsigned char[PSBUFLEN];
    //	if (m_pBufDecode == NULL)
    //	{
    //		return;
    //	}
    //}
    //dec_in.pBitstream = m_pBufDecode;
    //dec_in.nLen = size;
    //dec_in.chroma_format_idc = 1;
    //dec_in.bSvcdec = 1;
    //dec_in.bExtdecOnly = 0;
    //dec_in.check_authentication_flag = 2;

    //memcpy(m_pBufDecode, buffer, size);
    //ret = g_pfZXSVACDec_Decode(m_SvacHandle, &dec_in, &dec_out, _ext_info_common);


        /*if (pFrameRGB == NULL)
        {
            pFrameRGB = av_frame_alloc();
                int yuvformat = 0;
                if (dec_out.chroma_format_idc == 1)
                    yuvformat = 0;
                if (dec_out.chroma_format_idc == 2)
                    yuvformat = 1;

            ///将解码后的YUV数据转换成RGB32
            img_convert_ctx = sws_getContext(dec_out.nWidth, dec_out.nHeight,
                (AVPixelFormat)yuvformat, dec_out.nWidth, dec_out.nHeight,
                AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

            numBytes_rgb = avpicture_get_size(AV_PIX_FMT_RGB32, dec_out.nWidth, dec_out.nHeight);
            out_buffer_rgb = (uint8_t *)av_malloc(numBytes_rgb * sizeof(uint8_t));

            avpicture_fill((AVPicture *)pFrameRGB, out_buffer_rgb, AV_PIX_FMT_RGB32,
                dec_out.nWidth, dec_out.nHeight);
        }
        sws_scale(img_convert_ctx,
            (uint8_t const * const *)pFrame->data,
            pFrame->linesize, 0, dec_out.nHeight, pFrameRGB->data,
            pFrameRGB->linesize);*/


    if (m_bCanPreview)
    {
        if (dec_out.spatial_svc_flag && (dec_out.SVAC_STATE & 1))		//有增强层时，渲染增强层
        {
        }
        else {
            bool isLostPacket = 0;
            bool isKeyFrame = 1;

            int yuvSize = dec_out.nWidth * dec_out.nHeight * 2;
            BYTE *yuvBuffer = (BYTE *)malloc(yuvSize);
            pFrame = av_frame_alloc();
            pFrameRGB = av_frame_alloc();
            int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, dec_out.nWidth, dec_out.nHeight);
            rgbBuffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

            /*GetLocalTime(&sys);
            sprintf(temp, "21 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
            fwrite(temp, templen, 1, pflog);*/

            avpicture_fill((AVPicture *)pFrameRGB, rgbBuffer, AV_PIX_FMT_RGB32, dec_out.nWidth, dec_out.nHeight);
            avpicture_fill((AVPicture *)pFrame, yuvBuffer, AV_PIX_FMT_YUV420P, dec_out.nWidth, dec_out.nHeight);

            /*GetLocalTime(&sys);
            sprintf(temp, "22 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
            fwrite(temp, templen, 1, pflog);*/

            SwsContext *img_convert_ctx = sws_getContext(dec_out.nWidth, dec_out.nHeight, AV_PIX_FMT_YUV420P, dec_out.nWidth, dec_out.nHeight, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
            int rgbSize = numBytes;
            memcpy(yuvBuffer, dec_out.pY, m_BytePerSample* dec_out.nWidth*dec_out.nHeight);
            memcpy(yuvBuffer + (m_BytePerSample* dec_out.nWidth*dec_out.nHeight), dec_out.pU, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4);
            memcpy(yuvBuffer + (m_BytePerSample* dec_out.nWidth*dec_out.nHeight) + (m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4), dec_out.pV, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4);

            /*GetLocalTime(&sys);
            sprintf(temp, "23 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
            fwrite(temp, templen, 1, pflog);*/

            sws_scale(img_convert_ctx,
                (uint8_t const * const *)pFrame->data,
                pFrame->linesize, 0, dec_out.nHeight, pFrameRGB->data,
                pFrameRGB->linesize);

            /*GetLocalTime(&sys);
            sprintf(temp, "24 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
            fwrite(temp, templen, 1, pflog);*/

            //YV12ToBGR24_FFmpeg(yuvBuffer, rgbBuffer, dec_out.nWidth, dec_out.nHeight);
            //yuv2rgb8_c2((unsigned char*)dec_out.pY, (unsigned char*)dec_out.pU, (unsigned char*)dec_out.pV, rgbBuffer, dec_out.nWidth, dec_out.nHeight);


            dealWithRgb32Frame(rgbBuffer, numBytes, dec_out.nWidth, dec_out.nHeight, isLostPacket, isKeyFrame);

            /*GetLocalTime(&sys);
            sprintf(temp, "25 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
            fwrite(temp, templen, 1, pflog);*/

            free(yuvBuffer);
            av_free(rgbBuffer);
            rgbBuffer = NULL;


            //av_frame_free(&pFrame);
            //av_frame_free(&pFrameRGB);
            av_free(out_buffer_rgb);
            sws_freeContext(img_convert_ctx);
            pCodecCtx = NULL;
            pCodec = NULL;
            out_buffer_rgb = NULL;
            //pFrameRGB = NULL;
            //pFrame = NULL;


            //dealWithRgb32Frame(out_buffer_rgb, numBytes_rgb, dec_out.nWidth, dec_out.nHeight, isLostPacket, isKeyFrame);
        }
    }
    /*GetLocalTime(&sys);
    sprintf(temp, "3 %4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
    fwrite(temp, templen, 1, pflog);
    fclose(pflog);*/
}
#endif


void VideoChannel::dealWithRgb32Frame(const uint8_t *rgb32Buffer, const int bufferSize,  const int width, const int height, const bool isLostPacket, const bool isKeyFrame)
{
//    fprintf(stderr, "%s %d %d\n", __FUNCTION__, width, height);

    if (mVideoChannelEventHandle != nullptr)
    {
        if (mVideoChannelEventHandle == NULL) return;
        mVideoChannelEventHandle->onDisPlayVideoData(rgb32Buffer, bufferSize, width, height);
    }
}
