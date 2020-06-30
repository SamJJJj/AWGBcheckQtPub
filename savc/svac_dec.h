#ifndef __SVAC_DEC_H__
#define __SVAC_DEC_H__

#ifdef __cplusplus
extern "C" {
#endif

//for _WINDOWS
#ifdef _MSC_VER
#define EXT_API __declspec(dllexport)
#else
#define EXT_API extern
#endif


struct auth_info
{
	/* �Ƿ�������֤ */
	int do_auth;
	/* if do_ath == 1,  auth_result == 1: pass; auth_result == 0 : failed
	 * if do_ath == 0,  auth_result������
	 */
	int auth_result;
};

struct sec_info
{
	int encrypt_flag; 	//�Ƿ���� 1 ���� 0 û��
	int	auth_flag;		//�Ƿ�ǩ�� 1 ǩ�� 0 û��
	int	hash_only_IDR;	//�Ƿ�ֻ��IDR֡��hashǩ��
	int hash_period; 	//hash_only_IDRΪfalseʱһ��ǩ����֡��
};

typedef int (*PFSECURITYPARAMCB)(struct sec_info *param, void *context);

EXT_API void *SvacDecCreate(void);

/* ����16�ֽ�vkek���ĺ�vkek�汾�� */
EXT_API int SvacDecSetVkek(void *handle, char *vkek, char *version, int verion_len);

/* ������ǩ��Կ 64�ֽ� */
EXT_API int SvacDecSetPubkey(void *handle, char *pubkey);

/* ���ü��ܲ����ص����� */
EXT_API int SvacDecParamCallBack(void *handle, PFSECURITYPARAMCB callback, void *context);

/* ���н���
 * in_buff ��������ܵ�һ֡
 * in_len  ���������֡�ĳ���
 * out_buff ������ܺ��֡ �ڴ�ʹ���߷��䣬���鲻С��in_len + 8KB
 * outlen  ������ܺ�֡�ĳ���
 * info    �����ǩ���
 */
EXT_API int SvacDecProcess(void *handle, char *in_buff, int in_len, char *out_buff, int *outlen, struct auth_info *info);
EXT_API int SvacDecRelease(void *handle);

#ifdef __cplusplus
}
#endif

#endif
