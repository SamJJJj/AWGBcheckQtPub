#ifndef _ZXSVACDECLIB_H
#define _ZXSVACDECLIB_H
//#if (defined _WIN32) || (defined _WIN64)
//#include "StdAfx.h"
//#endif

#if (defined _WIN32) || (defined _WIN64)

#define ZXSVACDECLIB_API  __declspec(dllexport)

#define CALLMETHOD __stdcall
#define CALLBACK __stdcall
#define CDECCALL __cdecl

#elif defined linux
#include <stdint.h>
#define ZXSVACDECLIB_API extern "C"

#define CALLMETHOD
#define CALLBACK
#define CDECCALL
#define LONG int
#define BOOL int
#define FALSE 0
#define TRUE 1
#define DWORD uint32_t
#endif

//#if (defined linux)
//typedef void *HANDLE;
//#endif

//#include <map>
//#include <string>
//#include <vector>
#include "savc/base_defs.h"
//
//using namespace std;
//
//#ifndef SAFE_DELETE
//#define SAFE_DELETE(p) { if(p){delete(p);  (p)=NULL;} }
//#endif
//
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p){delete[] (p);  (p)=NULL;} }
#endif
//
//
//#define MAX_CUSTOM_EXT_SIZE          262144 //65536 //1024 // 256

#define MAX_CUSTOM_EXT_COUNT             16

typedef struct _EXT_INFO_COMMON
{
	unsigned char  ext_info_custom_num;
	EXT_INFO ext_info[MAX_CUSTOM_EXT_COUNT];
}EXT_INFO_COMMON;

//#ifndef DEFINE_SVAC_DEC_STATUS
//#define DEFINE_SVAC_DEC_STATUS
//typedef enum /*dec_status*/
//{
//	SVAC_DEC_NO_PICTURE = -4,		//SVAC�����û�н��ͼ��
//	SVAC_DEC_BUF_OVERFLOW,			//�ڲ��������
//	SVAC_DEC_PASSWORD_ERROR,		//SVAC�������
//	SVAC_DEC_ERROR,					//SVAC�������
//	SVAC_DEC_SUCCESS,				
//	SVAC_DEC_NEED_MORE_BITS			
//}SVAC_DEC_STATUS;
//#endif
//

#ifndef DEFINE_SVAC_ROI
#define DEFINE_SVAC_ROI
typedef struct
{
	int		roi_info;		//ROI ������Ϣ,����
	int 	top;			//ROI �������� [y_min]
	int 	left;			//ROI ������� [x_min]
	int 	bot;			//ROI �ױ����� [y_max]
	int 	right;			//ROI �ұ����� [x_max]
}SVAC_ROI;
#endif

#ifndef DEFINE_SVAC_EXT_RESERVED_DATA
#define DEFINE_SVAC_EXT_RESERVED_DATA
typedef struct
{
	unsigned int	extension_id;		//��չ����ID
	unsigned int	extension_length;	//��extension_length֮����ֽڳ��� 
	unsigned char*	reserved_data;		//����չ��Ԫ�����ݣ���extension_length֮��������ֽ� 
}SVAC_EXT_RESERVED_DATA;
#endif


#ifndef DEFINE_SVAC_OSD
#define DEFINE_SVAC_OSD
typedef struct
{
	int  osd_type;  //��Ϣ���ͣ�32��ʱ�䣻33����������ƣ�34���ص��ע
	int  code_type; //�����ʽ��0��UTF-8����
	int  align_type; //�����ʽ��0������룻1���Ҷ���
	int  char_size; //�����С
	int  char_type; //�����ʽ��0���׵׺ڱߣ�1���ڵװױߣ�2����ɫ��3����ɫ��4���Զ���ɫ
	int  osd_top; //�ַ��ϱ߽��ڻ����е�����
	int  osd_left; //�ַ���߽��ڻ����е�����
	int  osd_data_len; //OSD�����ֽڳ���
	unsigned char *osd_data; //OSD����

}SVAC_OSD;
#endif

#ifndef DEFINE_SVAC_AI_DATA
#define DEFINE_SVAC_AI_DATA
typedef struct
{
	int   analysis_id; // ���������id
	int   description_type; //������ʽ
	int   analysis_data_len; //����������ֽڳ���
	unsigned char  *analysis_data; //�����������
}SVAC_AI_DATA;
#endif


