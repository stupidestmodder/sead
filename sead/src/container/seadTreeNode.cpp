#include <container/seadTreeNode.h>

#include <basis/seadAssert.h>

namespace sead {

TreeNode::TreeNode()
{
    clearLinks();
}

void TreeNode::clearLinks()
{
    mPrev = nullptr;
    mNext = nullptr;
    mChild = nullptr;
    mParent = nullptr;
}

void TreeNode::detachAll()
{
    detachSubTree();
    clearChildLinksRecursively_();
    clearLinks();
}

void TreeNode::detachSubTree()
{
    if (mPrev)
    {
        mPrev->mNext = mNext;
        if (mNext)
        {
            mNext->mPrev = mPrev;
            mNext = nullptr;
        }

        mPrev = nullptr;
        mParent = nullptr;
    }
    else
    {
        if (mParent)
        {
            mParent->mChild = mNext;
            mParent = nullptr;
        }

        if (mNext)
        {
            mNext->mPrev = mPrev;
            mNext = nullptr;
        }
    }
}

void TreeNode::pushBackSibling(TreeNode* n)
{
    n->detachSubTree();

    TreeNode* node = this;
    while (node->mNext)
    {
        node = node->mNext;
    }

    node->mNext = n;
    n->mPrev = node;
    n->mParent = node->mParent;
}

void TreeNode::pushBackChild(TreeNode* n)
{
    n->detachSubTree();

    if (mChild)
    {
        mChild->pushBackSibling(n);
    }
    else
    {
        mChild = n;
        n->mParent = this;
    }
}

void TreeNode::pushFrontChild(TreeNode* n)
{
    n->detachSubTree();

    if (mChild)
    {
        n->mNext = mChild;
        mChild->mPrev = n;
    }

    mChild = n;
    n->mParent = this;
}

void TreeNode::insertBeforeSelf(TreeNode* n)
{
    n->detachSubTree();

    TreeNode* prev = mPrev;
    mPrev = n;

    n->mPrev = prev;
    n->mNext = this;

    if (prev)
    {
        prev->mNext = n;
    }
    else if (mParent)
    {
        mParent->mChild = n;
    }

    n->mParent = mParent;
}

void TreeNode::insertAfterSelf(TreeNode* n)
{
    n->detachSubTree();

    TreeNode* next = mNext;
    mNext = n;

    n->mPrev = this;
    n->mNext = next;

    if (next)
        next->mPrev = n;

    n->mParent = mParent;
}

const TreeNode* TreeNode::findRoot() const
{
    const TreeNode* r = this;
    const TreeNode* p = mParent;

    while (p)
    {
        SEAD_ASSERT(p != this);
        r = p;
        p = p->mParent;
    }

    return r;
}

TreeNode* TreeNode::findRoot()
{
    TreeNode* r = this;
    TreeNode* p = mParent;

    while (p)
    {
        SEAD_ASSERT(p != this);
        r = p;
        p = p->mParent;
    }

    return r;
}

s32 TreeNode::countChildren() const
{
    s32 count = 0;

    for (TreeNode* node = mChild; node; node = node->mNext)
    {
        count++;
    }

    return count;
}

void TreeNode::clearChildLinksRecursively_()
{
    TreeNode* next = mChild;

    while (next)
    {
        TreeNode* n = next;
        next = next->mNext;
        n->clearChildLinksRecursively_();
        n->clearLinks();
    }
}

} // namespace sead
