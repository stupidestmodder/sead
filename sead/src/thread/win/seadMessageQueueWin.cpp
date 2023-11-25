#include <thread/seadMessageQueue.h>

#include <basis/seadAssert.h>
#include <prim/seadScopedLock.h>

namespace sead {

MessageQueue::MessageQueue()
    : mCriticalSection()
    , mEvent(false)
    , mBuffer()
{
}

MessageQueue::~MessageQueue()
{
    free();
}

void MessageQueue::allocate(s32 size, Heap* heap)
{
    SEAD_ASSERT_MSG(size > 0, "MessageQueue size must not zero");

    mBuffer.allocBuffer(size, heap);
}

void MessageQueue::free()
{
    mBuffer.freeBuffer();
    mEvent.setSignal();
}

bool MessageQueue::push(Element message, BlockType blockType)
{
    if (blockType != BlockType::eBlock)
        return push_(message);

    while (!push_(message))
        mEvent.wait();

    return true;
}

bool MessageQueue::push_(Element message)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    bool success = mBuffer.pushBack(message);
    if (success)
        mEvent.setSignal();

    return success;
}

} // namespace sead
