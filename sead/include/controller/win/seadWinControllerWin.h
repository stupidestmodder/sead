#pragma once

#include <controller/seadController.h>
#include <controller/win/seadWinJoyPadDeviceWin.h>

namespace sead {

class WinController : public Controller
{
    SEAD_RTTI_OVERRIDE(WinController, Controller);

public:
    static const u8 cDefaultVKeyConfig[PadIdx::eMax];
    static const u8 cDefaultJoyPadConfig[PadIdx::eMax];

    static const s32 cStickClampMinDefault = 0x1000;
    static const s32 cStickClampMaxDefault = 0x7FFF;
    static const s32 cStickClampValueMin = 0x1;
    static const s32 cStickClampValueMax = 0x7FFF;

    static const s32 cJoyPadNumberInvalid = WinJoyPadDevice::cNumMax;

public:
    WinController(ControllerMgr* mgr, s32 joyPadNumber);

    void setVKeyConfig(const u8* keyConfig);
    void setVKeyConfig(PadIdx bit, u8 vkey);
    void setVKeyConfigDefault();

    s32 getJoyPadNumber() const { return mJoyPadNumber; }
    void setJoyPadNumber(s32 number) { mJoyPadNumber = number; }
    bool isJoyPadEnable() const;

    void getStickClamp(s32* min, s32* max) const;
    void setStickClamp(s32 min, s32 max);

    void setJoyPadConfig(const u8* joyPadConfig);
    void setJoyPadConfig(PadIdx bit, u8 no);
    void setJoyPadConfigDefault();

    bool isJoyPadEnableWhenWindowInactive() const { return mFlag.isOn(FlagMask::eIsJoyPadEnableWhenWindowInactive); }
    void setJoyPadEnableWhenWindowInactive(bool active) { mFlag.change(FlagMask::eIsJoyPadEnableWhenWindowInactive, active); }

    bool isEnableNormalizeAnalogStickAsCircle() const { return mFlag.isOn(FlagMask::eNormalizeAnalogStickAsCircle); }
    void setEnableNormalizeAnalogStickAsCircle(bool enable) { mFlag.change(FlagMask::eNormalizeAnalogStickAsCircle, enable); }

    bool isCursorCenterOrigin() const { return mFlag.isOn(FlagMask::eIsCursorCenterOrigin); }
    void setCursorCenterOrigin(bool active) { return mFlag.change(FlagMask::eIsCursorCenterOrigin, active); }

    void setCursorCenterOriginBound(f32 bound) { mCursorCenterOriginBound = bound; }

protected:
    void calcImpl_() override;

protected:
    enum FlagMask
    {
        eIsJoyPadEnableWhenWindowInactive = 1 << 0,
        eNormalizeAnalogStickAsCircle     = 1 << 1,
        eIsCursorCenterOrigin             = 1 << 2
    };

protected:
    s32 mJoyPadNumber;
    s32 mStickClampMin;
    s32 mStickClampMax;
    u8 mVKeyConfig[PadIdx::eMax];
    u8 mJoyPadConfig[PadIdx::eMax];
    BitFlag32 mFlag;
    f32 mCursorCenterOriginBound;
};

} // namespace sead
