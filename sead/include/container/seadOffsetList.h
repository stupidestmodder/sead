#pragma once

#include <container/seadListImpl.h>
#include <prim/seadPtrUtil.h>

namespace sead {

template <typename T>
class OffsetList : public ListImpl
{
protected:
    using CompareCallback = s32 (*)(const T* a, const T* b);

public:
    OffsetList()
        : ListImpl()
        , mOffset(-1)
    {
    }

    void initOffset(s32 offset) { mOffset = offset; }

    void clear() { ListImpl::clear(); }
    void unsafeClear() { ListImpl::unsafeClear(); }

    void pushBack(T* obj)
    {
        SEAD_ASSERT(mOffset >= 0);
        ListImpl::pushBack(objToListNode(obj));
    }

    void pushFront(T* obj)
    {
        SEAD_ASSERT(mOffset >= 0);
        ListImpl::pushFront(objToListNode(obj));
    }

    T* popBack() { return listNodeToObjWithNullCheck(ListImpl::popBack()); }
    T* popFront() { return listNodeToObjWithNullCheck(ListImpl::popFront()); }

    void insertBefore(const T* basis, T* obj)
    {
        ListImpl::insertBefore(objToListNode(basis), objToListNode(obj));
    }

    void insertAfter(const T* basis, T* obj)
    {
        ListImpl::insertAfter(objToListNode(basis), objToListNode(obj));
    }

    void erase(T* obj)
    {
        ListImpl::erase(objToListNode(obj));
    }

    T* front() const { return listNodeToObjWithNullCheck(ListImpl::front()); }
    T* back() const { return listNodeToObjWithNullCheck(ListImpl::back()); }

    T* prev(const T* obj) const
    {
        ListNode* prev = objToListNode(obj)->prev();
        if (prev == &mStartEnd)
        {
            return nullptr;
        }

        return listNodeToObj(prev);
    }

    T* next(const T* obj) const
    {
        ListNode* next = objToListNode(obj)->next();
        if (next == &mStartEnd)
        {
            return nullptr;
        }

        return listNodeToObj(next);
    }

    T* nth(s32 index) const { return listNodeToObjWithNullCheck(ListImpl::nth(index)); }
    s32 indexOf(const T* obj) const { return ListImpl::indexOf(objToListNode(obj)); }

    bool isNodeLinked(const T* obj) const { return objToListNode(obj)->isLinked(); }

    void swap(T* obj1, T* obj2) { ListImpl::swap(objToListNode(obj1), objToListNode(obj2)); }
    void moveAfter(T* basis, T* obj) { ListImpl::moveAfter(objToListNode(basis), objToListNode(obj)); }
    void moveBefore(T* basis, T* obj) { ListImpl::moveBefore(objToListNode(basis), objToListNode(obj)); }

