#include <gfx/seadDrawLockContext.h>

#if SEAD_GFX_BACKEND == SEAD_GFX_GL
#include <gfx/gl/seadGraphicsGL.h>
#include <thread/seadThread.h>
#endif

namespace sead {

DrawLockContext::DrawLockContext()
    : mContextLock()
#if SEAD_GFX_BACKEND == SEAD_GFX_GL
    , mContextHolderThread(nullptr)
    , mContextRefCounter(0)
    , mHGLRC(nullptr)
    , mHDC(nullptr)
#endif
{
}

DrawLockContext::~DrawLockContext()
{
}

void DrawLockContext::initialize(Heap*)
{
#if SEAD_GFX_BACKEND == SEAD_GFX_GL
    mHGLRC = GraphicsGL::instance()->getHGLRC();
    mHDC = GraphicsGL::instance()->getHDC();
#endif
}

void DrawLockContext::lock()
{
    mContextLock.lock();

#if SEAD_GFX_BACKEND == SEAD_GFX_GL
    Thread* currThread = ThreadMgr::instance()->getCurrentThread();
    if (mContextHolderThread == currThread && mContextRefCounter > 0)
    {
        mContextRefCounter++;
        return;
    }

    SEAD_ASSERT(mContextHolderThread == nullptr);
    SEAD_ASSERT(mContextRefCounter == 0);

    bool b = wglMakeCurrent(static_cast<HDC>(mHDC), static_cast<HGLRC>(mHGLRC));
    SEAD_ASSERT(b);

    mContextHolderThread = currThread;
    mContextRefCounter = 1;
#endif
}

void DrawLockContext::unlock()
{
#if SEAD_GFX_BACKEND == SEAD_GFX_GL
    Thread* currThread = ThreadMgr::instance()->getCurrentThread();
    SEAD_ASSERT(mContextHolderThread == currThread);
    SEAD_ASSERT(mContextRefCounter > 0);

    mContextRefCounter--;
    if (mContextRefCounter == 0)
    {
        mContextHolderThread = nullptr;

        glFinish();

        bool b = wglMakeCurrent(nullptr, nullptr);
        SEAD_ASSERT(b);
    }
#endif

    mContextLock.unlock();
}

} // namespace sead
