#pragma once

#include <basis/seadTypes.h>

namespace sead {

class Thread;

namespace hostio {

class PortBase
{
public:
    PortBase()
    {
    }

    virtual ~PortBase()
    {
    }

    virtual s32 send(const void* buf, s32 len) = 0;
    virtual s32 recv(void* buf, s32 len) = 0;
    virtual s32 getAvailableSize() = 0;
    virtual void polling() { }
    virtual bool isConnecting() const = 0;
    virtual void lock() { }
    virtual void unlock() { }
    virtual Thread* getThread() { return nullptr; }
};

struct PacketHeader
{
    u32 magic;
    s32 dataSize;
    u32 command;
    u32 extra;
};

} } // namespace sead::hostio
