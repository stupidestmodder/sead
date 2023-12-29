#pragma once

#include <framework/seadMethodTree.h>
#include <framework/seadTaskBase.h>

namespace sead {

class Task : public TaskBase
{
    SEAD_RTTI_OVERRIDE(Task, TaskBase);

public:
    explicit Task(const TaskConstructArg& arg);
    Task(const TaskConstructArg& arg, const char* name);
    ~Task() override;

    virtual void calc();
    virtual void draw();

protected:
    void attachCalcImpl() override;
    void attachDrawImpl() override;
    void detachCalcImpl() override;
    void detachDrawImpl() override;

public:
    void pauseCalc(bool b) override;
    void pauseDraw(bool b) override;
    void pauseCalcRec(bool b) override;
    void pauseDrawRec(bool b) override;
    void pauseCalcChild(bool b) override;
    void pauseDrawChild(bool b) override;

    const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const override;
    MethodTreeNode* getMethodTreeNode(s32 methodType) override;

protected:
    MethodTreeNode mCalcNode;
    MethodTreeNode mDrawNode;
};

} // namespace sead
