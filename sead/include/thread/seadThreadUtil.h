#pragma once

#include <basis/seadTypes.h>

#ifdef SEAD_PLATFORM_WINDOWS
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

namespace sead {

// TODO
class ThreadUtil
{
public:
    static s32 ConvertPrioritySeadToPlatform(s32 prio);
    static s32 ConvertPriorityPlatformToSead(s32 prio);
    static uintptr_t GetCurrentStackPointer();

    static void PlaceMemoryBarrier()
    {
#ifdef SEAD_PLATFORM_WINDOWS
        MemoryBarrier();
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
    }
};

} // namespace sead
