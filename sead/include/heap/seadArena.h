#pragma once

#include <basis/seadTypes.h>
#include <prim/seadPtrUtil.h>

namespace sead {

class Arena
{
public:
    Arena();
    ~Arena();

    void initialize(size_t size);
    void initialize(u8* start, size_t size);
    void destroy();

    u8* getStartAddr() { return mStart; }
    size_t getSize() const { return mSize; }
    bool isInclude(void* ptr) const { return PtrUtil::isInclude(ptr, mStart, mStart + mSize); }

private:
    u8* mStart;
    size_t mSize;
    bool mInitWithStartAddress;
};

} // namespace sead
