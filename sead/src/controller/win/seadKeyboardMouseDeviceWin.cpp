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

    for (u32 i = 0; i < cVKeyMax; i++)
    {
        mVKeyHold[i] = 0;
        mVKeyTrig[i] = 0;
        mVKeyRepeat[i] = 0;
    }
}

void KeyboardMouseDevice::calc()
{
    mFlags.set(FlagMask::eKeyEnable);

    MemUtil::fillZero(mVKeyTrig, cVKeyMax);
    MemUtil::fillZero(mVKeyRepeat, cVKeyMax);

    for (s32 i = 0; i < cVKeyMax; i++)
    {
        SHORT ret = GetAsyncKeyState(i);

        // TODO: Actualy do this.........

        if (ret & 1)
        {
            if (mVKeyHold[i] == VKeyFlag::eOn)
                mVKeyRepeat[i] = VKeyFlag::eOn;
            else
                mVKeyTrig[i] = VKeyFlag::eOn;
        }

        mVKeyHold[i] = (ret & 0x8000) != 0 ? static_cast<u8>(VKeyFlag::eOn) : static_cast<u8>(VKeyFlag::eOff);
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
