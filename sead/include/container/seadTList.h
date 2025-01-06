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
        , mData()
        , mList(nullptr)
    {
    }

    explicit TListNode(T ptr)
        : ListNode()
        , mData(ptr)
        , mList(nullptr)
    {
    }

    void insertBack(TListNode* n)
    {
        SEAD_ASSERT(mList);
        mList->insertAfter(this, n);
    }

    void insertFront(TListNode* n)
    {
        SEAD_ASSERT(mList);
        mList->insertBefore(this, n);
    }

    void erase()
    {
        if (mList)
        {
            mList->erase(this);
        }
    }

    T& val() { return mData; }
    const T& val() const { return mData; }

    TList<T>* list() { return mList; }
    const TList<T>* list() const { return mList; }

protected:
    T mData;
    TList<T>* mList;

    friend class TList<T>;
};

template <typename T>
class TList : public ListImpl
{
protected:
    using CompareCallback = s32 (*)(const T* a, const T* b);

public:
    TList()
        : ListImpl()
    {
    }

    void pushBack(TListNode<T>* obj)
    {
        if (obj->mList)
        {
            obj->mList->erase(obj);
        }

        obj->mList = this;
        ListImpl::pushBack(obj);
    }

    void pushFront(TListNode<T>* obj)
    {
        if (obj->mList)
        {
            obj->mList->erase(obj);
        }

        obj->mList = this;
        ListImpl::pushFront(obj);
    }

    TListNode<T>* popBack()
    {
        TListNode<T>* node = static_cast<TListNode<T>*>(ListImpl::popBack());
        if (node)
        {
            node->mList = nullptr;
        }

        return node;
    }

    TListNode<T>* popFront()
    {
        TListNode<T>* node = static_cast<TListNode<T>*>(ListImpl::popFront());
        if (node)
        {
            node->mList = nullptr;
        }

        return node;
    }

    void insertBefore(TListNode<T>* basis, TListNode<T>* obj)
    {
        if (obj->mList)
        {
            obj->mList->erase(obj);
        }

        obj->mList = this;
        ListImpl::insertBefore(basis, obj);
    }

    void insertAfter(TListNode<T>* basis, TListNode<T>* obj)
    {
        if (obj->mList)
        {
            obj->mList->erase(obj);
        }

        obj->mList = this;
        ListImpl::insertAfter(basis, obj);
    }

    void erase(TListNode<T>* obj)
    {
        if (obj->mList)
        {
            obj->mList = nullptr;
            ListImpl::erase(obj);
        }
    }

    TListNode<T>* front() const { return static_cast<TListNode<T>*>(ListImpl::front()); }
    TListNode<T>* back() const { return static_cast<TListNode<T>*>(ListImpl::back()); }

    TListNode<T>* prev(const TListNode<T>* obj) const
    {
        SEAD_ASSERT(obj->mList == this);

        ListNode* prev = obj->prev();
        if (prev == &mStartEnd)
        {
            return nullptr;
        }

        return static_cast<TListNode<T>*>(prev);
    }

    TListNode<T>* next(const TListNode<T>* obj) const
    {
        SEAD_ASSERT(obj->mList == this);

        ListNode* next = obj->next();
        if (next == &mStartEnd)
        {
            return nullptr;
        }

        return static_cast<TListNode<T>*>(next);
    }

    TListNode<T>* nth(s32 index) const { return static_cast<TListNode<T>*>(ListImpl::nth(index)); }
    s32 indexOf(const TListNode<T>* obj) const { return ListImpl::indexOf(obj); }

    bool isNodeLinked(const TListNode<T>* obj) const { return obj->isLinked(); }

    void swap(TListNode<T>* obj1, TListNode<T>* obj2) { ListImpl::swap(obj1, obj2); }
    void moveAfter(TListNode<T>* basis, TListNode<T>* obj) { ListImpl::moveAfter(basis, obj); }
    void moveBefore(TListNode<T>* basis, TListNode<T>* obj) { ListImpl::moveBefore(basis, obj); }

    void clear()
    {
        ListNode* node = mStartEnd.next();
        while (node != &mStartEnd)
        {
            ListNode* temp = node->next();
            erase(static_cast<TListNode<T>*>(node));
            node = temp;
        }
    }

    void unsafeClear() { ListImpl::unsafeClear(); }

