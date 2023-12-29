#include <framework/seadTaskMgr.h>

#include <basis/seadWarning.h>
#include <framework/seadFaderTask.h>
#include <framework/seadFramework.h>
#include <framework/seadMethodTreeMgr.h>
#include <framework/seadTaskEvent.h>
#include <heap/seadExpHeap.h>
#include <prim/seadScopedLock.h>
#include <thread/seadDelegateThread.h>

namespace sead {

TaskCreateContext::TaskCreateContext()
    : mCreatedTask(nullptr)
    , mArg()
    , mEventOnCreate()
{
}

TaskMgr::TaskCreateContextMgr::TaskCreateContextMgr(s32 size, Heap* heap)
    : ObjList()
{
    allocBuffer(size, heap);
}

bool TaskMgr::TaskCreateContextMgr::isCreateEmpty() const
{
    return isEmpty();
}

TaskCreateContext* TaskMgr::TaskCreateContextMgr::birthForCreate()
{
    TaskCreateContext* cc = birthBack();
    return cc;
}

TaskCreateContext* TaskMgr::TaskCreateContextMgr::getCreateFront()
{
    TaskCreateContext* cc = front();
    return cc;
}

TaskCreateContext* TaskMgr::TaskCreateContextMgr::find(TaskBase* task)
{
    for (TaskCreateContext* cc = front(); cc; cc = next(cc))
    {
        if (cc->mCreatedTask == task)
            return cc;
    }

    return nullptr;
}

void TaskMgr::TaskCreateContextMgr::kill(TaskCreateContext* cc)
{
    erase(cc);
}

TaskMgr::TaskMgr(const InitializeArg& arg)
    : mCriticalSection()
    , mParentFramework(arg.parent_framework)
    , mPrepareThread(nullptr)
    , mNullFaderTask(nullptr)
    , mPrepareList()
    , mPrepareDoneList()
    , mActiveList()
    , mStaticList()
    , mDyingList()
    , mDestroyableList()
    , mHeapArray()
    , mTaskCreateContextMgr(nullptr)
    , mMaxCreateQueueSize(arg.create_queue_size)
    , mRootTask(nullptr)
    , mRootTaskCreateArg()
    , mInitializeArg(arg)
    , mCalcDestructionTreeNode(nullptr)
{
    s32 rootHeapNum = HeapMgr::getRootHeapNum();
    for (s32 i = 0; i < rootHeapNum; i++)
    {
        mHeapArray.mHeaps[i] = ExpHeap::create(0, "sead::TaskMgr", HeapMgr::instance()->getRootHeap(i));
    }

    doInit_();

    mRootTaskCreateArg = arg.roottask_create_arg;

    beginCreateRootTask_();
}

TaskMgr* TaskMgr::initialize(const InitializeArg& arg)
{
    return new(arg.heap) TaskMgr(arg);
}

void TaskMgr::finalize()
{
    if (mPrepareThread)
    {
        mPrepareThread->quitAndDestroySingleThread(false);
        delete mPrepareThread;
        mPrepareThread = nullptr;
    }

    if (mRootTask)
    {
        destroyTaskSync(mRootTask);
        mRootTask = nullptr;
    }

    s32 rootHeapNum = HeapMgr::getRootHeapNum();
    for (s32 i = 0; i < rootHeapNum; i++)
    {
        if (mHeapArray.mHeaps[i])
        {
            mHeapArray.mHeaps[i]->destroy();
            mHeapArray.mHeaps[i] = nullptr;
        }
    }
}

bool TaskMgr::requestCreateTask(const TaskBase::CreateArg& arg)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (arg.fader)
        return arg.fader->startAsCreate_(arg);
    else
        return doRequestCreateTask_(arg, nullptr);
}

