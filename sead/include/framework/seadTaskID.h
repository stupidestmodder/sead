#pragma once

#include <basis/seadTypes.h>
#include <framework/seadHeapPolicies.h>
#include <prim/seadSafeString.h>

namespace sead {

class HeapArray;
class TaskBase;
class TaskMgr;
class TaskParameter;

struct TaskConstructArg
{
    TaskConstructArg()
        : heap_array(nullptr)
        , mgr(nullptr)
        , param(nullptr)
    {
    }

    TaskConstructArg(HeapArray* heapArray_, TaskMgr* mgr_, TaskParameter* param_)
        : heap_array(heapArray_)
        , mgr(mgr_)
        , param(param_)
    {
    }

    HeapArray* heap_array;
    TaskMgr* mgr;
    TaskParameter* param;
};

using TaskFactory = TaskBase* (*)(const TaskConstructArg& arg);

template <typename T>
TaskBase* TTaskFactory(const TaskConstructArg& arg)
{
    return new(arg.heap_array->getPrimaryHeap()) T(arg);
}

class TaskClassID
{
public:
    enum class Type
    {
        eInvalid = 0,
        eInt,
        eFactory,
        eString
    };

    using IntTaskCreator = TaskBase* (*)(s32, const TaskConstructArg&);
    using StringTaskCreator = TaskBase* (*)(const char*, const TaskConstructArg&);

public:
    TaskClassID()
        : mType(Type::eInvalid)
    {
        //* Yeahhh...
        *reinterpret_cast<uintptr_t*>(&mID) = 0;
    }

    TaskClassID(s32 i)
        : mType(Type::eInt)
    {
        mID.mInt = i;
    }

    TaskClassID(TaskFactory f)
        : mType(Type::eFactory)
    {
        mID.mFactory = f;
    }

    TaskClassID(const char* s)
        : mType(Type::eString)
    {
        mID.mString = s;
    }

    TaskBase* create(const TaskConstructArg& arg) const;

    //? Unofficial name
    static void setIntTaskCreator(IntTaskCreator creator)
    {
        sIntTaskCreator = creator;
    }

    //? Unofficial name
    static void setStringTaskCreator(StringTaskCreator creator)
    {
        sStringTaskCreator = creator;
    }

    friend bool operator==(const TaskClassID& a, const TaskClassID& b)
    {
        if (a.mType != b.mType)
            return false;

        switch (a.mType)
        {
            case TaskClassID::Type::eInt:
                return a.mID.mInt == b.mID.mInt;

            case TaskClassID::Type::eFactory:
                return a.mID.mFactory == b.mID.mFactory;

            case TaskClassID::Type::eString:
                return SafeString(a.mID.mString) == SafeString(b.mID.mString);

            default:
                SEAD_ASSERT_MSG(false, "UNKNOWN TYPE %d\n", a.mType);
        }

        return false;
    }

private:
    static IntTaskCreator sIntTaskCreator;
    static StringTaskCreator sStringTaskCreator;

private:
    Type mType;
    union
    {
        s32 mInt;
        TaskFactory mFactory;
        const char* mString;
    } mID;
};

// TODO
class TaskUserID
{
public:
    TaskUserID()
        : mID(-1)
    {
    }

private:
    s32 mID;
};

} // namespace sead
