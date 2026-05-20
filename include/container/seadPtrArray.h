#pragma once

#include <basis/seadNew.h>
#include <prim/seadMemUtil.h>
#include <random/seadRandom.h>

namespace sead {

class Heap;

class PtrArrayImpl
{
    SEAD_NO_COPY(PtrArrayImpl);

protected:
    using CompareCallbackImpl = s32 (*)(const void* a, const void* b);

public:
    PtrArrayImpl()
        : mPtrNum(0)
        , mPtrNumMax(0)
        , mPtrs(nullptr)
    {
    }

    PtrArrayImpl(s32 ptrNumMax, void* buf)
        : mPtrNum(0)
        , mPtrNumMax(0)
        , mPtrs(nullptr)
    {
        setBuffer(ptrNumMax, buf);
    }

    void allocBuffer(s32 ptrNumMax, s32 alignment = cDefaultAlignment);
    void allocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = cDefaultAlignment);
    bool tryAllocBuffer(s32 ptrNumMax, s32 alignment = cDefaultAlignment);
    bool tryAllocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = cDefaultAlignment);
    void freeBuffer();
    void setBuffer(s32 ptrNumMax, void* buf);

    bool isBufferReady() const { return mPtrs != nullptr; }
    bool isEmpty() const { return mPtrNum == 0; }
    bool isFull() const { return mPtrNum >= mPtrNumMax; }

    s32 size() const { return mPtrNum; }
    s32 maxSize() const { return mPtrNumMax; }

    void erase(s32 pos) { erase(pos, 1); }
    void erase(s32 pos, s32 num);

    void clear() { mPtrNum = 0; }

    // TODO
    void resize(s32);
    // TODO
    void unsafeResize(s32);

    void swap(s32 pos1, s32 pos2)
    {
        if (static_cast<u32>(pos1) >= static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "pos1[%d] exceed size[%d]", pos1, mPtrNum);
            return;
        }
    
        if (static_cast<u32>(pos2) >= static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "pos2[%d] exceed size[%d]", pos2, mPtrNum);
            return;
        }

        if (pos1 == pos2)
        {
            return;
        }

        void* ptr = mPtrs[pos1];
        mPtrs[pos1] = mPtrs[pos2];
        mPtrs[pos2] = ptr;
    }

    void reverse();

    // PtrArrayImpl& operator=(const PtrArrayImpl&);

    void shuffle()
    {
        Random random;
        shuffle(&random);
    }

    void shuffle(Random* random);

