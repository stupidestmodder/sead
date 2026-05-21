#include <thread/seadEvent.h>

#include <basis/seadAssert.h>

#include <cerrno>
#include <ctime>

namespace sead {

Event::Event()
    : IDisposer()
    , mCond(PTHREAD_COND_INITIALIZER)
    , mMutexInner(PTHREAD_MUTEX_INITIALIZER)
    , mIsManualReset(false)
    , mIsSignal(false)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
}

Event::Event(Heap* disposerHeap)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mCond(PTHREAD_COND_INITIALIZER)
    , mMutexInner(PTHREAD_MUTEX_INITIALIZER)
    , mIsManualReset(false)
    , mIsSignal(false)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
}

Event::Event(bool manualReset)
    : IDisposer()
    , mCond(PTHREAD_COND_INITIALIZER)
    , mMutexInner(PTHREAD_MUTEX_INITIALIZER)
    , mIsManualReset(false)
    , mIsSignal(false)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
    initialize(manualReset);
}

Event::Event(Heap* disposerHeap, bool manualReset)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mCond(PTHREAD_COND_INITIALIZER)
    , mMutexInner(PTHREAD_MUTEX_INITIALIZER)
    , mIsManualReset(false)
    , mIsSignal(false)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
    initialize(manualReset);
}

Event::~Event()
{
#if defined(SEAD_TARGET_DEBUG)
    mIsInitialized = false;
#endif // SEAD_TARGET_DEBUG

    pthread_mutex_destroy(&mMutexInner);
    pthread_cond_destroy(&mCond);
}

void Event::initialize(bool manualReset)
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(!mIsInitialized, "Event is already initialized.");
#endif // SEAD_TARGET_DEBUG

    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutex_init(&mMutexInner, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    mIsManualReset = manualReset;

#if defined(SEAD_TARGET_DEBUG)
    mIsInitialized = true;
#endif // SEAD_TARGET_DEBUG
}

void Event::wait()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    pthread_mutex_lock(&mMutexInner);

    s32 ret = 0;
    while (!mIsSignal)
    {
        ret = pthread_cond_wait(&mCond, &mMutexInner);
    }

    if (ret == 0 && !mIsManualReset)
        mIsSignal = false;

    pthread_mutex_unlock(&mMutexInner);
}

bool Event::wait(TickSpan timeout)
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    struct ::timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);

    s64 nanosec = 1'000'000'000LL * ts.tv_sec + ts.tv_nsec + timeout.toNanoSeconds();
    ts.tv_sec = nanosec / 1'000'000'000LL;
    ts.tv_nsec = nanosec % 1'000'000'000LL;

    pthread_mutex_lock(&mMutexInner);

    s32 ret = 0;
    while (!mIsSignal)
    {
        ret = pthread_cond_timedwait(&mCond, &mMutexInner, &ts);
        if (ret == ETIMEDOUT) //? Nintendo incorreclty checks for EHOSTUNREACH instead
        {
            break;
        }
    }

    if (ret == 0 && !mIsManualReset)
        mIsSignal = false;

    pthread_mutex_unlock(&mMutexInner);

    return ret == 0;
}

void Event::setSignal()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    pthread_mutex_lock(&mMutexInner);

    mIsSignal = true;

    if (mIsManualReset)
        pthread_cond_broadcast(&mCond);
    else
        pthread_cond_signal(&mCond);

    pthread_mutex_unlock(&mMutexInner);
}

void Event::resetSignal()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    pthread_mutex_lock(&mMutexInner);
    mIsSignal = false;
    pthread_mutex_unlock(&mMutexInner);
}

} // namespace sead
