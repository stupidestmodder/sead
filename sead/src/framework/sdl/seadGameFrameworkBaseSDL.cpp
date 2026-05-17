#include <framework/sdl/seadGameFrameworkBaseSDL.h>

#include <controller/seadControllerMgr.h>
#include <controller/sdl/seadKeyboardMouseDeviceSDL.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <heap/seadExpHeap.h>

//#include <timeapi.h>

namespace sead {

GameFrameworkBaseSDL* GameFrameworkBaseSDL::sInstance = nullptr;

void* SDLAlloc(size_t size)
{
    return GameFrameworkBaseSDL::sInstance->getSDLHeap()->tryAlloc(size);
}

void* SDLCalloc(size_t nmemb, size_t size)
{
    const size_t total = nmemb * size;
    void* ptr = GameFrameworkBaseSDL::sInstance->getSDLHeap()->tryAlloc(total);
    sead::MemUtil::fillZero(ptr, total);

    return ptr;
}

void* SDLRealloc(void* ptr, size_t size)
{
    return GameFrameworkBaseSDL::sInstance->getSDLHeap()->realloc(ptr, size);
}

void SDLFree(void* ptr)
{
    if (GameFrameworkBaseSDL::sInstance->getSDLHeap()->isInclude(ptr))
    {
        GameFrameworkBaseSDL::sInstance->getSDLHeap()->free(ptr);
    }
}

GameFrameworkBaseSDL::GameFrameworkBaseSDL(const CreateArg& arg)
    : GameFramework()
    , mArg(arg)
    , mWindow(nullptr)
    , mLastUpdateTime()
    , mFrameTime()
    , mLastDiffTime()
    , mExit(false)
    , mCallback(nullptr)
    , mDefaultFrameBuffer(nullptr)
    , mDefaultLogicalFrameBuffer()
    , mMouseWheel(0)
    , mSDLHeap(nullptr)
{
    SEAD_ASSERT(!sInstance);
    sInstance = this;

    mSDLHeap = sead::ExpHeap::create(50 * 1024 * 1024, "SDLHeap", sead::HeapMgr::instance()->findContainHeap(this), sead::Heap::eForward, true);

    // SDL_SetMemoryFunctions(&SDLAlloc, &SDLCalloc, &SDLRealloc, &SDLFree);

    createWindow_();
}

GameFrameworkBaseSDL::~GameFrameworkBaseSDL()
{
    sInstance = nullptr;
}

FrameBuffer* GameFrameworkBaseSDL::getMethodFrameBuffer(s32 methodType) const
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

const LogicalFrameBuffer* GameFrameworkBaseSDL::getMethodLogicalFrameBuffer(s32 methodType) const
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

bool GameFrameworkBaseSDL::setProcessPriority(ProcessPriority priority)
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

void GameFrameworkBaseSDL::createControllerMgr(TaskBase* rootTask)
{
    GameFramework::createControllerMgr(rootTask);

    KeyboardMouseDevice* device = DynamicCast<KeyboardMouseDevice>(ControllerMgr::instance()->getControlDevice(ControllerDefine::DeviceId::eKeyboardMouse));
    if (device)
        device->setMainWindowHandle(mWindow);
}

void GameFrameworkBaseSDL::setCaption(const SafeString& caption)
{
    SDL_SetWindowTitle(mWindow, caption.cstr());
}

bool GameFrameworkBaseSDL::setWaitVBlankInterval(u32 interval)
{
    if (!Graphics::instance()->setVBlankWaitInterval(interval))
        return false;

    mArg.wait_vblank = interval;
    return true;
}

void GameFrameworkBaseSDL::runImpl_()
{
    waitStartDisplayLoop_();

    mainLoop_();
}

MethodTreeMgr* GameFrameworkBaseSDL::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) SingleScreenMethodTreeMgr();
}

void GameFrameworkBaseSDL::createWindow_()
{
    sead::CurrentHeapSetter chs(mSDLHeap);

    //* Window creation
    {
        if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD))
        {
            SEAD_ASSERT_MSG(false, "Failed to init SDL! Error: %s", SDL_GetError());
        }   

        mWindow = SDL_CreateWindow(mArg.window_name.cstr(), mArg.width,
                                   mArg.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        SEAD_ASSERT_MSG( mWindow, "SDL_CreateWindow failed: %s", SDL_GetError());
    }

    setFps(mArg.fps);
    mLastUpdateTime.setNow();
    mLastUpdateTime -= mFrameTime;
}

void GameFrameworkBaseSDL::mainLoop_()
{
    Graphics::instance()->lockDrawContext();
    {
        procCalc_();
    }
    Graphics::instance()->unlockDrawContext();

    while (!mExit)
    {
        SDL_Event event;
        bool isEventPolled = false;

        {
            sead::CurrentHeapSetter chs(mSDLHeap);

            while (SDL_PollEvent(&event)) 
            {
                if (mCallback != nullptr)
                    mCallback(event);
               // break;

                switch (event.type) {
                    case SDL_EVENT_QUIT :
                        mExit = true;
                        isEventPolled = true;
                        break;

                    case SDL_EVENT_MOUSE_WHEEL :
                        mMouseWheel = event.wheel.y;
                        mMouseWheelDelta = event.wheel.y;
                        isEventPolled = true;
                        break;

                    default:
                        isEventPolled = true;
                        break;
                }
            }
        }
        
        procFrame_();

        TickSpan dt = mLastUpdateTime.diffToNow();

    if (dt < mFrameTime)
    {
        SDL_Delay(16);
        continue;
    }

    mLastUpdateTime.setNow();

       // SDL_Delay(1);

        // TickSpan dt = mLastUpdateTime.diffToNow();
        // if (mArg.wait_vblank > 0 || dt > mFrameTime)
        // {
        //     mLastDiffTime = dt;
        //     mLastUpdateTime.setNow();

        //     procFrame_();

        //     mMouseWheel = 0;

        //     SDL_Delay(0);
        // }

        // else if (dt + TickSpan::makeFromMilliSeconds(10) > mFrameTime)
        // {
        //     SDL_Delay(0);
        // }
        
        // else
        // {
        //     SDL_Delay(1);
        // }
    }
}

void GameFrameworkBaseSDL::procCalc_()
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
