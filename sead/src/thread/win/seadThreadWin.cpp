#include <thread/seadThread.h>

#include <basis/seadWarning.h>

#include <process.h>
#include <timeapi.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(ThreadMgr);

Thread::Thread(const SafeString& name, Heap* heap, s32 platformPriority, MessageQueue::BlockType blockType,
               MessageQueue::Element quitMsg, s32 stackSize, s32 msgQueueSize)
    : IDisposer()
    , INamable(name)
    , mMessageQueue()
    , mStackSize(stackSize)
    , mListNode(this)
    , mCurrentHeap(nullptr)
    , mFindContainHeapCache()
    , mBlockType(blockType)
    , mQuitMsg(quitMsg)
    , mID(0)
    , mState(State::eInitialized)
    , mHandle(0)
{
    mMessageQueue.allocate(msgQueueSize, heap);

    mHandle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, mStackSize, &Thread::winThreadFunc_, this, CREATE_SUSPENDED, &mID));
    if (!mHandle)
    {
        SEAD_ASSERT_MSG(false, "_beginthreadex failed. %d", GetLastError());
        return;
    }

    bool success = SetThreadPriority(mHandle, platformPriority);
    SEAD_ASSERT_MSG(success, "SetThreadPriority failed. %d", GetLastError());

    if (ThreadMgr::instance())
        ThreadMgr::instance()->addThread_(this);
    else
        SEAD_ASSERT_MSG(false, "ThreadMgr not initialized");
}

Thread::Thread(Heap* heap, HANDLE handle, u32 id)
    : IDisposer()
    , INamable("sead::MainThread")
    , mMessageQueue()
    , mStackSize(0)
    , mListNode(this)
    , mCurrentHeap(nullptr)
    , mFindContainHeapCache()
    , mBlockType(MessageQueue::BlockType::eNoBlock)
    , mQuitMsg(0x7FFFFFFF)
    , mID(id)
    , mState(State::eRunning)
    , mHandle(handle)
{
    mMessageQueue.allocate(32, heap);
}

Thread::~Thread()
{
    if (!ThreadMgr::instance())
    {
        SEAD_ASSERT_MSG(false, "ThreadMgr not initialized");
        return;
    }

    if (this != ThreadMgr::instance()->getMainThread())
    {
        ThreadMgr::instance()->removeThread_(this);

        if (mState == State::eRunning)
        {
            SEAD_ASSERT_MSG(false, "Thread is runnning. Do quit and waitDone");
            quitAndWaitDoneSingleThread(false);
        }
        else if (mState == State::eQuitting || mState == State::eTerminated)
        {
            SEAD_ASSERT_MSG(false, "Thread is not done. Do waitDone");
            waitDone();
        }

        CloseHandle(mHandle);
    }

    mMessageQueue.free();
}

bool Thread::start()
{
    if (mState != State::eInitialized)
    {
        SEAD_WARNING("Thread is already started or done. Can not start.");
        return false;
    }

    DWORD ret = ResumeThread(mHandle);

    if (mState == State::eInitialized)
        mState = State::eRunning;

    if (ret == 0 || ret == 1)
        return true;

    return false;
}

void Thread::waitDone()
{
    if (mState == State::eInitialized || mState == State::eReleased)
        return;

    WaitForSingleObject(mHandle, INFINITE);
    mState = State::eReleased;
}

void Thread::sleep(TickSpan span)
{
    s64 ms = span.toMilliSeconds();
    SEAD_ASSERT((ms & 0xFFFFFFFF00000000) == 0);

    timeBeginPeriod(1);
    Sleep(static_cast<DWORD>(ms));
    timeEndPeriod(1);
}

void Thread::yield()
{
    SwitchToThread();
}

void Thread::setPriority(s32 platformPriority)
{
    SetThreadPriority(mHandle, platformPriority);
}

s32 Thread::getPriority() const
{
    return GetThreadPriority(mHandle);
}

} // namespace sead
