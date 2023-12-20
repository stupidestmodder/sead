#pragma once

#include <framework/seadTaskBase.h>
#include <gfx/seadFrameBuffer.h>
#include <prim/seadDelegateEventSlot.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <time/seadTickSpan.h>

namespace sead {

class Arena;
class Heap;
class MethodTreeMgr;
class TaskMgr;

class Framework
{
    SEAD_RTTI_BASE(Framework);

public:
    struct CreateSystemTaskArg
    {
        CreateSystemTaskArg();

        TickSpan infloop_detection_span;
    };

    struct InitializeArg
    {
        InitializeArg();

        u32 heap_size;
        Arena* arena;
    };

    struct RunArg
    {
        RunArg();

        u32 prepare_stack_size;
        s32 prepare_priority;
    };

    enum class ProcessPriority
    {
        eIdle = 0,
        eNormal,
        eHigh,
        eRealTime
    };

public:
    Framework();
    virtual ~Framework();

    virtual void run(Heap* heap, const TaskBase::CreateArg& rootCreateArg, const RunArg& runArg);
    virtual void createSystemTasks(TaskBase*, const CreateSystemTaskArg&);
    virtual FrameBuffer* getMethodFrameBuffer(s32 methodType) const = 0;
    virtual LogicalFrameBuffer* getMethodLogicalFrameBuffer(s32 methodType) const { return getMethodFrameBuffer(methodType); }
    virtual bool setProcessPriority(ProcessPriority) { return false; }

    virtual void reserveReset(void* param)
    {
        mResetParameter = param;
        mReserveReset = true;
    }

protected:
    virtual void initRun_(Heap*) { }
    virtual void quitRun_(Heap*) { }
    virtual void runImpl_() { }
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap* heap) = 0;
    virtual void procReset_();

public:
    TaskMgr* getTaskMgr()
    {
        return mTaskMgr;
    }

    MethodTreeMgr* getMethodTreeMgr()
    {
        return mMethodTreeMgr;
    }

    static void initialize(const InitializeArg& arg);

protected:
    using ResetEvent = DelegateEvent<void*>;

    bool mReserveReset;
    void* mResetParameter;
    ResetEvent mResetEvent;
    TaskMgr* mTaskMgr;
    MethodTreeMgr* mMethodTreeMgr;
    Heap* mMethodTreeMgrHeap;
};

} // namespace sead
