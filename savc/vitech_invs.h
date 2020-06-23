#ifndef _VITECH_INVS
#define _VITECH_INVS

/*
* ��Ƶ�ṹ��ͷ�İ汾 V1.0.3
*/
#ifdef __cplusplus
extern "C"
{
#endif

#define INVS_MIN_NUM	4
#define INVS_MAX_NUM	32
#define INVS_TLVS_MAX_NUM	128

typedef unsigned char			VITECH_U8;
typedef unsigned short			VITECH_U16;
typedef unsigned int			VITECH_U32;
typedef unsigned long long      VITECH_U64;

typedef signed char 			VITECH_S8;
typedef short					VITECH_S16;
typedef int 					VITECH_S32;
typedef long long               VITECH_S64;

typedef float					VITECH_FLOAT;
typedef signed char 			VITECH_CHAR;
typedef void*					VITECH_HANDLE;
typedef int						VITECH_INT;

enum color
{
	black = 0,
	white,
	gray,
	red,
	blue,
	yellow,
	orange,
	brown,
	green,
	purple,
	indigo,
	pink,
	transparent,
	white_green,
	yellow_green,
	golden	=	51,
	slivery,
	other
};
//���򲿷�
typedef struct point
{
	VITECH_U16 x;
	VITECH_U16 y;
} INVS_POINT;

typedef struct zone
{
	VITECH_U16			points_num;			 
	INVS_POINT			points[INVS_MAX_NUM];	//������
} ZONE;

typedef struct rule
{
	VITECH_U16		time_out_frame;				//��ʱ֡��
	VITECH_U16		vehicle_rect_expire_frame;	//��������ʱ֡��
	VITECH_U16		people_rect_expire_frame;	// ���˳�ʱ֡��
	VITECH_U16		min_pupil_dis;				//��Сͫ��
	VITECH_U16		zone_num;
	ZONE			rec_zone[INVS_MAX_NUM];
} RULES;

//Ŀ���ⲿ��
typedef struct rect
{
	VITECH_U16		width;			// ��
	VITECH_U16		height;			// ��
	INVS_POINT		point;
} REC,*P_REC;

//����Ǳ��еģ�����û��ʹ�ܱ�־λ
typedef struct faceinfo
{
	VITECH_U16		id;				// ����id
	VITECH_U16		color;
	REC				rec;
} FACE_INFO,*P_FACE_INFO;

//��׼�ж����һЩ�˶�Ŀ������ṹ����ͬ�ģ�ֻ����һ��ʼ�ȶ���������
typedef FACE_INFO TARGET_MOVING_STATUS;		//�˶�Ŀ��λ��

//���ڿ���ͬ�������궼�ϲ�(������������)
typedef struct coor_info
{
	VITECH_U16		enable;		// 0. ������ 1. ������
	VITECH_U16		id;				// ʶ��id
	VITECH_U16		color;
	REC				rec;			//����
} COORDINATE_INFO,*P_COORDINATE_INFO;

typedef struct moving_target
{
	VITECH_U16				target_num;
	TARGET_MOVING_STATUS	finfos[INVS_MAX_NUM];

} MTAR,*P_MTAR;

//�˶�Ŀ������չ����
typedef struct tlvdata
{
	VITECH_U16		type;				//����
	VITECH_U32		length;				//����>64byte��Ҫ��ʹ������ռ�
	char			value[64];			//����
	char*			huge_value;			//�������ݲ���
} TLV,*P_TLV;

//
typedef struct tlvs_data
{
	VITECH_U16		id;					//������ �����Ա���Ϣ��Сͼ��Ҫ,��Ϊ��
	VITECH_U16		type;				//0��Ч,Ŀǰ����typeΪ0x1
	VITECH_U16		tlv_num;
	TLV				tlv_data[INVS_TLVS_MAX_NUM];

} TLVS,*P_TLVS;

typedef struct moving_target_ext
{
	TLVS	jpg_tlv;
	TLVS	cmp_tlv;
} MTAR_EXT,*P_MTAR_EXT;

typedef struct 
{
	VITECH_U64 tv_sec;
	VITECH_U64 tv_usec;
}INVS_TIME_VAL;

typedef struct absolute_time
{
	VITECH_U16	enable;
	VITECH_U16	time_format;  // 0 Ϊ INVS_TIME_VAL, 1Ϊ tim_abs
	union 
	{
		INVS_TIME_VAL 	tv;	
		struct 
		{
			char	year;	//0-127 //2000 ��ʼƫ��
			char	month;	// 1-12
			char 	day;	// 1-31
			char	hour;	// 0-23
			char	min;	// 0-59
			char 	second;	// 0 -59
			VITECH_S16	sec_fraction; // 0 -16394
		}	tim_abs;
	} abt;
} INVS_ABS_TIME,*PINVS_ABS_TIME;

/*
*	���ھͲ���Ҫ��ô��tlvs�ˣ�2-3�����ɰ������е�
*	tlv����
*/
typedef struct vehicle_property
{
	VITECH_U16		reseverd;				// ����
	INVS_ABS_TIME	abs_time;
	TLVS				vehicle_properies_tlvs;		// ����������Ϣ
} VEHICLE_PROPERTY,*P_VEHICLE_PROPERTY;

typedef struct people_property
{
	VITECH_U16		id; 				// ���Թ���id,��tlvs��idʧЧ
	MTAR				people_info;			//����λ�ÿ򣬸���MTAR���tar_num�ж�
	TLVS				people_properties_tlvs;
} PEOPLE_PROPERTY,*P_PEOPLE_PROPERTY;


#ifdef __cplusplus
}
#endif

#endif
