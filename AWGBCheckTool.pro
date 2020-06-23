#-------------------------------------------------
#
# Project created by QtCreator 2020-06-04T17:27:06
#
#-------------------------------------------------

QT       += core gui\
            xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
LIBS += libws2_32 libAdvapi32 -lpthread libwsock32
TARGET = AWGBCheckTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    AWQueue.c \
    Cond.cpp \
    beanstalk.c \
    getandparse.cpp \
        main.cpp \
        AWGBCheckTool.cpp \
    ConfigureGuide.cpp \
    mutex.cpp \
    rtpreceiver.cpp \
    showvideo.cpp \
    tcpsocket.cpp \
    videochannel.cpp

HEADERS += \
        AWGBCheckTool.h \
    AWQueue.h \
    Cond.h \
    ConfigureGuide.h \
    DataListThread.h \
    beanstalk.h \
    eventhandle.h \
    jrtplib3/rtcpapppacket.h \
    jrtplib3/rtcpbyepacket.h \
    jrtplib3/rtcpcompoundpacket.h \
    jrtplib3/rtcpcompoundpacketbuilder.h \
    jrtplib3/rtcppacket.h \
    jrtplib3/rtcppacketbuilder.h \
    jrtplib3/rtcprrpacket.h \
    jrtplib3/rtcpscheduler.h \
    jrtplib3/rtcpsdesinfo.h \
    jrtplib3/rtcpsdespacket.h \
    jrtplib3/rtcpsrpacket.h \
    jrtplib3/rtcpunknownpacket.h \
    jrtplib3/rtpabortdescriptors.h \
    jrtplib3/rtpaddress.h \
    jrtplib3/rtpbyteaddress.h \
    jrtplib3/rtpcollisionlist.h \
    jrtplib3/rtpconfig.h \
    jrtplib3/rtpdebug.h \
    jrtplib3/rtpdefines.h \
    jrtplib3/rtperrors.h \
    jrtplib3/rtpexternaltransmitter.h \
    jrtplib3/rtphashtable.h \
    jrtplib3/rtpinternalsourcedata.h \
    jrtplib3/rtpipv4address.h \
    jrtplib3/rtpipv4destination.h \
    jrtplib3/rtpipv6address.h \
    jrtplib3/rtpipv6destination.h \
    jrtplib3/rtpkeyhashtable.h \
    jrtplib3/rtplibraryversion.h \
    jrtplib3/rtpmemorymanager.h \
    jrtplib3/rtpmemoryobject.h \
    jrtplib3/rtppacket.h \
    jrtplib3/rtppacketbuilder.h \
    jrtplib3/rtppollthread.h \
    jrtplib3/rtprandom.h \
    jrtplib3/rtprandomrand48.h \
    jrtplib3/rtprandomrands.h \
    jrtplib3/rtprandomurandom.h \
    jrtplib3/rtprawpacket.h \
    jrtplib3/rtpsecuresession.h \
    jrtplib3/rtpselect.h \
    jrtplib3/rtpsession.h \
    jrtplib3/rtpsessionparams.h \
    jrtplib3/rtpsessionsources.h \
    jrtplib3/rtpsocketutil.h \
    jrtplib3/rtpsourcedata.h \
    jrtplib3/rtpsources.h \
    jrtplib3/rtpstructs.h \
    jrtplib3/rtptcpaddress.h \
    jrtplib3/rtptcptransmitter.h \
    jrtplib3/rtptimeutilities.h \
    jrtplib3/rtptransmitter.h \
    jrtplib3/rtptypes.h \
    jrtplib3/rtptypes_win.h \
    jrtplib3/rtpudpv4transmitter.h \
    jrtplib3/rtpudpv6transmitter.h \
    jthread/jmutex.h \
    jthread/jmutexautolock.h \
    jthread/jthread.h \
    jthread/jthreadconfig.h \
    libavcodec/ac3_parser.h \
    libavcodec/adts_parser.h \
    libavcodec/avcodec.h \
    libavcodec/avdct.h \
    libavcodec/avfft.h \
    libavcodec/d3d11va.h \
    libavcodec/dirac.h \
    libavcodec/dv_profile.h \
    libavcodec/dxva2.h \
    libavcodec/jni.h \
    libavcodec/mediacodec.h \
    libavcodec/qsv.h \
    libavcodec/vaapi.h \
    libavcodec/vdpau.h \
    libavcodec/version.h \
    libavcodec/videotoolbox.h \
    libavcodec/vorbis_parser.h \
    libavcodec/xvmc.h \
    libavdevice/avdevice.h \
    libavdevice/version.h \
    libavfilter/avfilter.h \
    libavfilter/buffersink.h \
    libavfilter/buffersrc.h \
    libavfilter/version.h \
    libavformat/avformat.h \
    libavformat/avio.h \
    libavformat/version.h \
    libavutil/adler32.h \
    libavutil/aes.h \
    libavutil/aes_ctr.h \
    libavutil/attributes.h \
    libavutil/audio_fifo.h \
    libavutil/avassert.h \
    libavutil/avconfig.h \
    libavutil/avstring.h \
    libavutil/avutil.h \
    libavutil/base64.h \
    libavutil/blowfish.h \
    libavutil/bprint.h \
    libavutil/bswap.h \
    libavutil/buffer.h \
    libavutil/camellia.h \
    libavutil/cast5.h \
    libavutil/channel_layout.h \
    libavutil/common.h \
    libavutil/cpu.h \
    libavutil/crc.h \
    libavutil/des.h \
    libavutil/dict.h \
    libavutil/display.h \
    libavutil/downmix_info.h \
    libavutil/encryption_info.h \
    libavutil/error.h \
    libavutil/eval.h \
    libavutil/ffversion.h \
    libavutil/fifo.h \
    libavutil/file.h \
    libavutil/frame.h \
    libavutil/hash.h \
    libavutil/hmac.h \
    libavutil/hwcontext.h \
    libavutil/hwcontext_cuda.h \
    libavutil/hwcontext_d3d11va.h \
    libavutil/hwcontext_drm.h \
    libavutil/hwcontext_dxva2.h \
    libavutil/hwcontext_mediacodec.h \
    libavutil/hwcontext_qsv.h \
    libavutil/hwcontext_vaapi.h \
    libavutil/hwcontext_vdpau.h \
    libavutil/hwcontext_videotoolbox.h \
    libavutil/imgutils.h \
    libavutil/intfloat.h \
    libavutil/intreadwrite.h \
    libavutil/lfg.h \
    libavutil/log.h \
    libavutil/lzo.h \
    libavutil/macros.h \
    libavutil/mastering_display_metadata.h \
    libavutil/mathematics.h \
    libavutil/md5.h \
    libavutil/mem.h \
    libavutil/motion_vector.h \
    libavutil/murmur3.h \
    libavutil/opt.h \
    libavutil/parseutils.h \
    libavutil/pixdesc.h \
    libavutil/pixelutils.h \
    libavutil/pixfmt.h \
    libavutil/random_seed.h \
    libavutil/rational.h \
    libavutil/rc4.h \
    libavutil/replaygain.h \
    libavutil/ripemd.h \
    libavutil/samplefmt.h \
    libavutil/sha.h \
    libavutil/sha512.h \
    libavutil/spherical.h \
    libavutil/stereo3d.h \
    libavutil/tea.h \
    libavutil/threadmessage.h \
    libavutil/time.h \
    libavutil/timecode.h \
    libavutil/timestamp.h \
    libavutil/tree.h \
    libavutil/twofish.h \
    libavutil/version.h \
    libavutil/xtea.h \
    libpostproc/postprocess.h \
    libpostproc/version.h \
    libswscale/swscale.h \
    libswscale/version.h \
    main.h \
    mutex.h \
    rtpreceiver.h \
    showvideo.h \
    tcpsocket.h \
    videochannel.h

FORMS += \
        AWGBCheckTool.ui \
        ConfigureGuide.ui \
_ConfigureGuide.ui
        

INCLUDEPATH += D:\Qt\myprojects\AWGBCheckTool

win32: LIBS += -L$$PWD/lib/ -llibjrtp.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/lib/ -llibjthread.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -llibavcodec.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -llibavdevice.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -llibavfilter.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -llibavformat.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:LIBS += -L$$PWD/./ -llibavutil.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -llibpostproc.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -llibswscale.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -llibswresample.dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/svaclib/ -llibsvac_dec

INCLUDEPATH += $$PWD/svaclib
DEPENDPATH += $$PWD/svaclib

win32: LIBS += -L$$PWD/svaclib/ -lrtspApp

INCLUDEPATH += $$PWD/svaclib
DEPENDPATH += $$PWD/svaclib

win32: LIBS += -L$$PWD/svaclib/ -lZxSvacDecLib

INCLUDEPATH += $$PWD/svaclib
DEPENDPATH += $$PWD/svaclib
