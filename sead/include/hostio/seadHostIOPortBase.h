#pragma once

#include <basis/seadTypes.h>

namespace sead { namespace hostio {

// TODO
class PortBase
{
public:
    PortBase();
    virtual ~PortBase();

    virtual void send() = 0;
    virtual void recv() = 0;
    virtual void getAvailableSize() = 0;
    virtual void polling();
    virtual void isConnecting() = 0;
    virtual void lock();
    virtual void unlock();
    virtual void getThread();
};

struct PacketHeader
{
    u32 magic;
    s32 dataSize;
    u32 command;
    u32 extra;
};

} } // namespace sead::hostio
