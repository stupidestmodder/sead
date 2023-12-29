#include <framework/seadSingleScreenMethodTreeMgr.h>

namespace sead {

SingleScreenMethodTreeMgr::SingleScreenMethodTreeMgr()
    : MethodTreeMgr()
    , mRootCalcNode(getTreeCriticalSection())
    , mSysCalcNode(getTreeCriticalSection())
    , mAppCalcNode(getTreeCriticalSection())
    , mRootDrawNode(getTreeCriticalSection())
    , mSysDrawNode(getTreeCriticalSection())
    , mAppDrawNode(getTreeCriticalSection())
    , mAppDrawFinalNode(getTreeCriticalSection())
{
    mRootCalcNode.setName("sead::RootCalc");
    mSysCalcNode.setName("sead::SysCalc");
    mAppCalcNode.setName("sead::AppCalc");

    mRootDrawNode.setName("sead::RootDraw");
    mSysDrawNode.setName("sead::SysDraw");
    mAppDrawNode.setName("sead::AppDraw");
    mAppDrawFinalNode.setName("sead::AppDrawFinal");

    mRootCalcNode.pushBackChild(&mSysCalcNode);
    mRootCalcNode.pushBackChild(&mAppCalcNode);

    mRootDrawNode.pushBackChild(&mAppDrawNode);
    mRootDrawNode.pushBackChild(&mAppDrawFinalNode);
    mRootDrawNode.pushBackChild(&mSysDrawNode);

    mSysCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
    mSysDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);

    mAppCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
    mAppDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
    mAppDrawFinalNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);

    mRootCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);
    mRootDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);
}

SingleScreenMethodTreeMgr::~SingleScreenMethodTreeMgr()
{
}

void SingleScreenMethodTreeMgr::attachMethod(s32 methodType, MethodTreeNode* node)
{
    switch (methodType)
    {
        case 0:
            mSysCalcNode.pushBackChild(node);
            break;

        case 1:
            mAppCalcNode.pushBackChild(node);
            break;

        case 2:
            mSysDrawNode.pushFrontChild(node);
            break;

        case 3:
            mAppDrawNode.pushFrontChild(node);
            break;

        case 4:
            mAppDrawFinalNode.pushFrontChild(node);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined MethodType(%d).", methodType);
    }
}

MethodTreeNode* SingleScreenMethodTreeMgr::getRootMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 0:
            return &mSysCalcNode;

        case 1:
            return &mAppCalcNode;

        case 2:
            return &mSysDrawNode;

        case 3:
            return &mAppDrawNode;

        case 4:
            return &mAppDrawFinalNode;

        default:
            SEAD_ASSERT_MSG(false, "Undefined MethodType(%d)", methodType);
            return nullptr;
    }
}

void SingleScreenMethodTreeMgr::pauseAll(bool p)
{
    if (p)
    {
        mRootCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);
        mRootDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);
    }
    else
    {
        mRootCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
        mRootDrawNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
    }
}

void SingleScreenMethodTreeMgr::pauseAppCalc(bool p)
{
    if (p)
        mAppCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eBoth);
    else
        mAppCalcNode.setPauseFlag(MethodTreeNode::PauseFlag::eNone);
}

void SingleScreenMethodTreeMgr::calc()
{
    mRootCalcNode.call();
}

void SingleScreenMethodTreeMgr::draw()
{
    mRootDrawNode.call();
}

} // namespace sead