protected:
    void* at(s32 idx) const
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mPtrNum);
            return nullptr;
        }

        return mPtrs[idx];
    }

    void* unsafeAt(s32 idx) const
    {
        SEAD_ASSERT_MSG(static_cast<u32>(idx) < static_cast<u32>(mPtrNum), "index exceeded [%d/%d]", idx, mPtrNum);
        return mPtrs[idx];
    }

    void* front() const { return at(0); }
    void* back() const { return at(mPtrNum - 1); }

    void pushBack(void* ptr)
    {
        if (isFull())
        {
            SEAD_ASSERT_MSG(false, "list is full.");
            return;
        }

        mPtrs[mPtrNum] = ptr;
        mPtrNum++;
    }

    void pushFront(void* ptr) { insert(0, ptr); }

    void* popBack()
    {
        if (isEmpty())
        {
            return nullptr;
        }

        mPtrNum--;
        return mPtrs[mPtrNum];
    }

    void* popFront()
    {
        if (isEmpty())
        {
            return nullptr;
        }

        void* ptr = mPtrs[0];
        erase(0);
        return ptr;
    }

    void replace(s32 pos, void* ptr)
    {
        if (static_cast<u32>(pos) >= static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "pos[%d] exceed size[%d]", pos, mPtrNum);
            return;
        }

        mPtrs[pos] = ptr;
    }

    void* find(const void* ptr, CompareCallbackImpl cmp) const
    {
        for (s32 i = 0; i < mPtrNum; i++)
        {
            if (cmp(mPtrs[i], ptr) == 0)
            {
                return mPtrs[i];
            }
        }

        return nullptr;
    }

    s32 search(const void* ptr, CompareCallbackImpl cmp) const
    {
        for (s32 i = 0; i < mPtrNum; i++)
        {
            if (cmp(mPtrs[i], ptr) == 0)
            {
                return i;
            }
        }

        return -1;
    }

    bool equal(const PtrArrayImpl& o, CompareCallbackImpl cmp) const
    {
        if (mPtrNum != o.mPtrNum)
        {
            return false;
        }

        for (s32 i = 0; i < mPtrNum; i++)
        {
            if (cmp(mPtrs[i], o.mPtrs[i]) != 0)
            {
                return false;
            }
        }

        return true;
    }

    s32 indexOf(const void* ptr) const
    {
        for (s32 i = 0; i < mPtrNum; i++)
        {
            if (mPtrs[i] == ptr)
            {
                return i;
            }
        }

        return -1;
    }

    void createVacancy(s32 pos, s32 num)
    {
        if (pos < mPtrNum)
        {
            MemUtil::copyOverlap(mPtrs + pos + num, mPtrs + pos, (mPtrNum - pos) * cPtrSize);
        }
    }

    void insert(s32 pos, void* ptr);
    void insertArray(s32 pos, void* array, s32 arrayLength, s32 elemSize);
    bool checkInsert(s32 pos, s32 num);

    void sort(CompareCallbackImpl cmp);
    void heapSort(CompareCallbackImpl cmp);
    s32 compare(const PtrArrayImpl& o, CompareCallbackImpl cmp) const;
    void uniq(CompareCallbackImpl cmp);
    s32 binarySearch(const void* ptr, CompareCallbackImpl cmp) const;

protected:
    s32 mPtrNum;
    s32 mPtrNumMax;
    void** mPtrs;
};

template <typename T>
class PtrArray : public PtrArrayImpl
{
protected:
    using CompareCallback = s32 (*)(const T* a, const T* b);

public:
    PtrArray()
        : PtrArrayImpl()
    {
    }

    PtrArray(s32 ptrNumMax, T** buffer)
        : PtrArrayImpl(ptrNumMax, buffer)
    {
    }

    T* at(s32 idx) const { return static_cast<T*>(PtrArrayImpl::at(idx)); }
    T* unsafeAt(s32 idx) const { return static_cast<T*>(PtrArrayImpl::unsafeAt(idx)); }

    T* operator[](s32 idx) const { return at(idx); }

    T* front() const { return static_cast<T*>(PtrArrayImpl::front()); }
    T* back() const { return static_cast<T*>(PtrArrayImpl::back()); }

    void pushBack(T* ptr) { PtrArrayImpl::pushBack(static_cast<void*>(ptr)); }
    void pushFront(T* ptr) { PtrArrayImpl::pushFront(static_cast<void*>(ptr)); }

    T* popBack() { return static_cast<T*>(PtrArrayImpl::popBack()); }
    T* popFront() { return static_cast<T*>(PtrArrayImpl::popFront()); }

    void insert(s32 pos, T* ptr) { PtrArrayImpl::insert(pos, static_cast<void*>(ptr)); }
    void insert(s32 pos, T* array, s32 arrayLength)
    {
        PtrArrayImpl::insertArray(pos, static_cast<void*>(array), arrayLength, sizeof(T));
    }

    void replace(s32 pos, T* ptr) { PtrArrayImpl::replace(pos, static_cast<void*>(ptr)); }

    s32 indexOf(const T* ptr) const { return PtrArrayImpl::indexOf(ptr); }

    void* getWork() const { return mPtrs; }

    void sort() { sort(&compareT); }
    void sort(CompareCallback cmp) { PtrArrayImpl::sort(reinterpret_cast<CompareCallbackImpl>(cmp)); }

