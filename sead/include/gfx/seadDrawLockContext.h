#pragma once

#include <thread/seadCriticalSection.h>

namespace sead {

#if SEAD_GFX_BACKEND == SEAD_GFX_GL
class Thread;
#endif

class DrawLockContext
{
public:
    DrawLockContext();
    virtual ~DrawLockContext();

    void initialize(Heap* heap);

    void lock();
    void unlock();

protected:
    CriticalSection mContextLock;

#if SEAD_GFX_BACKEND == SEAD_GFX_GL
    Thread* mContextHolderThread;
    s32 mContextRefCounter;
    void* mHGLRC; //* GL Context Handle
    void* mHDC;   //* Device Context Handle
#endif
};

} // namespace sead