#ifndef DEFINE_SVAC_EXT_DATA
#define DEFINE_SVAC_EXT_DATA
typedef struct
{
	int  TimeEnable;			//�Ƿ����ʱ����Ϣ
	int  SurveilEnable;			//�Ƿ���������Ϣ
	int  Surveil_alert_flag;		//�Ƿ����������Ϣ
	int  RoiEnable;			//ͼ���Ƿ����ROI��Ϣ

							//ʱ����Ϣ
	int  time_year;			//��
	int  time_month;			//��
	int  time_day;			//��
	int  time_hour;			//ʱ	
	int  time_minute;			//��	
	int  time_second;			//��
	int  time_fractional;		//���룬��1/16384��Ϊ��λ
	int  time_ref_date_flag;		//Ϊ1��ʾ���������յ���Ϣ
	int  FrameNum;			//ʱ���Ӧͼ��֡��frame_num��I֡Ϊ0������֡��֡��1

							//��չ�¼���Ϣ    
	int  Surveil_event_num[16];	//��չ�¼���
	int  Surveil_event_ID[16];	//��չ�¼�ID

								//�����¼���Ϣ    
	int  Surveil_alert_num;			//�����¼���Ŀ
	int  Surveil_alert_ID[16];		//�����¼���Ϣ		
	int  Surveil_alert_region[16];		//λ�ñ����Ϣ
	int  Surveil_alert_frame_num[16];	//����ͷ�����Ϣ

										//ROI��Ϣ		
	int  		RoiNum;				//ͼ�����ROI��Ŀ
	int  		RoiSkipMode;			//ROI��������ģʽ
	int  		Roi_SVC_SkipMode;	//ROI_SVC��������ģʽ		
	SVAC_ROI	RoiRect[15];			//������ROI������Ϣ		
	SVAC_ROI	Roi_SVC_ElRect[15];	//SVC��ǿ��ROI������Ϣ��SVC����Ч

									//�û��Զ�����չ��Ԫ���ݣ�����ⷵ�ظ���չ��Ԫ��ȫ������
	unsigned int   unit_num; //�Զ�����չ��Ԫ����
	unsigned int   ExtEnableFlag[8]; //ÿһ����־λ��ʾ�Զ�����չ��ϢID�Ƿ���ڷ���ֵ�С����7������λΪ0��ʾextension_idΪ7���Զ�����չ�������ݲ����ڣ�ֵΪ1���ʾ����
	SVAC_EXT_RESERVED_DATA   ext_reserved_data[32];

	int  AIinfoEnable;		//�Ƿ�������ܷ�����Ϣ
	int	OSDEnable;		//�Ƿ����OSD��չ��Ϣ
	int  GInfoEnable; 		//�Ƿ����������Ϣ

							//���ܷ�����Ϣ
	int   analysis_num; //�����������Ŀ
	SVAC_AI_DATA analysis_info[64];

	//OSD��Ϣ
	int	 osd_num; //OSD��Ϣ��Ŀ
	SVAC_OSD osd_info[4];

	//������Ϣ
	int  longitude_type; //0-������1-����
	int  longitude_degree; //���ȶ���
	int  longitude_fraction; //���ȶ����ķ�������
	int  latitude_type; //0-��γ��1-��γ
	int  latitude_degree; //γ�ȶ���
	int  latitude_fraction; // γ�ȶ����ķ�������
	int  gis_height; //�߶ȣ�����Ϊ��λ,
	int  gis_speed; //�ٶȣ�����/��Ϊ��λ
	int  yaw_degree; //����ǣ�0����������������������˳ʱ�����

					 //����������
	int 			Reserved[128];

}SVAC_EXT_DATA;
#endif


#ifndef DEFINE_CHECK_AUTHENTICATION_RESULT
#define DEFINE_CHECK_AUTHENTICATION_RESULT
typedef struct
{
	int		authentication_result_bl;	//����SVC��������ǩ�����0-ʧ�ܣ�1-�ɹ���2-û��ǩ�����ݣ�3-û����ǩ���
	int		authentication_result_el;	//����SVC��ǿ����ǩ�����0-ʧ�ܣ�1-�ɹ���2-û��ǩ�����ݣ�3-û����ǩ���
	int		FrameNum;					//��ǩ�����Ӧ֡��frame_num��I֡Ϊ0������֡��֡��1
	char	camera_id[20];				//��Ƶ��Դ�����ID
} CHECK_AUTHENTICATION_RESULT;
#endif


