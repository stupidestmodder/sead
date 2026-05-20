#pragma once

#include <framework/seadMethodTree.h>
#include <framework/seadTaskBase.h>

namespace sead {

class FaderTaskBase : public TaskBase
{
public:
    enum class Type
    {
        eCreate = 0,
        eTakeover,
        eTransit,
        ePush,
        ePop,
        eInvalid
    };

    enum class FaderState
    {
        eWait = 0,
        eFadeout,
        eLoop,
        eFadein
    };

    // TODO: SEAD_ENUM(FadeEvent)
    enum class FadeEvent
    {
        eFadeinStarted = 0,
        eFadeinEnded,
        eFadeoutStarted,
        eFadeoutEnded,
        eMax
    };

public:
    FaderTaskBase(const TaskConstructArg& arg, const char* name);

    void setFrames(s32 fadeout, s32 black, s32 fadein);

    void pauseCalc(bool b) override;
    void pauseCalcRec(bool b) override;
    void pauseCalcChild(bool b) override;

    MethodTreeNode* getMethodTreeNode(s32 methodType) override;

protected:
    void attachCalcImpl() override;
    void detachCalcImpl() override;

    virtual void doCalc_();
    virtual void onFadeEvent_(const FadeEvent& event);

    void fadein_();
    bool startCreate_();
    void onCreateDone_(TaskBase* task);

public:
    void enter() override;

    virtual void calc();

protected:
    void calcCore_();

    void setFaderState_(FaderState s);

    bool startAsCreate_(const CreateArg& createArg);
    bool startAsTakeover_(TaskBase* destroyTask, const CreateArg& createArg);
    bool startAsTransit_(TaskBase* from, TaskBase* to);
    bool startAsPush_(TaskBase* sleepTask, const CreateArg& createArg);
    bool startAsPop_(TaskBase* sleepTask, TaskBase* toTask);

    friend class TaskMgr;

protected:
    Type mType;
    TaskBase* mSrcTask;
    CreateArg mCreateArg;
    TaskBase* mDstTask;
    DelegateEvent<TaskBase*>::Slot mCreateDoneSlot;
    MethodTreeNode mCalcNode;
    MethodTreeNode mCalcDestructionNode;
    s32 mFrame;
    s32 mFadeoutFrame;
    s32 mLoopFrame;
    s32 mFadeinFrame;
    f32 mBlackness;
    bool mFinishing;
    FaderState mFaderState;
};

class FaderTask : public FaderTaskBase
{
public:
    FaderTask(const TaskConstructArg& arg, const char* name);
    ~FaderTask() override;

    void pauseDraw(bool b) override;
    void pauseDrawRec(bool b) override;
    void pauseDrawChild(bool b) override;

protected:
    void attachDrawImpl() override;
    void detachDrawImpl() override;

public:
    const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const override;
    MethodTreeNode* getMethodTreeNode(s32 methodType) override;

    virtual void draw();

protected:
    MethodTreeNode mDrawNode;
};

class NullFaderTask : public FaderTaskBase
{
public:
    explicit NullFaderTask(const TaskConstructArg& arg);

    virtual void draw();

    void pauseDraw(bool b) override;
    void pauseDrawRec(bool b) override;
    void pauseDrawChild(bool b) override;

protected:
    void attachDrawImpl() override;
    void detachDrawImpl() override;

public:
    const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const override;
    MethodTreeNode* getMethodTreeNode(s32 methodType) override;
};

} // namespace sead
