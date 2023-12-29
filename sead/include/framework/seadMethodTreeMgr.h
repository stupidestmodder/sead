#pragma once

#include <prim/seadRuntimeTypeInfo.h>
#include <thread/seadCriticalSection.h>

namespace sead {

class MethodTreeNode;

class MethodTreeMgr
{
    SEAD_RTTI_BASE(MethodTreeMgr);

public:
    MethodTreeMgr();
    virtual ~MethodTreeMgr();

    virtual void attachMethod(s32 methodType, MethodTreeNode* node) = 0;
    virtual MethodTreeNode* getRootMethodTreeNode(s32 methodType) = 0;
    virtual void pauseAll(bool p) = 0;
    virtual void pauseAppCalc(bool p) = 0;

    CriticalSection* getTreeCriticalSection()
    {
        return &mCS;
    }

protected:
    CriticalSection mCS;
};

} // namespace sead
