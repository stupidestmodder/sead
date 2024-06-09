#include <framework/win/seadGameFrameworkBaseWin.h>

#include <controller/seadControllerMgr.h>
#include <controller/win/seadKeyboardMouseDeviceWin.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>

#include <timeapi.h>

namespace sead {

GameFrameworkBaseWin* GameFrameworkBaseWin::sInstance = nullptr;

GameFrameworkBaseWin::GameFrameworkBaseWin(const CreateArg& arg)
    : GameFramework()
    , mArg(arg)
    , mHWnd(nullptr)
    , mLastUpdateTime()
    , mFrameTime()
    , mLastDiffTime()
    , mExit(false)
    , mMsgProcCallback(nullptr)
    , mDefaultFrameBuffer(nullptr)
    , mDefaultLogicalFrameBuffer()
    , mMouseWheel(0)
{
    SEAD_ASSERT(!sInstance);
    sInstance = this;

    createWindow_();
}

GameFrameworkBaseWin::~GameFrameworkBaseWin()
{
    sInstance = nullptr;
}

FrameBuffer* GameFrameworkBaseWin::getMethodFrameBuffer(s32 methodType) const
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

const LogicalFrameBuffer* GameFrameworkBaseWin::getMethodLogicalFrameBuffer(s32 methodType) const
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

bool GameFrameworkBaseWin::setProcessPriority(ProcessPriority priority)
{
    switch (priority)
    {
        case ProcessPriority::eIdle:
            SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eNormal:
            SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eHigh:
            SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eRealTime:
            SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
            return true;

        default:
            SEAD_ASSERT_MSG(false, "Undefined ProcessPriority.\n");
            return false;
    }
}

void GameFrameworkBaseWin::createControllerMgr(TaskBase* rootTask)
{
    GameFramework::createControllerMgr(rootTask);

    KeyboardMouseDevice* device = DynamicCast<KeyboardMouseDevice>(ControllerMgr::instance()->getControlDevice(ControllerDefine::DeviceId::eKeyboardMouse));
    if (device)
        device->setMainWindowHandle(mHWnd);
}

void GameFrameworkBaseWin::setCaption(const SafeString& caption)
{
    SetWindowTextA(mHWnd, caption.cstr());
}

bool GameFrameworkBaseWin::setWaitVBlankInterval(u32 interval)
{
    if (!Graphics::instance()->setVBlankWaitInterval(interval))
        return false;

    mArg.wait_vblank = interval;
    return true;
}

void GameFrameworkBaseWin::runImpl_()
{
    waitStartDisplayLoop_();

    mainLoop_();
}

MethodTreeMgr* GameFrameworkBaseWin::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) SingleScreenMethodTreeMgr();
}

void GameFrameworkBaseWin::createWindow_()
{
    //* Window creation
    {
        DWORD style = WS_TILEDWINDOW;

        RECT rect;
        SetRect(&rect, 0, 0, mArg.width, mArg.height);
        AdjustWindowRect(&rect, style, false);

        s32 width = rect.right - rect.left;
        s32 height = rect.bottom - rect.top;

        WNDCLASSA wndClass;
        MemUtil::fill(&wndClass, 0, sizeof(WNDCLASSA));

        wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wndClass.hCursor = LoadCursorA(nullptr, reinterpret_cast<LPCSTR>(IDC_ARROW));
        wndClass.lpfnWndProc = &GameFrameworkBaseWin::msgProc_;
        wndClass.lpszClassName = mArg.window_name.cstr();
        wndClass.hInstance = GetModuleHandleA(nullptr);

        if (!wndClass.hInstance)
        {
            SEAD_ASSERT_MSG(false, "GetModuleHandle failed");
            return;
        }

        if (RegisterClassA(&wndClass) == 0)
        {
            SEAD_ASSERT_MSG(false, "RegisterClass failed");
            return;
        }

        mHWnd = CreateWindowExA(0, mArg.window_name.cstr(), mArg.window_name.cstr(), style,
                                        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                                        nullptr, nullptr, wndClass.hInstance, nullptr);
        if (!mHWnd)
        {
            SEAD_ASSERT_MSG(false, "CreateWindow failed");
            return;
        }
    }

    setFps(mArg.fps);
    mLastUpdateTime.setNow();
    mLastUpdateTime -= mFrameTime;
}

void GameFrameworkBaseWin::mainLoop_()
{
    Graphics::instance()->lockDrawContext();
    {
        procCalc_();
    }
    Graphics::instance()->unlockDrawContext();

    while (!mExit)
    {
        MSG msg;
        if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            TickSpan dt = mLastUpdateTime.diffToNow();
            if (mArg.wait_vblank > 0 || dt > mFrameTime)
            {
                mLastDiffTime = dt;
                mLastUpdateTime.setNow();

                procFrame_();

                mMouseWheel = 0;

                Sleep(0);
            }
            else if (dt + TickSpan::makeFromMilliSeconds(10) > mFrameTime)
            {
                Sleep(0);
            }
            else
            {
                timeBeginPeriod(1);
                Sleep(1);
                timeEndPeriod(1);
            }
        }
    }
}

void GameFrameworkBaseWin::procCalc_()
{
    mCalcMeter.measureBegin();
    {
        mTaskMgr->beforeCalc();
        DynamicCast<SingleScreenMethodTreeMgr>(getMethodTreeMgr())->calc();
        mTaskMgr->afterCalc(); // TODO: Should we call this here ?
    }
    mCalcMeter.measureEnd();
}

LRESULT GameFrameworkBaseWin::msgProcImpl_(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam)
{
    if (mMsgProcCallback)
    {
        LRESULT result = mMsgProcCallback(hWnd, msg, wParam, lParam);
        if (result != 0)
            return result;
    }

    switch (msg)
    {
        case WM_KEYDOWN:
            if (wParam != VK_ESCAPE)
                break;

            //! Fallthrough

        case WM_CLOSE:
            mExit = true;
            return 1;

        case WM_MOUSEWHEEL:
            mMouseWheel += GET_WHEEL_DELTA_WPARAM(wParam);
            break;

        //! Temp
        case WM_SIZE:
		    if (wParam != SIZE_MINIMIZED)
            {
                f32 width = static_cast<f32>(LOWORD(lParam));
                f32 height = static_cast<f32>(HIWORD(lParam));

                if (mDefaultFrameBuffer)
                {
                    mDefaultFrameBuffer->setVirtualSize(width, height);
                    mDefaultFrameBuffer->setPhysicalArea(0.0f, 0.0f, width, height);
                }

                mDefaultLogicalFrameBuffer.setVirtualSize(width, height);
                mDefaultLogicalFrameBuffer.setPhysicalArea(0.0f, 0.0f, width, height);

                resize_(width, height);

                return 1;
            }
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

LRESULT __stdcall GameFrameworkBaseWin::msgProc_(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam)
{
    SEAD_ASSERT(sInstance);

    return sInstance->msgProcImpl_(hWnd, msg, wParam, lParam);
}

} // namespace sead
