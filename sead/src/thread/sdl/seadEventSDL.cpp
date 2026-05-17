#include <thread/seadEvent.h>

#include <basis/sdl/seadSDL.h>
#include <basis/seadAssert.h>

namespace sead {

Event::Event()
    : IDisposer()
    , mCond(nullptr)
    , mMutex(nullptr)
    , mManualReset(false)
    , mIsSignal(false)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
}

Event::Event(Heap* disposerHeap)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mCond(nullptr)
    , mMutex(nullptr)
    , mManualReset(false)
    , mIsSignal(false)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
}

Event::Event(bool manualReset)
    : IDisposer()
    , mCond(nullptr)
    , mMutex(nullptr)
    , mManualReset(false)
    , mIsSignal(false)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
    initialize(manualReset);
}

Event::Event(Heap* disposerHeap, bool manualReset)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mCond(nullptr)
    , mMutex(nullptr)
    , mManualReset(false)
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

    SDL_DestroyMutex(mMutex);
    SDL_DestroyCondition(mCond);
}

void Event::initialize(bool manualReset)
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(!mIsInitialized, "Event is already initialized.");
#endif // SEAD_TARGET_DEBUG

    mCond = SDL_CreateCondition();
    SEAD_ASSERT_MSG(mCond, "SDL_CreateCondition failed. %s", SDL_GetError());

    mMutex = SDL_CreateMutex();
    SEAD_ASSERT_MSG(mMutex, "SDL_CreateMutex failed. %s", SDL_GetError());

    mManualReset = manualReset;
    mIsSignal = false;

#if defined(SEAD_TARGET_DEBUG)
    mIsInitialized = true;
#endif // SEAD_TARGET_DEBUG
}

void Event::wait()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    SDL_LockMutex(mMutex);

    u32 ret = 0;
    while (!mIsSignal)
    {
       // SDL_WaitCondition(mCond, mMutex);
        ret = SDL_WaitConditionTimeout(mCond, mMutex, -1);
    }

    if (ret == 1 && !mManualReset)
        mIsSignal = false;

    SDL_UnlockMutex(mMutex);
}

bool Event::wait(TickSpan timeout)
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG
    SDL_LockMutex(mMutex);

    bool ret = true;

    while (!mIsSignal && ret)
        ret = SDL_WaitConditionTimeout(mCond, mMutex, timeout.toMilliSeconds());

    if (mIsSignal && ret && !mManualReset)
        mIsSignal = false;

    SDL_UnlockMutex(mMutex);

    return ret;
}

void Event::setSignal()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    SDL_LockMutex(mMutex);

    mIsSignal = true;

    if (!mManualReset)
        SDL_SignalCondition(mCond);
    else
        SDL_BroadcastCondition(mCond);

    SDL_UnlockMutex(mMutex);
}

void Event::resetSignal()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    SDL_LockMutex(mMutex);

    mIsSignal = false;

    SDL_UnlockMutex(mMutex);
}

} // namespace sead
