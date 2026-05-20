#pragma once

#include <basis/seadTypes.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

#if defined(SEAD_PLATFORM_SDL)
#include <basis/sdl/seadSDL.h>
#endif // SEAD_PLATFORM_SDL

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
#if defined(SEAD_PLATFORM_WINDOWS)
        MemoryBarrier();
#elif defined(SEAD_PLATFORM_SDL)
    // Acts as a memory barrier
    static SDL_AtomicInt dummy = {0};
    SDL_AddAtomicInt(&dummy, 0);
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
    }
};

} // namespace sead
