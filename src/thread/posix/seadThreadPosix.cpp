#include <thread/seadThread.h>

#include <basis/seadWarning.h>
#include <heap/seadHeap.h>
#include <math/seadMathCalcCommon.h>

#include <unistd.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>

namespace sead {

const s32 cGuardSize = 0x1000;

const s32 Thread::cDefaultPriority = 0;

SEAD_SINGLETON_DISPOSER_IMPL(ThreadMgr);

Thread::Thread(const SafeString& name, Heap* heap, s32 platformPriority, MessageQueue::BlockType blockType,
               MessageQueue::Element quitMsg, s32 stackSize, s32 msgQueueSize)
    : IDisposer()
    , INamable(name)
    , mMessageQueue()
    , mStackSize(Mathi::max(stackSize, PTHREAD_STACK_MIN))
    , mListNode(this)
    , mCurrentHeap(nullptr)
    , mFindContainHeapCache()
    , mBlockType(blockType)
    , mQuitMsg(quitMsg)
    , mID(0)
    , mState(State::eInitialized)
    , mHandle(0)
    , mAttr()
    , mStackBase(0)
    , mStackTop(nullptr)
    , mPriority(platformPriority)
    , mNameBuffer()
{
    mNameBuffer.cutOffCopy(getName());
    mMessageQueue.allocate(msgQueueSize, heap);

    pthread_attr_init(&mAttr);

    mStackSize = Mathi::roundUpPow2(mStackSize, cGuardSize);
    mStackTop = static_cast<u8*>(heap->alloc(mStackSize + cGuardSize, cGuardSize));

    mprotect(mStackTop, cGuardSize, PROT_NONE);
    pthread_attr_setstack(&mAttr, mStackTop + cGuardSize, mStackSize);

    if (ThreadMgr::instance())
        ThreadMgr::instance()->addThread_(this);
    else
        SEAD_ASSERT_MSG(false, "ThreadMgr not initialized");
}

Thread::Thread(Heap* heap, pthread_t handle)
    : IDisposer()
    , INamable("sead::MainThread")
    , mMessageQueue()
    , mStackSize(0)
    , mListNode(this)
    , mCurrentHeap(nullptr)
    , mFindContainHeapCache()
    , mBlockType(MessageQueue::BlockType::eNoBlock)
    , mQuitMsg(cDefaultQuitMsg)
    , mID(static_cast<u32>(handle))
    , mState(State::eRunning)
    , mHandle(handle)
    , mAttr()
    , mStackBase(0)
    , mStackTop(nullptr)
    , mPriority(0)
    , mNameBuffer()
{
    mNameBuffer.cutOffCopy(getName());
    mMessageQueue.allocate(cDefaultMsgQueueSize, heap);

    pthread_attr_init(&mAttr);
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

        pthread_attr_destroy(&mAttr);
        mprotect(mStackTop, mStackSize + cGuardSize, PROT_READ | PROT_WRITE);

        delete[] mStackTop;
    }

    mMessageQueue.free();
}

bool Thread::start()
{
    if (mState != State::eInitialized)
    {
        SEAD_WARNING("Thread is running or done. Can not start.\n");
        return false;
    }

    if (pthread_create(&mHandle, &mAttr, &posixThreadFunc_, this) != 0)
    {
        SEAD_WARNING("Failed to create thread.\n");
        return false;
    }

    mID = static_cast<u32>(mHandle);
    pthread_setname_np(mHandle, mNameBuffer.cstr());

    if (mState == State::eInitialized)
        mState = State::eRunning;

    return true;
}

void Thread::waitDone()
{
    if (mState == State::eInitialized || mState == State::eReleased)
        return;

    pthread_join(mHandle, nullptr);

    Thread::sleep(TickSpan::makeFromNanoSeconds(10));

    SEAD_ASSERT_MSG(mState == State::eTerminated, "Join failed?");
    mState = State::eReleased;
}

void Thread::sleep(TickSpan span)
{
    struct ::timespec ts;

    s64 nanosec = span.toNanoSeconds();
    ts.tv_sec = nanosec / 1'000'000'000LL;
    ts.tv_nsec = nanosec % 1'000'000'000LL;

    nanosleep(&ts, nullptr);
}

void Thread::yield()
{
    sched_yield();
}

void Thread::setPriority(s32 platformPriority)
{
    mPriority = platformPriority;
}

s32 Thread::getPriority() const
{
    return mPriority;
}

u32* Thread::getStackCheckStartAddress_() const
{
    return nullptr;
}

void* Thread::posixThreadFunc_(void* param)
{
    Thread* thread = static_cast<Thread*>(param);
    ThreadMgr::instance()->mThreadPtrTLS.setValue(reinterpret_cast<uintptr_t>(thread));

    setpriority(PRIO_PROCESS, gettid(), thread->mPriority);

    thread->mState = State::eRunning;
    thread->run_();
    thread->mState = State::eTerminated;

    pthread_exit(nullptr);
    return nullptr;
}

void ThreadMgr::initMainThread_(Heap* heap)
{
    mMainThread = new(heap) MainThread(heap, pthread_self());
    mThreadPtrTLS.setValue(reinterpret_cast<uintptr_t>(mMainThread));
}

u32 ThreadMgr::getCurrentThreadID_()
{
    return static_cast<u32>(pthread_self());
}

} // namespace sead
