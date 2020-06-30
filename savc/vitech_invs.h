#ifndef _VITECH_INVS
#define _VITECH_INVS

/*
* 视频结构化头文版本 V1.0.3
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
//规则部分
typedef struct point
{
	VITECH_U16 x;
	VITECH_U16 y;
} INVS_POINT;

typedef struct zone
{
	VITECH_U16			points_num;			 
	INVS_POINT			points[INVS_MAX_NUM];	//点坐标
} ZONE;

typedef struct rule
{
	VITECH_U16		time_out_frame;				//超时帧数
	VITECH_U16		vehicle_rect_expire_frame;	//机动车框超时帧数
	VITECH_U16		people_rect_expire_frame;	// 行人超时帧数
	VITECH_U16		min_pupil_dis;				//最小瞳距
	VITECH_U16		zone_num;
	ZONE			rec_zone[INVS_MAX_NUM];
} RULES;

//目标检测部分
typedef struct rect
{
	VITECH_U16		width;			// 宽
	VITECH_U16		height;			// 高
	INVS_POINT		point;
} REC,*P_REC;

//这个是必有的，所以没有使能标志位
typedef struct faceinfo
{
	VITECH_U16		id;				// 人脸id
	VITECH_U16		color;
	REC				rec;
} FACE_INFO,*P_FACE_INFO;

//标准中定义的一些运动目标基本结构是相同的，只不过一开始先定义人脸了
typedef FACE_INFO TARGET_MOVING_STATUS;		//运动目标位置

//后期考虑同所有坐标都合并(包括人脸坐标)
typedef struct coor_info
{
	VITECH_U16		enable;		// 0. 无数据 1. 有数据
	VITECH_U16		id;				// 识别id
	VITECH_U16		color;
	REC				rec;			//坐标
} COORDINATE_INFO,*P_COORDINATE_INFO;

typedef struct moving_target
{
	VITECH_U16				target_num;
	TARGET_MOVING_STATUS	finfos[INVS_MAX_NUM];

} MTAR,*P_MTAR;

//运动目标检测扩展部分
typedef struct tlvdata
{
	VITECH_U16		type;				//类型
	VITECH_U32		length;				//长度>64byte，要求使用申请空间
	char			value[64];			//数据
	char*			huge_value;			//超大数据部分
} TLV,*P_TLV;

//
typedef struct tlvs_data
{
	VITECH_U16		id;					//保留， 人脸对比信息和小图需要,可为空
	VITECH_U16		type;				//0无效,目前车牌type为0x1
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
	VITECH_U16	time_format;  // 0 为 INVS_TIME_VAL, 1为 tim_abs
	union 
	{
		INVS_TIME_VAL 	tv;	
		struct 
		{
			char	year;	//0-127 //2000 开始偏移
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
*	后期就不需要这么多tlvs了，2-3个即可包含所有的
*	tlv内容
*/
typedef struct vehicle_property
{
	VITECH_U16		reseverd;				// 保留
	INVS_ABS_TIME	abs_time;
	TLVS				vehicle_properies_tlvs;		// 车辆属性信息
} VEHICLE_PROPERTY,*P_VEHICLE_PROPERTY;

typedef struct people_property
{
	VITECH_U16		id; 				// 属性公共id,其tlvs中id失效
	MTAR				people_info;			//行人位置框，根据MTAR里的tar_num判断
	TLVS				people_properties_tlvs;
} PEOPLE_PROPERTY,*P_PEOPLE_PROPERTY;


#ifdef __cplusplus
}
#endif

#endif