#ifndef DEFINE_DEC_INPUT_PARAM
#define DEFINE_DEC_INPUT_PARAM
typedef struct
{
	void* pBitstream;	//�����׵�ַ
	int	nLen;		//��������
	int chroma_format_idc;	//����������YUV��ʽ��0- 4:0:0  1- 4:2:0  2- 4:2:2
	int  bSvcdec;		//������������ǿ������ʱ���Ƿ������ǿ�㣬0-��1-��
	int  bExtdecOnly; 	//�Ƿ�ֻ���������е���չ��Ϣ��0-��1-��
	int  check_authentication_flag;//��֡�����Ƿ�����ǩ��0-����ǩ��1-ֻ��I֡��ǩ��2-��֡��ǩ
	int  bTsvcdec;  // ����������ʱ��svc��ǿ������ʱ���Ƿ������ǿ�㣬0-��1-�ǣ�
	int  dec_type; 	// �������ͣ�1-ָ��ΪSVAC1.0����; 2-ָ��ΪSVAC2.0����
					//0-(�ݲ�֧��)�������ڲ�����Ӧʹ��SVAC1.0,����SVAC2.0����;
					//ע��dec_type�ɸ���SVACdec_PrefetchParam��õ�svac_version�������á�
	int  Reserved[24]; //������չ������
} DEC_INPUT_PARAM;
#endif

#ifndef DEFINE_DEC_OUTPUT_PARAM
#define DEFINE_DEC_OUTPUT_PARAM
typedef struct
{
	int	nIsEffect;		//bit0�����λ��������������ṹ����ͼ�������Ƿ���Ч��0-��Ч��1-��Ч��bit1������������ṹ������չ��Ϣ�����Ƿ���Ч��0-��Ч��1-��Ч��bit2������������ṹ������ǩ�����Ƿ���Ч��0-��Ч��1-��Ч��
						/* ͼ���������*/
	void* pY;			//�����y���������ַ��spatial_svc_flagΪ1ʱ����Ϊ������y������ַ
	void* pU;			//�����u���������ַ��spatial_svc_flagΪ1ʱ����Ϊ������u������ַ
	void* pV;			//�����v���������ַ��spatial_svc_flagΪ1ʱ����Ϊ������v������ַ
	void* pY_SVC;		// ����SVC��ǿ������y���������ַ
	void*pU_SVC;		// ����SVC��ǿ������u���������ַ
	void*pV_SVC;		// ����SVC��ǿ������v���������ַ
	int  frameType;  	// 0:I֡  1:P֡  2:B֡
	int 	nWidth;		//֡��
	int 	nHeight;		//֡��
	int 	nWidth_EL;		//����SVC��ǿ��֡��
	int 	nHeight_EL;		//����SVC��ǿ��֡��
	int  chroma_format_idc; //���������YUV��ʽ��0- 4:0:0  1- 4:2:0  2- 4:2:2

	int spatial_svc_flag;	// �����Ƿ�Ϊ����SVCģʽ��0-��1-��

	int	SVAC_STATE;			// ���λ(0)�������SVC��ǿ��ͼ���Ƿ������0-��1-��
	int	luma_bitdepth;		// ��ʾ���ȵı�����ȣ��Ա���Ϊ��λ
	int	chroma_bitdepth;	// ��ʾɫ�ȵı�����ȣ��Ա���Ϊ��λ
	int  FrameNum;	    //ͼ�����ݶ�Ӧ֡��frame_num��I֡Ϊ0������֡��֡��1
						/*��չ��Ϣ����*/
	SVAC_EXT_DATA   DecExtData;  // ��չ��Ϣ
								 /*��ǩ���*/
	CHECK_AUTHENTICATION_RESULT  CheckAuthData;//��ǩ���

	int  Reserved[200];//�����������������չ��
} DEC_OUTPUT_PARAM;
#endif

