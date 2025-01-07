#pragma once

#include <container/seadTreeMapImpl.h>

namespace sead {

template <typename Key>
TreeMapNode<Key>* TreeMapImpl<Key>::find(Node* node, const Key& key) const
{
    while (node)
    {
        s32 cmp = key.compare(node->mKey_);
        if (cmp < 0)
        {
            node = node->mLeft_;
        }
        else if (cmp > 0)
        {
            node = node->mRight_;
        }
        else
        {
            return node;
        }
    }

    return nullptr;
}

template <typename Key>
TreeMapNode<Key>* TreeMapImpl<Key>::insert(Node* h, Node* node)
{
    if (!h)
    {
        node->mColor_ = Node::cRed_;
        node->mLeft_ = nullptr;
        node->mRight_ = nullptr;
        return node;
    }

    s32 cmp = node->mKey_.compare(h->mKey_);
    if (cmp < 0)
    {
        h->mLeft_ = insert(h->mLeft_, node);
    }
    else if (cmp > 0)
    {
        h->mRight_ = insert(h->mRight_, node);
    }
    else
    {
        if (h != node)
        {
            node->mRight_ = h->mRight_;
            node->mLeft_ = h->mLeft_;
            node->mColor_ = h->mColor_;
            h->erase_();
        }

        h = node;
    }

    if (isRed(h->mRight_) && !isRed(h->mLeft_))
    {
        h = rotateLeft(h);
    }

    if (isRed(h->mLeft_) && isRed(h->mLeft_->mLeft_))
    {
        h = rotateRight(h);
    }

    if (isRed(h->mLeft_) && isRed(h->mRight_))
    {
        flipColors(h);
    }

    return h;
}

template <typename Key>
TreeMapNode<Key>* TreeMapImpl<Key>::erase(Node* h, const Key& key)
{
    if (key.compare(h->mKey_) < 0)
    {
        if (!isRed(h->mLeft_) && !isRed(h->mLeft_->mLeft_))
        {
            h = moveRedLeft(h);
        }

        h->mLeft_ = erase(h->mLeft_, key);
    }
    else
    {
        if (isRed(h->mLeft_))
        {
            h = rotateRight(h);
        }

        if (key.compare(h->mKey_) == 0 && !h->mRight_)
        {
            h->erase_();
            return nullptr;
        }

        if (!isRed(h->mRight_) && !isRed(h->mRight_->mLeft_))
        {
            h = moveRedRight(h);
        }

        if (key.compare(h->mKey_) == 0)
        {
            Node* newh = find(h->mRight_, getMin(h->mRight_)->mKey_);
            newh->mRight_ = eraseMin(h->mRight_);
            newh->mLeft_ = h->mLeft_;
            newh->mColor_ = h->mColor_;
            h->erase_();
            h = newh;
        }
        else
        {
            h->mRight_ = erase(h->mRight_, key);
        }
    }

    return fixUp(h);
}

template <typename Key>
inline TreeMapNode<Key>* TreeMapImpl<Key>::rotateLeft(Node* h)
{
    Node* x = h->mRight_;
    h->mRight_ = x->mLeft_;
    x->mLeft_ = h;
    x->mColor_ = h->mColor_;
    h->mColor_ = Node::cRed_;
    return x;
}

template <typename Key>
inline TreeMapNode<Key>* TreeMapImpl<Key>::rotateRight(Node* h)
{
    Node* x = h->mLeft_;
    h->mLeft_ = x->mRight_;
    x->mRight_ = h;
    x->mColor_ = h->mColor_;
    h->mColor_ = Node::cRed_;
    return x;
}

template <typename Key>
inline void TreeMapImpl<Key>::flipColors(Node* h)
{
    h->mColor_ = !h->mColor_;
    h->mLeft_->mColor_ = !h->mLeft_->mColor_;
    h->mRight_->mColor_ = !h->mRight_->mColor_;
}

template <typename Key>
inline bool TreeMapImpl<Key>::isRed(Node* h)
{
    if (!h)
    {
        return false;
    }

    return h->mColor_ == Node::cRed_;
}

template <typename Key>
inline TreeMapNode<Key>* TreeMapImpl<Key>::fixUp(Node* h)
{
    if (isRed(h->mRight_))
    {
        h = rotateLeft(h);
    }

    if (isRed(h->mLeft_) && isRed(h->mLeft_->mLeft_))
    {
        h = rotateRight(h);
    }

    if (isRed(h->mLeft_) && isRed(h->mRight_))
    {
        flipColors(h);
    }

    return h;
}

template <typename Key>
inline TreeMapNode<Key>* TreeMapImpl<Key>::moveRedLeft(Node* h)
{
    flipColors(h);

    if (isRed(h->mRight_->mLeft_))
    {
        h->mRight_ = rotateRight(h->mRight_);
        h = rotateLeft(h);
        flipColors(h);
    }

    return h;
}

template <typename Key>
inline TreeMapNode<Key>* TreeMapImpl<Key>::moveRedRight(Node* h)
{
    flipColors(h);

    if (isRed(h->mLeft_->mLeft_))
    {
        h = rotateRight(h);
        flipColors(h);
    }

    return h;
}

template <typename Key>
inline TreeMapNode<Key>* TreeMapImpl<Key>::eraseMin(Node* h)
{
    if (!h->mLeft_)
    {
        return nullptr;
    }

    if (!isRed(h->mLeft_) && !isRed(h->mLeft_->mLeft_))
    {
        h = moveRedLeft(h);
    }

    h->mLeft_ = eraseMin(h->mLeft_);

    return fixUp(h);
}

template <typename Key>
inline TreeMapNode<Key>* TreeMapImpl<Key>::getMin(Node* h)
{
    while (h->mLeft_)
    {
        h = h->mLeft_;
    }

    return h;
}

template <typename Key>
template <typename T>
inline void TreeMapImpl<Key>::forEach(Node* node, const T& fun)
{
    if (node->mLeft_)
    {
        forEach(node->mLeft_, fun);
    }

    fun(node);

    if (node->mRight_)
    {
        forEach(node->mRight_, fun);
    }
}

} // namespace sead
