#include <framework/seadInfLoopCheckerThread.h>

#include <framework/seadInfLoopChecker.h>

namespace sead {

InfLoopCheckerThread::InfLoopCheckerThread(TickSpan span, Heap* heap, s32 platformPriority, s32 stackSize, s32 msgQueueSize)
    : Thread("sead::InfLoopCheckerThread", heap, platformPriority, MessageQueue::BlockType::eNoBlock, cDefaultQuitMsg,
             stackSize, msgQueueSize)
    , mSpan(span)
{
}

InfLoopCheckerThread::~InfLoopCheckerThread()
{
}

void InfLoopCheckerThread::calc_(MessageQueue::Element)
{
    Thread::sleep(mSpan);

    InfLoopChecker* checker = InfLoopChecker::instance();
    if (checker)
        checker->countUp();
}

} // namespace sead
