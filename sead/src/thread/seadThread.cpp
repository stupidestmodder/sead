#include <thread/seadThread.h>

#include <basis/seadWarning.h>

namespace sead {

bool Thread::sendMessage(MessageQueue::Element msg, MessageQueue::BlockType blockType)
{
    if (msg == MessageQueue::cNullElement)
    {
        SEAD_ASSERT_MSG(false, "Can not send cNullElement(==%d)", MessageQueue::cNullElement);
        return false;
    }

    if (isDone())
    {
        SEAD_WARNING("Thread is done. Reject message: %d", msg);
        return false;
    }

    if (msg == mQuitMsg)
    {
        SEAD_ASSERT_MSG(false, "use quit()");
        return false;
    }

    return mMessageQueue.push(msg, blockType);
}

MessageQueue::Element Thread::recvMessage(MessageQueue::BlockType blockType)
{
    if (mState == State::eQuitting)
        return MessageQueue::cNullElement;

    return mMessageQueue.pop(blockType);
}

void Thread::quit(bool isJam)
{
    if (isDone())
    {
        SEAD_WARNING("Thread is done. Can not quit.");
        return;
    }

    if (mState == State::eRunning)
        mState = State::eQuitting;

    if (isJam)
        mMessageQueue.jam(mQuitMsg, MessageQueue::BlockType::eBlock);
    else
        mMessageQueue.push(mQuitMsg, MessageQueue::BlockType::eBlock);
}

void Thread::quitAndWaitDoneSingleThread(bool isJam)
{
    quit(isJam);
    waitDone();
}

void Thread::run_()
{
    while (true)
    {
#ifdef SEAD_DEBUG
        checkStackOverFlow(nullptr, 0);
#endif // SEAD_DEBUG

        MessageQueue::Element msg = mMessageQueue.pop(mBlockType);
        if (msg == mQuitMsg)
            break;

        calc_(msg);
    }
}

ThreadMgr::ThreadMgr()
    : mList()
    , mIterateLockCS()
    , mMainThread(nullptr)
    , mThreadPtrTLS()
{
}

} // namespace sead
