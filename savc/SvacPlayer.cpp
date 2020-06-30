//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MMSystem.h>
#include <windows.h>
#include "SvacPlayer.h"
#include <Mmsystem.h>
#pragma lib("Winmm.lib");
#define TIMER_RESOLUTION 1

#define PSBUFLEN (2 * 1024 * 1024)
CString g_libPath;

CritSec g_svac_critsec;

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

//int GetModulePathLen(char* cFileName)
//{
//	int pos = 0;
//
//	for (; pos < 1024; pos++)
//	{
//		if (memcmp(cFileName + pos, "vnplayer.dll", 12) == 0)break;
//	}
//
//	if (pos == 1024)
//	{
//		pos = -1;
//	}
//
//	return pos;
//}
int loadsvacdll()
{
	g_svac_critsec.Lock();
	static int g_load_svac_dll = 0;
	if (g_load_svac_dll == 0)
	{

		char* dllname = "ZxSvacDecLib.dll";
		HINSTANCE m_hInst = NULL;
		CString csStr = g_libPath;
		if (csStr != "")
		{
			if (csStr.GetAt(csStr.GetLength() - 1) != '\\')
			{
				csStr += "\\";
			}
			csStr += dllname;
			m_hInst = LoadLibrary(csStr.GetString());
		}

		if (m_hInst == NULL)
		{
			m_hInst = LoadLibrary(dllname);
			if (m_hInst == NULL)
			{
				int err = GetLastError();
				char te[128];
				sprintf_s(te, 128, "LoadLibrary %s failed! error = %d\n", dllname, err);
				OutputDebugString(te);
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


SvacPlayer::SvacPlayer(int port, HWND hwnd)
	: m_csRtsp(this)
{
	CSvacDecRtsp::InitRtsp();

	m_FindIFrame = false;

	m_SvacHandle = HANDLE(-1);
	memset(&m_param, 0, sizeof(SVAC_PREFETCH_PARAM));
	m_buf = NULL;
	m_svc_buf = NULL;
	m_BytePerSample = 0;

	m_pAudioDecodeBuf = NULL;
	m_psAudioType = 0;

	loadsvacdll();

	m_psbuf = new unsigned char[2 * 1024 * 1024];
	m_nPsBufLen = 0;
	m_port = port;
	m_hwnd = hwnd;
	m_render = NULL;
	m_hThread = NULL;
	m_bStop = false;
	m_nPlayCount = 0;

	m_nFrameGop = 0;
	m_nFrameIndex = 0;


	m_pPSbuf = NULL;
	m_buf = NULL;
	m_cbAddText = NULL;
	m_threadnum = 0;
	m_coreId = -1;


	m_nNeedSnapCnt = -1;
	m_bSaveYUVFile = true;
	m_bCanPreview = true;

	m_nTotalDecodeFrameCnt = 0;
	m_nFileFrameCnt = 0;
	m_nFileDecodeTime = 0;

	m_strMulticastIP = "";
	m_nMulticastPort = 34444;
	m_pBufDecode = NULL;

	m_ukey_name = "";
	m_ukey_pin = "";

	timeBeginPeriod(TIMER_RESOLUTION);
}

SvacPlayer::~SvacPlayer()
{
	SAFE_DELETE_ARRAY(m_buf);
	SAFE_DELETE_ARRAY(m_svc_buf);
	SAFE_DELETE_ARRAY(m_psbuf);
	SAFE_DELETE_ARRAY(m_pAudioDecodeBuf);
	SAFE_DELETE_ARRAY(m_pBufDecode);

	timeEndPeriod(TIMER_RESOLUTION);

	CSvacDecRtsp::UnInitRtsp();
}

int SvacPlayer::SetSvacFile(CString filename)
{
	m_filename = filename;
	return 0;
}

void SvacPlayer::SetThreadNum(int num)
{
	m_threadnum = num;
}

void SvacPlayer::SetCoreId(int id)
{
	m_coreId = id;
}

int SvacPlayer::SetSvacPassWord(CString password)
{
	m_password = password;
	return 0;
}
int SvacPlayer::SetSvacCert(CString root_cert, CString self_cert)
{
	m_cert_file_root = root_cert;
	m_cert_file_self = self_cert;
	return 0;
}
int SvacPlayer::SetSvacVKEK(CString version, CString vkek)
{
	m_crypt_key_version = version;
	m_crypt_key_for_video = vkek;
	return 0;
}

int SvacPlayer::GetSupportUKey(const char **p2Name, int nCnt)
{

	if (g_pfZXSVACDec_GetSupportUKey != NULL)
	{
		return g_pfZXSVACDec_GetSupportUKey(p2Name, nCnt);
	}

	return -1;

}



int SvacPlayer::SetPlayHwnd(HWND hwnd)
{
	m_hwnd = hwnd;
	return 0;
}
void SvacPlayer::WriteLog(const char *pFormat, ...)
{
	char filelog[256];
	sprintf(filelog, "svac_log%d.log", m_port + 1);
	FILE* pflog = fopen(filelog, "a+");
	if (pflog != NULL)
	{
		va_list args;
		va_start(args, pFormat);
		vfprintf(pflog, pFormat, args);
		va_end(args);

		fclose(pflog);
	}

}

void SvacPlayer::ShowError(int error)
{
	if (error >= 0)
	{
		return;
	}
	HWND hWnd = ::GetParent(m_hwnd);
	BOOL boRslt = PostMessage(hWnd, ERR_MSG, 0, error);
	boRslt = boRslt;
}

void SvacPlayer::WriteError(const char *pFunName, int error)
{
	if (error >= 0)
	{
		return;
	}

	WriteLog("%s error: %08x(%d) \n", pFunName, error, error);
}

int SvacPlayer::RtspPlayProc()
{
	char *MulticastIP;
	if (m_strMulticastIP.empty())
	{
		MulticastIP = 0;
	}
	else
	{
		MulticastIP = const_cast<char *>(m_strMulticastIP.c_str());
	}
	m_csRtsp.RealPlay_open(const_cast<char *>((LPCTSTR)m_filename), 0, MulticastIP, m_nMulticastPort);

	m_csRtsp.RealPlay_play();

	if (MulticastIP)
	{
		//组播仅等待，维持rtsp连接
		//接收rtp数据包由内部rtp session完成
		while (!m_bStop)
		{
			Sleep(100);
		}
	}
	else
	{
		//接收rtp over tcp数据包
		while (!m_bStop)
		{
			m_csRtsp.RealPlay_polltcp();
		}
	}

	m_csRtsp.RealPlay_close();

	return 0;
}

int SvacPlayer::PlayProc()
{
	unsigned char* buf = m_pPSbuf;
	bool boIsPS;

	memset(&m_beginsystime,0,sizeof(SYSTEMTIME));
	memset(&m_endsystime, 0, sizeof(SYSTEMTIME));

	GetLocalTime(&m_beginsystime);
	m_beginCount = timeGetTime();
	m_endCount = 0;
	m_decodeCount = 0;

	memset(&m_param, 0, sizeof(SVAC_PREFETCH_PARAM));


	if (m_cbAddText)
	{
		char temp[512] = { 0 };
		sprintf(temp, "开始解码时间:%d-%d-%d %d:%d:%d\n",
			m_beginsystime.wYear,
			m_beginsystime.wMonth, 
			m_beginsystime.wDay,
			m_beginsystime.wHour,
			m_beginsystime.wMinute,
			m_beginsystime.wSecond);
		m_cbAddText(m_pUser,temp);
	}

	CString strLowerFileName = m_filename;
	strLowerFileName.MakeLower();
	if (strLowerFileName.Find("rtsp://") == 0 || strLowerFileName.Find("rtsp:\\\\") == 0)		//链接rtsp实时流
	{
		int ret = RtspPlayProc();

		if (m_SvacHandle != HANDLE(-1))
		{
			g_pfZXSVACDec_Close(m_SvacHandle);
		}

		m_SvacHandle = HANDLE(-1);

		if (m_render != NULL)
		{
			delete m_render;
			m_render = NULL;
		}

		PRINT("解码完成！");

		return ret;
	}

	FILE* fin = fopen(m_filename.GetString(), "rb");
	if (fin == 0)
	{
		PRINT("Cann't open input file %s!\n", m_filename.GetString());
		return 0;
	}
	int buflen = 0;
	int res = 0;

	m_SvacHandle = HANDLE(-1);

	m_nFileFrameCnt = 0;
	m_nFileDecodeTime = 0;
	int iFrameNum = 0;
	unsigned long long dFrameTime = timeGetTime();
	unsigned long long nTime = timeGetTime();
	unsigned long long dTimeToSleep = 0;
	if (m_filename.Find(".dav") == -1)
	{
		while (!m_bStop)
		{
			int readlen = fread(m_pPSbuf+buflen, 1, PSBUFLEN-buflen, fin);
			if (readlen <= 0)
			{
				break;
			}
			readlen += buflen;
			buflen = 0;
			
			buf = m_pPSbuf;
			
			while (true)
			{
				int iRvCurren = ResearchPSTag(buf+buflen, readlen-buflen);			//查找第一个PS头的位置
				if (iRvCurren >= 0)
				{
					buflen += iRvCurren;
				}
				else
				{
					buflen = 0;
					break;
				}
				unsigned long long dFrameTimeTest = timeGetTime();
				int iRvNext = ResearchPSTag(buf+buflen+4, readlen-buflen-4);		//查找第二个PS头的位置
				if (iRvNext == 0)
				{
					iRvNext += 4;
					buflen += iRvNext;
					continue;
				}
				else if (iRvNext>0)
				{
					iRvNext += 4;
					DecodePSPacket(buf+buflen, iRvNext);							//解析出裸码流并解码
					buflen += iRvNext;
					//---------------------为了显示时按照每秒25帧来进行播放，此代码不做参考---------------------------
					if (m_bCanPreview)											
					{
						unsigned long long dFrameTimeEx = timeGetTime();
						dTimeToSleep = 39 - (dFrameTimeEx - dFrameTimeTest);
						iFrameNum++;
						if (39 > dTimeToSleep > 0)
						{
							Sleep(dTimeToSleep);
						}

						int dTimeCha = dFrameTimeEx - dFrameTime;
						if (dTimeCha >= 5000)
						{
							PRINT("----[PropertyTest]----ViewNo:%d------iFrameNum:%f fps---\n", m_port + 1, (double)iFrameNum * 1000 / (double)dTimeCha);
							dFrameTime = dFrameTimeEx;
							iFrameNum = 0;
						}
					}
					//---------------------------------------end-------------------------------------------------
					continue;
				}
				else if (iRvNext<0)
				{
//						buflen = 0;
					buf += buflen;
					buflen = readlen-buflen;
					memcpy(m_pPSbuf, buf, buflen);
					break;
				}
			}
		}
	}
	else
	{
		typedef struct _tagStSVACBaseHeader
		{
			UINT32 u32Header;										//帧标识
			UINT8 u8CodeType;										//编码格式
			UINT8 u8Channel;										//通道
			UINT8 u8FrameRate;										//帧率
			UINT8 u8FrameSerial;									//帧序号
			union
			{
				UINT16 u16Width;									//宽度（像素值）
				struct
				{
					UINT8 u8AudioType;
					UINT8 u8AudioChannel;
				};
			};
			union
			{
				UINT16 u16Heigth;									//高度（像素值）
				struct
				{
					UINT8 u8AudioSamplesPerSecond;
					UINT8 u8AudioBitsPerSample;
				};

			};
			INT32 u32TimestampInteger;								//时间
			INT16 u16TimestampMS;									//时间毫秒
			UINT8 u8ExternNum;										//扩展头个数
			UINT8 u8Reserved;
		}StSVACBaseHeader;

		typedef struct _tagStSVACExtBaseHeader
		{
			INT16 s16Type;
			INT16 s16Lenth;
		}StSVACExtBaseHeader;

#define SVAC_FRAME_I	0x000001ED
#define SVAC_FRAME_P	0x000001EC
#define SVAC_FRAME_B	0x000001EB
#define SVAC_FRAME_A	0x000001EA

#define SVAC_FRAME_I_N	0xED010000
#define SVAC_FRAME_P_N	0xEC010000
#define SVAC_FRAME_B_N	0xEB010000
#define SVAC_FRAME_A_N	0xEA010000

#if 0
		//if (m_port >= 10)
		{
			ShowError(-1);
		}
#endif
		int iCurPost = 0;
		while (!m_bStop)
		{
			buf = m_pPSbuf;
			StSVACBaseHeader *pHeader = (StSVACBaseHeader *)buf;					//获取Svac头信息
			int readlen = fread(buf, 1, sizeof(StSVACBaseHeader), fin);
			if (readlen != sizeof(StSVACBaseHeader))
				break;
			if ((pHeader->u32Header != SVAC_FRAME_I_N) &&							//判断是否是帧头
				(pHeader->u32Header != SVAC_FRAME_P_N) &&
				(pHeader->u32Header != SVAC_FRAME_B_N) &&
				(pHeader->u32Header != SVAC_FRAME_A_N))
			{
				iCurPost++;									//开始不是完整帧时，寻找下一个帧头
				fseek(fin, iCurPost, SEEK_SET);				
				continue;
			}
			buf += readlen;
			iCurPost += readlen;

			bool boIsBreak = false;
			int ExternNum = pHeader->u8ExternNum;									
			while (ExternNum > 0)
			{
				readlen = fread(buf, 1, sizeof(StSVACExtBaseHeader), fin);			//读取私有头扩展信息
				if (readlen != sizeof(StSVACExtBaseHeader))
				{
					boIsBreak = true;
					break;
				}
				StSVACExtBaseHeader *pExtBaseHeader = (StSVACExtBaseHeader *)buf;
				buf += readlen;
				iCurPost += readlen;

				readlen = fread(buf, 1, pExtBaseHeader->s16Lenth, fin);				
				if (readlen != pExtBaseHeader->s16Lenth)
				{
					boIsBreak = true;
					break;
				}
				buf += readlen;
				iCurPost += readlen;
				ExternNum--;
			}

			if (boIsBreak)
			{
				break;
			}

			int freamLen = 0;
			readlen = fread(&freamLen, 1, 4, fin);						//获取视频/音频流大小
			if (readlen != 4)
			{
				break;
			}
			iCurPost += readlen;

			readlen = fread(buf, 1, freamLen, fin);						//读取视频/音频流
			if (readlen != freamLen)
			{
				break;
			
			}
			iCurPost += readlen;

			unsigned long long dFrameTimeTest = timeGetTime();

			if (pHeader->u32Header == SVAC_FRAME_I_N)
			{
				decode(buf, freamLen, I_frame);
			}
			else if (pHeader->u32Header == SVAC_FRAME_P_N)
			{
				decode(buf, freamLen, P_frame);
			}
			else if (pHeader->u32Header == SVAC_FRAME_B_N)
			{
				decode(buf, freamLen, B_frame);
			}
			else if (pHeader->u32Header == SVAC_FRAME_A_N)
			{
				if (pHeader->u8AudioType == 0x40)	/* svac sudio */
				{
					decode(buf, freamLen, Audio_frame);
				}
			}
			//---------------------为了显示时按照每秒25帧来进行播放，此代码不做参考---------------------------
			if (m_bCanPreview && pHeader->u32Header != SVAC_FRAME_A_N)
			{
				unsigned long long dFrameTimeEx = timeGetTime();
				dTimeToSleep = 39 - (dFrameTimeEx - dFrameTimeTest);
				iFrameNum++;
				if (39 > dTimeToSleep > 0)
				{
					Sleep(dTimeToSleep);
				}
				
				int dTimeCha = dFrameTimeEx - dFrameTime;
				if (dTimeCha >= 5000)
				{
					PRINT("----[PropertyTest]----ViewNo:%d------iFrameNum:%f fps---\n", m_port + 1, (double)iFrameNum * 1000 / (double)dTimeCha);
					dFrameTime = dFrameTimeEx;
					iFrameNum = 0;
				}
			}
			//---------------------------------------end-------------------------------------------------
		}
	}

	nTime = timeGetTime() - nTime;

	if (m_SvacHandle != HANDLE(-1))
	{
		g_pfZXSVACDec_Close(m_SvacHandle);
	}

	m_SvacHandle = HANDLE(-1);

	if (m_render != NULL)
	{
		delete m_render;
		m_render = NULL;
	}


	fclose(fin);

	GetLocalTime(&m_endsystime);
	m_endCount = timeGetTime();
	
	if (!m_bStop && m_cbAddText)
	{
		char temp[512] = { 0 };
		sprintf(temp, "结束解码时间:%d-%d-%d %d:%d:%d,总解码时间:%fs,总帧数:%d,平均解码速度:%dfps.\n",
			m_endsystime.wYear,
			m_endsystime.wMonth,
			m_endsystime.wDay,
			m_endsystime.wHour,
			m_endsystime.wMinute,
			m_endsystime.wSecond,
			((double)m_endCount - (double)m_beginCount)/1000,
			m_decodeCount,
			m_decodeCount * 1000 / (m_endCount - m_beginCount));
		m_cbAddText(m_pUser, temp);
	}


	{		
		if (m_nFileFrameCnt != 0)
		{
			WriteLog("this thread decode the file (%s) (%d frames)with the speed of %lldms per frame, and %lld fps with other job\n",
				m_filename.GetString(), m_nFileFrameCnt, m_nFileDecodeTime / m_nFileFrameCnt, m_nFileFrameCnt * 1000 / nTime);
		}
		else
		{
			WriteLog("this thread decode the file (%s) none frame\n", m_filename.GetString());
		}
	}
	PRINT("解码完成！");
	return 0;
}

unsigned int WINAPI ThreadPlay(LPVOID pParam)
{
	SvacPlayer* pThis = (SvacPlayer*)pParam;

	if (pThis == NULL)
	{
		return 1;
	}

	if (pThis->m_pPSbuf)
	{
		delete pThis->m_pPSbuf;
		pThis->m_pPSbuf = NULL;
	}

	pThis->m_pPSbuf = new unsigned char[PSBUFLEN];

	if (pThis->m_pPSbuf == 0) {
		PRINT("Decoder error: no enough memory!\n");
		return 0;
	}

	__try
	{
		while (!pThis->m_bStop)
		{
			PRINT("port = %d,PlayCount = %d\n", pThis->m_port, pThis->m_nPlayCount + 1);
			pThis->PlayProc();
			pThis->m_nPlayCount++;
		}
		
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		PRINT("PlayProc except\n");
	}

	if (pThis->m_pPSbuf)
	{
		delete pThis->m_pPSbuf;
		pThis->m_pPSbuf = NULL;
	}

	{
		char filelog[256];
		sprintf(filelog, "svac_log%d.log", pThis->m_port + 1);
		FILE* pflog = fopen(filelog, "a+");
		if (pflog != NULL)
		{
			fprintf(pflog, "this thread decode %d frames total\n", pThis->m_nTotalDecodeFrameCnt);
			fclose(pflog);
		}
	}


	return 0;
}

int SvacPlayer::Start()
{
	if (!m_hThread)
	{
		m_bStop = false;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadPlay, (void*)this, 0, &m_dwPollProcThreadId);
	}

	return 0;
}

int SvacPlayer::stop()
{

	if (m_hThread)
	{
		m_bStop = true;

		PRINT("Stop ThreadPoll WaitForSingleObject\n");

		DWORD dw = WaitForSingleObject(m_hThread, INFINITE);

		switch (dw)
		{
		case WAIT_OBJECT_0:
			PRINT("Stop ThreadPoll WaitForSingleObject WAIT_OBJECT_0\n");
			break;

		case WAIT_TIMEOUT:
			PRINT("Stop ThreadPoll WaitForSingleObject WAIT_TIMEOUT\n");

			break;

		case WAIT_FAILED:
			PRINT("Stop ThreadPoll WaitForSingleObject WAIT_FAILED\n");
			break;
		}
		m_hThread = NULL;
	}


	return 0;
}




int SvacPlayer::ResearchPSTag(unsigned char* buf, unsigned int len)
{
	unsigned int code = 0xFFFFFFFF;
	unsigned char *pos = buf;	// 拿到数据区首指针
	int rest = len;
	int ret = -1;
	while (rest--)
	{
		code = (code << 8) | *pos++;
		if (code == 0x01BA)
		{
			ret = len - rest - 4;
			break;
		}
	}

	return ret;
}


STREAMDATA_TYPE SvacPlayer::checkTag(unsigned char* pBuf,unsigned  int size)
{
	unsigned char* pCurBuf = pBuf;
	unsigned int restsize = size;

	STREAMDATA_TYPE m_streamtype = Stream_No;
	unsigned int code = 0xFFFFFFFF;
	unsigned char *pos = pCurBuf;
	while (restsize--)
	{
		code = (code << 8) | *pos++;

		if (code == 0x01BA)
		{
			m_streamtype = Stream_PS;
			break;
		}
	}

	return m_streamtype;
}

void SvacPlayer::DecodePSPacket(unsigned char* pBuf, unsigned int size)
{
	unsigned char* pCurBuf = pBuf;
	m_PsBufLen = 0;
	int frametype = P_frame;
	while (!m_bStop)
	{
		if (pCurBuf >= (pBuf + size))
		{
			break;
		}

		unsigned int* pcode = (unsigned int*)pCurBuf;
		if (*pcode == 0xBA010000)												//查找P帧类型
		{
			frametype = P_frame;
			pCurBuf += 13;
			unsigned int pack_stuffing_length = *pCurBuf & 7;
			pCurBuf++;
			pCurBuf += pack_stuffing_length;
			continue;
		}
		else if (*pcode == 0xBB010000)											//查找I帧类型
		{
			int Systemheader_length = *(pCurBuf + 4) * 256 + *(pCurBuf + 5);
			pCurBuf += Systemheader_length + 4 + 2;
			frametype = I_frame;
			continue;
		}
		else if (*pcode == 0xBC010000)											//查找I帧类型
		{
			int psmlength = *(pCurBuf + 4) * 256 + *(pCurBuf + 5);
			frametype = I_frame;

			UINT32 u32PSILength = 0;
			u32PSILength = u32PSILength | pCurBuf[8];
			u32PSILength = (u32PSILength << 8) | pCurBuf[9];

			UINT32 u32ESMLength = 0;
			u32ESMLength = u32ESMLength | pCurBuf[10 + u32PSILength];
			u32ESMLength = (u32ESMLength << 8) | pCurBuf[11 + u32PSILength];

			UINT8 *pESM = pCurBuf + 12 + u32PSILength;
			UINT32 u32Tmp = 0;
			while (u32Tmp < u32ESMLength)
			{
				UINT32 u32StreamID = pESM[1 + u32Tmp];
				u32StreamID &= 0x000000FF;
				u32StreamID |= 0x00000100;
				if (u32StreamID == 0x01C0)										//查找音频类型
				{
					m_psAudioType = pESM[0 + u32Tmp];
				}

				UINT32 u32ESILength = 0;
				u32ESILength = u32ESILength | pESM[2 + u32Tmp];
				u32ESILength = (u32ESILength << 8) | pESM[3 + u32Tmp];
				if (u32ESILength == 0)
				{
					break;
				}
				u32Tmp += (2 + 2 + u32ESILength);

			}

			pCurBuf += psmlength + 4 + 2;
			continue;
		}
		else if (*pcode == 0xE0010000)													//PS视频数据
		{
			int pes_packet_length = *(pCurBuf + 4) * 256 + *(pCurBuf + 5);
			int pes_header_data_length = *(pCurBuf + 8);
			int payload_length = pes_packet_length - 3 - pes_header_data_length;
			pCurBuf += 9 + pes_header_data_length;

			if (payload_length < PSBUFLEN - m_PsBufLen)									//视频数据拼包
			{
				memcpy(m_psbuf + m_PsBufLen, pCurBuf, payload_length);
				m_PsBufLen += payload_length;
			}

			pCurBuf += payload_length;
			continue;
		}
		else if (*pcode == 0xC0010000)													//PS音频数据解析
		{
			int pes_packet_length = *(pCurBuf + 4) * 256 + *(pCurBuf + 5);
			int pes_header_data_length = *(pCurBuf + 8);
			int payload_length = pes_packet_length - 3 - pes_header_data_length;
			pCurBuf += 9 + pes_header_data_length;

			if (m_psAudioType == 155)													//音频解码,只支持(SVAC audio)类型音频，其他类型音频自行处理
			{
				decode(pCurBuf, payload_length, Audio_frame);
			}
			pCurBuf += payload_length;
			
			continue;
		}
		else
		{
			break;
		}
	}

	decode(m_psbuf, m_PsBufLen, frametype);

	m_PsBufLen = 0;

	return;
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

int SvacPlayer::decode(unsigned char* pBuf, int len, unsigned char tag)
{

	if (tag == Audio_frame && m_SvacHandle != ((HANDLE)-1))
	{
		if (m_pAudioDecodeBuf == NULL)
		{
			m_pAudioDecodeBuf = new unsigned char[10 * 1024];
		}

		if (m_pAudioDecodeBuf != NULL)
		{
			memset(m_pAudioDecodeBuf, 0, 10 * 1024);
			int declen = g_pfZXSVACDec_DecodeAudio(m_SvacHandle, pBuf, len, m_pAudioDecodeBuf);
			if (declen > 0)
			{
				char c8Str[64];
				sprintf(c8Str, "port_%d_audio_16bit_littleEnd_8K_Mono.pcm", m_port + 1, m_nFrameIndex);
				FILE *pFile = fopen(c8Str, "ab+");
				if (pFile != NULL)
				{
					fwrite(m_pAudioDecodeBuf, 1, declen, pFile);
					fclose(pFile);
				}
			}
		}
		return 1;
	}

	if (0)
	{
		if (tag == I_frame)
		{
			m_nFrameGop++;
		}
		if (m_nFrameGop == 1)
		{
			char c8Str[64];
			sprintf(c8Str, "port_%d_frame_%d.bin", m_port, m_nFrameIndex);
			FILE *pFile = fopen(c8Str, "wb+");
			if (pFile != NULL)
			{
				fwrite(pBuf, 1, len, pFile);
				fclose(pFile);
			}
		}
		if (m_nFrameGop != 0)
		{
			m_nFrameIndex++;
		}
	}

	if (tag == I_frame)
	{
		m_FindIFrame = true;
		SVAC_PREFETCH_PARAM param;
		memset(&param, 0, sizeof(SVAC_PREFETCH_PARAM));
		int ret = g_pfZXSVACDec_PrefetchParam(m_SvacHandle, pBuf, len, &param);
		if (ret == 0)
		{
			if (m_nNeedSnapCnt > 0)
			{

			}
			PRINT("ZXSVACDec_PrefetchParam,width = %d,height = %d,bit_depth_luma = %d,spatial_svc_flag = %d\n",
				param.width, param.height, param.bit_depth_luma, param.spatial_svc_flag);
			if (m_nPlayCount == 0)
			{
				char filelog[256];
				sprintf(filelog, "svac_log%d.log", m_port + 1);
				FILE* pflog = fopen(filelog, "a+");

				char temp[512] = { 0 };
				sprintf(temp, "ZXSVACDec_PrefetchParam,width = %d,height = %d,bit_depth_luma = %d,svc_flag = %d\n",
					param.width, param.height, param.bit_depth_luma, param.spatial_svc_flag);

				fwrite(temp, strlen(temp), 1, pflog);
				fclose(pflog);
			}

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
					return 0;
				}
				if (!m_bCanPreview)
				{
					if (m_render)
					{
						m_render->clean();
						delete m_render;
						m_render = NULL;
					}
				}

				if ((!m_cbAddText) && m_bCanPreview)
				{
					if (m_render)
					{
						m_render->clean();
						delete m_render;
						m_render = NULL;
					}

					m_render = new D3DOffscreenRender();
					int ret = m_render->init(m_port, m_hwnd, m_param.width, m_param.height);
					//ShowError(ret);
				}

				if (m_SvacHandle != HANDLE(-1))
				{
					g_pfZXSVACDec_Close(m_SvacHandle);
					m_SvacHandle = HANDLE(-1);
				}

				int res = 0;

				res = g_pfZXSVACDec_Open(m_SvacHandle, m_threadnum, m_coreId, m_param.svac_version);		//ZXSVACDec_Open返回值非0为成功，非0的数字是授权剩余天数。当正式授权过期时，也可以进行解码（m_SvacHandle为有效值），但解码的YUV会有logo																							//试用授权过期时，不可以进行解码（m_SvacHandle为无效值）。
				WriteError("ZXSVACDec_Open", res);
				//ShowError(res);

				if (m_SvacHandle == HANDLE(-1))
				{
					ShowError(res);
					return 0;
				}

				res = g_pfZXSVACDec_SetUKey(m_SvacHandle, m_ukey_name.GetString(), m_ukey_pin.GetString());
				if (res > 0)
				{
					/* PIN error, and the retry count left */
					PRINT("ZXSVACDec_SetUKey, UKey PIN error, and the remain retry cnt is :%d\n", res);
				}
				else if (res == 0)
				{	/* it's ok */
					PRINT("ZXSVACDec_SetUKey OK\n");
				}
				else if(res == -1)
				{
					PRINT("ZXSVACDec_SetUKey,unknow error\n");
					/* unknow err,  */
				}
				else if (res == -2)
				{
					/* ukey is not insert */
					PRINT("ZXSVACDec_SetUKey, UKey is not insert or not matched\n");
				}
				else if (res <= -3)
				{
					PRINT("ZXSVACDec_SetUKey, UKey PIN error, and the remain retry cnt is 0(locked)\n");
				}

				res = g_pfZXSVACDec_SetCommonExtensionInfo(m_SvacHandle, TRUE, TRUE);
				WriteError("ZXSVACDec_SetCommonExtensionInfo", res);
				ShowError(res);

				if (m_password != "")
				{
					PRINT("ZXSVACDec_SetPassWord,password = %s\n", m_password.GetString());
					g_pfZXSVACDec_SetPassWord(m_SvacHandle, (char *)m_password.GetString(), m_password.GetLength());
				}

				if ((m_crypt_key_for_video.GetLength() != 0) && (m_crypt_key_version.GetLength() != 0))
				{
					if (g_pfZXSVACDec_SetVKEKBase64 != NULL)
					{
						g_pfZXSVACDec_SetVKEKBase64(m_SvacHandle,
							(char *)(m_crypt_key_for_video.GetString()), m_crypt_key_for_video.GetLength(),
							(char *)(m_crypt_key_version.GetString()), m_crypt_key_version.GetLength());
					}

				}

				if (m_cert_file_root.GetLength() != 0 || m_cert_file_self.GetLength() != 0)
				{
					do
					{
						int s32ValidCertCnt = 0;		/* 数组中间不能出现无效参数 */
						SVAC_X509CertInfo stInfo[2] = { { 0 } };

						FILE *pFile = fopen(m_cert_file_root.GetString(), "rb");
						if (pFile != NULL)
						{
							fseek(pFile, 0, SEEK_END);
							int s32FileSize = ftell(pFile);

							stInfo[s32ValidCertCnt].CertificateData = (char *)malloc(s32FileSize);
							if (stInfo[s32ValidCertCnt].CertificateData == NULL)
							{
								fclose(pFile);
								break;
							}

							fseek(pFile, 0, SEEK_SET);
							fread(stInfo[s32ValidCertCnt].CertificateData, s32FileSize, 1, pFile);
							fclose(pFile);
							stInfo[s32ValidCertCnt].CertificateSize = s32FileSize;
							s32ValidCertCnt++;
						}

						pFile = fopen(m_cert_file_self.GetString(), "rb");
						if (pFile != NULL)
						{
							fseek(pFile, 0, SEEK_END);
							int s32FileSize = ftell(pFile);

							stInfo[s32ValidCertCnt].CertificateData = (char *)malloc(s32FileSize);
							if (stInfo[s32ValidCertCnt].CertificateData == NULL)
							{
								fclose(pFile);
								free(stInfo[s32ValidCertCnt].CertificateData);
								break;
							}

							fseek(pFile, 0, SEEK_SET);
							fread(stInfo[s32ValidCertCnt].CertificateData, s32FileSize, 1, pFile);
							fclose(pFile);
							stInfo[s32ValidCertCnt].CertificateSize = s32FileSize;
							s32ValidCertCnt++;
						}
						printf("g_pfZXSVACDec_SetTrustCerts: %p, insert count: %d\n",
							g_pfZXSVACDec_SetTrustCerts, s32ValidCertCnt);
						if (g_pfZXSVACDec_SetTrustCerts != NULL && s32ValidCertCnt != 0)
						{
							SVAC_SCP stSCP = { 0 };
							stSCP.bEnable = 1;
							stSCP.nCertCnt = s32ValidCertCnt;
							stSCP.pX509CertInfo = stInfo;
							g_pfZXSVACDec_SetTrustCerts(m_SvacHandle, &stSCP);

						}
						if (stInfo[0].CertificateData != NULL)
							free(stInfo[0].CertificateData);
						if (stInfo[1].CertificateData != NULL)
							free(stInfo[1].CertificateData);
					} while (0);
				}




			}
		}
		else
		{
			//return 0;
		}
	}
	else
	{
		if (m_FindIFrame == false)
		{
			return 0;
		}
		if (m_param.width == 0 || m_param.height == 0)
		{
			return 0;
		}
	}

	if (m_SvacHandle == HANDLE(-1))
	{
		return 0;
	}

	DEC_INPUT_PARAM		dec_in;
	memset(&dec_in, 0, sizeof(DEC_INPUT_PARAM));
	DEC_OUTPUT_PARAM	dec_out;
	memset(&dec_out, 0, sizeof(DEC_OUTPUT_PARAM));

	//dec_in.pBitstream = pBuf;
	//dec_in.nLen = len;
	//dec_in.chroma_format_idc = 1;
	//dec_in.bSvcdec = 1;
	//dec_in.bExtdecOnly = 0;
	//dec_in.check_authentication_flag = 2;

	//memset(m_buf, 0, m_BytePerSample*m_param.width* m_param.height * 15 / 8 + 16);
	//if (m_param.svc_flag)
	//{
	//	memset(m_svc_buf, 0, m_BytePerSample* m_param.width* m_param.height * 4 * 15 / 8 + 16);
	//}

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
// 	dec_out.nWidth = m_param.width;
// 	dec_out.nHeight = m_param.height;

	EXT_INFO_COMMON *_ext_info_common = (EXT_INFO_COMMON *)malloc(sizeof(EXT_INFO_COMMON));
	if (_ext_info_common != NULL)
	{
		memset(_ext_info_common, 0, sizeof(EXT_INFO_COMMON));
	}
	CAutoFree csAutoFree(_ext_info_common);

	m_nFileFrameCnt++;
	m_nTotalDecodeFrameCnt++;

	unsigned long long nCurTime = timeGetTime();
	int ret = 0;
	int tryCnt = 0;
	if (m_pBufDecode == NULL)
	{
		m_pBufDecode = new unsigned char[PSBUFLEN];
		if (m_pBufDecode == NULL)
		{
			return -1;
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
					g_pfZXSVACDec_SetPassWord(m_SvacHandle, (char *)m_password.GetString(), strlen(m_password));
				}
			}
		}
		else
		{
			break;
		}

		if (ret > 0)
		{

			break;
		}

		tryCnt++;
	}

	//free(pBufDecode);
	
	m_nFileDecodeTime += (timeGetTime() - nCurTime);

	m_decodeCount++;

	if (ret > 0)
	{
		if ((dec_out.nIsEffect & 0x1) == 0)
			return -13;

		if (!m_cbAddText) {
			if ((m_nPlayCount == 0) && m_bSaveYUVFile)
			{
				char file[256];
				char yuv_time[256] = {0};
				sprintf(yuv_time, "(%d-%d-%d-%d-%d-%d)",
					m_beginsystime.wYear,
					m_beginsystime.wMonth,
					m_beginsystime.wDay,
					m_beginsystime.wHour,
					m_beginsystime.wMinute,
					m_beginsystime.wSecond);
				sprintf(file, "port_%d_svac_yuv_%dbit_%d_%d%s.yuv", m_port + 1, m_param.bit_depth_luma, dec_out.nWidth, dec_out.nHeight, yuv_time);		//基本层YUV输出，未开启SVC时，为1960*1080。当开启SVC时，为960*540。
				FILE* pf = fopen(file, "ab+");
				fwrite(dec_out.pY, m_BytePerSample* dec_out.nWidth*dec_out.nHeight, 1, pf);
				fwrite(dec_out.pU, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf);
				fwrite(dec_out.pV, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf);
				fclose(pf);
				if (dec_out.spatial_svc_flag && (dec_out.SVAC_STATE & 1))				//判断是否有增强层，当开启SVC时，为1960*1080。
				{
					char file_el[256];
					sprintf(file_el, "port_%d_svac_EL_yuv_%dbit_%d_%d_%s.yuv", m_port + 1, m_param.bit_depth_luma, dec_out.nWidth_EL, dec_out.nHeight_EL, yuv_time);
					FILE* pf_el = fopen(file_el, "ab+");
					fwrite(dec_out.pY_SVC, m_BytePerSample* dec_out.nWidth_EL*dec_out.nHeight_EL, 1, pf_el);
					fwrite(dec_out.pU_SVC, m_BytePerSample*dec_out.nWidth_EL*dec_out.nHeight_EL / 4, 1, pf_el);
					fwrite(dec_out.pV_SVC, m_BytePerSample*dec_out.nWidth_EL*dec_out.nHeight_EL / 4, 1, pf_el);
					fclose(pf_el);
				}
			}

			if (m_nNeedSnapCnt > 0)
			{
				char file[256];
				sprintf(file, "snap(port %d)_svac_yuv_%d_%d.yuv", m_port + 1, dec_out.nWidth, dec_out.nHeight);
				FILE* pf = fopen(file, "ab+");
				fwrite(dec_out.pY, m_BytePerSample* dec_out.nWidth*dec_out.nHeight, 1, pf);
				fwrite(dec_out.pU, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf);
				fwrite(dec_out.pV, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf);
				fclose(pf);
				m_nNeedSnapCnt--;
			}

			printf_Ext_Info_Common(_ext_info_common, &(dec_out.DecExtData));


			PRINT("SVAC_DEC_SUCCESS,SvacHandle = %d,nWidth_bl = %d,nHeight_bl = %d, nWidth_el = %d, nHeight_el = %d, "
				"spatial_svc_flag = %d,SVAC_STATE = %d,FrameNum = %d，auth_bl = %d,auth_el = %d\n",
				m_SvacHandle, 
				dec_out.nWidth, dec_out.nHeight, dec_out.nWidth_EL, dec_out.nHeight_EL,
				dec_out.spatial_svc_flag, dec_out.SVAC_STATE, dec_out.FrameNum,
				dec_out.CheckAuthData.authentication_result_bl, dec_out.CheckAuthData.authentication_result_el);

			if (m_bCanPreview)
			{
				if (m_render != NULL)
				{
					int ret = -1;
					if (dec_out.spatial_svc_flag && (dec_out.SVAC_STATE & 1))		//有增强层时，渲染增强层
					{
						ret = m_render->render((unsigned char*)dec_out.pY_SVC, (unsigned char*)dec_out.pU_SVC, (unsigned char*)dec_out.pV_SVC, dec_out.nWidth_EL, dec_out.nHeight_EL, m_BytePerSample);
					} else {
						ret = m_render->render((unsigned char*)dec_out.pY, (unsigned char*)dec_out.pU, (unsigned char*)dec_out.pV, dec_out.nWidth, dec_out.nHeight, m_BytePerSample);
					}
					if (ret < 0 && (m_nPlayCount == 0))//渲染失败时，保存YUV文件
					{
						if (dec_out.spatial_svc_flag && (dec_out.SVAC_STATE & 1))
						{
							char render_err_ex[256] = { 0 };
							sprintf(render_err_ex, "[render_err](%d_%d_%d_%d).yuv", m_port + 1, m_param.bit_depth_luma, dec_out.nWidth_EL, dec_out.nHeight_EL);
							FILE* pf_err = fopen(render_err_ex, "ab+");
							fwrite(dec_out.pY_SVC, m_BytePerSample* dec_out.nWidth_EL*dec_out.nHeight_EL, 1, pf_err);
							fwrite(dec_out.pU_SVC, m_BytePerSample*dec_out.nWidth_EL*dec_out.nHeight_EL / 4, 1, pf_err);
							fwrite(dec_out.pV_SVC, m_BytePerSample*dec_out.nWidth_EL*dec_out.nHeight_EL / 4, 1, pf_err);
							fclose(pf_err);
						} else {
							char render_err_ex[256] = { 0 };
							sprintf(render_err_ex, "[render_err](EL_%d_%d_%d_%d).yuv", m_port + 1, m_param.bit_depth_luma, dec_out.nWidth, dec_out.nHeight);
							FILE* pf_err = fopen(render_err_ex, "ab+");
							fwrite(dec_out.pY, m_BytePerSample* dec_out.nWidth*dec_out.nHeight, 1, pf_err);
							fwrite(dec_out.pU, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf_err);
							fwrite(dec_out.pV, m_BytePerSample*dec_out.nWidth*dec_out.nHeight / 4, 1, pf_err);
							fclose(pf_err);
						}
					}
					WriteError("m_render->render", ret);
					//ShowError(ret);
				}
			}

		}

		return 1;
	}
	else if (ret == SVAC_DEC_PASSWORD_ERROR)
	{
		WriteLog("SVAC_DEC_PASSWORD_ERROR,SvacHandle = %d\n", m_SvacHandle);
		m_FindIFrame = false;
	}
	else if (ret == SVAC_DEC_NO_PICTURE)
	{
		WriteLog("SVAC_DEC_NO_PICTURE,SvacHandle = %d\n", m_SvacHandle);
	}
	else if (ret == SVAC_DEC_ERROR)
	{
		WriteLog("SVAC_DEC_ERROR,SvacHandle = %d\n", m_SvacHandle);
		m_FindIFrame = false;
	}
	else
	{
		WriteLog("SVAC_DEC Unknown Error,ret = %d,SvacHandle = %d\n", ret, m_SvacHandle);
	}

	return 0;
}

