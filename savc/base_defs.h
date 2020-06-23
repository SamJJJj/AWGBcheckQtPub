
#ifndef BASE_DEFS_H_FOR_STRUCTS
#define BASE_DEFS_H_FOR_STRUCTS
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef linux
#define LINUX
#endif

#include "savc/vitech_invs.h"

#if 0

#define TIME_INFO_LEN		19
#define MAX_NUM				18
#else
#define MAX_NUM				INVS_TLVS_MAX_NUM
#endif

#define SVAC_EXT_TYPE_DIVA_INFO					9      	///Diva���ܷ��������չ��Ϣ
#define SVAC_EXT_TYPE_IDCARD					129    	///���֤��չ��Ϣ  
#define SVAC_EXT_TYPE_OSD						130  	///osd��չ��Ϣ
#define SVAC_EXT_TYPE_CAR 						131  	///������Ϣ
#define SVAC_EXT_TYPE_MAC   					135  	///MAC��չ��Ϣ
#define SVAC_EXT_TYPE_MAC_EX 					194		///MAC��չ��Ϣ	�������鴦������
#define SVAC_EXT_TYPE_IoT 						201
#define	SVAC_EXT_TYPE_ANALYSIS_EXTENSION		17
#define	SVAC_EXT_TYPE_ANALYSIS_EXTENSION_EX		225		///���ȴ���64K��������Ϣ,

#define	SVAC_EXT_TYPE_ANALYSIS_EXTENSION_PEOPLE		(SVAC_EXT_TYPE_ANALYSIS_EXTENSION_EX * 256 + 3)		///���ȴ���64K��������Ϣ,
#define	SVAC_EXT_TYPE_ANALYSIS_EXTENSION_VEHICLE	(SVAC_EXT_TYPE_ANALYSIS_EXTENSION_EX * 256 + 4)		///���ȴ���64K��������Ϣ,
	/**���֤����**/
typedef struct 
{ 
	unsigned char id_number[18];   ///<���֤��
	unsigned char res1[1];			///<�ñ����ֽ���ΪIE�ؼ���ʾ��,ֵΪ0x00.
	unsigned char name[30];			///<����
	unsigned char sex[2];			///<�Ա�
	unsigned char nation[24];		///<����
	unsigned char bD[16];			///<��������
	unsigned char address[70];		///<��ͥסַ
	unsigned char time[19];			///<����ʱ��,ռ��19�ֽڣ�����"2014-07-24 15:07:30"
}SVAC_EXT_INFO_ID_CARD;

/**����ʶ���������**/
typedef struct 
{	
	char   	channelID;		 //������
	char  	plate[11];		 //����
	char   	plateColor;	 //������ɫ		
	char   	plateType;		 //��������
	char  	carColor;		 //������ɫ
	char	alarmType;		 //��������
	int     redBeginTime;	 //��ƿ�ʼʱ�䣬��λ��
	int     redEndTime;	 //��ƽ���ʱ�䣬��λ��
	char  	captureTime[8]	;//�����¼��ľ���ʱ�� ��-��-��-����-ʱ-��-��-����
}SVAC_EXT_INFO_CAR;

typedef struct
{
	unsigned char subtype;		//ͬһsubtype��OSD��չ��Ϣ��������Ҫ����ǰһ��ͬsubtype��OSD��չ��Ϣ
	unsigned char codeType;		//�ַ������ʽ����ǰ�� 0 ��ʹ�ã���ʾʹ�� UTF-8 ���� 
	unsigned char alignType;	//�ַ������ʽ��0 Ϊ����룬1 Ϊ�Ҷ��� 
	unsigned char charSize;		//�ַ������С�������ر�ʾ 
	unsigned char charType;		//�ַ���ʽ��0 Ϊ�׵׺ڱߣ�1 Ϊ�ڵװױߣ�2 Ϊ��ɫ���壬3 Ϊ��ɫ���� ��4Ϊ����Ӧ�����ݱ���ɫ�Զ��任������ɫ��
	unsigned char top_high;		//�ַ�λ�ø�8λ�������ر�ʾ
	unsigned char top_low;		//�ַ�λ�õ�8λ�������ر�ʾ
	unsigned char left_high;	//�ַ�λ�ø�8λ�������ر�ʾ 	
	unsigned char left_low;		//�ַ�λ�õ�8λ�������ر�ʾ 	
	unsigned char Len;			//�ַ��� osd_data ��ռ�õ��ֽ��� 
	unsigned char res[3];		
	unsigned char OSD_DATA[243];//OSD �ַ����ݣ��䳤���� len ȷ����֧�ֻ��У�����'\n'Ϊ���У�'\0'Ϊ��������ͬʱҲ�ܵ��ַ����ȵ�Լ������osd_data �������Ϊ 243 �ֽڡ�	
}SVAC_EXT_INFO_OSD;


typedef struct
{
	unsigned short Ts_time_year;						//��
	unsigned char  Ts_time_month;						//��
	unsigned char  Ts_time_day; 						//��
	unsigned char  Ts_time_hour;						//ʱ	
	unsigned char  Ts_time_minute;						//��	
	unsigned char  Ts_time_second;						//��
	unsigned short Ts_time_sec_fractional;        	/*����ķ�����Ϣ����1/16384��Ϊ��λ��ȡֵ��ΧΪ0��16383֮�䣨����0��16383��*/
}ABS_TIME;


typedef struct
{
	int type;
	union{
		SVAC_EXT_INFO_ID_CARD IdCard;
		SVAC_EXT_INFO_CAR Car_info;
		SVAC_EXT_INFO_OSD OSD_info;
		VEHICLE_PROPERTY Vechicle_Property;
		PEOPLE_PROPERTY People_Property;
		char Data[64 * 1024];
	}info;
}EXT_INFO;

#ifdef __cplusplus
}
#endif
#endif
