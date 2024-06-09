#include <thread/seadThreadUtil.h>

#include <basis/seadAssert.h>

namespace sead {

// TODO
s32 ThreadUtil::ConvertPrioritySeadToPlatform(s32 prio)
{
    SEAD_ASSERT(prio >= 0);
    SEAD_ASSERT(prio < 32);
    //return prio;
    return THREAD_PRIORITY_TIME_CRITICAL;
}

} // namespace sead
