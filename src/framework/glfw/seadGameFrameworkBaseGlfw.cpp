#include <framework/glfw/seadGameFrameworkBaseGlfw.h>

#include <controller/seadControllerMgr.h>
// #include <controller/win/seadKeyboardMouseDeviceWin.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <heap/seadExpHeap.h>

namespace sead {

GameFrameworkBaseGlfw* GameFrameworkBaseGlfw::sInstance = nullptr;

GameFrameworkBaseGlfw::GameFrameworkBaseGlfw(const CreateArg& arg)
    : GameFramework()
    , mArg(arg)
    , mWindow(nullptr)
    , mLastUpdateTime()
    , mFrameTime()
    , mLastDiffTime()
    , mExit(false)
    // , mMsgProcCallback(nullptr)
    , mDefaultFrameBuffer(nullptr)
    , mDefaultLogicalFrameBuffer()
    , mMouseWheel(0)
    , mGlfwHeap(nullptr)
{
    SEAD_ASSERT(!sInstance);
    sInstance = this;

    createWindow_();
}

GameFrameworkBaseGlfw::~GameFrameworkBaseGlfw()
{
    CurrentHeapSetter chs(mGlfwHeap);
    
    glfwDestroyWindow(mWindow);
    mWindow = nullptr;

    glfwTerminate();

    sInstance = nullptr;
}

FrameBuffer* GameFrameworkBaseGlfw::getMethodFrameBuffer(s32 methodType) const
{
    switch (methodType)
    {
        case 2:
        case 3:
        case 4:
            return mDefaultFrameBuffer;

        default:
            SEAD_ASSERT_MSG(false, "Undefined MethodType(%d).", methodType);
            return nullptr;
    }
}

const LogicalFrameBuffer* GameFrameworkBaseGlfw::getMethodLogicalFrameBuffer(s32 methodType) const
{
    switch (methodType)
    {
        case 2:
        case 3:
        case 4:
            return &mDefaultLogicalFrameBuffer;

        default:
            SEAD_ASSERT_MSG(false, "Undefined MethodType(%d).", methodType);
            return nullptr;
    }
}

bool GameFrameworkBaseGlfw::setProcessPriority(ProcessPriority priority)
{
    switch (priority)
    {
        case ProcessPriority::eIdle:
            // SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eNormal:
            // SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eHigh:
            // SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eRealTime:
            // SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
            return true;

        default:
            SEAD_ASSERT_MSG(false, "Undefined ProcessPriority.\n");
            return false;
    }
}

void GameFrameworkBaseGlfw::createControllerMgr(TaskBase* rootTask)
{
    GameFramework::createControllerMgr(rootTask);

    // KeyboardMouseDevice* device = DynamicCast<KeyboardMouseDevice>(ControllerMgr::instance()->getControlDevice(ControllerDefine::DeviceId::eKeyboardMouse));
    // if (device)
    //     device->setMainWindowHandle(mHWnd);
}

void GameFrameworkBaseGlfw::setCaption(const SafeString& caption)
{
    CurrentHeapSetter chs(mGlfwHeap);

    glfwSetWindowTitle(mWindow, caption.cstr());
}

bool GameFrameworkBaseGlfw::setWaitVBlankInterval(u32 interval)
{
    if (!Graphics::instance()->setVBlankWaitInterval(interval))
        return false;

    mArg.wait_vblank = interval;
    return true;
}

void GameFrameworkBaseGlfw::runImpl_()
{
    waitStartDisplayLoop_();

    mainLoop_();
}

MethodTreeMgr* GameFrameworkBaseGlfw::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) SingleScreenMethodTreeMgr();
}

void GameFrameworkBaseGlfw::createWindow_()
{
    mGlfwHeap = ExpHeap::create(5 * 1024 * 1024, "GlfwHeap", HeapMgr::instance()->findContainHeap(this), Heap::HeapDirection::eForward, true);

    //* Window creation
    {
        CurrentHeapSetter chs(mGlfwHeap);

        glfwSetErrorCallback([](s32 error, const char* description) {
            SEAD_ASSERT_MSG(false, "GLFW Error(%d): %s\n", error, description);
        });

        if (!glfwInit())
        {
            SEAD_ASSERT_MSG(false, "glfwInit failed");
            return;
        }

        // TODO: Should come from GameFrameworkGlfwGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(SEAD_PLATFORM_MACOSX)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        mWindow = glfwCreateWindow(mArg.width, mArg.height, mArg.window_name.cstr(), nullptr, nullptr);
        if (!mWindow)
        {
            SEAD_ASSERT_MSG(false, "glfwCreateWindow failed");
            return;
        }

        glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* window, s32 width, s32 height)
        {
            SEAD_ASSERT(sInstance);
            if (width == 0 || height == 0)
            {
                return;
            }

            if (sInstance->mDefaultFrameBuffer)
            {
                sInstance->mDefaultFrameBuffer->setVirtualSize(width, height);
                sInstance->mDefaultFrameBuffer->setPhysicalArea(0.0f, 0.0f, width, height);
            }

            sInstance->mDefaultLogicalFrameBuffer.setVirtualSize(width, height);
            sInstance->mDefaultLogicalFrameBuffer.setPhysicalArea(0.0f, 0.0f, width, height);

            sInstance->resize_(width, height);
        });

        glfwSetWindowRefreshCallback(mWindow, [](GLFWwindow* window)
        {
            SEAD_ASSERT(sInstance);
            sInstance->procFrame_();
        });

        glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
        {
            SEAD_ASSERT(sInstance);
            sInstance->requestExit();
        });
    }

    setFps(mArg.fps);
    mLastUpdateTime.setNow();
    mLastUpdateTime -= mFrameTime;
}

void GameFrameworkBaseGlfw::mainLoop_()
{
    CurrentHeapSetter chs(mGlfwHeap); // TODO: Remove

    Graphics::instance()->lockDrawContext();
    {
        procCalc_();
    }
    Graphics::instance()->unlockDrawContext();

    while (!mExit)
    {
        {
            CurrentHeapSetter chs(mGlfwHeap);

            glfwPollEvents();
        }

        {
            TickSpan dt = mLastUpdateTime.diffToNow();
            if (mArg.wait_vblank > 0 || dt > mFrameTime)
            {
                mLastDiffTime = dt;
                mLastUpdateTime.setNow();

                procFrame_();

                mMouseWheel = 0;

                Thread::sleep(TickSpan::makeFromMilliSeconds(0));
            }
            else if (dt + TickSpan::makeFromMilliSeconds(10) > mFrameTime)
            {
                Thread::sleep(TickSpan::makeFromMilliSeconds(0));
            }
            else
            {
                Thread::sleep(TickSpan::makeFromMilliSeconds(1));
            }
        }
    }
}

void GameFrameworkBaseGlfw::procCalc_()
{
    mCalcMeter.measureBegin();
    {
        mTaskMgr->beforeCalc();
        DynamicCast<SingleScreenMethodTreeMgr>(getMethodTreeMgr())->calc();
        mTaskMgr->afterCalc(); // TODO: Should we call this here ?
    }
    mCalcMeter.measureEnd();
}

} // namespace sead
