#pragma once

#include <container/seadListImpl.h>

namespace sead {

template <typename T>
class TList;

template <typename T>
class TListNode : public ListNode
{
public:
    TListNode()
        : ListNode()
        , mData(nullptr)
        , mList(nullptr)
    {
    }

    explicit TListNode(T ptr)
        : ListNode()
        , mData(ptr)
        , mList(nullptr)
    {
    }

    void insertBack(TListNode* node)
    {
        SEAD_ASSERT(mList);
        mList->insertAfter(this, node);
    }

    void insertFront(TListNode* node)
    {
        SEAD_ASSERT(mList);
        mList->insertBefore(this, node);
    }

    void erase()
    {
        if (mList)
            mList->erase(this);
    }

    T& val() { return mData; }
    const T& val() const { return mData; }

protected:
    T mData;
    TList<T>* mList;

    friend class TList<T>;
};

// TODO: Implement all functions
template <typename T>
class TList : public ListImpl
{
private:
    using CompareCallback = s32 (*)(const T*, const T*);

public:
    TList()
        : ListImpl()
    {
    }

    void pushBack(TListNode<T>* obj)
    {
        obj->erase();
        obj->mList = this;
        ListImpl::pushBack(obj);
    }

    void pushFront(TListNode<T>* obj)
    {
        obj->erase();
        obj->mList = this;
        ListImpl::pushFront(obj);
    }

    TListNode<T>* popBack()
    {
        TListNode<T>* node = static_cast<TListNode<T>*>(ListImpl::popBack());
        if (node)
            node->mList = nullptr;

        return node;
    }

    TListNode<T>* popFront()
    {
        TListNode<T>* node = static_cast<TListNode<T>*>(ListImpl::popFront());
        if (node)
            node->mList = nullptr;

        return node;
    }

    void insertBefore(TListNode<T>* basis, TListNode<T>* obj)
    {
        obj->erase();
        obj->mList = this;
        ListImpl::insertBefore(basis, obj);
    }

    void insertAfter(TListNode<T>* basis, TListNode<T>* obj)
    {
        obj->erase();
        obj->mList = this;
        ListImpl::insertAfter(basis, obj);
    }

    void erase(TListNode<T>* obj)
    {
        if (!obj->mList)
            return;

        obj->mList = nullptr;
        ListImpl::erase(obj);
    }

    TListNode<T>* front() const { return static_cast<TListNode<T>*>(ListImpl::front()); }
    TListNode<T>* back() const { return static_cast<TListNode<T>*>(ListImpl::back()); }

    TListNode<T>* prev(const TListNode<T>* obj) const
    {
        ListNode* node = obj->prev();
        if (node == &mStartEnd)
            return nullptr;

        return static_cast<TListNode<T>*>(node);
    }

    TListNode<T>* next(const TListNode<T>* obj) const
    {
        ListNode* node = obj->next();
        if (node == &mStartEnd)
            return nullptr;

        return static_cast<TListNode<T>*>(node);
    }

    TListNode<T>* nth(s32 index) const;
    s32 indexOf(const TListNode<T>* obj) const;
    bool isNodeLinked(const TListNode<T>* obj) const;
    void swap(TListNode<T>* obj1, TListNode<T>* obj2);
    void moveAfter(TListNode<T>* basis, TListNode<T>* obj);
    void moveBefore(TListNode<T>* basis, TListNode<T>* obj);
    void clear();
    void unsafeClear();
    void sort();
    void sort(CompareCallback cmp);
    void mergeSort();
    void mergeSort(CompareCallback cmp);
    TListNode<T>* find(const T* obj) const;
    TListNode<T>* find(const T* obj, CompareCallback cmp) const;
    void uniq();
    void uniq(CompareCallback cmp);

public:
    class iterator
    {
    public:
        iterator(TListNode<T>* ptr)
            : mPtr(ptr)
        {
        }

