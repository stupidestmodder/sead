#pragma once

#include <heap/seadDisposer.h>
#include <time/seadTickSpan.h>

#ifdef SEAD_PLATFORM_WINDOWS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // SEAD_PLATFORM_WINDOWS

namespace sead {

class Event : public IDisposer
{
    SEAD_NO_COPY(Event);

public:
    Event();
    explicit Event(Heap* disposerHeap);
    explicit Event(bool manualReset);
    Event(Heap* disposerHeap, bool manualReset);

    ~Event() override;

    void initialize(bool manualReset);
    void wait();
    void wait(TickSpan timeout);
    void setSignal();
    void resetSignal();

protected:
#ifdef SEAD_PLATFORM_WINDOWS
    HANDLE mEventInner;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

#ifdef SEAD_DEBUG
    bool mIsInitialized;
#endif // SEAD_DEBUG
};

} // namespace sead
