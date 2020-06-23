#include "VideoChannel.h"


#include "rtpreceiver.h"
#include "EventHandle.h"
#include <sys/time.h>

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

//extern
unsigned char vkek_bak[5] = {0,0,0,0,0};
unsigned int vkek_lenbak = 5;
unsigned char pKeyversion_bak[20] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char Pubkey_bak[] = {0,0,0,0};

int videotype;

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

#if 0
void VideoChannel::dealwithDataNode(const RtpDataNode &node)
{
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
                memcpy(vkek, vkek_bak, vkek_lenbak);
                memcpy(vkek_version, pKeyversion_bak, sizeof(vkek_version));
                vkek_version[19] = '\0';
                SvacDecSetVkek(handle, vkek, vkek_version, 32);
                SvacDecSetPubkey(handle, (char*)Pubkey_bak);
                if (id == TEST_FRAME_NUM)
                    return;

                ret = SvacDecProcess(handle, in_buff, h264length, out_buff, &outFrameLen, &info);
                if (ret)
                {
                    //printf("frame %d dec failed!\n", id);
                    return;
                }
                else
                {
                    if (outFrameLen > 0)
                    {
                        memcpy(mH264buf, out_buff, outFrameLen);
                        h264length = outFrameLen;

                        /*char *filelog4 = "test_dec.svac";
                        FILE* pflog4 = fopen(filelog4, "ab+");
                        fwrite(out_buff, outFrameLen, 1, pflog4);
                        fclose(pflog4);*/
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




#if 1
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



void VideoChannel::decodeSVACBuffer(uint8_t *pBuf, int len, int tag)
{
    if (tag == I_frame)
    {
        m_FindIFrame = true;
        SVAC_PREFETCH_PARAM param;
        memset(&param, 0, sizeof(SVAC_PREFETCH_PARAM));

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

                res = g_pfZXSVACDec_Open(m_SvacHandle, m_threadnum, m_coreId, m_param.svac_version);		//ZXSVACDec_Open返回值非0为成功，非0的数字是授权剩余天数。当正式授权过期时，也可以进行解码（m_SvacHandle为有效值），但解码的YUV会有logo																							//试用授权过期时，不可以进行解码（m_SvacHandle为无效值）。
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


    m_decodeCount++;


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


            avpicture_fill((AVPicture *)pFrameRGB, rgbBuffer, AV_PIX_FMT_RGB32, dec_out.nWidth, dec_out.nHeight);
            avpicture_fill((AVPicture *)pFrame, yuvBuffer, AV_PIX_FMT_YUV420P, dec_out.nWidth, dec_out.nHeight);


            SwsContext *img_convert_ctx = sws_getContext(dec_out.nWidth, dec_out.nHeight, AV_PIX_FMT_YUV420P, dec_out.nWidth, dec_out.nHeight, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
            int rgbSize = numBytes;
            memcpy(yuvBuffer, dec_out.pY, m_BytePerSample* dec_out.nWidth*dec_out.nHeight);
            memcpy(yuvBuffer + (m_BytePerSample* dec_out.nWidth*dec_out.nHeight), dec_out.pU, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4);
            memcpy(yuvBuffer + (m_BytePerSample* dec_out.nWidth*dec_out.nHeight) + (m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4), dec_out.pV, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4);


            sws_scale(img_convert_ctx,
                (uint8_t const * const *)pFrame->data,
                pFrame->linesize, 0, dec_out.nHeight, pFrameRGB->data,
                pFrameRGB->linesize);


            dealWithRgb32Frame(rgbBuffer, numBytes, dec_out.nWidth, dec_out.nHeight, isLostPacket, isKeyFrame);

            free(yuvBuffer);
            av_free(rgbBuffer);
            rgbBuffer = NULL;

            av_free(out_buffer_rgb);
            sws_freeContext(img_convert_ctx);
            pCodecCtx = NULL;
            pCodec = NULL;
            out_buffer_rgb = NULL;


        }
    }
}


void VideoChannel::dealWithRgb32Frame(const uint8_t *rgb32Buffer, const int bufferSize,  const int width, const int height, const bool isLostPacket, const bool isKeyFrame)
{
//    fprintf(stderr, "%s %d %d\n", __FUNCTION__, width, height);

    if (mVideoChannelEventHandle != nullptr)
    {
        if (mVideoChannelEventHandle == NULL) return;
        mVideoChannelEventHandle->onDisPlayVideoData(rgb32Buffer, bufferSize, width, height);
    }
}