#ifndef DEFINE_SVAC_PREFETCH_PARAM
#define DEFINE_SVAC_PREFETCH_PARAM
typedef struct
{
	int	width;	//��spatial_svc_flag=0 ʱ,ͼ���ȣ�spatial_svc_flag=1ʱ,ͼ����ǿ���ȡ� 
	int	height;	//��spatial_svc_flag=0 ʱ,ͼ��߶ȣ�spatial_svc_flag=1ʱ,ͼ����ǿ��߶ȡ� 
	int	roi_flag;	//�Ƿ���ROI��0-��1-��
	int	spatial_svc_flag;	//�Ƿ�֧�ֿ���SVC��0-��1-��
	int	chroma_format_idc; 	//YUVͼ���ʽ
	int	bit_depth_luma;		//Y�������ؾ���
	int	bit_depth_chroma;		//UV�������ؾ���
	int svac_version;     //������Ӧ��SVAC��׼�汾��1:SVAC1.0��2: SVAC2.0
	int temporal_svc_flag;	//�Ƿ�֧��ʱ��SVC��0-��1-��
	int spatial_svc_ratio; //��ǿ���������Ŀ������ű�����0: ����Ϊ4:3; 1 :  ����Ϊ2:1; 2 :  ����Ϊ4:1; 3 :  ����Ϊ6:1; 4 :  ����Ϊ8:1
	int frame_rate; //֡�ʣ�0: 25fps��1: 30fps��2: 50fps��3: 60fps��4: ��VUI ����������5,6 :  ������7 :  ��Ч֡�ʡ�����SVAC1.0���̶�ȡֵΪ7��
	int reserved[5];

} SVAC_PREFETCH_PARAM;
#endif

typedef enum
{
	SVAC_ENCRPT_SM1 = 0,
	SVAC_ENCRPT_AES128 = 2,
	SVAC_ENCRPT_SM4 = 3
} SVAC_ENCRYPT_TYPE;

typedef enum
{
	SVAC_ENCRPT_ECB = 0,
	SVAC_ENCRPT_OFB = 3
} SVAC_ENCRYPT_MODE;

typedef enum
{
	SVAC_HASH_MD5 = 0,
	SVAC_HASH_SM3 = 7
} SVAC_HASH_TYPE;

typedef enum
{
	SVAC_SIGN_RSA = 1,
	SVAC_SIGN_SM2 = 3,
	SVAC_SIGN_SM2_BASE64 = 4
} SVAC_SIGN_TYPE;

typedef enum
{
	SVAC_AUTH_Result_NODATA = 0, // û����ǩ���ݡ���֡Ӧ��Ҫ������ǩ����,���ǻ�û���յ�ǩ������ 
	SVAC_AUTH_Result_FAIL = 1,  // ��ǩʧ�� 1
	SVAC_AUTH_Result_OK = 2,  // ��ǩ�ɹ� 2
	SVAC_AUTH_Result_WAIT = 3, // �� HASH ֵ,����û���յ����һ֡ ���� û���յ���һ�� I ֡ , �����ռ����ݵĹ�����
	SVAC_AUTH_Result_LOST = 4, //
} SVAC_AUTH_RESULT;

typedef enum
{
	SVAC_STRM_NO_ENC = 0,   // ����û�м���
	SVAC_STRM_USER_ENC = 1, // ����ʹ���û����õ��������
	SVAC_STRM_UKEY_ENC = 2,  // ����ʹ��vkek��vek����
}SVAC_STRM_ENC_FLAG;// �����ļ���״̬

typedef struct
{
	unsigned int encrptFlag;		//�����Ƿ񱻼���		ȡֵ�ο�: SVAC_STRM_ENC_FLAG
	unsigned int authFlag;			//�����Ƿ���ǩ������	0:δǩ������           1:ǩ������
	unsigned int strmEncrptType;	// ���������㷨			ȡֵ�ο�: SVAC_ENCRYPT_TYPE
	unsigned int strmEncrptMode;	// ��������ģʽ			ȡֵ�ο�: SVAC_ENCRYPT_MODE
	unsigned int vekEncrptType;		// vek�����㷨			ȡֵ�ο�: SVAC_ENCRYPT_TYPE
	unsigned int vekEncrptMode;		// vek����ģʽ			ȡֵ�ο�: SVAC_ENCRYPT_MODE
	unsigned int hashType;			// ժҪ�㷨				ȡֵ�ο�: SVAC_HASH_TYPE
	unsigned int signType;			// ǩ���㷨				ȡֵ�ο�: SVAC_SIGN_TYPE
	unsigned int isDecWrong;		// �Ƿ������			0:������ȷ��1:���ܴ���
	unsigned int authResult;		// ��֤���				ȡֵ�ο�: SVAC_AUTH_RESULT
} SVAC_SEC_INFO;



