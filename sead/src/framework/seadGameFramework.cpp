#include <framework/seadGameFramework.h>

#include <basis/seadWarning.h>
#include <controller/seadControllerMgr.h>
#include <devenv/seadSeadMenuMgr.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <framework/seadInfLoopChecker.h>
#include <framework/seadInfLoopCheckerThread.h>
#include <framework/seadMethodTreeMgr.h>
#include <framework/seadProcessMeter.h>
#include <framework/seadTaskBase.h>
#include <framework/seadTaskMgr.h>
#include <heap/seadExpHeap.h>
#include <resource/seadResourceMgr.h>
#include <thread/seadThreadUtil.h>

static void DefaultLockFunc(bool isLock)
{
    if (isLock)
        sead::Graphics::instance()->lockDrawContext();
    else
        sead::Graphics::instance()->unlockDrawContext();
}

namespace sead {

GameFramework::GameFramework()
    : Framework()
    , mDisplayState(DisplayState::eHide)
    , mCalcMeter("calc", Color4f::cRed)
    , mDrawMeter("draw", Color4f::cGreen)
    , mGPUMeter("waitGPU", Color4f::cMagenta)
    , mCheckerThread(nullptr)
    , mFrameLockFunc(nullptr)
    , mProcDrawCallback(&DefaultLockFunc)
{
}

GameFramework::~GameFramework()
{
    if (mCheckerThread)
    {
        mCheckerThread->quitAndDestroySingleThread(false);
        delete mCheckerThread;
        mCheckerThread = nullptr;
    }
}

void GameFramework::initialize(const InitializeArg& arg)
{
    Framework::initialize(arg);

    SEAD_WARNING("GameFramework::initialize() is TODO");

    Heap* root = HeapMgr::instance()->getRootHeap(0);

    ExpHeap* heap = ExpHeap::create(root->getMaxAllocatableSize(), "sead::SystemManagers", root);

    {
        ExpHeap* mgrHeap = ExpHeap::create(heap->getMaxAllocatableSize(), "sead::ResourceMgr", heap);

        CurrentHeapSetter chs(mgrHeap);
        //ResourceMgr::createInstance(mgrHeap); // TODO

        mgrHeap->adjust();
    }

    {
        ExpHeap* mgrHeap = ExpHeap::create(heap->getMaxAllocatableSize(), "sead::FileDeviceMgr", heap);

        CurrentHeapSetter chs(mgrHeap);
        FileDeviceMgr::createInstance(mgrHeap);

        mgrHeap->adjust();
    }

    heap->adjust();
}

void GameFramework::startDisplay()
{
    if (mDisplayState == DisplayState::eHide)
        mDisplayState = DisplayState::eReady;
}

void GameFramework::lockFrameDrawContext()
{
    if (mFrameLockFunc)
        mFrameLockFunc(true);
}

void GameFramework::unlockFrameDrawContext()
{
    if (mFrameLockFunc)
        mFrameLockFunc(false);
}

void GameFramework::createSystemTasks(TaskBase* rootTask, const CreateSystemTaskArg& arg)
{
    Framework::createSystemTasks(rootTask, CreateSystemTaskArg());

    createControllerMgr(rootTask);
    createProcessMeter(rootTask);
    createSeadMenuMgr(rootTask);
    createInfLoopChecker(rootTask, arg.infloop_detection_span, arg.infloop_thread_stack_size);
}

void GameFramework::createControllerMgr(TaskBase* rootTask)
{
    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<ControllerMgr>);
    arg.parent = rootTask;

    mTaskMgr->createSingletonTaskSync<ControllerMgr>(arg);
}

void GameFramework::createProcessMeter(TaskBase* rootTask)
{
    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<ProcessMeter>);
    arg.parent = rootTask;

    mTaskMgr->createSingletonTaskSync<ProcessMeter>(arg);

    ProcessMeter::instance()->attachProcessMeterBar(&mCalcMeter);
    ProcessMeter::instance()->attachProcessMeterBar(&mDrawMeter);
    ProcessMeter::instance()->attachProcessMeterBar(&mGPUMeter);
}

void GameFramework::createSeadMenuMgr(TaskBase* rootTask)
{
    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<SeadMenuMgr>);
    arg.parent = rootTask;

    mTaskMgr->createSingletonTaskSync<SeadMenuMgr>(arg);
}

void GameFramework::createInfLoopChecker(TaskBase* rootTask, const TickSpan& infLoopSpan, s32 infLoopThreadStackSize)
{
    if (infLoopSpan.toS64() <= 0)
        return;

    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<InfLoopChecker>);
    arg.parent = rootTask;

    mTaskMgr->createSingletonTaskSync<InfLoopChecker>(arg);

    InfLoopChecker* checker = InfLoopChecker::instance();
    checker->setThreshold(2);

    SEAD_ASSERT(!mCheckerThread);
    mCheckerThread = new(rootTask->getHeapArray().getPrimaryHeap()) InfLoopCheckerThread(infLoopSpan / 2.0f,
                                                                                         rootTask->getHeapArray().getPrimaryHeap(),
                                                                                         ThreadUtil::ConvertPrioritySeadToPlatform(8),
                                                                                         infLoopThreadStackSize);
    mCheckerThread->start();
}

void GameFramework::waitStartDisplayLoop_()
{
    while (true)
    {
        Graphics::instance()->lockDrawContext();
        getTaskMgr()->beforeCalc();
        getTaskMgr()->afterCalc();
        Graphics::instance()->unlockDrawContext();

        if (getTaskMgr()->getRootTask() || mDisplayState != DisplayState::eHide)
            break;

        Thread::sleep(TickSpan::makeFromMilliSeconds(10));
    }

    getMethodTreeMgr()->pauseAll(false);

    startDisplay();
}

} // namespace sead
