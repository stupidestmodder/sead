#pragma once

#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class TaskEvent
{
    SEAD_RTTI_BASE(TaskEvent);

public:
    enum Type
    {
        ePopped = 0,
        eTransitOut,
        eTransitIn,
        eTakeoverOut,
        eTakeoverIn,
        eCalcPaused,
        eCalcResumed,
        eDrawPaused,
        eDrawResumed,
        eOther,
        eNone
    };

public:
    TaskEvent(Type type)
        : mType(type)
    {
    }

protected:
    Type mType;
};

} // namespace sead
