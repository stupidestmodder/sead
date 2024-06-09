#include <controller/seadController.h>

#include <controller/seadControllerAddon.h>
#include <controller/seadControllerWrapperBase.h>

namespace sead {

Controller::Controller(ControllerMgr* mgr)
    : ControllerBase(PadIdx::eMax, PadIdx::eLeftStickUp, PadIdx::eRightStickUp, PadIdx::eTouch)
    , mId(ControllerDefine::ControllerId::eNull)
    , mMgr(mgr)
    , mAddons()
    , mWrappers()
{
    mAddons.initOffset(offsetof(ControllerAddon, mListNode));
    mWrappers.initOffset(offsetof(ControllerWrapperBase, mListNode));
}

void Controller::calc()
{
    u32 prevHold = getHoldMask();
    bool prevPointerOn = isPointerOn();

    calcImpl_();

    updateDerivativeParams_(prevHold, prevPointerOn);

    {
        bool isAddonIdleAll = true;

        for (ControllerAddon& addon : mAddons)
        {
            if (addon.calc())
                isAddonIdleAll = false;
        }

        if (isAddonIdleAll && isIdle_())
            mIdleFrame++;
        else
            mIdleFrame = 0;
    }

    for (ControllerWrapperBase& wrapper : mWrappers)
    {
        wrapper.calc(prevHold, prevPointerOn);
    }
}

ControllerAddon* Controller::getAddon(ControllerDefine::AddonId id) const
{
    for (ControllerAddon& addon : mAddons)
    {
        if (addon.getId() == id)
            return &addon;
    }

    return nullptr;
}

ControllerAddon* Controller::getAddonByOrder(ControllerDefine::AddonId id, s32 index) const
{
    for (ControllerAddon& addon : mAddons)
    {
        if (addon.getId() == id)
        {
            if (index == 0)
                return &addon;

            index--;
        }
    }

    return nullptr;
}

bool Controller::isIdle_()
{
    return isIdleBase_();
}

void Controller::setIdle_()
{
    setIdleBase_();

    for (ControllerWrapperBase& wrapper : mWrappers)
    {
        wrapper.setIdle();
    }
}

} // namespace sead
