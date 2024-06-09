#include <framework/seadFaderTask.h>

#include <framework/seadFramework.h>
#include <framework/seadMethodTreeMgr.h>
#include <framework/seadTaskEvent.h>
#include <framework/seadTaskMgr.h>
#include <gfx/seadCamera.h>
#include <gfx/seadGraphics.h>
#include <gfx/seadPrimitiveDrawer.h>
#include <gfx/seadProjection.h>
#include <gfx/seadViewport.h>
#include <prim/seadScopedLock.h>

namespace sead {

class ScreenFiller
{
public:
    ScreenFiller(const LogicalFrameBuffer* fb)
        : mViewport(*fb)
        , mProjection(1.0f, 2000.0f, mViewport)
        , mCamera(mProjection)
        , mLogicalFrameBuffer(fb)
    {
    }

    void draw(f32 alpha);

private:
    Viewport mViewport;
    OrthoProjection mProjection;
    OrthoCamera mCamera;
    const LogicalFrameBuffer* mLogicalFrameBuffer;
};

void ScreenFiller::draw(f32 alpha)
{
    PrimitiveDrawer drawer(nullptr);

    mViewport.apply(nullptr, *mLogicalFrameBuffer);

    drawer.setProjection(&mProjection);
    drawer.setCamera(&mCamera);

    const f32 w = mLogicalFrameBuffer->getVirtualSize().x;
    const f32 h = mLogicalFrameBuffer->getVirtualSize().y;
    BoundBox2f box(-w, -h, w, h);

    drawer.begin();
    {
        drawer.drawQuad(PrimitiveDrawer::QuadArg().setBoundBox(box, 0.0f).setColor(Color4f(1.0f, 1.0f, 1.0f, alpha)));
    }
    drawer.end();
}

FaderTaskBase::FaderTaskBase(const TaskConstructArg& arg, const char* name)
    : TaskBase(arg, name)
    , mType(Type::eInvalid)
    , mSrcTask(nullptr)
    , mCreateArg()
    , mDstTask(nullptr)
    , mCreateDoneSlot(this, &FaderTaskBase::onCreateDone_)
    , mCalcNode(nullptr)
    , mCalcDestructionNode(nullptr)
    , mFrame(0)
    , mFadeoutFrame(30)
    , mLoopFrame(mFadeoutFrame + 1)
    , mFadeinFrame(mLoopFrame + 30)
    , mBlackness(0.0f)
    , mFinishing(false)
    , mFaderState(FaderState::eWait)
{
    mCalcNode.setName(name);
    mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);

    mCalcNode.bind(this, &FaderTaskBase::calc, name);
    mCalcDestructionNode.bind(this, &FaderTaskBase::calcCore_, "calcCore_");
}

void FaderTaskBase::setFrames(s32 fadeout, s32 black, s32 fadein)
{
    SEAD_ASSERT(fadeout > 0);
    SEAD_ASSERT(black > 0);
    SEAD_ASSERT(fadein > 0);

    mFadeoutFrame = fadeout;
    mLoopFrame = mFadeoutFrame + black;
    mFadeinFrame = mLoopFrame + fadein;
}

void FaderTaskBase::pauseCalc(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eSelf);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void FaderTaskBase::pauseCalcRec(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void FaderTaskBase::pauseCalcChild(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eChild);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

MethodTreeNode* FaderTaskBase::getMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 0:
        case 1:
            return &mCalcNode;

        default:
            SEAD_ASSERT_MSG(false, "undefined methodType(%d).", methodType);
            return nullptr;
    }
}

void FaderTaskBase::attachCalcImpl()
{
    getTaskMgr()->mCalcDestructionTreeNode.pushBackChild(&mCalcDestructionNode);

    attachMethodWithCheck(1, &mCalcNode);
    mCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void FaderTaskBase::detachCalcImpl()
{
    mCalcNode.detachAll();
}

void FaderTaskBase::doCalc_()
{
}

void FaderTaskBase::onFadeEvent_(const FadeEvent&)
{
}

void FaderTaskBase::fadein_()
{
    mFinishing = true;
}

bool FaderTaskBase::startCreate_()
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);

    if (mType == Type::eTakeover)
    {
        mTaskMgr->doDestroyTask_(mSrcTask);
        mSrcTask = nullptr;
    }

    bool b = mTaskMgr->doRequestCreateTask_(mCreateArg, &mCreateDoneSlot);
    return b;
}

void FaderTaskBase::onCreateDone_(TaskBase* task)
{
    mDstTask = task;
    mDstTask->setFlag_(1);

    switch (mType)
    {
        case Type::eCreate:
        case Type::eTakeover:
        case Type::ePush:
            fadein_();
    }
}

