#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>
#include <prim/seadMemUtil.h>

namespace sead {

class Heap;
class Random;

class PtrArrayImpl
{
    SEAD_NO_COPY(PtrArrayImpl);

protected:
    using CompareCallbackImpl = s32 (*)(const void*, const void*);

public:
    PtrArrayImpl()
        : mPtrNum(0)
        , mPtrNumMax(0)
        , mPtrs(nullptr)
    {
    }

    PtrArrayImpl(s32 ptrNumMax, void* buffer)
        : mPtrNum(0)
        , mPtrNumMax(0)
        , mPtrs(nullptr)
    {
        setBuffer(ptrNumMax, buffer);
    }

    void allocBuffer(s32 ptrNumMax, s32 alignment = alignof(void*));
    void allocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = alignof(void*));
    bool tryAllocBuffer(s32 ptrNumMax, s32 alignment = alignof(void*));
    bool tryAllocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = alignof(void*));
    void freeBuffer();
    void setBuffer(s32 ptrNumMax, void* buffer);

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
            return;

        void* ptr = mPtrs[pos1];
        mPtrs[pos1] = mPtrs[pos2];
        mPtrs[pos2] = ptr;
    }

    void reverse();

    // PtrArrayImpl& operator=(const PtrArrayImpl& o);

    void shuffle();
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
            return nullptr;

        mPtrNum--;
        return mPtrs[mPtrNum];
    }

    void* popFront()
    {
        if (isEmpty())
            return nullptr;

        void* ptr = front();
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
                return mPtrs[i];
        }

        return nullptr;
    }

    s32 search(const void* ptr, CompareCallbackImpl cmp) const
    {
        for (s32 i = 0; i < mPtrNum; i++)
        {
            if (cmp(mPtrs[i], ptr) == 0)
                return i;
        }

        return -1;
    }

    bool equal(const PtrArrayImpl& o, CompareCallbackImpl cmp) const
    {
        if (mPtrNum != o.mPtrNum)
            return false;

        for (s32 i = 0; i < mPtrNum; i++)
        {
            if (cmp(mPtrs[i], o.mPtrs[i]) != 0)
                return false;
        }

        return true;
    }

    s32 indexOf(const void* ptr) const
    {
        for (s32 i = 0; i < mPtrNum; i++)
        {
            if (mPtrs[i] == ptr)
                return i;
        }

        return -1;
    }

    void createVacancy(s32 pos, s32 num)
    {
        if (pos >= mPtrNum)
            return;

        MemUtil::copyOverlap(mPtrs + pos + num, mPtrs + pos, (mPtrNum - pos) * sizeof(void*));
    }

    void insert(s32 pos, void* ptr);
    void insertArray(s32 pos, void* array, s32 arrayLength, s32 elemSize);
    bool checkInsert(s32 pos, s32 num);
    // TODO
    void sort(CompareCallbackImpl cmp);
    // TODO
    void heapSort(CompareCallbackImpl cmp);
    s32 compare(const PtrArrayImpl& o, CompareCallbackImpl cmp) const;
    void uniq(CompareCallbackImpl cmp);
    s32 binarySearch(const void* ptr, CompareCallbackImpl cmp) const;

protected:
    s32 mPtrNum;
    s32 mPtrNumMax;
    void** mPtrs;
};

// TODO
template <typename T>
class PtrArray : public PtrArrayImpl
{
protected:
    using CompareCallback = s32 (*)(const T*, const T*);

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

    void pushBack(T* ptr) { PtrArrayImpl::pushBack((void*)ptr); }
    void pushFront(T* ptr) { PtrArrayImpl::pushFront((void*)ptr); }

    T* popBack() { return static_cast<T*>(PtrArrayImpl::popBack()); }
    T* popFront() { return static_cast<T*>(PtrArrayImpl::popFront()); }

    void insert(s32 pos, T* ptr) { PtrArrayImpl::insert(pos, (void*)ptr); }
    void insert(s32 pos, T* array, s32 arrayLength) { PtrArrayImpl::insertArray(pos, (void*)array, arrayLength, sizeof(T)); }

