#pragma once

#include <thread/seadCriticalSection.h>

namespace sead {

class DrawLockContext
{
public:
    DrawLockContext();
    virtual ~DrawLockContext();

    void initialize(Heap* heap);

    void lock();
    void unlock();

protected:
    CriticalSection mCriticalSection;
};

} // namespace sead
