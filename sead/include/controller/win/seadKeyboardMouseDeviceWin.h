#pragma once

#include <controller/seadControlDevice.h>
#include <math/seadVector.h>
#include <prim/seadBitFlag.h>
#include <prim/seadLongBitFlag.h>

namespace sead {

class KeyboardMouseDevice : public ControlDevice
{
    SEAD_RTTI_OVERRIDE(KeyboardMouseDevice, ControlDevice);

public:
    enum class ActiveWindow
    {
        eNone = 0,
        eMain,
        eSub
    };

    static const s32 cVKeyMax = 256;

public:
    explicit KeyboardMouseDevice(ControllerMgr* mgr);

    void calc() override;

    bool isVkeyHold(s32 vkey) const { return mVkeyHold.isOnBit(vkey); }
    bool isVkeyTrig(s32 vkey) const { return mVkeyTrig.isOnBit(vkey); }
    bool isVkeyRelease(s32 vkey) const { return mVkeyRelease.isOnBit(vkey); }
    bool isVkeyRepeat(s32 vkey) const { return mVkeyRepeat.isOnBit(vkey); }
    bool isVkeyTrigWithRepeat(s32 vkey) const { return isVkeyTrig(vkey) || isVkeyRepeat(vkey); }

    bool isKeyEnable() const { return mFlags.isOn(FlagMask::eKeyEnable); }
    bool isCursorEnable() const { return mFlags.isOn(FlagMask::eCursorEnable); }
    bool isClientAreaEnable() const { return mFlags.isOn(FlagMask::eClientAreaEnable); }
    bool isCursorOnClientArea() const { return mFlags.isOn(FlagMask::eCursorOnClientArea); }

    ActiveWindow getActiveWindow() const { return mActiveWindow; }

    const Vector2f& getCursorClientAreaPos() const { return mCursorClientAreaPos; }
    const Vector2f& getCursorClientAreaPosCenterOrigin() const { return mCursorClientAreaPosCenterOrigin; }
    const Vector2f& getCursorScreenPos() const { return mCursorScreenPos; }

    s32 getWheel() const { return mMouseWheel; }
    s32 getWheelDelta() const { return mMouseWheelDelta; }

    void setMainWindowHandle(void* hWnd) { mMainWindowHandle = hWnd; }
    void* getMainWindowHandle() const { return mMainWindowHandle; }

    void setSubWindowHandle(void* hWnd) { mSubWindowHandle = hWnd; }
    void* getSubWindowHandle() const { return mSubWindowHandle; }

protected:
    enum FlagMask
    {
        eKeyEnable          = 1 << 0,
        eCursorEnable       = 1 << 1,
        eCursorOnClientArea = 1 << 2,
        eClientAreaEnable   = 1 << 3
    };

protected:
    LongBitFlag<cVKeyMax> mVkeyHold;
    LongBitFlag<cVKeyMax> mVkeyTrig;
    LongBitFlag<cVKeyMax> mVkeyRelease;
    LongBitFlag<cVKeyMax> mVkeyRepeat;
    Vector2f mCursorClientAreaPos;
    Vector2f mCursorClientAreaPosCenterOrigin;
    Vector2f mCursorScreenPos;
    BitFlag32 mFlags;
    void* mMainWindowHandle;
    void* mSubWindowHandle;
    ActiveWindow mActiveWindow;
    s32 mMouseWheelDelta;
    s32 mMouseWheel;
};

} // namespace sead
