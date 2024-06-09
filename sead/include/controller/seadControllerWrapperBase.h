#pragma once

#include <controller/seadControllerBase.h>
#include <heap/seadDisposer.h>

namespace sead {

class Controller;

class ControllerWrapperBase : public ControllerBase, public IDisposer
{
    SEAD_RTTI_OVERRIDE(ControllerWrapperBase, ControllerBase);

public:
    ControllerWrapperBase();
    ~ControllerWrapperBase() override;

    void registerWith(Controller* controller, bool copyRepeatSetting);
    void unregister();
    void copyRepeatSetting(const Controller* controller);

    Controller* getController() const { return mController; }

    void setEnable(bool enable);
    void setEnableOtherWrappers(bool enable) const;

    virtual void calc(u32 prevHold, bool prevPointerOn) = 0;
    virtual void setIdle();

    bool isEnable() const { return mIsEnable; }

protected:
    virtual bool isIdle_();

protected:
    Controller* mController;
    bool mIsEnable;
    ListNode mListNode;
    u8 mPadConfig[PadIdx::eMaxBase];

    friend class Controller;
};

} // namespace sead
