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
	/* 是否做了认证 */
	int do_auth;
	/* if do_ath == 1,  auth_result == 1: pass; auth_result == 0 : failed
	 * if do_ath == 0,  auth_result无意义
	 */
	int auth_result;
};

struct sec_info
{
	int encrypt_flag; 	//是否加密 1 加密 0 没有
	int	auth_flag;		//是否签名 1 签名 0 没有
	int	hash_only_IDR;	//是否只对IDR帧做hash签名
	int hash_period; 	//hash_only_IDR为false时一次签名的帧数
};

typedef int (*PFSECURITYPARAMCB)(struct sec_info *param, void *context);

EXT_API void *SvacDecCreate(void);

/* 设置16字节vkek明文和vkek版本号 */
EXT_API int SvacDecSetVkek(void *handle, char *vkek, char *version, int verion_len);

/* 设置验签公钥 64字节 */
EXT_API int SvacDecSetPubkey(void *handle, char *pubkey);

/* 设置加密参数回调函数 */
EXT_API int SvacDecParamCallBack(void *handle, PFSECURITYPARAMCB callback, void *context);

/* 进行解密
 * in_buff 输入待解密的一帧
 * in_len  输入待解密帧的长度
 * out_buff 输出解密后的帧 内存使用者分配，建议不小于in_len + 8KB
 * outlen  输出解密后帧的长度
 * info    输出验签结果
 */
EXT_API int SvacDecProcess(void *handle, char *in_buff, int in_len, char *out_buff, int *outlen, struct auth_info *info);
EXT_API int SvacDecRelease(void *handle);

#ifdef __cplusplus
}
#endif

#endif
