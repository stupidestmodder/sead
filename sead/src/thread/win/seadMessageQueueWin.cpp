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
    if (size <= 0)
    {
        SEAD_ASSERT_MSG(false, "MessageQueue size must not zero");
        return;
    }

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

MessageQueue::Element MessageQueue::pop(BlockType blockType)
{
    if (blockType != BlockType::eBlock)
        return pop_();

    Element msg;
    while ((msg = pop_()) == cNullElement)
        mEvent.wait();

    return msg;
}

MessageQueue::Element MessageQueue::pop_()
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (mBuffer.getNum() <= 0)
        return cNullElement;

    Element msg;
    mBuffer.popFront(&msg);

    mEvent.setSignal();

    return msg;
}

MessageQueue::Element MessageQueue::peek(BlockType blockType) const
{
    if (blockType != BlockType::eBlock)
        return peek_();

    Element msg;
    while ((msg = peek_()) == cNullElement)
        mEvent.wait();

    return msg;
}

MessageQueue::Element MessageQueue::peek_() const
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (mBuffer.getNum() <= 0)
        return cNullElement;

    Element msg;
    msg = mBuffer.front();

    return msg;
}

bool MessageQueue::jam(Element message, BlockType blockType)
{
    if (blockType != BlockType::eBlock)
        return jam_(message);

    while (!jam_(message))
        mEvent.wait();

    return true;
}

bool MessageQueue::jam_(Element message)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    bool success = mBuffer.pushFront(message);
    if (success)
        mEvent.setSignal();

    return success;
}

} // namespace sead
