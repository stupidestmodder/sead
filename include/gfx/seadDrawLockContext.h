#pragma once

#include <hostio/seadHostIONode.h>
#include <thread/seadCriticalSection.h>

namespace sead {

#if defined(SEAD_USE_GL)
class Thread;
#endif

class DrawLockContext : public hostio::Node
{
public:
    DrawLockContext();
    virtual ~DrawLockContext();

    void initialize(Heap* heap);

#if defined(SEAD_TARGET_DEBUG)
    // TODO
    //void genMessage(hostio::Context* context) override;
#endif // SEAD_TARGET_DEBUG

    void lock();
    void unlock();

protected:
    CriticalSection mContextLock;

#if defined(SEAD_USE_GL)
    Thread* mContextHolderThread;
    s32 mContextRefCounter;
#if defined(SEAD_PLATFORM_GLFW)
    void* mWindow; //* GLFW Window Handle
#elif defined(SEAD_PLATFORM_WINDOWS)
    void* mHGLRC; //* GL Context Handle
    void* mHDC;   //* Device Context Handle
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM
#endif
};

} // namespace sead
