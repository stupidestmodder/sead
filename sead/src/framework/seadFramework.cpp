#include <framework/seadFramework.h>

#include <framework/seadMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>
#include <random/seadGlobalRandom.h>
#include <thread/seadThread.h>

namespace sead {

Framework::InitializeArg::InitializeArg()
    : heap_size(0x3000000)
    , arena(nullptr)
{
}

Framework::RunArg::RunArg()
    : prepare_stack_size(0)
    , prepare_priority(-1)
{
}

Framework::Framework()
    : mReserveReset(false)
    , mResetParameter(nullptr)
    , mResetEvent()
    , mTaskMgr(nullptr)
    , mMethodTreeMgr(nullptr)
    , mMethodTreeMgrHeap(nullptr)
{
}

Framework::~Framework()
{
    if (mTaskMgr)
    {
        mTaskMgr->finalize();
        delete mTaskMgr;
        mTaskMgr = nullptr;
    }

    if (mMethodTreeMgr)
    {
        delete mMethodTreeMgr;
        mMethodTreeMgr = nullptr;
    }

    if (mMethodTreeMgrHeap)
    {
        mMethodTreeMgrHeap->destroy();
        mMethodTreeMgrHeap = nullptr;
    }
}

void Framework::initialize(const InitializeArg& arg)
{
    if (arg.arena)
        HeapMgr::initialize(arg.arena);
    else
        HeapMgr::initialize(arg.heap_size);

    Heap* heap = HeapMgr::instance()->getRootHeap(0);

    {
        Heap* threadHeap = ExpHeap::create(0, "sead::ThreadMgr", heap);

        ThreadMgr::createInstance(threadHeap);
        ThreadMgr::instance()->initialize(threadHeap);

        threadHeap->adjust();
    }

    GlobalRandom::createInstance(heap);
}

void Framework::run(Heap* heap, const TaskBase::CreateArg& rootCreateArg, const RunArg& runArg)
{
    initRun_(heap);

    {
        Heap* methodTreeHeap = ExpHeap::create(0, "sead::MethodTreeMgr", heap);
        mMethodTreeMgr = createMethodTreeMgr_(methodTreeHeap);
        methodTreeHeap->adjust();

        mMethodTreeMgrHeap = methodTreeHeap;
    }

    TaskMgr::InitializeArg arg(rootCreateArg);

    if (runArg.prepare_stack_size != 0)
        arg.prepare_stack_size = runArg.prepare_stack_size;

    if (runArg.prepare_priority != -1)
        arg.prepare_priority = runArg.prepare_priority;

    arg.heap = heap;
    arg.parent_framework = this;

    mTaskMgr = TaskMgr::initialize(arg);

    runImpl_();

    quitRun_(heap);
}

void Framework::createSystemTasks(TaskBase*, const CreateSystemTaskArg&)
{
}

void Framework::procReset_()
{
    if (mReserveReset)
    {
        mResetEvent.fire(mResetParameter);

        mTaskMgr->destroyAllAndCreateRoot();

        mReserveReset = false;
        mResetParameter = nullptr;
    }
}

} // namespace sead
