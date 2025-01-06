#pragma once

#include <basis/seadTypes.h>

namespace sead {

template <typename Key>
class TreeMapImpl;

template <typename Key>
class TreeMapNode
{
protected:
    static const bool cRed_ = true;
    static const bool cBlack_ = false;

public:
    TreeMapNode()
        : mLeft_(nullptr)
        , mRight_(nullptr)
        , mColor_(cRed_)
        , mKey_()
    {
    }

    virtual ~TreeMapNode()
    {
    }

protected:
    virtual void erase_() = 0;

protected:
    TreeMapNode* mLeft_;
    TreeMapNode* mRight_;
    bool mColor_;
    Key mKey_;

    friend class TreeMapImpl<Key>;
};

template <typename Key>
class TreeMapImpl
{
public:
    using Node = TreeMapNode<Key>;

public:
    TreeMapImpl()
        : mRoot(nullptr)
    {
    }

    Node* find(const Key& key) const
    {
        return find(mRoot, key);
    }

    bool contains(const Key& key) const
    {
        return find(key) != nullptr;
    }

    void insert(Node* node)
    {
        mRoot = insert(mRoot, node);
        mRoot->mColor_ = Node::cBlack_;
    }

    void erase(const Key& key)
    {
        mRoot = erase(mRoot, key);
        if (mRoot)
        {
            mRoot->mColor_ = Node::cBlack_;
        }
    }

    template <typename T>
    void forEach(const T& fun) const
    {
        if (mRoot)
        {
            forEach(mRoot, fun);
        }
    }

protected:
    Node* find(Node* node, const Key& key) const;
    Node* insert(Node* h, Node* node);
    Node* erase(Node* h, const Key& key);

    static Node* min(Node* h);
    static Node* eraseMin(Node* h);
    static Node* moveRedLeft(Node* h);
    static Node* moveRedRight(Node* h);
    static Node* fixUp(Node* h);
    static Node* rotateLeft(Node* h);
    static Node* rotateRight(Node* h);
    static void flipColors(Node* h);
    static bool isRed(Node* h);

    template <typename T>
    static void forEach(Node* node, const T& fun);

protected:
    Node* mRoot;
};

} // namespace sead

#include <container/seadTreeMapImpl.hpp>
