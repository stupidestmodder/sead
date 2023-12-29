#pragma once

#include <framework/seadMethodTree.h>
#include <framework/seadMethodTreeMgr.h>

namespace sead {

class SingleScreenMethodTreeMgr : public MethodTreeMgr
{
    SEAD_RTTI_OVERRIDE(SingleScreenMethodTreeMgr, MethodTreeMgr);

public:
    SingleScreenMethodTreeMgr();
    ~SingleScreenMethodTreeMgr() override;

    void attachMethod(s32 methodType, MethodTreeNode* node) override;
    MethodTreeNode* getRootMethodTreeNode(s32 methodType) override;
    void pauseAll(bool p) override;
    void pauseAppCalc(bool p) override;

    void calc();
    void draw();

protected:
    MethodTreeNode mRootCalcNode;
    MethodTreeNode mSysCalcNode;
    MethodTreeNode mAppCalcNode;
    MethodTreeNode mRootDrawNode;
    MethodTreeNode mSysDrawNode;
    MethodTreeNode mAppDrawNode;
    MethodTreeNode mAppDrawFinalNode;
};

} // namespace sead
