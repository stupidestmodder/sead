#include <thread/seadThreadUtil.h>

#include <basis/seadAssert.h>

namespace sead {

// TODO
s32 ThreadUtil::ConvertPrioritySeadToPlatform(s32 prio)
{
    SEAD_ASSERT(prio >= 0);
    SEAD_ASSERT(prio < 32);
    //return prio;
    #if defined(SEAD_PLATFORM_SDL)
    return SDL_THREAD_PRIORITY_TIME_CRITICAL;

    #elif defined(SEAD_PLATFORM_WINDOWS)
    return THREAD_PRIORITY_TIME_CRITICAL;
    #else
        #error "Unsupported platform"
    #endif
}

} // namespace sead
