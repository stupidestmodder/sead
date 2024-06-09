#pragma once

#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <prim/seadBitFlag.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class ControllerBase
{
    SEAD_RTTI_BASE(ControllerBase);

public:
    static const f32 cStickHoldThresholdDefault;
    static const f32 cStickReleaseThresholdDefault;
    static const Vector2f cInvalidPointer;
    static const Vector2i cInvalidPointerS32;

public:
    ControllerBase(s32 padBitMax, s32 leftStickCrossStartBit, s32 rightStickCrossStartBit, s32 touchKeyBit);

    u32 getHoldMask() const { return mPadHold.getDirect(); }
    u32 getTrigMask() const { return mPadTrig.getDirect(); }
    u32 getTrigMaskWithRepeat() const { return mPadRepeat.getDirect() | mPadTrig.getDirect(); }
    u32 getReleaseMask() const { return mPadRelease.getDirect(); }
    u32 getRepeatMask() const { return mPadRepeat.getDirect(); }

    u32 getPadHoldCount(s32 bit) const;

    const Vector2f& getLeftStick() const { return mLeftStick; }
    const Vector2f& getRightStick() const { return mRightStick; }
    f32 getLeftAnalogTrigger() const { return mLeftAnalogTrigger; }
    f32 getRightAnalogTrigger() const { return mRightAnalogTrigger; }

    const Vector2f& getPointer() const
    {
        if (isPointerOn())
            return mPointer;
        else
            return cInvalidPointer;
    }

    // TODO: Does this also check isPointerOn() ?
    const Vector2i& getPointerPrev() const { return mPointerS32; }

    bool isPointerOn() const { return mPointerFlag.isOn(PointerFlagMask::eOn); }
    bool isPointerOnNow() const { return mPointerFlag.isOn(PointerFlagMask::eOnNow); }
    bool isPointerOffNow() const { return mPointerFlag.isOn(PointerFlagMask::eOffNow); }
    bool isPointerUnkFlag3() const { return mPointerFlag.isOn(PointerFlagMask::eUnkFlag3); } // TODO: Proper enum name

    bool isHold(u32 mask) const { return mask & getHoldMask(); }
    bool isTrig(u32 mask) const { return mask & getTrigMask(); }
    bool isTrigWithRepeat(u32 mask) const { return mask & getTrigMaskWithRepeat(); }
    bool isHoldAll(u32 mask) const { return (mask & getHoldMask()) == mask; }
    bool isRelease(u32 mask) const { return mask & getReleaseMask(); }
    bool isRepeat(u32 mask) const { return mask & getRepeatMask(); }

    void setPadRepeat(u32 mask, u8 delayFrame, u8 pulseFrame);

    void setLeftStickCrossThreshold(f32 hold, f32 release);
    void setRightStickCrossThreshold(f32 hold, f32 release);

    void setPointerBound(const BoundBox2f& bound);
    const BoundBox2f& getPointerBound() const { return mPointerBound; }

protected:
    void setPointerWithBound_(bool isOn, bool touchkeyHold, const Vector2f& pos);
    void updateDerivativeParams_(u32 prevHold, bool prevPointerOn);
    u32 getStickHold_(u32 prevHold, const Vector2f& stick, f32 holdThreshold, f32 releaseThreshold, s32 startBit);
    bool isIdleBase_();
    void setIdleBase_();
    u32 createStickCrossMask_();

protected:
    enum PadIdx
    {
        eMaxBase = 32
    };

    enum PointerFlagMask
    {
        eOn       = 1 << 0,
        eOnNow    = 1 << 1,
        eOffNow   = 1 << 2,
        eUnkFlag3 = 1 << 3 // TODO: Figure proper name
    };

    enum
    {
        cCrossUp = 0,
        cCrossDown,
        cCrossLeft,
        cCrossRight
    };

protected:
    BitFlag32 mPadTrig;
    BitFlag32 mPadRelease;
    BitFlag32 mPadRepeat;
    BitFlag32 mPointerFlag;
    Vector2i mPointerS32;
    BoundBox2f mPointerBound;
    u32 mPadHoldCounts[PadIdx::eMaxBase];
    u8 mPadRepeatDelays[PadIdx::eMaxBase];
    u8 mPadRepeatPulses[PadIdx::eMaxBase];
    f32 mLeftStickHoldThreshold;
    f32 mRightStickHoldThreshold;
    f32 mLeftStickReleaseThreshold;
    f32 mRightStickReleaseThreshold;
    s32 mPadBitMax;
    s32 mLeftStickCrossStartBit;
    s32 mRightStickCrossStartBit;
    s32 mTouchKeyBit;
    s32 mIdleFrame;
    BitFlag32 mPadHold;
    Vector2f mPointer;
    Vector2f mLeftStick;
    Vector2f mRightStick;
    f32 mLeftAnalogTrigger;
    f32 mRightAnalogTrigger;
};

} // namespace sead
