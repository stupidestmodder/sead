#pragma once

#include <basis/seadAssert.h>
#include <basis/seadNew.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadPtrUtil.h>

namespace sead {

// TODO
template <typename T>
class Buffer
{
protected:
    using CompareCallback = s32 (*)(const T*, const T*);

public:
    Buffer()
        : mSize(0)
        , mBuffer(nullptr)
    {
    }

    Buffer(s32 size, T* buffer)
        : mSize(0)
        , mBuffer(nullptr)
    {
        setBuffer(size, buffer);
    }

    void allocBuffer(s32 size, s32 alignment = alignof(void*))
    {
        if (!tryAllocBuffer(size, alignment))
            AllocFailAssert(nullptr, size * sizeof(T), alignment);
    }

    void allocBuffer(s32 size, Heap* heap, s32 alignment = alignof(void*))
    {
        if (!tryAllocBuffer(size, heap, alignment))
            AllocFailAssert(heap, size * sizeof(T), alignment);
    }

    bool tryAllocBuffer(s32 size, s32 alignment = alignof(void*))
    {
        SEAD_ASSERT(mBuffer == nullptr);

        if (size <= 0)
        {
            SEAD_ASSERT_MSG(false, "size[%d] must be larger than zero", size);
            return false;
        }

        T* buffer = new(alignment, std::nothrow) T[size];
        if (!buffer)
            return false;

        setBuffer(size, buffer);
        SEAD_ASSERT_MSG(PtrUtil::isAlignedPow2(mBuffer, Mathi::abs(alignment)),
                        "don't set alignment for a class with destructor");
        return true;
    }

    bool tryAllocBuffer(s32 size, Heap* heap, s32 alignment = alignof(void*))
    {
        SEAD_ASSERT(mBuffer == nullptr);

        if (size <= 0)
        {
            SEAD_ASSERT_MSG(false, "size[%d] must be larger than zero", size);
            return false;
        }

        T* buffer = new(heap, alignment, std::nothrow) T[size];
        if (!buffer)
            return false;

        setBuffer(size, buffer);
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
            mSize = 0;
        }
    }

    void setBuffer(s32 size, T* buffer)
    {
        if (size <= 0)
        {
            SEAD_ASSERT_MSG(false, "size[%d] must be larger than zero", size);
            return;
        }

        if (!buffer)
        {
            SEAD_ASSERT_MSG(false, "buffer is null");
            return;
        }

        mBuffer = buffer;
        mSize = size;
    }

    bool isBufferReady() const { return mBuffer != nullptr; }

    T& operator()(s32);
    const T& operator()(s32) const;

    T& operator[](s32 idx)
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mSize);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

    const T& operator[](s32 idx) const
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mSize);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

    T* get(s32 idx)
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mSize);
            return nullptr;
        }

        return &mBuffer[idx];
    }

    const T* get(s32 idx) const
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mSize);
            return nullptr;
        }

        return &mBuffer[idx];
    }

    T* unsafeGet(s32 idx)
    {
        SEAD_ASSERT_MSG(static_cast<u32>(idx) < static_cast<u32>(mSize), "index exceeded [%d/%d]", idx, mSize);
        return &mBuffer[idx];
    }

    const T* unsafeGet(s32 idx) const
    {
        SEAD_ASSERT_MSG(static_cast<u32>(idx) < static_cast<u32>(mSize), "index exceeded [%d/%d]", idx, mSize);
        return &mBuffer[idx];
    }

    T& front() { return mBuffer[0]; }
    const T& front() const { return mBuffer[0]; }
    T& back() { return mBuffer[mSize - 1]; }
    const T& back() const { return mBuffer[mSize - 1]; }

    s32 size() const { return mSize; }
    s32 getSize() const { return mSize; }

    T* getBufferPtr() { return mBuffer; }
    const T* getBufferPtr() const { return mBuffer; }

    u32 getByteSize() const { return mSize * sizeof(T); }

    Buffer& operator=(const Buffer&);

    bool isRangeValid(s32) const
    {
        return static_cast<u32>(idx) < static_cast<u32>(mSize);
    }

    void fill(const T& value)
    {
        for (s32 i = 0; i < mSize; i++)
            mBuffer[i] = value;
    }

    void heapSort(CompareCallback, s32, s32);
    void heapSort(s32, s32);
    s32 binarySearch(const T&) const;

    template <typename Key>
    s32 binarySearch(const Key&, s32 (*)(const T&, const Key&)) const;

    // TODO: Iterators

protected:
    static s32 compareT(const T*, const T*);
    static s32 defaultBinarySearchCompare(const T&, const T&);

protected:
    s32 mSize;
    T* mBuffer;
};

} // namespace sead
