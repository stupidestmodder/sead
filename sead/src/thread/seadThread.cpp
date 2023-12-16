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

ThreadMgr::~ThreadMgr()
{
    ScopedLock<CriticalSection> lock(&mIterateLockCS);

    for (Thread* thread : mList)
    {
        thread->quit(false);
    }

    bool threadsDone;
    do
    {
        threadsDone = true;

        for (Thread* thread : mList)
        {
            threadsDone &= thread->isDone();
        }

        Thread::yield();
    } while (!threadsDone);

    for (Thread* thread : mList)
    {
        thread->waitDone();
    }

    sInstance = nullptr;
}

void ThreadMgr::initialize(Heap* heap)
{
    initMainThread_(heap);
    SEAD_ASSERT(mMainThread);
}

void ThreadMgr::destroy()
{
    destroyMainThread_();
}

bool ThreadMgr::isMainThread() const
{
    return getCurrentThread() == mMainThread;
}

void ThreadMgr::quitAndWaitDoneMultipleThread(Thread** threads, s32 num, bool isJam)
{
    for (s32 i = 0; i < num; i++)
    {
        threads[i]->quit(isJam);
    }

    ThreadMgr::waitDoneMultipleThread(threads, num);
}

void ThreadMgr::waitDoneMultipleThread(Thread* const* threads, s32 num)
{
    bool threadsDone;
    do
    {
        threadsDone = true;

        for (s32 i = 0; i < num; i++)
        {
            threadsDone &= threads[i]->isDone();
        }

        Thread::yield();
    } while (!threadsDone);

    for (s32 i = 0; i < num; i++)
    {
        threads[i]->waitDone();
    }
}

void ThreadMgr::checkCurrentThreadStackOverFlow(const char* pos, s32 line)
{
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* currentThread = threadMgr->getCurrentThread();
    if (currentThread)
        currentThread->checkStackOverFlow(pos, line);
}

void ThreadMgr::checkCurrentThreadStackEndCorruption(const char* pos, s32 line)
{
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* currentThread = threadMgr->getCurrentThread();
    if (currentThread)
        currentThread->checkStackEndCorruption(pos, line);
}

void ThreadMgr::checkCurrentThreadStackPointerOverFlow(const char* pos, s32 line)
{
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* currentThread = threadMgr->getCurrentThread();
    if (currentThread)
        currentThread->checkStackPointerOverFlow(pos, line);
}

void ThreadMgr::destroyMainThread_()
{
    if (mMainThread)
    {
        delete mMainThread;
        mMainThread = nullptr;
    }
}

} // namespace sead