void FaderTaskBase::enter()
{
    setFaderState_(FaderState::eWait);
}

void FaderTaskBase::calc()
{
    switch (mFaderState)
    {
        case FaderState::eFadeout:
            mBlackness = mFadeoutFrame > 0 ? static_cast<f32>(mFrame) / static_cast<f32>(mFadeoutFrame + 1) : 0.0f;

            if (mFrame == 0)
                onFadeEvent_(FadeEvent::eFadeoutStarted);

            doCalc_();
            break;

        case FaderState::eLoop:
            doCalc_();
            mBlackness = 1.0f;
            break;

        case FaderState::eFadein:
            doCalc_();

            if (mFadeinFrame != mLoopFrame)
                mBlackness = 1.0f - static_cast<f32>(mFrame - mLoopFrame) / static_cast<f32>(mFadeinFrame - mLoopFrame);
            
            break;

        default:
            return;
    }

    mFrame++;
}

void FaderTaskBase::calcCore_()
{
    switch (mFaderState)
    {
        case FaderState::eFadeout:
            if (mFrame > mFadeoutFrame)
            {
                CriticalSection& cs = mTaskMgr->mCriticalSection;

                mFrame = mFadeoutFrame + 1;

                if (cs.tryLock())
                {
                    onFadeEvent_(FadeEvent::eFadeoutEnded);
                    setFaderState_(FaderState::eLoop);

                    switch (mType)
                    {
                        case Type::eTakeover:
                            startCreate_();
                            break;

                        case Type::eTransit:
                            SEAD_ASSERT(mSrcTask);
                            SEAD_ASSERT(mDstTask);

                            mSrcTask->pauseCalc(true);
                            mSrcTask->pauseDraw(true);

                            mDstTask->pauseCalc(false);
                            mDstTask->pauseDraw(false);

                            mSrcTask->onEvent(TaskEvent::eTransitOut);
                            mDstTask->onEvent(TaskEvent::eTransitIn);

                            fadein_();
                            break;

                        case Type::ePush:
                            SEAD_ASSERT(mSrcTask);

                            mSrcTask->pauseCalc(true);
                            mSrcTask->pauseDraw(true);

                            break;

                        case Type::ePop:
                            TaskBase* nextTask = mSrcTask->getParentTask();
                            SEAD_ASSERT(nextTask);

                            if (mDstTask)
                            {
                                mTaskMgr->doDestroyTask_(mSrcTask);

                                while (nextTask != mDstTask)
                                {
                                    mTaskMgr->doDestroyTask_(nextTask);

                                    nextTask = nextTask->getParentTask();
                                }

                                nextTask->pauseCalc(false);
                                nextTask->pauseDraw(false);
                            }
                            else
                            {
                                mTaskMgr->doDestroyTask_(mSrcTask);

                                nextTask->pauseCalc(false);
                                nextTask->pauseDraw(false);
                            }

                            nextTask->onEvent(TaskEvent::ePopped);

                            fadein_();
                            break;
                    }

                    cs.unlock();
                }
            }

            break;

        case FaderState::eLoop:
            if (mFrame > mLoopFrame)
            {
                mFrame = mLoopFrame + 1;

                if (mFinishing)
                {
                    if (mType == Type::eTakeover)
                        mDstTask->onEvent(TaskEvent::eTakeoverIn);

                    onFadeEvent_(FadeEvent::eFadeinStarted);
                    setFaderState_(FaderState::eFadein);
                }
                else
                {
                    mFrame = mLoopFrame + 1;
                }
            }

            break;

        case FaderState::eFadein:
            if (mFrame > mFadeinFrame)
            {
                onFadeEvent_(FadeEvent::eFadeinEnded);

                mFrame = mFadeinFrame + 1;

                setFaderState_(FaderState::eWait);

                if (mSrcTask)
                    mSrcTask->resetFlag_(1);

                if (mDstTask)
                    mDstTask->resetFlag_(1);
            }

            break;
    }
}

void FaderTaskBase::setFaderState_(FaderState s)
{
    mFaderState = s;

    if (mFaderState == FaderState::eWait)
        detachCalcDraw();
}

bool FaderTaskBase::startAsCreate_(const CreateArg& createArg)
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);

    if (mFaderState != FaderState::eWait)
        return false;

    mType = Type::eCreate;
    mCreateArg = createArg;

    if (!startCreate_())
        return false;

    mSrcTask = nullptr;
    mFrame = mFadeoutFrame;

    setFaderState_(FaderState::eLoop);

    mFinishing = false;
    mBlackness = 1.0f;

    attachCalcDraw();

    return true;
}