TaskBase* TaskMgr::createTaskSync(const TaskBase::CreateArg& arg)
{
    mCriticalSection.lock();

    TaskBase* task;

    HeapArray ha;
    createHeap_(&ha, arg);

    task = doCreateTask_(arg, &ha);

    {
        s32 numRootHeaps = HeapMgr::getRootHeapNum();
        for (s32 i = 0; i < numRootHeaps; i++)
        {
            if (task->mHeapArray.mHeaps[i])
                task->mHeapArray.mHeaps[i]->setName(task->getName());
        }
    }

    if (arg.instance_cb)
        arg.instance_cb(task);

    task->mState = TaskBase::State::ePrepare;

    {
        CurrentHeapSetter setter(ha.getPrimaryHeap());

        task->prepare();
    }

    for (s32 i = 0; i < HeapMgr::getRootHeapNum(); i++)
    {
        const HeapPolicy& p = arg.heap_policies[i];
        if (p.adjust)
            task->adjustHeapWithSlackWithoutLock_(i, p.adjust_slack);
    }

    changeTaskState_(task, TaskBase::State::ePrepareDone);
    changeTaskState_(task, TaskBase::State::eRunning);

    if (arg.create_callback)
    {
        DelegateEvent<TaskBase*> e;
        e += *arg.create_callback;

        e.fire(task);
    }

    if (arg.created_task)
        *arg.created_task = task;

    mCriticalSection.unlock();

    return task;
}

void TaskMgr::destroyTaskSync(TaskBase* task)
{
    bool b = getFramework()->getMethodTreeMgr()->getTreeCriticalSection()->tryLock();
    SEAD_ASSERT(b);

    if (b)
    {
        doDestroyTask_(task);
        getFramework()->getMethodTreeMgr()->getTreeCriticalSection()->unlock();
    }
}

void TaskMgr::requestDestroyTask(TaskBase* task, FaderTaskBase* fader)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (!task->checkFlag_(2) && !task->checkFlag_(4))
    {
        task->setFlag_(2);
        task->onDestroy();
    }

    while (task->child())
    {
        requestDestroyTask(task->child()->val(), fader);
    }
}

bool TaskMgr::requestTakeover(const TaskBase::TakeoverArg& arg)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    TaskBase* from = arg.src_task;
    FaderTaskBase* fader = arg.fader ? arg.fader : mNullFaderTask;

    SEAD_ASSERT_MSG(from, "requestTakeover from is null");

    if (from->checkFlag_(1))
        return false;

    return fader->startAsTakeover_(from, arg);
}

bool TaskMgr::requestTransition(TaskBase* from, TaskBase* to, FaderTaskBase* fader)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (!fader)
        fader = mNullFaderTask;

    if (from->checkFlag_(1) || to->checkFlag_(1))
        return false;

    return fader->startAsTransit_(from, to);
}

bool TaskMgr::requestPush(const TaskBase::PushArg& arg)
{
    SEAD_ASSERT(arg.src_task);
    SEAD_ASSERT(arg.parent);
    SEAD_ASSERT(arg.src_task == arg.parent);

    if (!arg.src_task || !arg.parent)
        return false;

    if (arg.src_task != arg.parent)
        return false;

    ScopedLock<CriticalSection> lock(&mCriticalSection);

    FaderTaskBase* fader = arg.fader ? arg.fader : mNullFaderTask;
    TaskBase* from = arg.src_task;

    if (from->checkFlag_(1))
        return false;

    return fader->startAsPush_(from, arg);
}

TaskBase* TaskMgr::pushSync(const TaskBase::PushArg& arg)
{
    SEAD_ASSERT(arg.src_task);
    SEAD_ASSERT(arg.parent);
    SEAD_ASSERT(arg.src_task == arg.parent);

    if (!arg.src_task || !arg.parent)
        return nullptr;

    if (arg.src_task != arg.parent)
        return nullptr;

    ScopedLock<CriticalSection> lock(&mCriticalSection);

    {
        TaskBase* src = arg.src_task;
        src->pauseCalc(true);
        src->pauseDraw(true);
    }

    return createTaskSync(arg);
}

bool TaskMgr::requestPop(TaskBase* popTask, FaderTaskBase* fader)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (!fader)
        fader = mNullFaderTask;

    if (popTask->checkFlag_(1))
        return false;

    return fader->startAsPop_(popTask, nullptr);
}

bool TaskMgr::requestPop(TaskBase* popTask, TaskBase* toTask, FaderTaskBase* fader)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (!fader)
        fader = mNullFaderTask;

    if (popTask->checkFlag_(1))
        return false;

    if (!popTask->isDescendantOf(toTask))
        return false;

    return fader->startAsPop_(popTask, toTask);
}

bool TaskMgr::popSync(TaskBase* popTask)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (popTask->checkFlag_(1))
        return false;

    TaskBase* parent = popTask->getParentTask();
    if (!parent)
        return false;

    doDestroyTask_(popTask);

    parent->pauseCalc(false);
    parent->pauseDraw(false);

    parent->onEvent(TaskEvent::ePopped);

    return true;
}

