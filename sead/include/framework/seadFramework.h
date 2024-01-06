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
        CreateSystemTaskArg()
            : infloop_detection_span(0)
        {
        }

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

    using ResetEvent = DelegateEvent<void*>;

public:
    Framework();
    virtual ~Framework();

    static void initialize(const InitializeArg& arg);

    virtual void run(Heap* heap, const TaskBase::CreateArg& rootCreateArg, const RunArg& runArg);
    virtual void createSystemTasks(TaskBase* rootTask, const CreateSystemTaskArg& arg);
    virtual FrameBuffer* getMethodFrameBuffer(s32 methodType) const = 0;
    virtual const LogicalFrameBuffer* getMethodLogicalFrameBuffer(s32 methodType) const { return getMethodFrameBuffer(methodType); }

    virtual bool setProcessPriority(ProcessPriority priority)
    {
        SEAD_UNUSED(priority);
        return false;
    }

    virtual void reserveReset(void* param)
    {
        mReserveReset = true;
        mResetParameter = param;
    }

    TaskMgr* getTaskMgr()
    {
        return mTaskMgr;
    }

    MethodTreeMgr* getMethodTreeMgr()
    {
        return mMethodTreeMgr;
    }

    //? Unofficial name but probably exists
    void registerResetEvent(ResetEvent::Slot& slot)
    {
        mResetEvent.connect(slot);
    }

    //? Unofficial name but probably exists
    void unregisterResetEvent(ResetEvent::Slot& slot)
    {
        mResetEvent.disconnect(slot);
    }

protected:
    virtual void initRun_(Heap* heap) { SEAD_UNUSED(heap); }
    virtual void quitRun_(Heap* heap) { SEAD_UNUSED(heap); }
    virtual void runImpl_() { }
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap* heap) = 0;
    virtual void procReset_();

protected:
    bool mReserveReset;
    void* mResetParameter;
    ResetEvent mResetEvent;
    TaskMgr* mTaskMgr;
    MethodTreeMgr* mMethodTreeMgr;
    Heap* mMethodTreeMgrHeap;
};

} // namespace sead
