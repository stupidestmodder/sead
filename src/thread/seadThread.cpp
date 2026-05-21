#include <thread/seadThread.h>

#include <basis/seadWarning.h>
#include <hostio/seadHostIOContext.h>
#include <hostio/seadHostIOEvent.h>
#include <hostio/seadHostIOFramework.h>
#include <hostio/seadHostIOMgr.h>
#include <hostio/seadHostIORoot.h>
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
#if defined(SEAD_TARGET_DEBUG)
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
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
#endif // SEAD_TARGET_DEBUG
}

#if defined(SEAD_TARGET_DEBUG)
void Thread::listenPropertyEvent(const hostio::PropertyEvent* ev)
{
    SEAD_UNUSED(ev);

    // switch (ev->id)
    // {
    //     case 'soex':
    //     {
    //         SEAD_WARNING("This platform cannot set stack overflow exception.");
    //         break;
    //     }
    // }
}

void Thread::genMessage(hostio::Context* context)
{
    hostio::Context::ContextBufferAccessor* ctxBuf = context->beginHTMLLabel("");
    if (ctxBuf)
    {
        BufferedSafeString buf(static_cast<char*>(ctxBuf->getBuffer()), ctxBuf->getMaxSize());
        buf.format(
            "<font face=\"ＭＳ ゴシック\"><table><tr><th>Name</th><td>%s</td></tr><tr><th>ID</th><td>%d</td></tr><tr><th>Priority</th><td>%d</td></tr><tr><th>BlockType</th><td>%s</td></tr><tr><th>QuitMessage</th><td>%d</td></tr><tr><th>StackSize</th><td>%d</td></tr><tr><th>StackUsedSizePeak</th><td>%d (%d%%)</td></tr><tr><th>State</th><td>%s</td></tr></table></font>",
            getName().cstr(), mID, getPriority(), getBlockType() == MessageQueue::BlockType::eBlock ? "Block" : "NoBlock", mQuitMsg, getStackSize(),
            calcStackUsedSizePeak(), 0, ""//mState.text()
        );

        context->endHTMLLabel(buf.calcLength());
    }
}
#endif // SEAD_TARGET_DEBUG

void Thread::checkStackOverFlow(const char* pos, s32 line) const
{
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);

#if defined(SEAD_TARGET_DEBUG)
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
    // Nothing
#else
    checkStackPointerOverFlow(pos, line);
    checkStackEndCorruption(pos, line);
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_TARGET_DEBUG
}

void Thread::checkStackEndCorruption(const char* pos, s32 line) const
{
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);

#if defined(SEAD_TARGET_DEBUG)
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
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
#endif // SEAD_TARGET_DEBUG
}

void Thread::checkStackPointerOverFlow(const char* pos, s32 line) const
{
    SEAD_UNUSED(pos);
    SEAD_UNUSED(line);

#if defined(SEAD_TARGET_DEBUG)
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
    // Nothing
#else
    // TODO
    SEAD_ASSERT(false);
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_TARGET_DEBUG
}

void Thread::setStackOverflowExceptionEnable(bool enable)
{
    //! Idk what platform actually uses this

    SEAD_UNUSED(enable);

#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
    // Nothing
#else
    SEAD_WARNING("This platform cannot set stack overflow exception.");
#endif // SEAD_PLATFORM_WINDOWS
}

void Thread::run_()
{
    while (true)
    {
#if defined(SEAD_TARGET_DEBUG)
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
        // Nothing
#else
        checkStackOverFlow(nullptr, 0);
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_TARGET_DEBUG

        MessageQueue::Element msg = mMessageQueue.pop(mBlockType);
        if (msg == mQuitMsg)
            break;

        calc_(msg);
    }
}

void Thread::initStackCheck_()
{
#if defined(SEAD_TARGET_DEBUG)
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
    // Nothing
#else
    u32* stackCheck = getStackCheckStartAddress_();
    u32* stackCheckEnd = static_cast<u32*>(PtrUtil::addOffset(mStackTop, mStackSize));

    for (; stackCheck < stackCheckEnd; stackCheck++)
    {
        *stackCheck = cStackCanaryMagic;
    }
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_TARGET_DEBUG
}

void Thread::initStackCheckWithCurrentStackPointer_()
{
#if defined(SEAD_TARGET_DEBUG)
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
    // Nothing
#else
    u32* stackCheck = getStackCheckStartAddress_();
    u32* stackCheckEnd = reinterpret_cast<u32*>(ThreadUtil::GetCurrentStackPointer());

    for (; stackCheck < stackCheckEnd; stackCheck++)
    {
        *stackCheck = cStackCanaryMagic;
    }
#endif // SEAD_PLATFORM_WINDOWS
#endif // SEAD_TARGET_DEBUG
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

void ThreadMgr::initHostIO()
{
#if defined(SEAD_TARGET_DEBUG)
    hostio::AddNode(HostIOMgr::instance()->getSeadRoot(), "ThreadMgr", this, "$SEAD_META_THREADMGR");
#endif // SEAD_TARGET_DEBUG
}

#if defined(SEAD_TARGET_DEBUG)
void ThreadMgr::listenPropertyEvent(const hostio::PropertyEvent* ev)
{
    SEAD_UNUSED(ev);
}

void ThreadMgr::genMessage(hostio::Context* context)
{
    context->genNode(mMainThread->getName(), mMainThread, "$SEAD_META_THREAD");

    ScopedLock<CriticalSection> lock(&mIterateLockCS);
    for (auto it = mList.begin(); it != mList.end(); ++it)
    {
        Thread* thread = *it;
        context->genNode(thread->getName(), thread, "$SEAD_META_THREAD");
    }
}
#endif // SEAD_TARGET_DEBUG

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
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
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
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
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
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
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
