#include <controller/seadControllerWrapper.h>

#include <prim/seadMemUtil.h>

namespace sead {

const u8 ControllerWrapper::cPadConfigDefault[Controller::PadIdx::eMax] = {
    Controller::PadIdx::eA,
    Controller::PadIdx::eB,
    Controller::PadIdx::eC,
    Controller::PadIdx::eX,
    Controller::PadIdx::eY,
    Controller::PadIdx::eZ,
    Controller::PadIdx::e2,
    Controller::PadIdx::e1,
    Controller::PadIdx::eHome,
    Controller::PadIdx::eMinus,
    Controller::PadIdx::ePlus,
    Controller::PadIdx::eStart,
    Controller::PadIdx::eSelect,
    Controller::PadIdx::eL,
    Controller::PadIdx::eR,
    Controller::PadIdx::eTouch,
    Controller::PadIdx::eUp,
    Controller::PadIdx::eDown,
    Controller::PadIdx::eLeft,
    Controller::PadIdx::eRight,
    Controller::PadIdx::eLeftStickUp,
    Controller::PadIdx::eLeftStickDown,
    Controller::PadIdx::eLeftStickLeft,
    Controller::PadIdx::eLeftStickRight,
    Controller::PadIdx::eRightStickUp,
    Controller::PadIdx::eRightStickDown,
    Controller::PadIdx::eRightStickLeft,
    Controller::PadIdx::eRightStickRight
};

ControllerWrapper::ControllerWrapper()
    : ControllerWrapperBase()
{
    MemUtil::copy(mPadConfig, cPadConfigDefault, Controller::PadIdx::eMax);
}

void ControllerWrapper::calc(u32 prevHold, bool prevPointerOn)
{
    if (mIsEnable && mController && mController->isConnected())
    {
        mPadHold = BitFlag32(createPadMaskFromControllerPadMask_(mController->getHoldMask()));

        mLeftStick = mController->getLeftStick();
        mRightStick = mController->getRightStick();
        mLeftAnalogTrigger = mController->getLeftAnalogTrigger();
        mRightAnalogTrigger = mController->getRightAnalogTrigger();

        setPointerWithBound_(mController->isPointerOn(), mTouchKeyBit >= 0 ? mPadHold.isOnBit(mTouchKeyBit) : false, mController->mPointer);
        updateDerivativeParams_(createPadMaskFromControllerPadMask_(prevHold), prevPointerOn);
    }
    else
    {
        setIdle();
    }

    if (isIdle_())
        mIdleFrame++;
    else
        mIdleFrame = 0;
}

void ControllerWrapper::setPadConfig(s32 padbitMax, const u8* padConfig, bool enableStickcrossEmulation)
{
    SEAD_ASSERT_MSG(padbitMax <= PadIdx::eMaxBase, "padbitMax[%d] must be equal or smaller than %d", padbitMax, PadIdx::eMaxBase);
    mPadBitMax = padbitMax;

    MemUtil::copy(mPadConfig, padConfig, padbitMax);

    mLeftStickCrossStartBit = -1;
    mRightStickCrossStartBit = -1;

    if (enableStickcrossEmulation)
    {
        for (s32 i = 0; i < padbitMax; i++)
        {
            if (padConfig[i] == Controller::PadIdx::eLeftStickUp)
                mLeftStickCrossStartBit = i;
            else if (padConfig[i] == Controller::PadIdx::eRightStickUp)
                mRightStickCrossStartBit = i;
        }
    }

    mTouchKeyBit = -1;

    for (s32 i = 0; i < padbitMax; i++)
    {
        if (padConfig[i] == Controller::PadIdx::eTouch)
        {
            mTouchKeyBit = i;
            break;
        }
    }
}

u32 ControllerWrapper::createPadMaskFromControllerPadMask_(u32 controllerMask) const
{
    BitFlag32 controllerMaskFlag(controllerMask);
    BitFlag32 maskFlag;

    for (s32 i = 0; i < mPadBitMax; i++)
    {
        if (controllerMaskFlag.isOnBit(mPadConfig[i]))
            maskFlag.setBit(i);
    }

    return maskFlag.getDirect();
}

} // namespace sead
