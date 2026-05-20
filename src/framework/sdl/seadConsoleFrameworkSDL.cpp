#include <framework/sdl/seadConsoleFrameworkSDL.h>

#include <basis/sdl/seadSDL.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <thread/seadThread.h>

namespace sead {

ConsoleFrameworkSDL::ConsoleFrameworkSDL()
    : Framework()
{
}

ConsoleFrameworkSDL::~ConsoleFrameworkSDL()
{
}

void ConsoleFrameworkSDL::initialize(const InitializeArg& arg)
{
    Framework::initialize(arg);
}

bool ConsoleFrameworkSDL::setProcessPriority(ProcessPriority priority)
{
    switch (priority)
    {
        case ProcessPriority::eIdle:
            // SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eNormal:
            // SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eHigh:
            // SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
            return true;

        case ProcessPriority::eRealTime:
            // SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
            return true;

        default:
            SEAD_ASSERT_MSG(false, "Undefined ProcessPriority.\n");
            return false;
    }
}

void ConsoleFrameworkSDL::runImpl_()
{
    SingleScreenMethodTreeMgr* methodTreeMgr = DynamicCast<SingleScreenMethodTreeMgr>(getMethodTreeMgr());

    while (!mTaskMgr->getRootTask())
    {
        mTaskMgr->beforeCalc();
        mTaskMgr->afterCalc();

        Thread::sleep(TickSpan::makeFromMilliSeconds(10));
    }

    methodTreeMgr->pauseAll(false);

    while (true)
    {
        methodTreeMgr->draw();

        mTaskMgr->beforeCalc();
        methodTreeMgr->calc();
        mTaskMgr->afterCalc();

        procReset_();

        Thread::sleep(TickSpan::makeFromMilliSeconds(16));
    }
}

MethodTreeMgr* ConsoleFrameworkSDL::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) SingleScreenMethodTreeMgr();
}

} // namespace sead