    void sort() { sort(&compareT); }
    void sort(CompareCallback cmp)
    {
        ListImpl::sort(mOffset, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    void mergeSort() { mergeSort(&compareT); }
    void mergeSort(CompareCallback cmp)
    {
        ListImpl::mergeSort(mOffset, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    T* find(const T* obj) const { return find(obj, &compareT); }
    T* find(const T* obj, CompareCallback cmp) const
    {
        return listNodeToObjWithNullCheck(ListImpl::find(obj, mOffset, reinterpret_cast<CompareCallbackImpl>(cmp)));
    }

    void uniq() { uniq(&compareT); }
    void uniq(CompareCallback cmp)
    {
        ListImpl::uniq(mOffset, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

public:
    class iterator
    {
    public:
        iterator(T* ptr, s32 offset)
            : mPtr(ptr)
            , mOffset(offset)
        {
        }

        iterator& operator++()
        {
            ListNode* next = static_cast<ListNode*>(PtrUtil::addOffset(mPtr, mOffset))->next();
            mPtr = static_cast<T*>(PtrUtil::addOffset(next, -mOffset));
            return *this;
        }

        T& operator*() const { return *mPtr; }
        T* operator->() const { return mPtr; }

        friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        T* mPtr;
        s32 mOffset;

        friend class constIterator;
    };

    class constIterator
    {
    public:
        constIterator(const T* ptr, s32 offset)
            : mPtr(ptr)
            , mOffset(offset)
        {
        }

        constIterator(const iterator& it)
            : mPtr(it.mPtr)
            , mOffset(it.mOffset)
        {
        }

        constIterator& operator++()
        {
            const ListNode* next = static_cast<const ListNode*>(PtrUtil::addOffset(mPtr, mOffset))->next();
            mPtr = static_cast<const T*>(PtrUtil::addOffset(next, -mOffset));
            return *this;
        }

        const T& operator*() const { return *mPtr; }
        const T* operator->() const { return mPtr; }

        friend bool operator==(const constIterator& lhs, const constIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const constIterator& lhs, const constIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        const T* mPtr;
        s32 mOffset;
    };

    class robustIterator
    {
    public:
        robustIterator(T* ptr, s32 offset)
            : mPtr(ptr)
            , mNext(static_cast<ListNode*>(PtrUtil::addOffset(ptr, offset))->next())
            , mOffset(offset)
        {
        }

        robustIterator& operator++()
        {
            mPtr = static_cast<T*>(PtrUtil::addOffset(mNext, -mOffset));
            mNext = mNext->next();
            return *this;
        }

        T& operator*() const { return *mPtr; }
        T* operator->() const { return mPtr; }

        friend bool operator==(const robustIterator& lhs, const robustIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const robustIterator& lhs, const robustIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        T* mPtr;
        ListNode* mNext;
        s32 mOffset;
    };

    class reverseIterator
    {
    public:
        reverseIterator(T* ptr, s32 offset)
            : mPtr(ptr)
            , mOffset(offset)
        {
        }

        reverseIterator& operator++()
        {
            ListNode* prev = static_cast<ListNode*>(PtrUtil::addOffset(mPtr, mOffset))->prev();
            mPtr = static_cast<T*>(PtrUtil::addOffset(prev, -mOffset));
            return *this;
        }

        T& operator*() const { return *mPtr; }
        T* operator->() const { return mPtr; }

        friend bool operator==(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        T* mPtr;
        s32 mOffset;

        friend class reverseConstIterator;
    };

    class reverseConstIterator
    {
    public:
        reverseConstIterator(const T* ptr, s32 offset)
            : mPtr(ptr)
            , mOffset(offset)
        {
        }

        reverseConstIterator(const reverseIterator& it)
            : mPtr(it.mPtr)
            , mOffset(it.mOffset)
        {
        }

        reverseConstIterator& operator++()
        {
            const ListNode* prev = static_cast<const ListNode*>(PtrUtil::addOffset(mPtr, mOffset))->prev();
            mPtr = static_cast<const T*>(PtrUtil::addOffset(prev, -mOffset));
            return *this;
        }

        const T& operator*() const { return *mPtr; }
        const T* operator->() const { return mPtr; }

        friend bool operator==(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        const T* mPtr;
        s32 mOffset;
    };

    class reverseRobustIterator
    {
    public:
        reverseRobustIterator(T* ptr, s32 offset)
            : mPtr(ptr)
            , mPrev(static_cast<ListNode*>(PtrUtil::addOffset(ptr, offset))->prev())
            , mOffset(offset)
        {
        }

        reverseRobustIterator& operator++()
        {
            mPtr = static_cast<T*>(PtrUtil::addOffset(mPrev, -mOffset));
            mPrev = mPrev->prev();
            return *this;
        }

        T& operator*() const { return *mPtr; }
        T* operator->() const { return mPtr; }

        friend bool operator==(const reverseRobustIterator& lhs, const reverseRobustIterator& rhs) { return lhs.mPtr == rhs.mPtr; }
        friend bool operator!=(const reverseRobustIterator& lhs, const reverseRobustIterator& rhs) { return lhs.mPtr != rhs.mPtr; }

    protected:
        T* mPtr;
        ListNode* mPrev;
        s32 mOffset;
    };

public:
    iterator begin() const { return iterator(listNodeToObj(mStartEnd.next()), mOffset); }
    // constIterator begin() const { return constIterator(listNodeToObj(mStartEnd.next()), mOffset); }
    iterator end() const { return iterator(listNodeToObj(&mStartEnd), mOffset); }
    // constIterator end() const { return constIterator(listNodeToObj(&mStartEnd), mOffset); }
    iterator toIterator(T* obj) const { return iterator(obj, mOffset); }
    // constIterator toIterator(const T* obj) const { return constIterator(obj, mOffset); }

    constIterator constBegin() const { return constIterator(listNodeToObj(mStartEnd.next()), mOffset); }
    constIterator constEnd() const { return constIterator(listNodeToObj(&mStartEnd), mOffset); }
    constIterator toConstIterator(const T* obj) const { return constIterator(obj, mOffset); }

    robustIterator robustBegin() { return robustIterator(listNodeToObj(mStartEnd.next()), mOffset); }
    robustIterator robustEnd() { return robustIterator(listNodeToObj(&mStartEnd), mOffset); }
    robustIterator toRobustIterator(T* obj) { return robustIterator(obj, mOffset); }

    reverseIterator reverseBegin() const { return reverseIterator(listNodeToObj(mStartEnd.prev()), mOffset); }
    // reverseConstIterator reverseBegin() const { return reverseConstIterator(listNodeToObj(mStartEnd.prev()), mOffset); }
    reverseIterator reverseEnd() const { return reverseIterator(listNodeToObj(&mStartEnd), mOffset); }
    // reverseConstIterator reverseEnd() const { return reverseConstIterator(listNodeToObj(&mStartEnd), mOffset); }
    reverseIterator toReverseIterator(T* obj) const { return reverseIterator(obj, mOffset); }
    // reverseConstIterator toReverseIterator(const T* obj) const { return reverseConstIterator(obj, mOffset); }

    reverseConstIterator reverseConstBegin() const { return reverseConstIterator(listNodeToObj(mStartEnd.prev()), mOffset); }
    reverseConstIterator reverseConstEnd() const { return reverseConstIterator(listNodeToObj(&mStartEnd), mOffset); }
    reverseConstIterator toReverseConstIterator(const T* obj) const { return reverseConstIterator(obj, mOffset); }

    reverseRobustIterator reverseRobustBegin() { return reverseRobustIterator(listNodeToObj(mStartEnd.prev()), mOffset); }
    reverseRobustIterator reverseRobustEnd() { return reverseRobustIterator(listNodeToObj(&mStartEnd), mOffset); }
    reverseRobustIterator toReverseRobustIterator(T* obj) { return reverseRobustIterator(obj, mOffset); }

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

    ListNode* objToListNode(const T* obj) const
    {
        return reinterpret_cast<ListNode*>(PtrUtil::addOffset(obj, mOffset));
    }

    T* listNodeToObj(const ListNode* node) const
    {
        return reinterpret_cast<T*>(PtrUtil::addOffset(node, -mOffset));
    }

    T* listNodeToObjWithNullCheck(const ListNode* node) const
    {
        return node ? listNodeToObj(node) : nullptr;
    }

protected:
    s32 mOffset;
};

} // namespace sead
