#include <controller/win/seadKeyboardMouseDeviceWin.h>

#include <basis/win/seadWindows.h>
#include <controller/seadControllerBase.h>
#include <controller/seadControllerMgr.h>
#include <framework/win/seadGameFrameworkBaseWin.h>
#include <prim/seadMemUtil.h>

static BOOL _stdcall EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    if (GetCurrentProcessId() == processId && GetForegroundWindow() == hwnd)
    {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return false;
    }

    return true;
}

namespace sead {

KeyboardMouseDevice::KeyboardMouseDevice(ControllerMgr* mgr)
    : ControlDevice(mgr)
    , mVkeyHold()
    , mVkeyTrig()
    , mVkeyRelease()
    , mVkeyRepeat()
    , mCursorClientAreaPos(ControllerBase::cInvalidPointer)
    , mCursorClientAreaPosCenterOrigin(ControllerBase::cInvalidPointer)
    , mCursorScreenPos(ControllerBase::cInvalidPointer)
    , mFlags()
    , mMainWindowHandle(nullptr)
    , mSubWindowHandle(nullptr)
    , mActiveWindow(ActiveWindow::eNone)
    , mMouseWheelDelta(0)
    , mMouseWheel(0)
{
    mId = ControllerDefine::DeviceId::eKeyboardMouse;
}

void KeyboardMouseDevice::calc()
{
    mFlags.set(FlagMask::eKeyEnable);

    mVkeyTrig.makeAllZero();
    mVkeyRelease.makeAllZero();
    mVkeyRepeat.makeAllZero();

    for (s32 i = 0; i < cVKeyMax; i++)
    {
        SHORT keyState = GetAsyncKeyState(i);

        bool held = mVkeyHold.isOnBit(i);
        bool hold = (keyState & 0x8000) != 0;

        mVkeyHold.changeBit(i, hold);

        if (hold)
        {
            if (!held)
            {
                mVkeyTrig.setBit(i);
            }
        }
        else
        {
            if (held)
            {
                mVkeyRelease.setBit(i);
            }
        }

        if ((keyState & 1) != 0 && held)
        {
            mVkeyRepeat.setBit(i);
        }
    }

    {
        mFlags.reset(FlagMask::eCursorEnable);
        mFlags.reset(FlagMask::eCursorOnClientArea);
        mFlags.reset(FlagMask::eClientAreaEnable);

        mMouseWheelDelta = 0;
        mMouseWheel = 0;

        POINT point;
        if (GetCursorPos(&point))
        {
            mFlags.set(FlagMask::eCursorEnable);

            mCursorScreenPos.set(static_cast<f32>(point.x), static_cast<f32>(point.y));
        }

        if (!mMainWindowHandle)
            EnumWindows(&EnumWindowsProc, reinterpret_cast<LPARAM>(&mMainWindowHandle));

        HWND foregroundHWnd = GetForegroundWindow();

        if (mMainWindowHandle && reinterpret_cast<HWND>(mMainWindowHandle) == foregroundHWnd)
            mActiveWindow = ActiveWindow::eMain;
        else if (mSubWindowHandle && reinterpret_cast<HWND>(mSubWindowHandle) == foregroundHWnd)
            mActiveWindow = ActiveWindow::eSub;
        else
            mActiveWindow = ActiveWindow::eNone;

        if (mActiveWindow == ActiveWindow::eNone)
            return;

        GameFrameworkBaseWin* fw = DynamicCast<GameFrameworkBaseWin>(mMgr->getFramework());
        if (fw)
        {
            mMouseWheel = fw->getMouseWheel();
            mMouseWheelDelta = mMouseWheel / WHEEL_DELTA;
        }

        if (!isCursorEnable())
            return;

        POINT clientPoint = point;
        if (ScreenToClient(foregroundHWnd, &clientPoint))
        {
            mFlags.set(FlagMask::eClientAreaEnable);

            RECT rect;
            if (GetClientRect(foregroundHWnd, &rect) &&
                0 <= clientPoint.x && clientPoint.x <= rect.right &&
                0 <= clientPoint.y && clientPoint.y <= rect.bottom)
            {
                mFlags.set(FlagMask::eCursorOnClientArea);
            }

            {
                f32 clientX = static_cast<f32>(clientPoint.x);
                f32 clientY = static_cast<f32>(clientPoint.y);

                mCursorClientAreaPos.set(clientX, clientY);
                mCursorClientAreaPosCenterOrigin.set((clientX / rect.right) * 2.0f - 1.0f, (clientY / rect.bottom) * 2.0f - 1.0f);
            }
        }
    }
}

} // namespace sead