void TaskMgr::beforeCalc()
{
    calcCreation_();
}

void TaskMgr::afterCalc()
{
    calcDestruction_();
}

TaskBase* TaskMgr::findTask(const TaskClassID& classID)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    for (TaskBase* o : mActiveList)
    {
        if (o->mState == TaskBase::State::eRunning && o->mClassID == classID)
            return o;
    }

    return nullptr;
}

void TaskMgr::destroyAllAndCreateRoot()
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (mRootTask)
        destroyTaskSync(mRootTask);

    mPrepareList.clear();
    mActiveList.clear();
    mDyingList.clear();
    mDestroyableList.clear();

    for (s32 i = 0; i < HeapMgr::getRootHeapNum(); i++)
    {
        Heap* heap = mHeapArray.mHeaps[i];
        if (heap)
            heap->freeAll();
    }

    doInit_();
    beginCreateRootTask_();
}

void TaskMgr::doInit_()
{
    Heap* heap = mHeapArray.getPrimaryHeap();
    SEAD_ASSERT(heap);

    mPrepareThread = new(heap) DelegateThread("Prepare Thread",
                                              new(heap) Delegate2<TaskMgr, Thread*, MessageQueue::Element>(this, &TaskMgr::prepare_),
                                              heap, mInitializeArg.prepare_priority != -1 ? mInitializeArg.prepare_priority : Thread::cDefaultPriority,
                                              MessageQueue::BlockType::eBlock, Thread::cDefaultQuitMsg, mInitializeArg.prepare_stack_size);
    mPrepareThread->start();

    {
        HeapArray heapArray;
        heapArray.mHeaps[0] = heap;

        TaskConstructArg arg(&heapArray, this, nullptr);
        mNullFaderTask = new(heap) NullFaderTask(arg);

        mNullFaderTask->setName("NullFader");
        mNullFaderTask->mState = TaskBase::State::eRunning;
    }

    mTaskCreateContextMgr = new(heap) TaskCreateContextMgr(mMaxCreateQueueSize, heap);
}

void TaskMgr::appendToList_(TaskBase::List& ls, TaskBase* task)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    task->mTaskListNode.erase();

    for (TaskBase* o : ls)
    {
        if (o->mTag < task->mTag)
        {
            o->mTaskListNode.insertFront(&task->mTaskListNode);
            return;
        }
    }

    ls.pushBack(&task->mTaskListNode);
}

void TaskMgr::createHeap_(HeapArray* ha, const TaskBase::CreateArg& arg)
{
    ha->mPrimaryIndex = arg.heap_policies.getPrimaryHeapIndex();

    s32 numRootHeaps = HeapMgr::getRootHeapNum();

    ScopedLock<CriticalSection> lock(&mCriticalSection);

    for (s32 i = 0; i < numRootHeaps; i++)
    {
        const HeapPolicy& policy = arg.heap_policies[i];

        if (policy.dont_create)
        {
            SEAD_ASSERT_MSG(i != ha->getPrimaryHeapIndex(), "cannot use dont_create for primary heap");

            ha->mHeaps[i] = nullptr;
            continue;
        }

        Heap* parentHeap = nullptr;
        if (policy.parent)
        {
            parentHeap = policy.parent;
        }
        else
        {
            if (arg.parent)
                parentHeap = arg.parent->mHeapArray.mHeaps[i];
            else
                parentHeap = mHeapArray.mHeaps[i];
        }

        if (!parentHeap)
        {
            SEAD_WARNING("parent heap is absent\n");
            SEAD_ASSERT_MSG(i != ha->getPrimaryHeapIndex(), "cannot build primary heap");

            ha->mHeaps[i] = nullptr;
            continue;
        }

        SEAD_ASSERT_MSG(parentHeap->getFreeSize() > 0, "[%d] parentHeap(%s) freeSize is 0", i, parentHeap->getName().cstr());

        Heap::HeapDirection direction = !policy.temporary ? Heap::HeapDirection::eForward : Heap::HeapDirection::eReverse;

        Heap* heap = nullptr;
        if (policy.create_slack == 0 || policy.size != 0)
        {
            heap = ExpHeap::create(policy.size, "TaskHeap", parentHeap, direction);
        }
        else
        {
            size_t size = parentHeap->getMaxAllocatableSize();
            if (policy.create_slack < size)
            {
                heap = ExpHeap::create(size - policy.create_slack, "TaskHeap", parentHeap, direction);
            }
            else
            {
                SEAD_ASSERT_MSG(false, "Can't create TaskHeap in parentHeap(%s). (maxAllocatableSize: %zu, create_slack: %u)",
                                parentHeap->getName().cstr(), size, policy.create_slack);
            }
        }

        SEAD_ASSERT(heap);
        ha->mHeaps[i] = heap;
    }
}

