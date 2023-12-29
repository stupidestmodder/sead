#include <framework/seadMethodTree.h>

#include <thread/seadCriticalSection.h>

namespace sead {

void MethodTreeNode::call()
{
    lock_();
    callRec_();
    unlock_();
}

void MethodTreeNode::detachAll()
{
    CriticalSection* prev = mCriticalSection;
    attachMutexRec_(nullptr);
    mCriticalSection = prev;

    lock_();
    TreeNode::detachAll();
    unlock_();

    mCriticalSection = nullptr;
}

void MethodTreeNode::pushBackChild(MethodTreeNode* o)
{
    lock_();

    o->detachSubTree();
    o->mCriticalSection = mCriticalSection;

    {
        MethodTreeNode* child = o->getChild();
        if (child)
            child->attachMutexRec_(mCriticalSection);
    }

    TTreeNode::pushBackChild(o);

    unlock_();
}

void MethodTreeNode::pushFrontChild(MethodTreeNode* o)
{
    lock_();

    o->detachSubTree();
    o->mCriticalSection = mCriticalSection;

    {
        MethodTreeNode* child = o->getChild();
        if (child)
            child->attachMutexRec_(mCriticalSection);
    }

    TTreeNode::pushFrontChild(o);

    unlock_();
}

void MethodTreeNode::callRec_()
{
    if (mPauseFlag.isOff(PauseFlag::eSelf))
        mDelegate.invoke();

    TTreeNode<MethodTreeNode*>* childNode = child();

    if (mPauseFlag.isOff(PauseFlag::eChild))
    {
        while (childNode)
        {
            childNode->val()->callRec_();
            childNode = childNode->val()->next();
        }
    }
}

void MethodTreeNode::attachMutexRec_(CriticalSection* m) const
{
    mCriticalSection = m;

    MethodTreeNode* child = getChild();
    if (child)
        child->attachMutexRec_(m);

    MethodTreeNode* next = getNext();
    if (next)
        next->attachMutexRec_(m);
}

void MethodTreeNode::lock_()
{
    if (mCriticalSection)
        mCriticalSection->lock();
}

void MethodTreeNode::unlock_()
{
    if (mCriticalSection)
        mCriticalSection->unlock();
}

} // namespace sead
