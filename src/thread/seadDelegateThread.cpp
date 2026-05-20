#include <thread/seadDelegateThread.h>

namespace sead {

DelegateThread::DelegateThread(const SafeString& name, IDelegate2<Thread*, MessageQueue::Element>* deleg, Heap* heap, s32 platformPriority,
                               MessageQueue::BlockType blockType, MessageQueue::Element quitMsg, s32 stackSize, s32 msgQueueSize)
    : Thread(name, heap, platformPriority, blockType, quitMsg, stackSize, msgQueueSize)
    , mDelegate(deleg)
{
}

DelegateThread::~DelegateThread()
{
}

void DelegateThread::calc_(MessageQueue::Element msg)
{
    mDelegate->invoke(this, msg);
}

} // namespace sead
