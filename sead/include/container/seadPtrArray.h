#pragma once

#include <basis/seadTypes.h>
#include <basis/seadAssert.h>

namespace sead {

class Heap;
class Random;

// TODO
class PtrArrayImpl
{
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
    void unsafeResize(s32);
    void swap(s32 pos1, s32 pos2);

    void reverse();

    PtrArrayImpl& operator=(const PtrArrayImpl&);

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

    void* unsafeAt(s32 idx) const { return mPtrs[idx]; }

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

    void pushFront(void*);

    void* popBack()
    {
        if (mPtrNum <= 0)
            return nullptr;

        mPtrNum--;
        return mPtrs[mPtrNum];
    }

    void* popfront();
    void replace(s32, void*);
    void* find(const void*, CompareCallbackImpl) const;
    s32 search(const void*, CompareCallbackImpl) const;
    bool equal(const PtrArrayImpl&, CompareCallbackImpl) const;
    s32 indexOf(const void*) const;
    void createVacancy(s32, s32);
    void insert(s32, void*);
    void insertArray(s32, void*, s32, s32);
    bool checkInsert(s32, s32);
    void sort(CompareCallbackImpl);
    void heapSort(CompareCallbackImpl);
    s32 compare(const PtrArrayImpl&, CompareCallbackImpl) const;
    void uniq(CompareCallbackImpl);
    s32 binarySearch(const void*, CompareCallbackImpl) const;

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
