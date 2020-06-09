#pragma once

typedef unsigned char uc;

typedef struct pGBStart_S
{
    int modeType;   //测试目标类型：0,IPC;1,NVR;2,解码器;3,上级平台;4,下级平台
    int protoType;  //协议类型: 0,35114; 1,28281
    int authMethod; //认证方式: 0,双向; 1,单向
    int charSet;    //字符集: 0,utf-8; 1,gb2312
    uc localId[32];    //本地ID
    int localIdLen;
    uc validTime[16]; //有效期
    int validTimeLen;
    uc localIp[16];   //本地IP
    int localIpLen;
    uc beatTime[16];  //心跳间隔
    int beatTimeLen;
    uc localPort[16]; //本地端口
    int localPortLen;
    uc beatCnt[16];   //心跳次数
    int beatCntLen;
    uc localArea[16]; //本地域
    int localAreaLen;
    uc mediaPort[16]; //媒体端口
    int mediaPortLen;
    uc path[1024];     //设备证书路径
    int pathLen;
    uc passwd[32];    //密码
    int passwdLen;

}GBStart_s, *pGBStart_s;
