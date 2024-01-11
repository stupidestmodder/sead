#include <framework/seadInfLoopChecker.h>

namespace sead {

SEAD_TASK_SINGLETON_DISPOSER_IMPL(InfLoopChecker);

InfLoopChecker::InfLoopChecker(const TaskConstructArg& arg)
    : CalculateTask(arg, "sead::InfLoopChecker")
    , mLoopCounter(0)
    , mLoopThreshold(600)
    , mEnable(true)
    , mInfLoopEvent()
    , mDisableCounter(0)
{
}

InfLoopChecker::~InfLoopChecker()
{
}

void InfLoopChecker::countUp()
{
    if (!mEnable || mDisableCounter.getValue() != 0)
        return;

    if (getCounter() < getThreshold())
    {
        mLoopCounter++;
        return;
    }

    onInfLoop_();
}

void InfLoopChecker::prepare()
{
    adjustHeapAll();
}

void InfLoopChecker::calc()
{
    resetCounter();
}

void InfLoopChecker::onInfLoop_()
{
    InfLoopParam param;
    mInfLoopEvent.fire(param);

    SEAD_ASSERT_MSG(false, "Inf-Loop detected.");
}

} // namespace sead
