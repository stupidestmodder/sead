#include <thread/seadThread.h>

#include <basis/seadWarning.h>
#include <thread/seadThreadUtil.h>

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

static const u32 cStackCanaryMagic = 0x5EAD5CEC;

s32 Thread::calcStackUsedSizePeak() const
{
#ifdef SEAD_DEBUG
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
    return 0;
#else
    u32* stackCheck = getStackCheckStartAddress_();
    if (stackCheck)
    {
        u32* stackCheckEnd = static_cast<u32*>(PtrUtil::addOffset(mStackTop, mStackSize));
        for (; stackCheck < stackCheckEnd; stackCheck++)
        {
            if (*stackCheck != cStackCanaryMagic)
                return PtrUtil::diff(stackCheckEnd, stackCheck);
        }
    }

    return 0;
#endif // SEAD_PLATFORM_WINDOWS
#else
    return 0;
#endif // SEAD_DEBUG
}

void Thread::checkStackOverFlow(const char* pos, s32 line) const
{
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);

#ifdef SEAD_DEBUG
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
#else
    checkStackPointerOverFlow(pos, line);
    checkStackEndCorruption(pos, line);
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_DEBUG
}

void Thread::checkStackEndCorruption(const char* pos, s32 line) const
{
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);

#ifdef SEAD_DEBUG
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
#else
    if (this == ThreadMgr::instance()->getMainThread())
        return;

    u32* stackCheckAddr = getStackCheckStartAddress_();
    if (!stackCheckAddr)
        return;

    SEAD_ASSERT_MSG(*stackCheckAddr == cStackCanaryMagic, "sead::Thread Stack End Corruption! [%s:0x%p]\n  Source File: %s\n  Line Number: %d\n  Stack Size: %d",
                    getName().cstr(), this, pos ? pos : SafeString::cEmptyString.cstr(), line, getStackSize());
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_DEBUG
}

void Thread::checkStackPointerOverFlow(const char* pos, s32 line) const
{
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);

#ifdef SEAD_DEBUG
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
#else
    // TODO
    SEAD_ASSERT(false);
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_DEBUG
}

void Thread::setStackOverflowExceptionEnable(bool enable)
{
    //! Idk what platform actually uses this

    SEAD_UNUSED(enable);

#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
#else
    SEAD_WARNING("This platform cannot set stack overflow exception.");
#endif // SEAD_PLATFORM_WINDOWS
}

void Thread::run_()
{
    while (true)
    {
#ifdef SEAD_DEBUG
#ifdef SEAD_PLATFORM_WINDOWS
        // Nothing
#else
        checkStackOverFlow(nullptr, 0);
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_DEBUG

        MessageQueue::Element msg = mMessageQueue.pop(mBlockType);
        if (msg == mQuitMsg)
            break;

        calc_(msg);
    }
}

void Thread::initStackCheck_()
{
#ifdef SEAD_DEBUG
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
#else
    u32* stackCheck = getStackCheckStartAddress_();
    u32* stackCheckEnd = static_cast<u32*>(PtrUtil::addOffset(mStackTop, mStackSize));

    for (; stackCheck < stackCheckEnd; stackCheck++)
    {
        *stackCheck = cStackCanaryMagic;
    }
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_DEBUG
}

void Thread::initStackCheckWithCurrentStackPointer_()
{
#ifdef SEAD_DEBUG
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
#else
    u32* stackCheck = getStackCheckStartAddress_();
    u32* stackCheckEnd = reinterpret_cast<u32*>(ThreadUtil::GetCurrentStackPointer());

    for (; stackCheck < stackCheckEnd; stackCheck++)
    {
        *stackCheck = cStackCanaryMagic;
    }
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_DEBUG
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
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);
#else
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* currentThread = threadMgr->getCurrentThread();
    if (currentThread)
        currentThread->checkStackOverFlow(pos, line);
#endif // SEAD_PLATFORM_WINDOWS
}

void ThreadMgr::checkCurrentThreadStackEndCorruption(const char* pos, s32 line)
{
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);
#else
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* currentThread = threadMgr->getCurrentThread();
    if (currentThread)
        currentThread->checkStackEndCorruption(pos, line);
#endif // SEAD_PLATFORM_WINDOWS
}

void ThreadMgr::checkCurrentThreadStackPointerOverFlow(const char* pos, s32 line)
{
#ifdef SEAD_PLATFORM_WINDOWS
    // Nothing
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);
#else
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* currentThread = threadMgr->getCurrentThread();
    if (currentThread)
        currentThread->checkStackPointerOverFlow(pos, line);
#endif // SEAD_PLATFORM_WINDOWS
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
