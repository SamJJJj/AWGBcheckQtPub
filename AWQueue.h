#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

/**
 *  函数描述：
 *      队列句柄初始化。
 *  参数描述：
 *      [OUT]*QHandle: 队列句柄。
 *      [I N]*ConnectHost: 链接地址。为NULL时默认为127.0.0.1。
 *      [I N]ConnectPort: 链接端口。为0时使用默认端口。
 *      [I N]*WriteTube: 写入时队列名。
 *      [I N]*ReadTube: 读取时队列名。
 *  返回值：
 *      成功返回0，失败返回错误码。
 *  作者: Void_Sora
 *  最后修改日期: 2019/11/05  
 */
int AW_BSQueue_Init(int *QHandle, unsigned char *ConnectHost, unsigned int ConnectPort, unsigned char *WriteTube, unsigned char *ReadTube);

/**
 *  函数描述：
 *      队列句柄销毁。
 *  参数描述：
 *      [I N]QHandle: 待销毁句柄。
 *  返回值：
 *      无
 *  作者: Void_Sora
 *  最后修改日期: 2019/11/05      
 */
void AW_BSQueue_Destroy(int QHandle);

/**
 *  函数描述：
 *      向队列中写入数据。
 *  参数描述：
 *      [I N]QHandle: 队列句柄。
 *      [I N]*Buf: 待写入数据。
 *      [I N]*Buf_len: 待写入数据长度。
 *  返回值：
 *      成功返回0，失败返回错误码。
 *  作者: Void_Sora
 *  最后修改日期: 2019/11/05
 */
int AW_BSQueue_PutBuffer(int QHandle, unsigned char *Buf, unsigned int Buf_len);

/**
 *  函数描述：
 *      从队列中提取数据。
 *  参数描述：
 *      [I N]QHandle: 队列句柄。
 *      [OUT]*Buf: 取出数据缓存区。
 *      [I N | OUT]*Buf_len: 输入时为Buf数据缓存区长度，输出时为取出数据长度。
 *  返回值：
 *      成功返回0，失败返回错误码。
 *  作者: Void_Sora
 *  最后修改日期: 2019/11/05
 */
int AW_BSQueue_GetBuffer(int QHandle, unsigned char *Buf, unsigned int *Buf_len);

#ifdef __cplusplus
    }
#endif