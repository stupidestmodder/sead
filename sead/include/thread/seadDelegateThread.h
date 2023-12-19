#pragma once

#include <thread/seadThread.h>

namespace sead {

class DelegateThread : public Thread
{
public:
    DelegateThread(const SafeString& name, IDelegate2<Thread*, MessageQueue::Element>* deleg, Heap* heap, s32 platformPriority,
                   MessageQueue::BlockType blockType, MessageQueue::Element quitMsg, s32 stackSize, s32 msgQueueSize);
    ~DelegateThread() override;

protected:
    void calc_(MessageQueue::Element msg) override;

protected:
    IDelegate2<Thread*, MessageQueue::Element>* mDelegate;
};

} // namespace sead
