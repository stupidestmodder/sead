#pragma once

#include <basis/seadAssert.h>
#include <basis/seadNew.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadPtrUtil.h>

namespace sead {

// TODO
template <typename T>
class RingBuffer
{
public:
    RingBuffer()
        : mBuffer(nullptr)
        , mNumMax(0)
        , mStart(0)
        , mNum(0)
    {
    }

    RingBuffer(s32 numMax, T* buffer)
        : mBuffer(nullptr)
        , mNumMax(0)
        , mStart(0)
        , mNum(0)
    {
        setBuffer(numMax, buffer);
    }

    void clear()
    {
        mNum = 0;
        mStart = 0;
    }

    void allocBuffer(s32 numMax, s32 alignment = alignof(void*))
    {
        if (!tryAllocBuffer(numMax, alignment))
            AllocFailAssert(nullptr, numMax * sizeof(T), alignment);
    }

    void allocBuffer(s32 numMax, Heap* heap, s32 alignment = alignof(void*))
    {
        if (!tryAllocBuffer(numMax, heap, alignment))
            AllocFailAssert(heap, numMax * sizeof(T), alignment);
    }

    bool tryAllocBuffer(s32 numMax, s32 alignment = alignof(void*))
    {
        SEAD_ASSERT(mBuffer == nullptr);

        if (numMax <= 0)
        {
            SEAD_ASSERT_MSG(false, "numMax[%d] must be larger than zero", numMax);
            return false;
        }

        T* buffer = new(alignment, std::nothrow) T[numMax];
        if (!buffer)
            return false;

        setBuffer(numMax, buffer);
        SEAD_ASSERT_MSG(PtrUtil::isAlignedPow2(mBuffer, Mathi::abs(alignment)),
                        "don't set alignment for a class with destructor");
        return true;
    }

    bool tryAllocBuffer(s32 numMax, Heap* heap, s32 alignment = alignof(void*))
    {
        SEAD_ASSERT(mBuffer == nullptr);

        if (numMax <= 0)
        {
            SEAD_ASSERT_MSG(false, "numMax[%d] must be larger than zero", numMax);
            return false;
        }

        T* buffer = new(heap, alignment, std::nothrow) T[numMax];
        if (!buffer)
            return false;

        setBuffer(numMax, buffer);
        SEAD_ASSERT_MSG(PtrUtil::isAlignedPow2(mBuffer, Mathi::abs(alignment)),
                        "don't set alignment for a class with destructor");
        return true;
    }

    void freeBuffer()
    {
        if (isBufferReady())
        {
            delete[] mBuffer;
            mBuffer = nullptr;
            mNumMax = 0;
            mNum = 0;
            mStart = 0;
        }
    }

    void setBuffer(s32 numMax, T* buffer)
    {
        if (numMax <= 0)
        {
            SEAD_ASSERT_MSG(false, "numMax[%d] must be larger than zero", numMax);
            return;
        }

        if (!buffer)
        {
            SEAD_ASSERT_MSG(false, "buffer is null");
            return;
        }

        mBuffer = buffer;
        mNumMax = numMax;
        mNum = 0;
        mStart = 0;
    }

    bool isBufferReady() const { return mBuffer != nullptr; }

    T& operator()(s32 idx)
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mNum))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mNum);
            return mBuffer[0];
        }

        return mBuffer[getIndex(idx)];
    }

    const T& operator()(s32 idx) const
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mNum))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mNum);
            return mBuffer[0];
        }

        return mBuffer[getIndex(idx)];
    }

    T& operator[](s32 idx);
    const T& operator[](s32 idx) const;

    T* get(s32 idx);
    const T* get(s32 idx) const;

    T* unsafeGet(s32 idx) { return &mBuffer[getIndex(idx)]; }
    const T* unsafeGet(s32 idx) const { return &mBuffer[getIndex(idx)]; }

    T* getFromBack(s32 idx);
    const T* getFromBack(s32 idx) const;

    T& front() { return operator()(0); }
    const T& front() const { return operator()(0); }
    T& back();
    const T& back() const;

    bool pushBack(const T& t)
    {
        if (isFull())
            return false;

        *unsafeGet(mNum++) = t;
        return true;
    }

    void forcePushBack(const T&);

    bool pushFront(const T& t)
    {
        if (isFull())
            return false;

        mStart = (mStart < 1 ? mNumMax : mStart) - 1;
        mNum++;

        *unsafeGet(0) = t;
        return true;
    }

    void forcePushFront(const T&);

    T* unsafeBirthBack();
    T* unsafeBirthFront();

    void popBack();
    void popBack(T*);
    void popBack(T);
    void popFront();

    void popFront(T* t)
    {
        *t = front();

        if (mNum > 0)
        {
            mStart++;
            mStart = mStart >= mNumMax ? 0 : mStart;
            mNum--;
        }
    }

    void popFront(T);

    void eraseAt(s32);

    s32 size() const { return mNum; }
    s32 getNum() const { return mNum; }
    s32 maxSize() const { return mNumMax; }
    bool isEmpty() const { return mNum == 0; }
    bool isFull() const { return mNum >= mNumMax; }

    // TODO: Iterators

protected:
    s32 getIndex(s32 idx) const
    {
        s32 realIdx = mStart + idx;
        if (realIdx >= mNumMax)
            realIdx -= mNumMax;
        return realIdx;
    }

protected:
    T* mBuffer;
    s32 mNumMax;
    s32 mStart;
    s32 mNum;
};

template <typename T, s32 N>
class FixedRingBuffer : public RingBuffer<T>
{
public:
    FixedRingBuffer()
        : RingBuffer<T>(N, mWork)
        , mWork()
    {
    }

    void allocBuffer(s32 numMax, s32 alignment = alignof(void*)) = delete;
    void allocBuffer(s32 numMax, Heap* heap, s32 alignment = alignof(void*)) = delete;
    bool tryAllocBuffer(s32 numMax, s32 alignment = alignof(void*)) = delete;
    bool tryAllocBuffer(s32 numMax, Heap* heap, s32 alignment = alignof(void*)) = delete;
    void freeBuffer() = delete;
    void setBuffer(s32 numMax, T* buffer) = delete;

protected:
    T mWork[N];
};

} // namespace sead
