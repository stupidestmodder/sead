#pragma once

#include <container/seadTList.h>
#include <container/seadTreeNode.h>
#include <framework/seadHeapPolicies.h>
#include <framework/seadTaskID.h>
#include <heap/seadDisposer.h>
#include <prim/seadBitFlag.h>
#include <prim/seadDelegateEventSlot.h>
#include <prim/seadNamable.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class FaderTaskBase;
class Framework;
class MethodTreeMgr;
class MethodTreeNode;
class TaskEvent;
class TaskMgr;
class TaskParameter;

class TaskBase : public TTreeNode<TaskBase*>, public IDisposer, public INamable
{
    SEAD_RTTI_BASE(TaskBase);

public:
    enum class State
    {
        eCreated = 0,
        ePrepare,
        ePrepareDone,
        eSleep,
        eRunning,
        eDying,
        eDestroyable,
        eDead
    };

    enum class Tag
    {
        eSystem = 0,
        eApp    = 1
    };

    struct CreateArg
    {
        using SingletonFunc = void (*)(TaskBase*);

        CreateArg();
        explicit CreateArg(const TaskClassID& classID);

        TaskClassID factory;
        HeapPolicies heap_policies;
        TaskBase* parent;
        TaskParameter* parameter;
        FaderTaskBase* fader;
        TaskBase* src_task;
        TaskBase** created_task;
        DelegateEvent<TaskBase*>::Slot* create_callback;
        TaskUserID user_id;
        Tag tag;
        SingletonFunc instance_cb;
    };

    struct TakeoverArg : public CreateArg
    {
        TakeoverArg(TaskBase* src, const TaskClassID& dst, FaderTaskBase* fader);
        TakeoverArg(const TaskClassID& dst, FaderTaskBase* fader);
    };

    struct PushArg : public CreateArg
    {
        PushArg(TaskBase* src, const TaskClassID& dst, FaderTaskBase* fader);
        PushArg(const TaskClassID& dst, FaderTaskBase* fader);
    };

    struct MgrTaskArg : public CreateArg
    {
        explicit MgrTaskArg(const TaskClassID& classID);
    };

    struct SystemMgrTaskArg : public MgrTaskArg
    {
        explicit SystemMgrTaskArg(const TaskClassID& classID);
    };

    using CArg = TaskConstructArg;

    using ListNode = TListNode<TaskBase*>;
    using List = TList<TaskBase*>;

public:
    explicit TaskBase(const TaskConstructArg& arg);
    TaskBase(const TaskConstructArg& arg, const char* name);

    virtual void pauseCalc(bool b) = 0;
    virtual void pauseDraw(bool b) = 0;

    virtual void pauseCalcRec(bool b) = 0;
    virtual void pauseDrawRec(bool b) = 0;

    virtual void pauseCalcChild(bool b) { SEAD_UNUSED(b); }
    virtual void pauseDrawChild(bool b) { SEAD_UNUSED(b); }

    void attachCalc();
    void attachDraw();
    void attachCalcDraw();

    void detachCalc()
    {
        detachCalcImpl();
    }

    void detachDraw()
    {
        detachDrawImpl();
    }

    void detachCalcDraw()
    {
        detachCalc();
        detachDraw();
    }

    ~TaskBase() override;

    virtual void prepare();
    virtual void enterCommon();
    virtual void enter();
    virtual void exit();
    virtual void onEvent(const TaskEvent& event);

protected:
    virtual void attachCalcImpl() = 0;
    virtual void attachDrawImpl() = 0;
    virtual void detachCalcImpl() = 0;
    virtual void detachDrawImpl() = 0;

public:
    bool isConnectable(TaskBase* child) const;
    bool isDescendantOf(TaskBase* ancestor) const;

    void attachMethodWithCheck(s32 methodType, MethodTreeNode* node);

    bool requestCreateTask(const CreateArg& arg);
    TaskBase* createTaskSync(const CreateArg& arg);
    TaskBase* createChildTaskSync(CreateArg& arg);

    bool requestTakeover(const TakeoverArg& arg);
    bool requestTransition(TaskBase* next, FaderTaskBase* fader);

    bool requestPush(const PushArg& arg);
    bool requestPop();
    TaskBase* pushSync(const PushArg& arg);

    void adjustHeap(s32 heapIndex);
    void adjustHeapAll();
    void adjustHeapWithSlack(s32 heapIndex, u32 slackSize);

    TaskBase* getParentTask() const
    {
        return parent() ? parent()->val() : nullptr;
    }

    const HeapArray& getHeapArray() const
    {
        return mHeapArray;
    }

    TaskMgr* getTaskMgr() const
    {
        return mTaskMgr;
    }

    TaskParameter* getParameter()
    {
        return mParameter;
    }

    Tag getTag() const
    {
        return mTag;
    }

    Framework* getFramework() const;
    MethodTreeMgr* getMethodTreeMgr() const;

    virtual const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const = 0;
    virtual MethodTreeNode* getMethodTreeNode(s32 methodType) = 0;

    virtual void onDestroy()
    {
        doneDestroy();
    }

    void doneDestroy();

protected:
    void adjustHeapWithSlackWithoutLock_(s32 heapIndex, u32 slackSize);

    void setFlag_(u32 f)
    {
        mInternalFlag.set(f);
    }

    void resetFlag_(u32 f)
    {
        mInternalFlag.reset(f);
    }

    void clearFlag_()
    {
        mInternalFlag.makeAllZero();
    }

    bool checkFlag_(u32 f) const
    {
        return mInternalFlag.isOn(f);
    }

    friend class FaderTaskBase;

protected:
    TaskParameter* mParameter;
    BitFlag32 mInternalFlag;
    ListNode mTaskListNode;
    HeapArray mHeapArray;
    TaskMgr* mTaskMgr;
    State mState;
    Tag mTag;
    TaskClassID mClassID;

    friend class TaskMgr;
};

} // namespace sead