TaskBase* TaskMgr::doCreateTask_(const TaskBase::CreateArg& arg, HeapArray* ha)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    TaskClassID factory = arg.factory;
    TaskBase::Tag tag = arg.tag;

    TaskBase* task;
    {
        CurrentHeapSetter setter(ha->getPrimaryHeap());

        TaskConstructArg carg;
        carg.heap_array = ha;
        carg.mgr = this;
        carg.param = arg.parameter;

        task = factory.create(carg);
        SEAD_ASSERT(task);
    }

    task->mClassID = factory;
    task->mTag = tag;

    if (arg.parent)
        arg.parent->pushBackChild(task);

    return task;
}

bool TaskMgr::doRequestCreateTask_(const TaskBase::CreateArg& arg, DelegateEvent<TaskBase*>::Slot* systemSlot)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    TaskCreateContext* cc = mTaskCreateContextMgr->birthForCreate();
    if (!cc)
    {
        SEAD_ASSERT_MSG(false, "CreateQueue is full");
        return false;
    }

    cc->mArg = arg;

    DelegateEvent<TaskBase*>::Slot* slot = arg.create_callback;

    if (systemSlot)
        cc->mEventOnCreate += *systemSlot;

    if (slot)
        cc->mEventOnCreate += *slot;

    return true;
}

void TaskMgr::doDestroyTask_(TaskBase* task)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    while (task->child())
    {
        doDestroyTask_(task->child()->val());
    }

    if (changeTaskState_(task, TaskBase::State::eDead))
    {
        task->detachAll();

        HeapArray ha = task->mHeapArray;

        s32 numRootHeaps = HeapMgr::getRootHeapNum();
        for (s32 i = 0; i < numRootHeaps; i++)
        {
            Heap* heap = ha.mHeaps[i];
            if (heap)
                heap->destroy();
        }
    }
}

bool TaskMgr::changeTaskState_(TaskBase* task, TaskBase::State state)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    TaskBase::State currState = task->mState;
    if (currState == state)
        return false;

    switch (state)
    {
        case TaskBase::State::eCreated:
            SEAD_ASSERT_MSG(false, "Cannot Change State to eCreated\n");
            return false;

        case TaskBase::State::ePrepare:
            if (currState != TaskBase::State::eCreated)
            {
                SEAD_ASSERT_MSG(false, "err");
                return false;
            }

            task->mState = TaskBase::State::ePrepare;
            appendToList_(mPrepareList, task);

            if (mPrepareThread && !mPrepareThread->sendMessage(1, MessageQueue::BlockType::eNoBlock))
            {
                SEAD_ASSERT_MSG(false, "failed to wakeup prepare thread for[%s]", task->getName().cstr());
                return false;
            }

            return true;

        case TaskBase::State::ePrepareDone:
            SEAD_ASSERT(currState == TaskBase::State::ePrepare);

            task->mState = TaskBase::State::ePrepareDone;
            task->mTaskListNode.erase();

            return true;

        case TaskBase::State::eRunning:
            SEAD_ASSERT(currState == TaskBase::State::ePrepareDone);

            task->mState = TaskBase::State::eRunning;
            task->mTaskListNode.erase();
            appendToList_(mActiveList, task);

            task->enterCommon();

            return true;

        case TaskBase::State::eDying:
            task->mState = TaskBase::State::eDying;
            return true;

        case TaskBase::State::eDestroyable:
            if (currState != TaskBase::State::eRunning)
                return false;

            task->mState = TaskBase::State::eDestroyable;
            task->detachCalcDraw();
            appendToList_(mDestroyableList, task);

            return true;

        case TaskBase::State::eDead:
            task->exit();
            task->mState = TaskBase::State::eDead;
            task->mTaskListNode.erase();

            return true;

        default:
            SEAD_ASSERT_MSG(false, "Unknown State %d\n", state);
            return false;
    }
}