    void replace(s32 pos, T* ptr) { PtrArrayImpl::replace(pos, (void*)ptr); }

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

    s32 compare(const PtrArray* o , CompareCallback cmp) const
    {
        return PtrArrayImpl::compare(*o, reinterpret_cast<CompareCallbackImpl>(cmp));
    }

    T* find(const T* ptr) const;
    T* find(const T* ptr, CompareCallback cmp) const;

    s32 search(const T* ptr) const;
    s32 search(const T* ptr, CompareCallback cmp) const;

    s32 binarySearch(const T* ptr) const;
    s32 binarySearch(const T* ptr, CompareCallback cmp) const;

    bool operator==(const PtrArray&) const;
    bool operator!=(const PtrArray&) const;
    bool operator<(const PtrArray&) const;
    bool operator<=(const PtrArray&) const;
    bool operator>(const PtrArray&) const;
    bool operator>=(const PtrArray&) const;

    void uniq();
    void uniq(CompareCallback cmp);

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
            ++mPPtr;
            return *this;
        }

        T& operator*() const
        {
            return **mPPtr;
        }

        T* operator->() const
        {
            return *mPPtr;
        }

        T* getPtr() const
        {
            return *mPPtr;
        }

        friend bool operator==(const iterator& it1, const iterator& it2)
        {
            return it1.mPPtr == it2.mPPtr;
        }

        friend bool operator!=(const iterator& it1, const iterator& it2)
        {
            return it1.mPPtr != it2.mPPtr;
        }

    protected:
        T* const* mPPtr;
    };

    class constIterator
    {
    public:
        constIterator(const T* const* pptr)
            : mPPtr(pptr)
        {
        }

        constIterator& operator++()
        {
            ++mPPtr;
            return *this;
        }

        const T& operator*() const
        {
            return **mPPtr;
        }

        const T* operator->() const
        {
            return *mPPtr;
        }

        const T* getPtr() const
        {
            return *mPPtr;
        }

        friend bool operator==(const constIterator& it1, const constIterator& it2)
        {
            return it1.mPPtr == it2.mPPtr;
        }

        friend bool operator!=(const constIterator& it1, const constIterator& it2)
        {
            return it1.mPPtr != it2.mPPtr;
        }

    protected:
        const T* const* mPPtr;
    };

    // TODO
    class reverseIterator { };

    // TODO
    class reverseConstIterator { };

    iterator begin() const { return iterator(reinterpret_cast<T**>(mPtrs)); }
    iterator end() const { return iterator(reinterpret_cast<T**>(mPtrs) + mPtrNum); }
    iterator toIterator(s32) const;

    iterator constBegin() const { return constIterator(reinterpret_cast<T**>(mPtrs)); }
    iterator constEnd() const { constIterator(reinterpret_cast<T**>(mPtrs) + mPtrNum); }
    iterator toConstIterator(s32) const;

    iterator reverseBegin() const;
    iterator reverseEnd() const;
    iterator toReverseIterator(s32) const;

    iterator reverseConstBegin() const;
    iterator reverseConstEnd() const;
    iterator toReverseConstIterator(s32) const;

protected:
    static s32 compareT(const T*, const T*);
};

template <typename T, s32 N>
class FixedPtrArray : public PtrArray<T>
{
public:
    FixedPtrArray()
        : PtrArray<T>(N, mWork)
        , mWork()
    {
    }

    void allocBuffer(s32 ptrNumMax, s32 alignment = alignof(void*)) = delete;
    void allocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = alignof(void*)) = delete;
    bool tryAllocBuffer(s32 ptrNumMax, s32 alignment = alignof(void*)) = delete;
    bool tryAllocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment = alignof(void*)) = delete;
    void freeBuffer() = delete;
    void setBuffer(s32 ptrNumMax, void* buffer) = delete;

protected:
    T* mWork[N];
};

} // namespace sead
