#include <framework/seadTaskID.h>

#include <basis/seadAssert.h>

namespace sead {

TaskClassID::IntTaskCreator TaskClassID::sIntTaskCreator = nullptr;
TaskClassID::StringTaskCreator TaskClassID::sStringTaskCreator = nullptr;

TaskBase* TaskClassID::create(const TaskConstructArg& arg) const
{
    TaskBase* task = nullptr;

    switch (mType)
    {
        case Type::eInt:
            SEAD_ASSERT(sIntTaskCreator);
            if (sIntTaskCreator)
                task = sIntTaskCreator(mID.mInt, arg);
            break;

        case Type::eFactory:
            task = mID.mFactory(arg);
            break;

        case Type::eString:
            SEAD_ASSERT(sStringTaskCreator);
            if (sStringTaskCreator)
                task = sStringTaskCreator(mID.mString, arg);
            break;

        default:
            SEAD_ASSERT_MSG(false, "unknown Type %d\n", mType);
    }

    return task;
}

} // namespace sead