void TaskMgr::calcCreation_()
{
    if (!mCriticalSection.tryLock())
        return;

    if (!mTaskCreateContextMgr->isCreateEmpty())
    {
        TaskCreateContext* cc = mTaskCreateContextMgr->getCreateFront();
        if (cc)
        {
            if (!cc->mCreatedTask)
            {
                const TaskBase::CreateArg& arg = cc->mArg;

                HeapArray ha;
                createHeap_(&ha, arg);

                TaskBase* task = doCreateTask_(arg, &ha);

                s32 numRootHeaps = HeapMgr::getRootHeapNum();
                for (s32 i = 0; i < numRootHeaps; i++)
                {
                    Heap* heap = task->mHeapArray.mHeaps[i];
                    if (heap)
                        heap->setName(task->getName());
                }

                cc->mCreatedTask = task;

                bool b = changeTaskState_(task, TaskBase::State::ePrepare);
                SEAD_ASSERT_MSG(b, "failed to changeState to prepare\n");
            }
            else
            {
                TaskBase* task = cc->mCreatedTask;
                if (task->mState == TaskBase::State::ePrepareDone)
                {
                    changeTaskState_(task, TaskBase::State::eRunning);

                    if (cc->mArg.created_task)
                        *cc->mArg.created_task = task;

                    cc->mEventOnCreate.fire(task);

                    mTaskCreateContextMgr->kill(cc);
                }
            }
        }
    }

    mCriticalSection.unlock();
}

void TaskMgr::calcDestruction_()
{
    mCalcDestructionTreeNode.call();

    if (!mCriticalSection.tryLock())
        return;

    for (TaskBase::List::iterator it = mActiveList.begin(); it != mActiveList.end(); )
    {
        TaskBase* task = *it;
        ++it;

        if (task->checkFlag_(2) && destroyable_(task))
            changeTaskState_(task, TaskBase::State::eDestroyable);
    }

    for (TaskBase::List::iterator it = mDestroyableList.begin(); it != mDestroyableList.end(); )
    {
        TaskBase* task = *it;
        task->detachCalcDraw();
        task->detachAll();

        doDestroyTask_(task);

        it = mDestroyableList.begin();
    }

    mCriticalSection.unlock();
}

void TaskMgr::prepare_(Thread*, MessageQueue::Element msg)
{
    SEAD_UNUSED(msg);

    TaskBase* task = nullptr;
    {
        ScopedLock<CriticalSection> lock(&mCriticalSection);

        if (mPrepareList.begin() != mPrepareList.end())
            task = *mPrepareList.begin();
    }

    if (task)
    {
        SEAD_ASSERT(task->mState == TaskBase::State::ePrepare);

        Heap* primaryHeap = task->mHeapArray.getPrimaryHeap();
        CurrentHeapSetter setter(primaryHeap);

        SEAD_ASSERT(primaryHeap->getFreeSize() > 0);

        task->prepare();

        {
            TaskCreateContext* cc = mTaskCreateContextMgr->find(task);
            SEAD_ASSERT_MSG(cc, "no create context for task (0x%p)\n", task);

            for (s32 i = 0; i < HeapMgr::getRootHeapNum(); i++)
            {
                const HeapPolicy& p = cc->mArg.heap_policies[i];
                if (p.adjust)
                    task->adjustHeapWithSlackWithoutLock_(i, p.adjust_slack);
            }

            changeTaskState_(task, TaskBase::State::ePrepareDone);
        }
    }

    Thread::yield();
}

void TaskMgr::beginCreateRootTask_()
{
    mRootTask = nullptr;

    mRootTaskCreateArg.fader = nullptr;
    mRootTaskCreateArg.created_task = &mRootTask;
    mRootTaskCreateArg.tag = TaskBase::Tag::eApp;

    requestCreateTask(mRootTaskCreateArg);
}

bool TaskMgr::destroyable_(TaskBase* task)
{
    ScopedLock<CriticalSection> lock(&mCriticalSection);

    if (!task->checkFlag_(2) || !task->checkFlag_(4) || task->mState != TaskBase::State::eRunning)
        return false;

    while (task->child())
    {
        bool b = destroyable_(task->child()->val());

        if (!b)
            return false;
    }

    return true;
}

} // namespace sead