inline void *GetTLVData(TLV *pTLV)
{
	return (void *)(pTLV->length > 64 ? pTLV->huge_value : pTLV->value);
}


void SvacPlayer::printf_Ext_Info_Common(EXT_INFO_COMMON* _ext_info_common, SVAC_EXT_DATA* ext_data)
{
	if (m_nPlayCount != 0)
		return;

	if (_ext_info_common != NULL)
	{
		for (int i = 0; i < _ext_info_common->ext_info_custom_num; i++)
		{
			EXT_INFO ext_info;
			memcpy(&ext_info, &_ext_info_common->ext_info[i], sizeof(EXT_INFO));

			if (ext_info.type == SVAC_EXT_TYPE_IDCARD)
			{
				char sex[16] = { 0 };
				memcpy(sex, ext_info.info.IdCard.sex, 2);
				WriteLog("SVAC_EXT_IDCARD,id_number:%s,name:%s,sex:%s,nation:%s,birthday:%s,address:%s,time:%s,\n",
					ext_info.info.IdCard.id_number,
					ext_info.info.IdCard.name,
					sex,
					ext_info.info.IdCard.nation,
					ext_info.info.IdCard.bD,
					ext_info.info.IdCard.address,
					ext_info.info.IdCard.time);
			}
			else if (ext_info.type == SVAC_EXT_TYPE_OSD)
			{
				WriteLog("SVAC_EXT_OSD,svactime:%d-%d-%d %d:%d:%d,OSD_DATA:%s\n",
					ext_data->time_year,
					ext_data->time_month,
					ext_data->time_day,
					ext_data->time_hour,
					ext_data->time_minute,
					ext_data->time_second,
					ext_info.info.OSD_info.OSD_DATA);
			}
			else if (ext_info.type == SVAC_EXT_TYPE_CAR)
			{
				WriteLog("SVAC_EXT_CAR,plate:%s,captureTime:%d-%d-%d %d:%d:%d:%d\n",
					ext_info.info.Car_info.plate,
					ext_info.info.Car_info.captureTime[0],
					ext_info.info.Car_info.captureTime[1], 
					ext_info.info.Car_info.captureTime[2],
					ext_info.info.Car_info.captureTime[4], 
					ext_info.info.Car_info.captureTime[5], 
					ext_info.info.Car_info.captureTime[6], 
					ext_info.info.Car_info.captureTime[7]);
			}
			else if (ext_info.type == SVAC_EXT_TYPE_IoT)
			{
				WriteLog("get some IoT info, %d-%d-%d %d:%d:%d.%d\n",
					ext_data->time_year,
					ext_data->time_month,
					ext_data->time_day,
					ext_data->time_hour,
					ext_data->time_minute,
					ext_data->time_second, (ext_data->time_fractional >> 6) * 4);
			}
			else if (ext_info.type == SVAC_EXT_TYPE_ANALYSIS_EXTENSION_EX ||
				ext_info.type == SVAC_EXT_TYPE_ANALYSIS_EXTENSION_VEHICLE)
			{
				VEHICLE_PROPERTY *pInfo = &(ext_info.info.Vechicle_Property);

				WriteLog("get vehicle info, vehicle_id: %d\n", pInfo->vehicle_properies_tlvs.id);
				WriteLog("\t abs_time: used flag %d\n", pInfo->abs_time.enable);

				if (pInfo->abs_time.enable)
				{
					if (pInfo->abs_time.time_format == 0)
					{
						__time64_t s64Time = pInfo->abs_time.abt.tv.tv_sec;

						struct tm stTime = { 0 };

						_localtime64_s(&stTime, &s64Time);
						_gmtime64_s(&stTime, &s64Time);

						WriteLog("\t abs_time: %4d-%02d-%02d %02d:%02d:%02d.%06d\n",
							stTime.tm_year + 1900,
							stTime.tm_mon,
							stTime.tm_mday,
							stTime.tm_hour,
							stTime.tm_min,
							stTime.tm_sec,
							pInfo->abs_time.abt.tv.tv_usec);
					}
					else
					{
						VITECH_U64 tv_usec = (VITECH_U64)((((double)pInfo->abs_time.abt.tim_abs.sec_fraction) / 16383.0) * 1000000.0 + 0.5);

						WriteLog("\t abs_time: %4d-%02d-%02d %02d:%02d:%02d.%06d\n",
							pInfo->abs_time.abt.tim_abs.year + 2000,
							pInfo->abs_time.abt.tim_abs.month,
							pInfo->abs_time.abt.tim_abs.day,
							pInfo->abs_time.abt.tim_abs.hour,
							pInfo->abs_time.abt.tim_abs.min,
							pInfo->abs_time.abt.tim_abs.second,
							tv_usec);
					}
				}



				//WriteLog("\n\t info: used flag %d\n", pInfo->vehicle_info.enable);
				//WriteLog("\t info: ID %d\n", pInfo->vehicle_info.id);

				int s32TagCnt = -1;

				for (unsigned short j = 0; 
					j < sizeof(VEHICLE_PROPERTY::vehicle_properies_tlvs) / sizeof(TLVS);
					j++)
				{
					TLVS *pTLVS = &(pInfo->vehicle_properies_tlvs) + j;
					TLV *pTLV = pTLVS->tlv_data;
					for (unsigned short i = 0; i < pTLVS->tlv_num; i++, pTLV++)
					{
						char *pData = (char *)GetTLVData(pTLV);
						switch (pTLV->type)
						{
							case 0x02:
							{
								REC *pRect = (REC *)pData;

								WriteLog("\t info(length %d): rec x: %d, y: %d, w: %d, h: %d\n",
									pTLV->length,
									pRect->width,
									pRect->height,
									pRect->point.x,
									pRect->point.y);

								break;
							}
							case 0x03:
							{
								WriteLog("\t lan: (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x04:
							{
								WriteLog("\t direction: (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x11:
							{
								WriteLog("\t plate: num (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x12:
							{
								WriteLog("\t plate: num score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x13:
							{
								WriteLog("\t plate: color (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x14:
							{
								WriteLog("\t plate: color score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x15:
							{
								if (pTLV->length == sizeof(REC))
								{
									REC *pRect = (REC *)pData;
									WriteLog("\t plate: pos (length %d) pos: w-%d, h-%d, x-%d, y-%d\n",
										pTLV->length,
										pRect->width,
										pRect->height,
										pRect->point.x,
										pRect->point.y);
								}
								else
								{
									WriteLog("\t plate: pos (length error %d)\n", pTLV->length);
								}
								break;
							}

							case 0x21:
							{
								WriteLog("\t brand: car type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x22:
							{
								WriteLog("\t brand: car type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x23:
							{
								WriteLog("\t brand: car brand (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x24:
							{
								WriteLog("\t brand: car brand score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x25:
							{
								WriteLog("\t brand: car mode (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x26:
							{
								WriteLog("\t brand: car mode score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x27:
							{
								WriteLog("\t brand: car color (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x28:
							{
								WriteLog("\t brand: car color score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x29:
							{
								WriteLog("\t brand: car year model (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x2a:
							{
								WriteLog("\t brand: car year model score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}


							case 0x31:
							{
								WriteLog("\t sunroof: score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x32:
							{
								WriteLog("\t sunroof: score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x33:
							{
								WriteLog("\t sunroof: luggage score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x34:
							{
								WriteLog("\t sunroof: luggage score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x35:
							{
								WriteLog("\t sunroof: sun louver score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x36:
							{
								WriteLog("\t sunroof: sun louver score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x37:
							{
								WriteLog("\t sunroof: hang score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x38:
							{
								WriteLog("\t sunroof: hang score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}

							case 0x41:
							{
								WriteLog("\t tag: (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								s32TagCnt = ((unsigned char *)pData)[0];
								break;
							}
							case 0x42:
							{
								WriteLog("\t tag: (length %d)\n", pTLV->length);
								if (s32TagCnt > 0)
								{
									if (pTLV->length != sizeof(REC) * s32TagCnt)
									{
										WriteLog("\t tag length error\n");
									}
									else
									{
										REC *pRect = (REC*)pData;
										for (int j = 0; j < s32TagCnt; j++, pRect++)
										{
											WriteLog("\t tag(%d) pos: w-%d, h-%d, x-%d, y-%d\n",
												j + 1,
												pRect->width,
												pRect->height,
												pRect->point.x,
												pRect->point.y);

										}
									}
								}

								break;
							}


							case 0x51:
							{
								WriteLog("\t driver: has driver score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x52:
							{
								WriteLog("\t driver: has driver score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x53:
							{
								WriteLog("\t driver: has seat belt score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x54:
							{
								WriteLog("\t driver: has seat belt score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x55:
							{
								WriteLog("\t driver: has calling score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x56:
							{
								WriteLog("\t driver: has calling score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x57:
							{
								if (pTLV->length == sizeof(REC))
								{
									REC *pRect = (REC *)pData;
									WriteLog("\t driver: pos (length %d) pos: w-%d, h-%d, x-%d, y-%d\n",
										pTLV->length,
										pRect->width,
										pRect->height,
										pRect->point.x,
										pRect->point.y);
								}
								else
								{
									WriteLog("\t driver: pos (length error %d)\n", pTLV->length);
								}
								break;
							}


							case 0x61:
							{
								WriteLog("\t codriver: has driver score (length %d) %d\n", pTLV->length, pData[0]);
								break;
							}
							case 0x62:
							{
								WriteLog("\t codriver: has driver score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x63:
							{
								WriteLog("\t codriver: has seat belt score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x64:
							{
								WriteLog("\t codriver: has seat belt score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x65:
							{
								WriteLog("\t codriver: has calling score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x66:
							{
								WriteLog("\t codriver: has calling score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x67:
							{
								if (pTLV->length == sizeof(REC))
								{
									REC *pRect = (REC *)pData;
									WriteLog("\t codriver: pos (length %d) pos: w-%d, h-%d, x-%d, y-%d\n",
										pTLV->length,
										pRect->width,
										pRect->height,
										pRect->point.x,
										pRect->point.y);
								}
								else
								{
									WriteLog("\t codriver: pos (length error %d)\n", pTLV->length);
								}
								break;
							}



							case 0x71:
							{
								WriteLog("\t passenger: has child score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x72:
							{
								WriteLog("\t passenger: has child score auxiliary (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}

							default:
								break;
						}

					}
				}
			}
			else if (ext_info.type == SVAC_EXT_TYPE_ANALYSIS_EXTENSION_PEOPLE)
			{
				PEOPLE_PROPERTY *pInfo = &(ext_info.info.People_Property);
				WriteLog("get people info:\n");
				{
					MTAR *pRect = &(pInfo->people_info);
					FACE_INFO *pFaceInfo = pRect->finfos;
					for (unsigned short i = 0; i < pRect->target_num; i++, pFaceInfo++)
					{
						WriteLog("\t face info: id %d\n", pFaceInfo->id);
						WriteLog("\t face info: color %04x\n", pFaceInfo->color);
						WriteLog("\t face info: rec x: %d, y: %d, w: %d, h: %d\n",
							pFaceInfo->rec.point.x, pFaceInfo->rec.point.y,
							pFaceInfo->rec.width, pFaceInfo->rec.height);
					}
				}

				for (unsigned short j = 0;
					j < sizeof(PEOPLE_PROPERTY::people_properties_tlvs) / sizeof(TLVS);
					j++)
				{
					TLVS *pTLVS = &(pInfo->people_properties_tlvs) + j;
					TLV *pTLV = pTLVS->tlv_data;
					for (unsigned short i = 0; i < pTLVS->tlv_num; i++, pTLV++)
					{
						char *pData = (char *)GetTLVData(pTLV);
						switch (pTLV->type)
						{
							case 0x01:
							{
								WriteLog("\t people: age type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x02:
							{
								WriteLog("\t people: age type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x11:
							{
								WriteLog("\t people: race type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x12:
							{
								WriteLog("\t people: race type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x21:
							{
								WriteLog("\t people: secuality type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x22:
							{
								WriteLog("\t people: secuality type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}


							case 0x31:
							{
								WriteLog("\t people dress: tops type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x32:
							{
								WriteLog("\t people dress: tops type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x33:
							{
								WriteLog("\t people dress: tops color (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x34:
							{
								WriteLog("\t people dress: tops color score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x41:
							{
								WriteLog("\t people dress: lower type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x42:
							{
								WriteLog("\t people dress: lower type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x43:
							{
								WriteLog("\t people dress: lower color (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x44:
							{
								WriteLog("\t people dress: lower color score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x51:
							{
								WriteLog("\t people dress: backpack type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x52:
							{
								WriteLog("\t people dress: backpack type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x53:
							{
								WriteLog("\t people dress: hat type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x54:
							{
								WriteLog("\t people dress: hat type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}
							case 0x55:
							{
								WriteLog("\t people dress: hair type (length %d) %s\n", pTLV->length, pData);
								break;
							}
							case 0x56:
							{
								WriteLog("\t people dress: hair type score (length %d) %d\n", pTLV->length, ((unsigned char *)pData)[0]);
								break;
							}

							default:
								break;
						}
					}
				}
			}
		}
	}
	else
	{
		WriteLog("_ext_info_common memory error\n");
	}

}

void SvacPlayer::SetAddTextCallBack(void* pUser, Callback_AddText _cbAddText) {
	m_pUser = pUser;
	m_cbAddText = _cbAddText;
};

void SvacPlayer::GetVersion(char *_version)
{
	SDK_VERSION stuSdkVersion;
	memset(&stuSdkVersion, 0, sizeof(SDK_VERSION));
	int iRet = g_pfZXSVACDec_GetSdkVersion(&stuSdkVersion);
	if (iRet == 0)
	{
		DWORD nVer = stuSdkVersion.dwSdkVersion;
		DWORD nVer1 = (nVer & 0xff000000) >> 24;
		DWORD nVer2 = (nVer & 0x00ff0000) >> 16;
		DWORD nVer3 = (nVer & 0x0000ff00) >> 8;
		DWORD nVer4 = nVer & 0x000000ff;
		
		DWORD nVer_ex = stuSdkVersion.dwSVACDecoderVersion;
		DWORD nVer1_ex = (nVer_ex & 0xff000000) >> 24;
		DWORD nVer2_ex = (nVer_ex & 0x00ff0000) >> 16;
		DWORD nVer3_ex = (nVer_ex & 0x0000ff00) >> 8;
		DWORD nVer4_ex = nVer_ex & 0x000000ff;

		sprintf(_version,"SvacSdkVersion:(%d.%d.%d.%d) SvacDecVersion:(%d.%d.%d.%d)", nVer1, nVer2, nVer3, nVer4, nVer1_ex, nVer2_ex, nVer3_ex, nVer4_ex);
	}
}

int SvacPlayer::ModifyVerValue(int _iVersion, int _iWidth, int _iHeight)
{
	int iVersion = _iVersion;
	iVersion |= (_iWidth << 2);
	iVersion |= (_iHeight << 16);
	return iVersion;
}
