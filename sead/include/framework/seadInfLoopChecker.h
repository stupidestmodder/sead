#pragma once

#include <framework/seadCalculateTask.h>
#include <framework/seadTaskMgr.h>
#include <thread/seadAtomic.h>

namespace sead {

class InfLoopChecker : public CalculateTask
{
    SEAD_RTTI_OVERRIDE(InfLoopChecker, CalculateTask);
    SEAD_TASK_SINGLETON_DISPOSER(InfLoopChecker);

public:
    struct InfLoopParam
    {
        u8 dummy;
    };

    using InfLoopEvent = DelegateEvent<const InfLoopParam&>;

public:
    explicit InfLoopChecker(const TaskConstructArg& arg);
    ~InfLoopChecker() override;

    void countUp();

    void prepare() override;
    void calc() override;

    u32 getCounter() const
    {
        return mLoopCounter;
    }

    void resetCounter()
    {
        mLoopCounter = 0;
    }

    u32 getThreshold() const
    {
        return mLoopThreshold;
    }

    void setThreshold(u32 threashold)
    {
        SEAD_ASSERT(threashold > 1);
        mLoopThreshold = threashold;
    }

    //? Unofficial name but probably exists
    void setDisableCounter(u32 counter)
    {
        mDisableCounter.setValue(counter);
    }

    //? Unofficial name but probably exists
    void setEnable(bool enable)
    {
        mEnable = enable;
    }

    //? Unofficial name but probably exists
    void registerCallback(InfLoopEvent::Slot& slot)
    {
        mInfLoopEvent.connect(slot);
    }

    //? Unofficial name but probably exists
    void unregisterCallback(InfLoopEvent::Slot& slot)
    {
        mInfLoopEvent.disconnect(slot);
    }

protected:
    void onInfLoop_();

protected:
    u32 mLoopCounter;
    u32 mLoopThreshold;
    bool mEnable;
    InfLoopEvent mInfLoopEvent;
    AtomicU32 mDisableCounter;
};

} // namespace sead
