#include <gfx/seadDrawLockContext.h>

#if defined(SEAD_USE_GL)
#include <gfx/gl/seadGraphicsGL.h>
#include <thread/seadThread.h>

#if defined(SEAD_PLATFORM_GLFW)
#include <basis/glfw/seadGlfw.h>
#endif // SEAD_PLATFORM

#endif

namespace sead {

DrawLockContext::DrawLockContext()
    : mContextLock()
#if defined(SEAD_USE_GL)
    , mContextHolderThread(nullptr)
    , mContextRefCounter(0)
#if defined(SEAD_PLATFORM_GLFW)
    , mWindow(nullptr)
#elif defined(SEAD_PLATFORM_WINDOWS)
    , mHGLRC(nullptr)
    , mHDC(nullptr)
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM
#endif
{
}

DrawLockContext::~DrawLockContext()
{
}

void DrawLockContext::initialize(Heap*)
{
#if defined(SEAD_USE_GL)
#if defined(SEAD_PLATFORM_GLFW)
    mWindow = GraphicsGL::instance()->getWindow();
#elif defined(SEAD_PLATFORM_WINDOWS)
    mHGLRC = GraphicsGL::instance()->getHGLRC();
    mHDC = GraphicsGL::instance()->getHDC();
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM
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

#if defined(SEAD_PLATFORM_GLFW)
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(mWindow));
#elif defined(SEAD_PLATFORM_WINDOWS)
    bool b = wglMakeCurrent(static_cast<HDC>(mHDC), static_cast<HGLRC>(mHGLRC));
    SEAD_ASSERT(b);
#endif

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

#if defined(SEAD_PLATFORM_GLFW)
        glfwMakeContextCurrent(nullptr);
#elif defined(SEAD_PLATFORM_WINDOWS)
        bool b = wglMakeCurrent(nullptr, nullptr);
        SEAD_ASSERT(b);
#endif

    }
#endif

    mContextLock.unlock();
}

} // namespace sead
