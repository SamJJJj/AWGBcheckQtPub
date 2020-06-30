#pragma once

//#include "utils.h"
#include "ZxSvacDecLib.h"
//#include "d3doffscreenrender.h"
//#include <string>
//using namespace std;

//#include "SvacDecDemoRtsp.h"

#define  PS_Packet_Start_Code_Len       4  
#define  PS_Header_Len_aligned          16      //考虑字节对齐下的PS_Header
#define MUX_RATE 6106

enum
{
	I_frame = 1,
	B_frame = 2,
	P_frame = 3,

	Audio_frame = 4,
};

//
//enum
//{
//	EXTTYPE_SVAC_BASE_LAYER_LENGTH = 1,
//};
//typedef struct _SVACStreamHeaderInfo
//{
//	unsigned char tag[4];
//	unsigned char encodeType;
//	unsigned char resv1;
//	unsigned char rate;        //2
//	unsigned char seq;
//	unsigned short width;      // 4&5
//	unsigned short height;		//6&7
//	unsigned int time;			//8-11
//	unsigned char resv2;		//12
//	unsigned char resv3;		//13
//	unsigned char extNum;		//14
//	unsigned char resv4;		//15
//	unsigned int len;		//15
//}SVACStreamHeaderInfo;
//
//typedef enum
//{
//	Stream_No,
//	Stream_NewStream,
//	Stream_AviStream,
//	Stream_ShStream,
//	Stream_Standard,
//	Stream_DHSTD,
//	Stream_SVAC,
//	Stream_PS
//}STREAMDATA_TYPE;