    void sort() { sort(&compareT); }
    void sort(CompareCallback cmp)
    {
        ListImpl::sort(-static_cast<s32>(offsetof(TListNode<T>, mData)), reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    void mergeSort() { mergeSort(&compareT); }
    void mergeSort(CompareCallback cmp)
    {
        ListImpl::mergeSort(-static_cast<s32>(offsetof(TListNode<T>, mData)), reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    TListNode<T>* find(const T* obj) const { return find(obj, &compareT); }
    TListNode<T>* find(const T* obj, CompareCallback cmp) const
    {
        return static_cast<TListNode<T>*>(ListImpl::find(obj, -static_cast<s32>(offsetof(TListNode<T>, mData)), reinterpret_cast<CompareCallbackImpl>(cmp)));
    }

    void uniq() { uniq(&compareT); }
    void uniq(CompareCallback cmp)
    {
        ListImpl::uniq(-static_cast<s32>(offsetof(TListNode<T>, mData)), reinterpret_cast<CompareCallbackImpl>(cmp));
    }

public:
    class iterator
    {
    public:
        explicit iterator(TListNode<T>* ptr)
            : mPtr(ptr)
        {
        }

        iterator& operator++()
        {
            mPtr = static_cast<TListNode<T>*>(mPtr->next());
            return *this;
        }

        T& operator*() const { return mPtr->mData; }
        T* operator->() const { return &mPtr->mData; }

        friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        TListNode<T>* mPtr;

        friend class constIterator;
    };

    class constIterator
    {
    public:
        explicit constIterator(const TListNode<T>* ptr)
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

        const T& operator*() const { return mPtr->mData; }
        const T* operator->() const { return &mPtr->mData; }

        friend bool operator==(const constIterator& lhs, const constIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const constIterator& lhs, const constIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        const TListNode<T>* mPtr;
    };

    class robustIterator
    {
    public:
        explicit robustIterator(TListNode<T>* ptr)
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

        TListNode<T>& operator*() const { return *mPtr; }
        TListNode<T>* operator->() const { return mPtr; }

        friend bool operator==(const robustIterator& lhs, const robustIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const robustIterator& lhs, const robustIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        TListNode<T>* mPtr;
        TListNode<T>* mNext;
    };

    class reverseIterator
    {
    public:
        explicit reverseIterator(TListNode<T>* ptr)
            : mPtr(ptr)
        {
        }

        reverseIterator& operator++()
        {
            mPtr = static_cast<TListNode<T>*>(mPtr->prev());
            return *this;
        }

        T& operator*() const { return mPtr->mData; }
        T* operator->() const { return &mPtr->mData; }

        friend bool operator==(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        TListNode<T>* mPtr;

        friend class reverseConstIterator;
    };

    class reverseConstIterator
    {
    public:
        explicit reverseConstIterator(const TListNode<T>* ptr)
            : mPtr(ptr)
        {
        }

        reverseConstIterator(const reverseIterator& it)
            : mPtr(it.mPtr)
        {
        }

        reverseConstIterator& operator++()
        {
            mPtr = static_cast<const TListNode<T>*>(mPtr->prev());
            return *this;
        }

        const T& operator*() const { return mPtr->mData; }
        const T* operator->() const { return &mPtr->mData; }

        friend bool operator==(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        const TListNode<T>* mPtr;
    };

    class reverseRobustIterator
    {
    public:
        explicit reverseRobustIterator(TListNode<T>* ptr)
            : mPtr(ptr)
            , mPrev(static_cast<TListNode<T>*>(ptr->prev()))
        {
        }

        reverseRobustIterator& operator++()
        {
            mPtr = mPrev;
            mPrev = static_cast<TListNode<T>*>(mPrev->prev());
            return *this;
        }

        TListNode<T>& operator*() const { return *mPtr; }
        TListNode<T>* operator->() const { return mPtr; }

        friend bool operator==(const reverseRobustIterator& lhs, const reverseRobustIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const reverseRobustIterator& lhs, const reverseRobustIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        TListNode<T>* mPtr;
        TListNode<T>* mPrev;
    };

public:
    iterator begin() const { return iterator(static_cast<TListNode<T>*>(mStartEnd.next())); }
    // constIterator begin() const { return constIterator(static_cast<TListNode<T>*>(mStartEnd.next())); }
    iterator end() const { return iterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    // constIterator end() const { return constIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    iterator toIterator(TListNode<T>* obj) const { return iterator(obj); }
    // constIterator toIterator(const TListNode<T>* obj) const { return constIterator(obj); }

    constIterator constBegin() const { return constIterator(static_cast<TListNode<T>*>(mStartEnd.next())); }
    constIterator constEnd() const { return constIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    constIterator toConstIterator(const TListNode<T>* obj) const { return constIterator(obj); }

    robustIterator robustBegin() const { return robustIterator(static_cast<TListNode<T>*>(mStartEnd.next())); }
    robustIterator robustEnd() const { return robustIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    robustIterator toRobustIterator(TListNode<T>* obj) const { return robustIterator(obj); }

    reverseIterator reverseBegin() const { return reverseIterator(static_cast<TListNode<T>*>(mStartEnd.prev())); }
    // reverseConstIterator reverseBegin() const { return reverseConstIterator(static_cast<TListNode<T>*>(mStartEnd.prev())); }
    reverseIterator reverseEnd() const { return reverseIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    // reverseConstIterator reverseEnd() const { return reverseConstIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    reverseIterator toReverseIterator(TListNode<T>* obj) const { return reverseIterator(obj); }
    // reverseConstIterator toReverseIterator(const TListNode<T>* obj) const { return reverseConstIterator(obj); }

    reverseConstIterator reverseConstBegin() const { return reverseConstIterator(static_cast<TListNode<T>*>(mStartEnd.prev())); }
    reverseConstIterator reverseConstEnd() const { return reverseConstIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    reverseConstIterator toReverseConstIterator(const TListNode<T>* obj) const { return reverseConstIterator(obj); }

    reverseRobustIterator reverseRobustBegin() const { return reverseRobustIterator(static_cast<TListNode<T>*>(mStartEnd.prev())); }
    reverseRobustIterator reverseRobustEnd() const { return reverseRobustIterator(static_cast<TListNode<T>*>(const_cast<ListNode*>(&mStartEnd))); }
    reverseRobustIterator toReverseRobustIterator(TListNode<T>* obj) const { return reverseRobustIterator(obj); }

protected:
    static s32 compareT(const T* a, const T* b)
    {
        if (*a < *b)
        {
            return -1;
        }

        if (*b < *a)
        {
            return 1;
        }

        return 0;
    }
};

} // namespace sead
