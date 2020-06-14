#include "AWGBCheckTool.h"

dataPacket::dataPacket(QObject*)
{
    ;
}

dataPacket::~dataPacket()
{
    ;
}

void dataPacket::run()
{
;
    emit isDone();
}


void dataPacket::setdataPacket(int h)
{
    handle = h;
}

