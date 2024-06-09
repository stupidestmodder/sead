#include <thread/seadEvent.h>

#include <basis/seadAssert.h>

namespace sead {

Event::Event()
    : IDisposer()
    , mEventInner(nullptr)
#ifdef SEAD_DEBUG
    , mIsInitialized(false)
#endif // SEAD_DEBUG
{
}

Event::Event(Heap* disposerHeap)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mEventInner(nullptr)
#ifdef SEAD_DEBUG
    , mIsInitialized(false)
#endif // SEAD_DEBUG
{
}

Event::Event(bool manualReset)
    : IDisposer()
    , mEventInner(nullptr)
#ifdef SEAD_DEBUG
    , mIsInitialized(false)
#endif // SEAD_DEBUG
{
    initialize(manualReset);
}

Event::Event(Heap* disposerHeap, bool manualReset)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mEventInner(nullptr)
#ifdef SEAD_DEBUG
    , mIsInitialized(false)
#endif // SEAD_DEBUG
{
    initialize(manualReset);
}

Event::~Event()
{
#ifdef SEAD_DEBUG
    mIsInitialized = false;
#endif // SEAD_DEBUG

    bool success = CloseHandle(mEventInner);
    SEAD_ASSERT_MSG(success, "CloseHandle failed. %d", GetLastError());
}

void Event::initialize(bool manualReset)
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(!mIsInitialized, "Event is already initialized.");
#endif // SEAD_DEBUG

    mEventInner = CreateEventA(nullptr, manualReset, false, nullptr);
    SEAD_ASSERT_MSG(mEventInner, "CreateEvent failed. %d", GetLastError());

#ifdef SEAD_DEBUG
    mIsInitialized = true;
#endif // SEAD_DEBUG
}

void Event::wait()
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_DEBUG

    WaitForSingleObject(mEventInner, INFINITE);
}

bool Event::wait(TickSpan timeout)
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_DEBUG

    return WaitForSingleObject(mEventInner, static_cast<DWORD>(timeout.toMilliSeconds())) == WAIT_OBJECT_0;
}

void Event::setSignal()
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_DEBUG

    bool success = SetEvent(mEventInner);
    SEAD_ASSERT_MSG(success, "SetEvent failed. %d", GetLastError());
}

void Event::resetSignal()
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(mIsInitialized, "Event is not initialized.");
#endif // SEAD_DEBUG

    bool success = ResetEvent(mEventInner);
    SEAD_ASSERT_MSG(success, "ResetEvent failed. %d", GetLastError());
}

} // namespace sead
