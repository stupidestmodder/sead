#include <gfx/seadDrawLockContext.h>

#if defined(SEAD_USE_GL)
#include <gfx/gl/seadGraphicsGL.h>
#include <thread/seadThread.h>
#endif

namespace sead {

DrawLockContext::DrawLockContext()
    : mContextLock()
#if defined(SEAD_USE_GL)
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
#if defined(SEAD_USE_GL)
    mHGLRC = GraphicsGL::instance()->getHGLRC();
    mHDC = GraphicsGL::instance()->getHDC();
#endif
}

void DrawLockContext::lock()
{
    mContextLock.lock();

#if defined(SEAD_USE_GL)
    Thread* currThread = ThreadMgr::instance()->getCurrentThread();
    if (mContextHolderThread == currThread && mContextRefCounter > 0)
    {
        mContextRefCounter++;
        return;
    }

    SEAD_ASSERT(mContextHolderThread == nullptr);
    SEAD_ASSERT(mContextRefCounter == 0);
    #if defined(SEAD_PLATFORM_SDL)
    bool b = SDL_GL_MakeCurrent(static_cast<SDL_Window*>(mHDC), static_cast<SDL_GLContext>(mHGLRC));
    #elif defined(SEAD_PLATFORM_WINDOWS)
    bool b = wglMakeCurrent(static_cast<HDC>(mHDC), static_cast<HGLRC>(mHGLRC));
    #endif
    SEAD_ASSERT(b == 0);

    mContextHolderThread = currThread;
    mContextRefCounter = 1;
#endif
}

void DrawLockContext::unlock()
{
#if defined(SEAD_USE_GL)
    Thread* currThread = ThreadMgr::instance()->getCurrentThread();
    SEAD_ASSERT(mContextHolderThread == currThread);
    SEAD_ASSERT(mContextRefCounter > 0);

    mContextRefCounter--;
    if (mContextRefCounter == 0)
    {
        mContextHolderThread = nullptr;

        glFinish();

        #if defined(SEAD_PLATFORM_SDL)
        bool b = SDL_GL_MakeCurrent(nullptr, nullptr);
        #elif defined(SEAD_PLATFORM_WINDOWS)
        bool b = wglMakeCurrent(nullptr, nullptr);
        #endif
        SEAD_ASSERT(b == 0);
    }
#endif

    mContextLock.unlock();
}

} // namespace sead
