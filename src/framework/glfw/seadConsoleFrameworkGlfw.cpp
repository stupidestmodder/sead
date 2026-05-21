#include <framework/glfw/seadConsoleFrameworkGlfw.h>

#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <thread/seadThread.h>

namespace sead {

ConsoleFrameworkGlfw::ConsoleFrameworkGlfw()
    : Framework()
{
}

ConsoleFrameworkGlfw::~ConsoleFrameworkGlfw()
{
}

void ConsoleFrameworkGlfw::initialize(const InitializeArg& arg)
{
    Framework::initialize(arg);
}

bool ConsoleFrameworkGlfw::setProcessPriority(ProcessPriority priority)
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

void ConsoleFrameworkGlfw::runImpl_()
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

MethodTreeMgr* ConsoleFrameworkGlfw::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) SingleScreenMethodTreeMgr();
}

} // namespace sead