bool FaderTaskBase::startAsTakeover_(TaskBase* destroyTask, const CreateArg& createArg)
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);

    if (mFaderState != FaderState::eWait)
        return false;

    mType = Type::eTakeover;

    if (!mTaskMgr->changeTaskState_(destroyTask, State::eDying))
        return false;

    mSrcTask = destroyTask;
    mSrcTask->setFlag_(1);
    mSrcTask->onEvent(TaskEvent::eTakeoverOut);

    mCreateArg = createArg;
    mFrame = 0;

    setFaderState_(FaderState::eFadeout);

    mFinishing = false;
    mBlackness = 0.0f;

    attachCalcDraw();

    return true;
}

bool FaderTaskBase::startAsTransit_(TaskBase* from, TaskBase* to)
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);

    if (mFaderState != FaderState::eWait)
        return false;

    mType = Type::eTransit;

    mSrcTask = from;
    mDstTask = to;

    mSrcTask->setFlag_(1);
    mDstTask->setFlag_(1);

    mFrame = 0;

    setFaderState_(FaderState::eFadeout);

    mFinishing = false;
    mBlackness = 0.0f;

    attachCalcDraw();

    return true;
}

bool FaderTaskBase::startAsPush_(TaskBase* sleepTask, const CreateArg& createArg)
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);

    if (mFaderState != FaderState::eWait)
        return false;

    mType = Type::ePush;
    mCreateArg = createArg;

    if (!startCreate_())
        return false;

    mSrcTask = sleepTask;
    mSrcTask->setFlag_(1);

    mFrame = 0;

    setFaderState_(FaderState::eFadeout);

    mFinishing = false;
    mBlackness = 0.0f;

    attachCalcDraw();

    return true;
}

bool FaderTaskBase::startAsPop_(TaskBase* sleepTask, TaskBase* toTask)
{
    ScopedLock<CriticalSection> lock(&mTaskMgr->mCriticalSection);

    if (mFaderState != FaderState::eWait)
        return false;

    mType = Type::ePop;

    mSrcTask = sleepTask;
    mSrcTask->setFlag_(1);

    mDstTask = toTask;

    mFrame = 0;

    setFaderState_(FaderState::eFadeout);

    mFinishing = false;
    mBlackness = 0.0f;

    attachCalcDraw();

    return true;
}

FaderTask::FaderTask(const TaskConstructArg& arg, const char* name)
    : FaderTaskBase(arg, name)
    , mDrawNode(nullptr)
{
    mDrawNode.setName(name);
    mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);

    mDrawNode.bind(this, &FaderTask::draw, name);
}

FaderTask::~FaderTask()
{
}

void FaderTask::pauseDraw(bool b)
{
    if (b)
        mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eSelf);
    else
        mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void FaderTask::pauseDrawRec(bool b)
{
    if (b)
        mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);
    else
        mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void FaderTask::pauseDrawChild(bool b)
{
    if (b)
        mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eChild);
    else
        mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void FaderTask::attachDrawImpl()
{
    attachMethodWithCheck(4, &mDrawNode);

    mDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void FaderTask::detachDrawImpl()
{
    mDrawNode.detachAll();
}

const RuntimeTypeInfo::Interface* FaderTask::getCorrespondingMethodTreeMgrTypeInfo() const
{
    return MethodTreeMgr::getRuntimeTypeInfoStatic();
}

MethodTreeNode* FaderTask::getMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 2:
        case 3:
        case 4:
            return &mDrawNode;

        default:
            return FaderTaskBase::getMethodTreeNode(methodType);
    }
}

void FaderTask::draw()
{
    SEAD_ASSERT(Graphics::instance());

    ScreenFiller(getFramework()->getMethodLogicalFrameBuffer(3)).draw(mBlackness);
}

NullFaderTask::NullFaderTask(const TaskConstructArg& arg)
    : FaderTaskBase(arg, "NullFaderTask")
{
    mFadeoutFrame = 0;
    mLoopFrame = 0;
    mFadeinFrame = 0;
}

void NullFaderTask::draw()
{
}

void NullFaderTask::pauseDraw(bool)
{
}

void NullFaderTask::pauseDrawRec(bool)
{
}

void NullFaderTask::pauseDrawChild(bool)
{
}

void NullFaderTask::attachDrawImpl()
{
}

void NullFaderTask::detachDrawImpl()
{
}

const RuntimeTypeInfo::Interface* NullFaderTask::getCorrespondingMethodTreeMgrTypeInfo() const
{
    return MethodTreeMgr::getRuntimeTypeInfoStatic();
}

MethodTreeNode* NullFaderTask::getMethodTreeNode(s32)
{
    return nullptr;
}

} // namespace sead
