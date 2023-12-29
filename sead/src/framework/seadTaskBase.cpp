#include <framework/seadTaskBase.h>

#include <framework/seadFramework.h>
#include <framework/seadMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <heap/seadHeap.h>
#include <prim/seadMemUtil.h>
#include <prim/seadScopedLock.h>

namespace sead {

TaskBase::CreateArg::CreateArg()
    : factory()
    , heap_policies()
    , parent(nullptr)
    , parameter(nullptr)
    , fader(nullptr)
    , src_task(nullptr)
    , created_task(nullptr)
    , create_callback(nullptr)
    , user_id()
    , tag(TaskBase::Tag::eApp)
    , instance_cb(nullptr)
{
}

TaskBase::CreateArg::CreateArg(const TaskClassID& classID)
    : factory(classID)
    , heap_policies()
    , parent(nullptr)
    , parameter(nullptr)
    , fader(nullptr)
    , src_task(nullptr)
    , created_task(nullptr)
    , create_callback(nullptr)
    , user_id()
    , tag(TaskBase::Tag::eApp)
    , instance_cb(nullptr)
{
}

TaskBase::TakeoverArg::TakeoverArg(TaskBase* src, const TaskClassID& dst, FaderTaskBase* fader)
    : CreateArg(dst)
{
    this->src_task = src;
    this->fader = fader;
}

TaskBase::TakeoverArg::TakeoverArg(const TaskClassID& dst, FaderTaskBase* fader)
    : CreateArg(dst)
{
    this->src_task = nullptr;
    this->fader = fader;
}

TaskBase::PushArg::PushArg(TaskBase* src, const TaskClassID& dst, FaderTaskBase* fader)
    : CreateArg(dst)
{
    this->src_task = src;
    this->parent = src;
    this->fader = fader;
}

TaskBase::PushArg::PushArg(const TaskClassID& dst, FaderTaskBase* fader)
    : CreateArg(dst)
{
    this->src_task = nullptr;
    this->parent = nullptr;
    this->fader = fader;
}

TaskBase::MgrTaskArg::MgrTaskArg(const TaskClassID& classID)
    : CreateArg(classID)
{
    this->heap_policies.setAdjustAll(true);
    this->heap_policies.useOnlyPrimaryHeap();
}

TaskBase::SystemMgrTaskArg::SystemMgrTaskArg(const TaskClassID& classID)
    : MgrTaskArg(classID)
{
    this->tag = Tag::eSystem;
}

TaskBase::TaskBase(const TaskConstructArg& arg)
    : TTreeNode(this)
    , IDisposer()
    , INamable()
    , mParameter(arg.param)
    , mInternalFlag()
    , mTaskListNode(this)
    , mHeapArray(*arg.heap_array)
    , mTaskMgr(arg.mgr)
    , mState(State::eCreated)
    , mTag(Tag::eApp)
    , mClassID()
{
    if (mParameter)
        SEAD_ASSERT(!MemUtil::isStack(mParameter));

    clearFlag_();

    setName("Task");
}

TaskBase::TaskBase(const TaskConstructArg& arg, const char* name)
    : TTreeNode(this)
    , IDisposer()
    , INamable()
    , mParameter(arg.param)
    , mInternalFlag()
    , mTaskListNode(this)
    , mHeapArray(*arg.heap_array)
    , mTaskMgr(arg.mgr)
    , mState(State::eCreated)
    , mTag(Tag::eApp)
    , mClassID()
{
    if (mParameter)
        SEAD_ASSERT(!MemUtil::isStack(mParameter));

    clearFlag_();

    setName(name);
}

void TaskBase::attachCalc()
{
    attachCalcImpl();
}

void TaskBase::attachDraw()
{
    attachDrawImpl();
}

void TaskBase::attachCalcDraw()
{
    attachCalcImpl();
    attachDrawImpl();
}

TaskBase::~TaskBase()
{
    if (getTaskMgr())
    {
        while (child())
        {
            mTaskMgr->destroyTaskSync(child()->val());
        }
    }

    mState = State::eDead;

    detachAll();
    mTaskListNode.erase();
}

void TaskBase::prepare()
{
}

void TaskBase::enterCommon()
{
    attachCalcDraw();

    pauseCalc(false);
    pauseDraw(false);

    enter();
}

void TaskBase::enter()
{
}

void TaskBase::exit()
{
}

void TaskBase::onEvent(const TaskEvent&)
{
}

bool TaskBase::isConnectable(TaskBase* child) const
{
    return getCorrespondingMethodTreeMgrTypeInfo()->isDerived(child->getCorrespondingMethodTreeMgrTypeInfo());
}

bool TaskBase::isDescendantOf(TaskBase* ancestor) const
{
    for (TaskBase* parent = getParentTask(); parent; parent = parent->getParentTask())
    {
        if (parent == ancestor)
            return true;
    }

    return false;
}

void TaskBase::attachMethodWithCheck(s32 methodType, MethodTreeNode* node)
{
    SEAD_ASSERT_MSG(getMethodTreeMgr()->getRuntimeTypeInfo()->isDerived(getCorrespondingMethodTreeMgrTypeInfo()), "illigal MethodTreeMgr Type.");

    getMethodTreeMgr()->attachMethod(methodType, node);
}

bool TaskBase::requestCreateTask(const CreateArg& arg)
{
    return mTaskMgr->requestCreateTask(arg);
}

TaskBase* TaskBase::createTaskSync(const CreateArg& arg)
{
    return mTaskMgr->createTaskSync(arg);
}

TaskBase* TaskBase::createChildTaskSync(CreateArg& arg)
{
    arg.parent = this;
    return mTaskMgr->createTaskSync(arg);
}

bool TaskBase::requestTakeover(const TakeoverArg& arg)
{
    TakeoverArg arg_ = arg;
    arg_.src_task = this;

    return mTaskMgr->requestTakeover(arg_);
}

bool TaskBase::requestTransition(TaskBase* next, FaderTaskBase* fader)
{
    return mTaskMgr->requestTransition(this, next, fader);
}

bool TaskBase::requestPush(const PushArg& arg)
{
    PushArg arg_ = arg;
    arg_.parent = this;
    arg_.src_task = this;

    return mTaskMgr->requestPush(arg_);
}

bool TaskBase::requestPop()
{
    return mTaskMgr->requestPop(this, nullptr);
}

TaskBase* TaskBase::pushSync(const PushArg& arg)
{
    PushArg arg_ = arg;
    arg_.parent = this;
    arg_.src_task = this;

    return mTaskMgr->pushSync(arg_);
}

void TaskBase::adjustHeap(s32 heapIndex)
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);
    adjustHeapWithSlackWithoutLock_(heapIndex, 0);
}

void TaskBase::adjustHeapAll()
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);
    for (s32 i = 0; i < HeapMgr::getRootHeapNum(); i++)
    {
        adjustHeapWithSlackWithoutLock_(i, 0);
    }
}

void TaskBase::adjustHeapWithSlack(s32 heapIndex, u32 slackSize)
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);
    adjustHeapWithSlackWithoutLock_(heapIndex, slackSize);
}

void TaskBase::adjustHeapWithSlackWithoutLock_(s32 heapIndex, u32 slackSize)
{
    Heap* heap = mHeapArray.getHeap(heapIndex);

    if (heap && !mHeapArray.mAdjusted[heapIndex])
    {
        mHeapArray.mAdjusted[heapIndex] = true;

        void* slackMemory = slackSize != 0 ? heap->alloc(slackSize) : nullptr;

        heap->adjust();

        if (slackMemory)
            heap->free(slackMemory);
    }
}

Framework* TaskBase::getFramework() const
{
    return getTaskMgr()->getFramework();
}

MethodTreeMgr* TaskBase::getMethodTreeMgr() const
{
    return getTaskMgr()->getFramework()->getMethodTreeMgr();
}

void TaskBase::doneDestroy()
{
    setFlag_(4);
}

} // namespace sead