/** X509֤����Ϣ */
typedef struct {
	int CertificateType;			///< ֤���ʽ�����ã�Ŀǰ�̶�Ϊ0
	char* CertificateData;			///< ֤��ԭʼ���ݣ������ƣ�
	int CertificateSize;			///< ֤��CertificateData���ֽ���
	int CrlType;					///< �����б��ʽ��0-�ޣ��������֤������CRL�ַ��㣬�����֤��ķַ����ȡCRL�����򲻼��CRL����1-CrlData����ΪCRL�ļ����ݣ�2-CrlDataΪָ��CRL�ļ���URL����HTTP���ӣ���
	char* CrlData;					///< �����б��ʽ��������CrlType����ȡֵ����
	int CrlSize;					///< CrlData���ֽ���
}SVAC_X509CertInfo;


typedef struct _tagSVAC_SingnatureCompareParams
{
	BOOL bEnable;
	SVAC_X509CertInfo *pX509CertInfo;
	int nCertCnt;
}SVAC_SCP;

typedef enum
{
	SVAC_S_SM1_OFB_PKCS5 = 0x00000001,
	SVAC_S_SM4_OFB_PKCS5 = 0x00000002,
	SVAC_H_SM3 = 0x00000001,
	SVAC_A_SM2 = 0x00000001,
	SVAC_SI_SM3_SM2 = 0x00000001
} SVAC_BD_CAPS;

typedef struct
{
 	SVAC_BD_CAPS ACaps;  //�ǶԳ��㷨����
 	SVAC_BD_CAPS HCaps;  //�Ӵ��㷨����
 	SVAC_BD_CAPS SCaps;  //�Գ��㷨����
 	SVAC_BD_CAPS SICaps; //ǩ���㷨����
} SVAC_CAPS;

typedef struct
{
	unsigned char *data;
	int				len;
} SVAC_ELEM;

typedef struct
{
	SVAC_ELEM svrID;
	SVAC_ELEM r1;
} SVAC_SIGN_INPARAM1;

typedef struct
{
	SVAC_ELEM r2;
	SVAC_ELEM sign1;
} SVAC_SIGN_OUTPARAM1;

typedef struct
{
	// verify sign1
	SVAC_ELEM r1;
	SVAC_ELEM r2;
	SVAC_ELEM svrID;
	SVAC_ELEM sign;		// contain sign1, after thar svac the sign2

						// sign sign2
	SVAC_ELEM devID;
	SVAC_ELEM cVkek; 	// contain input cryptkey enc by sdk pbkey, 
						// if len=0, creat by sdk, return cryptkey enc by sdk pbkey
	SVAC_ELEM keyVer;   // cVkek ��Ӧ�� vkek version
} SVAC_VEFY_INPARAM1;

typedef struct
{
	SVAC_ELEM cryptKey; //SM2���ܺ�����ݣ�������base64����
	SVAC_ELEM sign2;
} SVAC_VEFY_OUTPARAM1;

typedef struct
{
	// verify sign2
	SVAC_ELEM r1;
	SVAC_ELEM r2;
	SVAC_ELEM devID;
	SVAC_ELEM sign2;
	SVAC_ELEM cryptKey;
	SVAC_ELEM keyVer;
	SVAC_ELEM srvID;	// search server pub key
} SVAC_VEFY_INPARAM2;

typedef struct
{
	SVAC_ELEM date;
	SVAC_ELEM method;
	SVAC_ELEM from;
	SVAC_ELEM to;
	SVAC_ELEM callID;
	SVAC_ELEM msg;
	SVAC_ELEM nonce;
} SVAC_SIPPARAM;

typedef struct
{
	SVAC_ELEM vkekVer;	/* ��ʶ vkek  ��, Ψһ , �� vkek һһ��Ӧ  */
	SVAC_ELEM cVkek;    /* ������ʽ�� vkek , ���ڽ��� vek          */
} SVAC_VKEK_PARAM;

