#pragma once

#include <basis/seadAssert.h>
#include <container/seadListImpl.h>
#include <prim/seadPtrUtil.h>

namespace sead {

template <typename T>
class OffsetList : public ListImpl
{
protected:
    using CompareCallback = s32 (*)(const T*, const T*);

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
        ListNode* node = objToListNode(obj)->prev();
        if (node == &mStartEnd)
            return nullptr;

        return listNodeToObj(node);
    }

    T* next(const T* obj) const
    {
        ListNode* node = objToListNode(obj)->next();
        if (node == &mStartEnd)
            return nullptr;

        return listNodeToObj(node);
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
        return listNodeToObj(ListImpl::find(obj, mOffset, reinterpret_cast<CompareCallbackImpl>(cmp)));
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

        T& operator*() const
        {
            return *mPtr;
        }

        T* operator->() const
        {
            return mPtr;
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

        const T& operator*() const
        {
            return *mPtr;
        }

        const T* operator->() const
        {
            return mPtr;
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

        T& operator*() const
        {
            return *mPtr;
        }

        T* operator->() const
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
        T* mPtr;
        ListNode* mNext;
        s32 mOffset;
    };

    // TODO
    class reverseIterator { };

    // TODO
    class reverseConstIterator { };

    // TODO
    class reverseRobustIterator { };

    iterator begin() const { return iterator(listNodeToObj(mStartEnd.next()), mOffset); }
    iterator end() const { return iterator(listNodeToObj(&mStartEnd), mOffset); }
    iterator toIterator(T* obj) const { return iterator(obj, mOffset); }

    constIterator constBegin() const { return constIterator(listNodeToObj(mStartEnd.next()), mOffset); }
    constIterator constEnd() const { return constIterator(listNodeToObj(&mStartEnd), mOffset); }
    constIterator toConstIterator(const T* obj) const { return constIterator(obj, mOffset); }

    robustIterator robustBegin() { return robustIterator(listNodeToObj(mStartEnd.next()), mOffset); }
    robustIterator robustEnd() { return robustIterator(listNodeToObj(&mStartEnd), mOffset); }
    robustIterator toRobustIterator(T* obj) { return robustIterator(obj, mOffset); }

    reverseIterator reverseBegin() const;
    reverseIterator reverseEnd() const;
    reverseIterator toReverseIterator(T* obj) const;

    reverseConstIterator reverseConstBegin() const;
    reverseConstIterator reverseConstEnd() const;
    reverseConstIterator toReverseConstIterator(const T* obj) const;

    reverseRobustIterator reverseRobustBegin();
    reverseRobustIterator reverseRobustEnd();
    reverseRobustIterator toReverseRobustIterator(T* obj);

protected:
    static s32 compareT(const T* a, const T* b)
    {
        if (*a < *b)
            return -1;
        if (*b < *a)
            return 1;

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
