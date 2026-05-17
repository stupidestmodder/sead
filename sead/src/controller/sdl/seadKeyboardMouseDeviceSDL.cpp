#include <controller/win/seadKeyboardMouseDeviceWin.h>

#include <basis/sdl/seadSDL.h>
#include <controller/seadControllerBase.h>
#include <controller/seadControllerMgr.h>
#include <framework/sdl/seadGameFrameworkBaseSDL.h>
#include <prim/seadMemUtil.h>

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

    s32 numKeys;
    const bool* keyArray = SDL_GetKeyboardState(&numKeys);

    for (s32 i = 0; i < numKeys; i++)
    {
        bool held = mVkeyHold.isOnBit(i);
        bool hold = (keyArray[i]) != 0;

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

        // if ((keyState & 1) != 0 && held)
        // {
        //     mVkeyRepeat.setBit(i);
        // }
    }

    {
        mFlags.reset(FlagMask::eCursorEnable);
        mFlags.reset(FlagMask::eCursorOnClientArea);
        mFlags.reset(FlagMask::eClientAreaEnable);

        mMouseWheelDelta = 0;
        mMouseWheel = 0;

        float mouseX, mouseY;
        u32 mouseButtons = SDL_GetGlobalMouseState(&mouseX, &mouseY);

        mFlags.set(FlagMask::eCursorEnable);
        mCursorScreenPos.set(static_cast<f32>(mouseX), static_cast<f32>(mouseY));

        if (!mMainWindowHandle)
            mMainWindowHandle = SDL_GetKeyboardFocus();

        SDL_Window* focusWindow = SDL_GetKeyboardFocus();

        if (mMainWindowHandle && reinterpret_cast<SDL_Window*>(mMainWindowHandle) == focusWindow)
            mActiveWindow = ActiveWindow::eMain;
        else if (mSubWindowHandle && reinterpret_cast<SDL_Window*>(mSubWindowHandle) == focusWindow)
            mActiveWindow = ActiveWindow::eSub;
        else
            mActiveWindow = ActiveWindow::eNone;

        if (mActiveWindow == ActiveWindow::eNone)
            return;

        GameFrameworkBaseSDL* fw = DynamicCast<GameFrameworkBaseSDL>(mMgr->getFramework());
        if (fw)
        {
            mMouseWheelDelta = fw->getMouseWheelDelta();
        }

        if (!isCursorEnable())
            return;

        mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Rect rect;
        SDL_GetWindowPosition(focusWindow, &rect.x, &rect.y);
        SDL_GetWindowSize(focusWindow, &rect.w, &rect.h);

        if (1)
        {
            mFlags.set(FlagMask::eClientAreaEnable);

            u32 windowsFlags = SDL_GetWindowFlags(focusWindow);

            if (windowsFlags & SDL_WINDOW_MOUSE_FOCUS)
            {
                mFlags.set(FlagMask::eCursorOnClientArea);
            }

            {
                f32 clientX = static_cast<f32>(mouseX);
                f32 clientY = static_cast<f32>(mouseY);

                mCursorClientAreaPos.set(clientX, clientY);
                mCursorClientAreaPosCenterOrigin.set((clientX / (rect.x + rect.w)) * 2.0f - 1.0f, (clientY / (rect.y + rect.h)) * 2.0f - 1.0f);
            }
        }
    }
}

} // namespace sead
