#include <controller/seadControllerWrapperBase.h>

#include <controller/seadController.h>
#include <prim/seadMemUtil.h>

namespace sead {

ControllerWrapperBase::ControllerWrapperBase()
    : ControllerBase(Controller::PadIdx::eMax, -1, -1, Controller::PadIdx::eTouch)
    , IDisposer()
    , mController(nullptr)
    , mIsEnable(true)
    , mListNode()
{
}

ControllerWrapperBase::~ControllerWrapperBase()
{
    unregister();
}

void ControllerWrapperBase::registerWith(Controller* controller, bool copyRepeatSetting_)
{
    SEAD_ASSERT(controller);

    unregister();

    controller->mWrappers.pushBack(this);
    mController = controller;

    if (copyRepeatSetting_)
        copyRepeatSetting(mController);
}

void ControllerWrapperBase::unregister()
{
    if (mController)
    {
        mController->mWrappers.erase(this);
        mController = nullptr;
    }
}

void ControllerWrapperBase::copyRepeatSetting(const Controller* controller)
{
    MemUtil::copy(mPadRepeatDelays, controller->mPadRepeatDelays, PadIdx::eMaxBase);
    MemUtil::copy(mPadRepeatPulses, controller->mPadRepeatPulses, PadIdx::eMaxBase);
}

void ControllerWrapperBase::setEnable(bool enable)
{
    mIsEnable = enable;
}

void ControllerWrapperBase::setEnableOtherWrappers(bool enable) const
{
    if (!mController)
        return;

    for (ControllerWrapperBase& wrapper : mController->mWrappers)
    {
        if (&wrapper != this)
            wrapper.setEnable(enable);
    }
}

void ControllerWrapperBase::setIdle()
{
    setIdleBase_();
}

bool ControllerWrapperBase::isIdle_()
{
    return isIdleBase_();
}

} // namespace sead
