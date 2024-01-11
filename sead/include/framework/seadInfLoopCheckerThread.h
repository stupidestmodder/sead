#pragma once

#include <thread/seadThread.h>

namespace sead {

class InfLoopCheckerThread : public Thread
{
public:
    InfLoopCheckerThread(TickSpan span, Heap* heap, s32 platformPriority = cDefaultPriority,
                         s32 stackSize = cDefaultStackSize, s32 msgQueueSize = cDefaultMsgQueueSize);
    ~InfLoopCheckerThread() override;

protected:
    void calc_(MessageQueue::Element msg) override;

protected:
    TickSpan mSpan;
};

} // namespace sead
