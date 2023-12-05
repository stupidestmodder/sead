#pragma once

#include <basis/seadTypes.h>

namespace sead {

// TODO
class StreamSrc
{
public:
    virtual u32 read(void*, u32);
    virtual u32 write(const void*, u32);
    virtual u32 skip(s32);
    virtual void rewind();
    virtual bool isEOF();
    virtual bool flush();
};

} // namespace sead