    void heapSort() { heapSort(&compareT); }
    void heapSort(CompareCallback cmp) { PtrArrayImpl::heapSort(reinterpret_cast<CompareCallbackImpl>(cmp)); }

    bool equal(const PtrArray* o, CompareCallback cmp) const
    {
        return PtrArrayImpl::equal(*o, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    s32 compare(const PtrArray* o, CompareCallback cmp) const
    {
        return PtrArrayImpl::compare(*o, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    T* find(const T* ptr) const { return find(ptr, &compareT); }
    T* find(const T* ptr, CompareCallback cmp) const
    {
        return static_cast<T*>(PtrArrayImpl::find(ptr, reinterpret_cast<CompareCallbackImpl>(cmp)));
    }

    s32 search(const T* ptr) const { return search(ptr, &compareT); }
    s32 search(const T* ptr, CompareCallback cmp) const
    {
        return PtrArrayImpl::search(ptr, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    s32 binarySearch(const T* ptr) const { return binarySearch(ptr, &compareT); }
    s32 binarySearch(const T* ptr, CompareCallback cmp) const
    {
        return PtrArrayImpl::binarySearch(ptr, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    bool operator==(const PtrArray& o) const { return equal(&o, &compareT); }
    bool operator!=(const PtrArray& o) const { return !equal(&o, &compareT); }
    bool operator<(const PtrArray& o) const { return compare(&o, &compareT) < 0; }
    bool operator<=(const PtrArray& o) const { return compare(&o, &compareT) <= 0; }
    bool operator>(const PtrArray& o) const { return compare(&o, &compareT) > 0; }
    bool operator>=(const PtrArray& o) const { return compare(&o, &compareT) >= 0; }

    void uniq() { uniq(&compareT); }
    void uniq(CompareCallback cmp) { PtrArrayImpl::uniq(reinterpret_cast<CompareCallbackImpl>(cmp)); }

public:
    class iterator
    {
    public:
        iterator(T* const* pptr)
            : mPPtr(pptr)
        {
        }

        iterator& operator++()
        {
            mPPtr++;
            return *this;
        }

        T& operator*() const { return **mPPtr; }
        T* operator->() const { return *mPPtr; }

        friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs.mPPtr == rhs.mPPtr; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs) { return lhs.mPPtr != rhs.mPPtr; }

        T* getPtr() const { return *mPPtr; }

    protected:
        T* const* mPPtr;

        friend class constIterator;
    };

    class constIterator
    {
    public:
        constIterator(const T* const* pptr)
            : mPPtr(pptr)
        {
        }

        constIterator(const iterator& it)
            : mPPtr(it.mPPtr)
        {
        }

        constIterator& operator++()
        {
            mPPtr++;
            return *this;
        }

        const T& operator*() const { return **mPPtr; }
        const T* operator->() const { return *mPPtr; }

        friend bool operator==(const constIterator& lhs, const constIterator& rhs) { return lhs.mPPtr == rhs.mPPtr; }
        friend bool operator!=(const constIterator& lhs, const constIterator& rhs) { return lhs.mPPtr != rhs.mPPtr; }

        const T* getPtr() const { return *mPPtr; }

    protected:
        const T* const* mPPtr;
    };

    class reverseIterator
    {
    public:
        reverseIterator(T* const* pptr)
            : mPPtr(pptr)
        {
        }

        reverseIterator& operator++()
        {
            mPPtr--;
            return *this;
        }

        T& operator*() const { return **mPPtr; }
        T* operator->() const { return *mPPtr; }

        friend bool operator==(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mPPtr == rhs.mPPtr; }
        friend bool operator!=(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mPPtr != rhs.mPPtr; }

        T* getPtr() const { return *mPPtr; }

    protected:
        T* const* mPPtr;

        friend class reverseConstIterator;
    };

    class reverseConstIterator
    {
    public:
        reverseConstIterator(const T* const* pptr)
            : mPPtr(pptr)
        {
        }

        reverseConstIterator(const reverseIterator& it)
            : mPPtr(it.mPPtr)
        {
        }

        reverseConstIterator& operator++()
        {
            mPPtr--;
            return *this;
        }

        const T& operator*() const { return **mPPtr; }
        const T* operator->() const { return *mPPtr; }

        friend bool operator==(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mPPtr == rhs.mPPtr; }
        friend bool operator!=(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mPPtr != rhs.mPPtr; }

        const T* getPtr() const { return *mPPtr; }

    protected:
        const T* const* mPPtr;
    };

public:
    iterator begin() const { return iterator(reinterpret_cast<T**>(mPtrs)); }
    // constIterator begin() const { return constIterator(reinterpret_cast<T**>(mPtrs)); }
    iterator end() const { return iterator(reinterpret_cast<T**>(mPtrs) + mPtrNum); }
    // constIterator end() const { return constIterator(reinterpret_cast<T**>(mPtrs) + mPtrNum); }

    iterator toIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mPtrNum, idx);
            return end();
        }

        return iterator(reinterpret_cast<T**>(mPtrs) + idx);
    }

    // constIterator toIterator(s32 idx) const
    // {
    //     if (static_cast<u32>(idx) > static_cast<u32>(mPtrNum))
    //     {
    //         SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mPtrNum, idx);
    //         return end();
    //     }
    //
    //     return constIterator(reinterpret_cast<T**>(mPtrs) + idx);
    // }

    constIterator constBegin() const { return constIterator(reinterpret_cast<T**>(mPtrs)); }
    constIterator constEnd() const { return constIterator(reinterpret_cast<T**>(mPtrs) + mPtrNum); }

    constIterator toConstIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mPtrNum, idx);
            return constEnd();
        }

        return constIterator(reinterpret_cast<T**>(mPtrs) + idx);
    }

    reverseIterator reverseBegin() const { return reverseIterator(reinterpret_cast<T**>(mPtrs) + mPtrNum - 1); }
    // reverseConstIterator reverseBegin() const { return reverseConstIterator(reinterpret_cast<T**>(mPtrs) + mPtrNum - 1); }
    reverseIterator reverseEnd() const { return reverseIterator(reinterpret_cast<T**>(mPtrs) - 1); }
    // reverseConstIterator reverseEnd() const { return reverseConstIterator(reinterpret_cast<T**>(mPtrs) - 1); }

    reverseIterator toReverseIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mPtrNum, idx);
            return reverseEnd();
        }

        return reverseIterator(reinterpret_cast<T**>(mPtrs) + idx);
    }

    // reverseConstIterator toReverseIterator(s32 idx) const
    // {
    //     if (static_cast<u32>(idx) > static_cast<u32>(mPtrNum))
    //     {
    //         SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mPtrNum, idx);
    //         return reverseEnd();
    //     }
    //
    //     return reverseConstIterator(reinterpret_cast<T**>(mPtrs) + idx);
    // }

    reverseConstIterator reverseConstBegin() const { return reverseConstIterator(reinterpret_cast<T**>(mPtrs) + mPtrNum - 1); }
    reverseConstIterator reverseConstEnd() const { return reverseConstIterator(reinterpret_cast<T**>(mPtrs) - 1); }

    reverseConstIterator toReverseConstIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mPtrNum))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mPtrNum, idx);
            return reverseConstEnd();
        }

        return reverseConstIterator(reinterpret_cast<T**>(mPtrs) + idx);
    }

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

template <typename T, s32 N>
class FixedPtrArray : public PtrArray<T>
{
public:
    FixedPtrArray()
        : PtrArray<T>()
    {
        PtrArray<T>::setBuffer(N, mWork);
    }

    void allocBuffer(s32 ptrNumMax, s32 alignment = cDefaultAlignment) = delete;
    void allocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 ptrNumMax, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    void freeBuffer() = delete;
    void setBuffer(s32 ptrNumMax, void* buf) = delete;

protected:
    T* mWork[N];
};

} // namespace sead
