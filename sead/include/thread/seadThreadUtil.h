#pragma once

#include <basis/seadTypes.h>

namespace sead {

// TODO
class ThreadUtil
{
public:
    static s32 ConvertPrioritySeadToPlatform(s32 prio);
    static s32 ConvertPriorityPlatformToSead(s32 prio);
    static uintptr_t GetCurrentStackPointer();
    static void PlaceMemoryBarrier();
};

} // namespace sead