        iterator& operator++()
        {
            mPtr = static_cast<TListNode<T>*>(mPtr->next());
            return *this;
        }

        T& operator*() const
        {
            return mPtr->mData;
        }

        T* operator->() const
        {
            return &mPtr->mData;
        }

        friend bool operator==(const iterator& it1, const iterator& it2)
        {
            return it1.mPtr == it2.mPtr;
        }

        friend bool operator!=(const iterator& it1, const iterator& it2)
        {
            return it1.mPtr != it2.mPtr;
        }

    protected:
        TListNode<T>* mPtr;

        friend class constIterator;
    };

    class constIterator
    {
    public:
        constIterator(const TListNode<T>* ptr)
            : mPtr(ptr)
        {
        }

        constIterator(const iterator& it)
            : mPtr(it.mPtr)
        {
        }

        constIterator& operator++()
        {
            mPtr = static_cast<const TListNode<T>*>(mPtr->next());
            return *this;
        }

        const T& operator*() const
        {
            return mPtr->mData;
        }

        const T* operator->() const
        {
            return &mPtr->mData;
        }

        friend bool operator==(const constIterator& it1, const constIterator& it2)
        {
            return it1.mPtr == it2.mPtr;
        }

        friend bool operator!=(const constIterator& it1, const constIterator& it2)
        {
            return it1.mPtr != it2.mPtr;
        }

    protected:
        const TListNode<T>* mPtr;
    };

    class robustIterator
    {
    public:
        robustIterator(TListNode<T>* ptr)
            : mPtr(ptr)
            , mNext(static_cast<TListNode<T>*>(ptr->next()))
        {
        }

        robustIterator& operator++()
        {
            mPtr = mNext;
            mNext = static_cast<TListNode<T>*>(mNext->next());
            return *this;
        }

        TListNode<T>& operator*() const
        {
            return *mPtr;
        }

        TListNode<T>* operator->() const
        {
            return mPtr;
        }

        friend bool operator==(const robustIterator& it1, const robustIterator& it2)
        {
            return it1.mPtr == it2.mPtr;
        }

        friend bool operator!=(const robustIterator& it1, const robustIterator& it2)
        {
            return it1.mPtr != it2.mPtr;
        }

    protected:
        TListNode<T>* mPtr;
        TListNode<T>* mNext;
    };

    // TODO
    class reverseIterator { };

    // TODO
    class reverseConstIterator { };

    // TODO
    class reverseRobustIterator { };

    iterator begin() const { return iterator(static_cast<TListNode<T>*>(mStartEnd.next())); }
    iterator end() const { return iterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    iterator toIterator(TListNode<T>* obj) const { return iterator(obj); }

    constIterator constBegin() const { return constIterator(static_cast<TListNode<T>*>(mStartEnd.next())); }
    constIterator constEnd() const { return constIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    constIterator toConstIterator(const TListNode<T>* obj) const { return constIterator(obj); }

    robustIterator robustBegin() const { return robustIterator(static_cast<TListNode<T>*>(mStartEnd.next())); }
    robustIterator robustEnd() const { return robustIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    robustIterator toRobustIterator(TListNode<T>* obj) const { return robustIterator(obj); }

    reverseIterator reverseBegin() const;
    reverseIterator reverseEnd() const;
    reverseIterator toReverseIterator(TListNode<T>* obj) const;

    reverseConstIterator reverseConstBegin() const;
    reverseConstIterator reverseConstEnd() const;
    reverseConstIterator toReverseConstIterator(const TListNode<T>* obj) const;

    reverseRobustIterator reverseRobustBegin() const;
    reverseRobustIterator reverseRobustEnd() const;
    reverseRobustIterator toReverseRobustIterator(TListNode<T>* obj) const;

protected:
    static s32 compareT(const T* a, const T* b)
    {
        if (*a < *b)
            return -1;
        if (*b < *a)
            return 1;

        return 0;
    }
};

} // namespace sead
