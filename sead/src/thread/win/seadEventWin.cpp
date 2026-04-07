#include <thread/seadEvent.h>

#include <basis/seadAssert.h>

namespace sead {

Event::Event()
    : IDisposer()
    , mEventInner(nullptr)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
}

Event::Event(Heap* disposerHeap)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mEventInner(nullptr)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
}

Event::Event(bool manualReset)
    : IDisposer()
    , mEventInner(nullptr)
#if defined(SEAD_TARGET_DEBUG)
    , mIsInitialized(false)
#endif // SEAD_TARGET_DEBUG
{
    initialize(manualReset);
}

Event::Event(Heap* disposerHeap, bool manualReset)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mEventInner(nullptr)
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

    bool success = CloseHandle(mEventInner);
    SEAD_ASSERT_MSG(success, "CloseHandle failed. %d", GetLastError());
}

void Event::initialize(bool manualReset)
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(!mIsInitialized, "Event is already initialized.");
#endif // SEAD_TARGET_DEBUG

    mEventInner = CreateEventA(nullptr, manualReset, false, nullptr);
    SEAD_ASSERT_MSG(mEventInner, "CreateEvent failed. %d", GetLastError());

#if defined(SEAD_TARGET_DEBUG)
    mIsInitialized = true;
#endif // SEAD_TARGET_DEBUG
}

void Event::wait()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    WaitForSingleObject(mEventInner, INFINITE);
}

bool Event::wait(TickSpan timeout)
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    return WaitForSingleObject(mEventInner, static_cast<DWORD>(timeout.toMilliSeconds())) == WAIT_OBJECT_0;
}

void Event::setSignal()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    bool success = SetEvent(mEventInner);
    SEAD_ASSERT_MSG(success, "SetEvent failed. %d", GetLastError());
}

void Event::resetSignal()
{
#if defined(SEAD_TARGET_DEBUG)
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_TARGET_DEBUG

    bool success = ResetEvent(mEventInner);
    SEAD_ASSERT_MSG(success, "ResetEvent failed. %d", GetLastError());
}

} // namespace sead