typedef struct
{
	SVAC_ELEM certID;
	SVAC_ELEM pbKey;
} SVAC_PUBKEY_PARAM;

typedef struct
{
	SVAC_ELEM bindID;	// ˵����֤����ĸ�ID�󶨣�serID or devID
	SVAC_ELEM certID;
	SVAC_ELEM cert;
} SVAC_CERT_PARAM;

typedef struct {
	DWORD    dwSdkVersion;  //��sdk�汾�����и�16λ�ǰ汾�ţ���16λ��build�š�����:1.0.1.5
	DWORD    dwSVACDecoderVersion; //SVAC�������汾,���и�16λ�ǰ汾�ţ���16λ��build�š�����:3.1.1.20
	DWORD	 dwSVAC1DecoderVersion; //SVAC������SVAC1�汾,���и�16λ�ǰ汾�ţ���16λ��build�š�����:3.1.1.20
	DWORD	 dwSVAC2DecoderVersion; //SVAC������SVAC2�汾,���и�16λ�ǰ汾�ţ���16λ��build�š�����:3.1.1.20
	DWORD    dwRes[6];   //Ԥ���ֶ�
}SDK_VERSION;

ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_Init();
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_GetSupportUKey(const char **p2Name, int nPointerCnt);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_Open(HANDLE& handle, int thread_num, int core_num, int svac_version);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_SetUKey(HANDLE handle, const char *pUKeyName, const char *pKeyPin);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_Close(HANDLE handle);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_PrefetchParam(HANDLE handle, unsigned char *pBuf, int uBufLen, SVAC_PREFETCH_PARAM *pPreParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_Decode(HANDLE handle,
	DEC_INPUT_PARAM* decin,
	DEC_OUTPUT_PARAM* decout,
	EXT_INFO_COMMON* pExt_info);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_DecodeAudio(HANDLE handle, unsigned char *pSrcBuf, int uBufLen, unsigned char *pDestBuf);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_SetPassWord(HANDLE handle, char* password, int pswlength);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_SetTrustCerts(HANDLE handle, SVAC_SCP *pStSCP);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_SetRSAKey(HANDLE handle, unsigned int rsa_e, unsigned int rsa_n);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_SetVKEKBase64(HANDLE handle, char *pVKEKBase64,
	int s32Length, char *pVersion, int s32VersionLength);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_SetCommonExtensionInfo(HANDLE handle, BOOL bCommonExtensionInfo, BOOL bDeduplication);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_GetSdkVersion(SDK_VERSION* pSdkVersion);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_GetSECInfo(HANDLE handle, SVAC_SEC_INFO *pSecInfo);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_SetSecurityLib(HANDLE handle,
	void *hVSECLib, void* hVSEC);

ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_DecryptOpen(HANDLE &handle);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_Decrypt(HANDLE handle, int uStreamType, unsigned char *pStreamIn, int uStreamInLen, \
	unsigned char *pStreamOut, int uStreamOutLen, int uDecryptMode);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_GetCaps(HANDLE handle, SVAC_CAPS *pCaps);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_GetR1(HANDLE handle, unsigned char *pData, unsigned int *pLen);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_GetSign1(HANDLE handle, SVAC_SIGN_INPARAM1 *pInParam, SVAC_SIGN_OUTPARAM1 *pOutParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_VefySign1(HANDLE handle, SVAC_VEFY_INPARAM1 *pInParam, SVAC_VEFY_OUTPARAM1 *pOutParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_VefySign2(HANDLE handle, SVAC_VEFY_INPARAM2 *pInParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_SIPAuth(HANDLE handle, SVAC_SIPPARAM *pSipParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_SIPVefy(HANDLE handle, SVAC_SIPPARAM *pSipParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_VkekTransf(HANDLE handle, SVAC_VKEK_PARAM *pVkekParam, SVAC_PUBKEY_PARAM *pPubkParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_GetCryptKey(HANDLE handle, SVAC_VKEK_PARAM *pVkekParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_BD_Open(HANDLE& handle);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_DEC_SetCert(HANDLE& handle, SVAC_CERT_PARAM *pCertParam);
ZXSVACDECLIB_API int CALLMETHOD ZXSVACDec_CERT_SetEncCert(HANDLE& handle, SVAC_CERT_PARAM *pCertParam);
#endif