#define AUTH_FUN_DEFINE(fun) extern PF##fun g_pf##fun
#define AUTH_FUN_INIT(fun) PF##fun g_pf##fun = NULL
#define AUTH_GET_FUN(fun) g_pf##fun  = (PF##fun)GetProcAddress(m_hInst, #fun);\
if (g_pf##fun == NULL)\
{\
g_svac_critsec.UnLock();\
return 0;\
}

typedef int(__stdcall* PFZXSVACDec_Init)();
typedef int(__stdcall* PFZXSVACDec_Open)(HANDLE& handle, int thread_num, int core_num, int svac_version);
typedef int(__stdcall* PFZXSVACDec_Close)(HANDLE handle);

typedef int(__stdcall* PFZXSVACDec_PrefetchParam)(HANDLE handle, unsigned char *pBuf, int uBufLen, SVAC_PREFETCH_PARAM *pPreParam);
typedef int(__stdcall* PFZXSVACDec_Decode)(HANDLE handle,
	DEC_INPUT_PARAM* decin,
	DEC_OUTPUT_PARAM* decout,
	EXT_INFO_COMMON* pExt_info);
typedef int(__stdcall* PFZXSVACDec_DecodeAudio)(HANDLE handle, unsigned char *src_buf, unsigned int buflen, unsigned char* dest_buf);
typedef int(__stdcall* PFZXSVACDec_SetPassWord)(HANDLE handle, char* password, int pswlength);
typedef int(__stdcall* PFZXSVACDec_SetTrustCerts)(HANDLE handle, SVAC_SCP *pStSCP);
typedef int(__stdcall* PFZXSVACDec_SetRSAKey)(HANDLE handle, unsigned int rsa_e, unsigned int rsa_n);
typedef int(__stdcall* PFZXSVACDec_SetVKEKBase64)(HANDLE handle, char *pVKEKBase64,
	int s32Length, char *pVersion, int s32VersionLength);
typedef int(__stdcall* PFZXSVACDec_SetCommonExtensionInfo)(HANDLE handle,
	BOOL bCommonExtensionInfo, BOOL bDeduplication);

typedef int(__stdcall* PFZXSVACDec_GetSdkVersion)(SDK_VERSION* pSdkVersion);
typedef int(__stdcall* PFZXSVACDec_GetSECInfo)(HANDLE handle, SVAC_SEC_INFO *pSecInfo);
typedef int(__stdcall* PFZXSVACDec_SetSecurityLib)(HANDLE handle, void *hVSECLib, void* hVSEC);
typedef int(__stdcall* PFZXSVACDec_GetSupportUKey)(const char **p2Name, int nPointerCnt);
typedef int(__stdcall* PFZXSVACDec_SetUKey)(HANDLE handle, const char *pUKeyName, const char *pKeyPin);



AUTH_FUN_DEFINE(ZXSVACDec_Open);
AUTH_FUN_DEFINE(ZXSVACDec_Close);
AUTH_FUN_DEFINE(ZXSVACDec_PrefetchParam);
AUTH_FUN_DEFINE(ZXSVACDec_Decode);
AUTH_FUN_DEFINE(ZXSVACDec_DecodeAudio);
AUTH_FUN_DEFINE(ZXSVACDec_SetPassWord);
AUTH_FUN_DEFINE(ZXSVACDec_SetTrustCerts);
AUTH_FUN_DEFINE(ZXSVACDec_SetRSAKey);
AUTH_FUN_DEFINE(ZXSVACDec_SetVKEKBase64);
AUTH_FUN_DEFINE(ZXSVACDec_SetCommonExtensionInfo);
AUTH_FUN_DEFINE(ZXSVACDec_GetSdkVersion);
AUTH_FUN_DEFINE(ZXSVACDec_GetSECInfo);
AUTH_FUN_DEFINE(ZXSVACDec_SetSecurityLib);
AUTH_FUN_DEFINE(ZXSVACDec_GetSupportUKey);
AUTH_FUN_DEFINE(ZXSVACDec_SetUKey);
//extern CritSec g_svac_critsec;
//
//int loadsvacdll();
//
//
//typedef unsigned char		u8;
//typedef unsigned int		u32;
//
typedef void(__stdcall *Callback_AddText)(void* pDemo,char * pText);

class SvacPlayer
{
//public:
//	SvacPlayer(int port, HWND hwnd);
//	~SvacPlayer();
//
//	void SetMulticastAddr(string ip = "224.11.0.2", int port = 34444)
//	{
//		if (ip.empty())
//		{
//			m_strMulticastIP = "224.11.0.2";
//			m_nMulticastPort = 34444;
//		}
//		else
//		{
//			m_strMulticastIP = ip;
//			m_nMulticastPort = port;
//		}
//	}
//	int SetSvacFile(CString filename);
//	void SetThreadNum(int num);
//	void SetCoreId(int id);
//	int SetSvacPassWord(CString password);
//	int SetSvacCert(CString root_cert, CString self_cert);
//	int SetSvacVKEK(CString version, CString vkek);
//	int SetSvacUsingUKey(CString UKeyName) 
//	{
//		m_ukey_name = UKeyName;
//		return 0;
//	};
//	int SetSvacUsingUKeyPin(CString UKeyPin)
//	{
//		m_ukey_pin = UKeyPin;
//		return 0;
//	};
//
//	int SetPlayHwnd(HWND hwnd);
//	int Start();
//	int stop();
//
//	STREAMDATA_TYPE checkTag(unsigned char* pBuf, unsigned int size);
//	void DecodePSPacket(unsigned char* pBuf, unsigned int size);
//	int ResearchPSTag(unsigned char* buf, unsigned int len);
//
//	int decode(unsigned char* pBuf, int len, unsigned char tag);
//	void printf_Ext_Info_Common(EXT_INFO_COMMON* _ext_info_common, SVAC_EXT_DATA* ext_data);
//	void SetAddTextCallBack(void* pUser, Callback_AddText _cbAddText);
//	int PlayProc();
//	int RtspPlayProc();
//	void GetVersion(char *_version);
//	void BeginSnapYUV(int nCount) 
//	{ 
//		m_nNeedSnapCnt = nCount < 0 ? 30 : nCount;
//	};
//
//	void ClosePreview(bool boIsClose)
//	{
//		m_bCanPreview = !boIsClose;
//	};
//
//	void CloseFileSave(bool boIsClose)
//	{
//		m_bSaveYUVFile = !boIsClose;
//	};
//
//	int GetSupportUKey(const char **p2Name, int nCnt);
//
//	void WriteLog(const char *pFormat, ...);
//	void ShowError(int error);
//	void WriteError(const char *pFunName, int error);
//	int ModifyVerValue(int _iVersion, int _iWidth, int _iHeight);
//	int m_nPlayCount;
//	int m_port;
//	bool m_bStop;
//	int m_nFrameGop;
//	int m_nFrameIndex;
//
//	unsigned char* m_pPSbuf;
//	int m_PsBufLen;
//
//	int m_nNeedSnapCnt;
//
//	bool m_bSaveYUVFile;
//	bool m_bCanPreview;
//
//	int m_nTotalDecodeFrameCnt;
//	int m_nFileFrameCnt;
//	unsigned long long m_nFileDecodeTime;
//
//private:
//	
//	HANDLE m_hThread;
//	unsigned int m_dwPollProcThreadId;
//	string m_strMulticastIP;
//	int m_nMulticastPort;
//
//	CSvacDecDemoRtsp m_csRtsp;
//
//	HANDLE m_SvacHandle;
//
//	SVAC_PREFETCH_PARAM m_param;
//	int m_BytePerSample;
//	unsigned char* m_buf;
//	unsigned char* m_svc_buf;
//	unsigned char* m_psbuf;
//	int m_nPsBufLen;
//	bool m_FindIFrame;
//
//	unsigned char *m_pAudioDecodeBuf;
//	unsigned char m_psAudioType;
//
//
//	HWND m_hwnd;
//	CString m_filename;
//	CString m_password;
//	CString m_cert_file_root;
//	CString m_cert_file_self;
//	CString m_crypt_key_version;
//	CString m_crypt_key_for_video;
//	CString m_ukey_name;
//	CString m_ukey_pin;
//
//	D3DOffscreenRender* m_render;
//	Callback_AddText m_cbAddText;
//	void* m_pUser;
//	
//	SYSTEMTIME m_beginsystime;
//	SYSTEMTIME m_endsystime;
//
//	unsigned long long m_beginCount;
//	unsigned long long m_endCount;
//	int m_decodeCount;
//
//	int m_threadnum;
//	int m_coreId;
//	unsigned char *m_pBufDecode;				//临时存储裸码流。
};

