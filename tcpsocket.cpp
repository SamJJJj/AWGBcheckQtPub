#include "tcpsocket.h"
#include <iostream>

#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtppacket.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"
#include "jrtplib3/rtpsourcedata.h"
#include "jrtplib3/rtpmemoryobject.h"
#include "jrtplib3/rtptcptransmitter.h"
#include "jrtplib3/rtptcpaddress.h"


using namespace jrtplib;
using namespace std;
//#define RTP_SUPPORT_THREAD


inline void checkerror(int rtperr)
{
    if (rtperr < 0)
    {
        cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        exit(-1);
    }
}

TcpListener::TcpListener()
{
    mIsStop = true;
    mIsThreadRunning = false;
    activeIp = new char[15];
    sClient = NULL;
}

TcpListener::~TcpListener()
{
    delete activeIp;
}

void TcpListener::start()
{
    mIsStop = false;
    //�����µ��߳�ʵ�ֶ�ȡ��Ƶ�ļ�
    std::thread([&](TcpListener *pointer)
    {
        pointer->run();

    }, this).detach();
}

void TcpListener::stop(bool isWait) {
    mIsStop = true;

    if (isWait)
    {
        while (mIsThreadRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void TcpListener::tcpListenerStart() {

    const int packSize = 1500;
    RTPSessionParams sessParams;
    RTPTCPTransmitter trans2 = RTPTCPTransmitter(0);
    RTPSession sess2;
    sessParams.SetProbationType(RTPSources::NoProbation);
//    sessParams.SetOwnTimestampUnit(1.0 / packSize);
    sessParams.SetOwnTimestampUnit(1.0/50.0);
    sessParams.SetMaximumPacketSize(packSize + 64);
    sessParams.SetAcceptOwnPackets(true);
    cout << "link_start" << endl;

    bool threadsafe = false;
#ifdef RTP_SUPPORT_THREAD
    threadsafe = true;
#endif // RTP_SUPPORT_THREAD
    threadsafe = false;
    cout<< 1 <<endl;
    checkerror(trans2.Init(threadsafe));
    cout<< 2 <<endl;
    checkerror(trans2.Create(65535, 0));
    cout<< 3 <<endl;
    checkerror(sess2.Create(sessParams, &trans2));
    qDebug("Session 2 created ");
    checkerror(sess2.AddDestination(RTPTCPAddress(sClient)));
    cout<< 5 <<endl;
//    std::vector<uint8_t> pack(packSize);
    //这是一个循环
    while (1)
    //这是第一个循环的左大括号
    {
        //mIsStop = true;
        sess2.BeginDataAccess();
        if (sess2.GotoFirstSourceWithData())
        {
            do
            {
                RTPPacket *pack;

                while ((pack = sess2.GetNextPacket()) != NULL)
                {
                    RTPSourceData *mRTPSourceData = sess2.GetCurrentSourceInfo();
                    uint32_t ssrc = mRTPSourceData->GetSSRC();
                    int cameraId = ssrc - 100000000;
                    cout << 123 <<endl;
//					channel->inputRtpBuffer(pack->GetPayloadData(), pack->GetPayloadLength(), pack->GetSequenceNumber(), pack->HasMarker());



                    // we don't longer need the packet, so
                    // we'll delete it
                    sess2.DeletePacket(pack);
                }
            } while (sess2.GotoNextSourceWithData());
        }
        sess2.EndDataAccess();
#ifndef RTP_SUPPORT_THREAD
        //checkerror(sess1.Poll());
        checkerror(sess2.Poll());
#endif // RTP_SUPPORT_THREAD
        //sess1.BYEDestroy(RTPTime(10,0),0,0);
        //sess2.BYEDestroy(RTPTime(10, 0), 0, 0);
        if (sign == 0) {
            sess2.BYEDestroy(RTPTime(10, 0), 0, 0);
            break;
        }
    }
}

void TcpListener::run() {
    mIsThreadRunning = true;
    sign = 1;

    //��ʼ��WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return;
    }

    //�����׽���
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
//        qDebug("socket error !");
        return;
    }

    //��IP�Ͷ˿�
    sockaddr_in sin;
    sin.sin_family = AF_INET;
//    connectType = 1;
    if (connectType == 2) {
        sin.sin_port = htons(10010);
        sin.sin_addr.S_un.S_addr = INADDR_ANY;
        if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
        {
            qDebug("Cannot bind sockaddr");
        }

        //��ʼ����
        if (listen(slisten, 5) == SOCKET_ERROR)
        {
            qDebug("listen error !");
            return;
        }
//        SocketType sClient;
        sockaddr_in remoteAddr;
        int nAddrlen = sizeof(remoteAddr);
        while (sign) {
            cout << "pro" << endl;
            sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
            cout << "link_start" << endl;
            if (sClient == INVALID_SOCKET) {
//                qDebug("Can't accept incoming connection!");
                return;
            }
//            tcpListenerStart();





            const int packSize = 1500;
            RTPSessionParams sessParams;
            RTPTCPTransmitter trans2 = RTPTCPTransmitter(0);
            RTPSession sess2;
            sessParams.SetProbationType(RTPSources::NoProbation);
            sessParams.SetOwnTimestampUnit(1.0 / packSize);
            sessParams.SetMaximumPacketSize(packSize + 64);


            bool threadsafe = false;
        #ifdef RTP_SUPPORT_THREAD
            threadsafe = true;
        #endif // RTP_SUPPORT_THREAD

            checkerror(trans2.Init(threadsafe));
            checkerror(trans2.Create(65535, 0));
            checkerror(sess2.Create(sessParams, &trans2));
            qDebug("Session 2 created ");
            checkerror(sess2.AddDestination(RTPTCPAddress(sClient)));
            cout<< 5 <<endl;
        //    std::vector<uint8_t> pack(packSize);
            cout << "one sleep" <<endl;
            int w = 0;
            while (1)
            {
                Sleep(1000);
                sess2.Poll();
//                w++;
//                cout << w <<endl;
                //mIsStop = true;
                sess2.BeginDataAccess();
                int e = sess2.GotoFirstSourceWithData();
                cout << e <<endl;
                if (sess2.GotoFirstSourceWithData())
                {
                    do
                    {
                        Sleep(1000);
                        cout << 3 <<endl;
                        RTPPacket *pack;

                        while ((pack = sess2.GetNextPacket()) != NULL)
                        {
                            Sleep(1000);
                            cout << 4 <<endl;
                            RTPSourceData *mRTPSourceData = sess2.GetCurrentSourceInfo();
                            uint32_t ssrc = mRTPSourceData->GetSSRC();
                            int cameraId = ssrc - 100000000;
                            cout << cameraId <<endl;
                            cout << 123 << endl;
//                            VideoChannel* channel = NULL;


//                            if (channel != nullptr)
//                            {
//                                channel->inputRtpBuffer(pack->GetPayloadData(), pack->GetPayloadLength(), pack->GetSequenceNumber(), pack->HasMarker());
//                            }


                            // we don't longer need the packet, so
                            // we'll delete it
                            sess2.DeletePacket(pack);
                            cout << "two sleep" <<endl;
                        }
                    } while (sess2.GotoNextSourceWithData());
                }
                sess2.EndDataAccess();
        #ifndef RTP_SUPPORT_THREAD
                //checkerror(sess1.Poll());
//                checkerror(sess2.Poll());
        #endif // RTP_SUPPORT_THREAD
                //sess1.BYEDestroy(RTPTime(10,0),0,0);
                //sess2.BYEDestroy(RTPTime(10, 0), 0, 0);
                if (sign == 0) {
                    sess2.BYEDestroy(RTPTime(10, 0), 0, 0);
                    break;
                }
            }






            if (sign == 0) {
                closesocket(slisten);
                closesocket(sClient);
            }
        }
    }
    else if (connectType == 1) {
        sin.sin_port = htons(activePort);
        sin.sin_addr.S_un.S_addr = inet_addr(activeIp);
        while (sign) {
            if (connect(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
                qDebug("Cannot connect Ipc");
            }
//            qDebug(u8"�ɹ����ӵ�Ipc��%s:%d \r\n",activeIp,activePort);
//            tcpListenerStart();




            const int packSize = 1500;
            RTPSessionParams sessParams;
            RTPTCPTransmitter trans2 = RTPTCPTransmitter(0);
            RTPSession sess2;
            sessParams.SetProbationType(RTPSources::NoProbation);
            sessParams.SetOwnTimestampUnit(1.0 / packSize);
            sessParams.SetMaximumPacketSize(packSize + 64);


            bool threadsafe = false;
        #ifdef RTP_SUPPORT_THREAD
            threadsafe = true;
        #endif // RTP_SUPPORT_THREAD

            checkerror(trans2.Init(threadsafe));
            checkerror(trans2.Create(65535, 0));
            checkerror(sess2.Create(sessParams, &trans2));
            qDebug("Session 2 created ");
            checkerror(sess2.AddDestination(RTPTCPAddress(slisten)));
            cout<< 5 <<endl;
        //    std::vector<uint8_t> pack(packSize);
            Sleep(1000);
            cout << "one sleep" <<endl;
            //这是一个循环
            while (1)
            //这是第一个循环的左大括号
            {
                Sleep(1000);
                sess2.Poll();
                cout << 2 <<endl;
                //mIsStop = true;
                sess2.BeginDataAccess();
                if (sess2.GotoFirstSourceWithData())
                {
                    do
                    {
                        Sleep(1000);
                        cout << 3 <<endl;
                        RTPPacket *pack;

                        while ((pack = sess2.GetNextPacket()) != NULL)
                        {
                            Sleep(1000);
                            cout << 4 <<endl;
                            RTPSourceData *mRTPSourceData = sess2.GetCurrentSourceInfo();
                            cout << 5 <<endl;
                            uint32_t ssrc = mRTPSourceData->GetSSRC();
                            cout << 6 <<endl;
                            int cameraId = ssrc - 100000000;
                            cout << 7 <<endl;
                            cout << cameraId <<endl;
                            cout << 123 << endl;
//        					channel->inputRtpBuffer(pack->GetPayloadData(), pack->GetPayloadLength(), pack->GetSequenceNumber(), pack->HasMarker());



                            // we don't longer need the packet, so
                            // we'll delete it
                            sess2.DeletePacket(pack);
                            cout << "two sleep" <<endl;
                        }
                    } while (sess2.GotoNextSourceWithData());
                }
                sess2.EndDataAccess();
        #ifndef RTP_SUPPORT_THREAD
                //checkerror(sess1.Poll());
//                checkerror(sess2.Poll());
        #endif // RTP_SUPPORT_THREAD
                //sess1.BYEDestroy(RTPTime(10,0),0,0);
                //sess2.BYEDestroy(RTPTime(10, 0), 0, 0);
                if (sign == 0) {
                    sess2.BYEDestroy(RTPTime(10, 0), 0, 0);
                    break;
                }
            }







            if (sign == 0) {
                closesocket(slisten);
            }
        }
    }}
