#pragma once

#include <basis/seadTypes.h>

namespace sead {

class TreeNode
{
public:
    TreeNode();

    void clearLinks();
    void detachAll();
    void detachSubTree();
    void pushBackSibling(TreeNode* n);
    void pushBackChild(TreeNode* n);
    void pushFrontChild(TreeNode* n);
    void insertBeforeSelf(TreeNode* n);
    void insertAfterSelf(TreeNode* n);

    const TreeNode* findRoot() const;
    TreeNode* findRoot();
    s32 countChildren() const;

    bool isRoot() const { return mParent == nullptr; }
    bool isLeaf() const { return mChild == nullptr; }

protected:
    void clearChildLinksRecursively_();

protected:
    TreeNode* mParent;
    TreeNode* mChild;
    TreeNode* mNext;
    TreeNode* mPrev;
};

template <typename T>
class TTreeNode : public TreeNode
{
public:
    TTreeNode()
        : TreeNode()
        , mData(nullptr)
    {
    }

    explicit TTreeNode(T ptr)
        : TreeNode()
        , mData(ptr)
    {
    }

    TTreeNode* parent() const { return static_cast<TTreeNode*>(mParent); }
    TTreeNode* child() const { return static_cast<TTreeNode*>(mChild); }
    TTreeNode* next() const { return static_cast<TTreeNode*>(mNext); }
    TTreeNode* prev() const { return static_cast<TTreeNode*>(mPrev); }

    const TTreeNode* findRoot() const { return static_cast<const TTreeNode*>(TreeNode::findRoot()); }
    TTreeNode* findRoot() { return static_cast<TTreeNode*>(TreeNode::findRoot()); }

    void pushBackSibling(TTreeNode* o) { TreeNode::pushBackSibling(o); }
    void pushBackChild(TTreeNode* o) { TreeNode::pushBackChild(o); }
    void pushFrontChild(TTreeNode* o) { TreeNode::pushFrontChild(o); }
    void insertBeforeSelf(TTreeNode* o) { TreeNode::insertBeforeSelf(o); }
    void insertAfterSelf(TTreeNode* o) { TreeNode::insertAfterSelf(o); }

    T& val() { return mData; }
    const T& val() const { return mData; }

    // TODO: Iterators

protected:
    T mData;
};

} // namespace sead
