#pragma once

#include <basis/seadAssert.h>
#include <basis/seadNew.h>
#include <basis/seadRawPrint.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadPtrUtil.h>

namespace sead {

template <typename T>
class Buffer
{
protected:
    using CompareCallback = s32 (*)(const T* a, const T* b);

public:
    Buffer()
        : mSize(0)
        , mBuffer(nullptr)
    {
    }

    Buffer(s32 size, T* bufferptr)
        : mSize(0)
        , mBuffer(nullptr)
    {
        if (size > 0 && bufferptr)
        {
            mSize = size;
            mBuffer = bufferptr;
        }
        else
        {
            SEAD_ASSERT_MSG(false, "illegal param: size[%d] bufferptr[" SEAD_FMT_UINTPTR "]", size, bufferptr);
        }
    }

    void allocBuffer(s32 size, s32 alignment = cDefaultAlignment)
    {
        if (!tryAllocBuffer(size, alignment))
        {
            AllocFailAssert(nullptr, size * sizeof(T), alignment);
        }
    }

    void allocBuffer(s32 size, Heap* heap, s32 alignment = cDefaultAlignment)
    {
        if (!tryAllocBuffer(size, heap, alignment))
        {
            AllocFailAssert(heap, size * sizeof(T), alignment);
        }
    }

    bool tryAllocBuffer(s32 size, s32 alignment = cDefaultAlignment)
    {
        SEAD_ASSERT(mBuffer == nullptr);

        if (size <= 0)
        {
            SEAD_ASSERT_MSG(false, "size[%d] must be larger than zero", size);
            return false;
        }

        T* bufferptr = new(alignment, std::nothrow) T[size];
        if (!bufferptr)
        {
            return false;
        }

        setBuffer(size, bufferptr);
        SEAD_ASSERT_MSG(
            PtrUtil::isAlignedPow2(mBuffer, Mathi::abs(alignment)),
            "don't set alignment for a class with destructor"
        );
        return true;
    }

    bool tryAllocBuffer(s32 size, Heap* heap, s32 alignment = cDefaultAlignment)
    {
        SEAD_ASSERT(mBuffer == nullptr);

        if (size <= 0)
        {
            SEAD_ASSERT_MSG(false, "size[%d] must be larger than zero", size);
            return false;
        }

        T* bufferptr = new(heap, alignment, std::nothrow) T[size];
        if (!bufferptr)
        {
            return false;
        }

        setBuffer(size, bufferptr);
        SEAD_ASSERT_MSG(
            PtrUtil::isAlignedPow2(mBuffer, Mathi::abs(alignment)),
            "don't set alignment for a class with destructor"
        );
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

    void setBuffer(s32 size, T* bufferptr)
    {
        if (size <= 0)
        {
            SEAD_ASSERT_MSG(false, "size[%d] must be larger than zero", size);
            return;
        }

        if (!bufferptr)
        {
            SEAD_ASSERT_MSG(false, "bufferptr is null");
            return;
        }

        mSize = size;
        mBuffer = bufferptr;
    }

    bool isBufferReady() const { return mBuffer != nullptr; }

    T& operator()(s32 idx)
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mSize);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

    const T& operator()(s32 idx) const
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "index exceeded [%d/%d]", idx, mSize);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

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

    T& front() { return operator()(0); }
    const T& front() const { return operator()(0); }

    T& back() { return operator()(mSize - 1); }
    const T& back() const { return operator()(mSize - 1); }

    s32 size() const { return mSize; }
    s32 getSize() const { return mSize; }

    T* getBufferPtr() { return mBuffer; }
    const T* getBufferPtr() const { return mBuffer; }

    u32 getByteSize() const { return mSize * sizeof(T); }

    // Buffer& operator=(const Buffer&);

    bool isRangeValid(s32 idx) const
    {
        return static_cast<u32>(idx) < static_cast<u32>(mSize);
    }

    void fill(const T& value)
    {
        for (s32 i = 0; i < mSize; i++)
        {
            mBuffer[i] = value;
        }
    }

    void heapSort(CompareCallback cmp, s32 begin, s32 end)
    {
        s32 range = end - begin;
        if (range < 1)
        {
            SEAD_ASSERT_MSG(range == 0, "illegal index[%d/%d/%d]", begin, end, mSize);
            return;
        }

        if ((begin >= mSize || end > mSize) || (begin < mSize && end == mSize))
        {
            SEAD_ASSERT_MSG(false, "illegal index[%d/%d/%d]", begin, end, mSize);
            return;
        }

        s32 i;
        s32 k;
        s32 t;
        s32 n;
        T x;
        T* ptrs;

        n = range + 1;
        ptrs = mBuffer + begin;
        for (i = n / 2; i > 0; i--)
        {
            x = ptrs[i - 1];

            k = i;
            while (t = k * 2, t <= n)
            {
                if (t < n && cmp(&ptrs[t - 1], &ptrs[t]) < 0)
                {
                    t++;
                }

                if (cmp(&x, &ptrs[t - 1]) >= 0)
                {
                    break;
                }

                ptrs[k - 1] = ptrs[t - 1];
                k = t;
            }

            ptrs[k - 1] = x;
        }

        while (n > 1)
        {
            x = ptrs[n - 1];
            ptrs[n - 1] = ptrs[0];

            n--;
            k = 1;
            while (t = k * 2, t <= n)
            {
                if (t < n && cmp(&ptrs[t - 1], &ptrs[t]) < 0)
                {
                    t++;
                }

                if (cmp(&x, &ptrs[t - 1]) >= 0)
                {
                    break;
                }

                ptrs[k - 1] = ptrs[t - 1];
                k = t;
            }

            ptrs[k - 1] = x;
        }
    }

    void heapSort(s32 begin, s32 end)
    {
        heapSort(&compareT, begin, end);
    }

    s32 binarySearch(const T& value) const
    {
        return binarySearch(value, &defaultBinarySearchCompare);
    }

    template <typename Value>
    s32 binarySearch(const Value& value, s32 (*cmp)(const T& a, const Value& b)) const
    {
        if (size() == 0)
        {
            return -1;
        }

        s32 left;
        s32 right;
        s32 mid;
        s32 cmpRet;

        left = 0;
        right = size() - 1;
        while (left < right)
        {
            mid = (left + right) / 2;

            cmpRet = cmp(mBuffer[mid], value);
            if (cmpRet == 0)
            {
                return mid;
            }

            if (cmpRet < 0)
            {
                left = mid + 1;
            }
            else
            {
                right = mid;
            }
        }

        if (cmp(mBuffer[left], value) == 0)
        {
            return left;
        }

        return -1;
    }

public:
    class iterator
    {
    public:
        iterator()
            : mIndex(0)
            , mBuffer(nullptr)
        {
        }

        explicit iterator(Buffer* buffer)
            : mIndex(0)
            , mBuffer(buffer->mBuffer)
        {
        }

        iterator(Buffer* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer->mBuffer)
        {
        }

        iterator& operator++()
        {
            mIndex++;
            return *this;
        }

        T& operator*() const { return mBuffer[mIndex]; }
        T* operator->() const { return &(mBuffer[mIndex]); }

        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return lhs.mIndex == rhs.mIndex;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return lhs.mIndex != rhs.mIndex;
        }

        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        T* mBuffer;

        friend class constIterator;
    };

    class constIterator
    {
    public:
        constIterator()
            : mIndex(0)
            , mBuffer(nullptr)
        {
        }

        explicit constIterator(const Buffer* buffer)
            : mIndex(0)
            , mBuffer(buffer->mBuffer)
        {
        }

        constIterator(const Buffer* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer->mBuffer)
        {
        }

        explicit constIterator(iterator it)
            : mIndex(it.mIndex)
            , mBuffer(it.mBuffer)
        {
        }

        constIterator& operator++()
        {
            mIndex++;
            return *this;
        }

        const T& operator*() const { return mBuffer[mIndex]; }
        const T* operator->() const { return &(mBuffer[mIndex]); }

        friend bool operator==(const constIterator& lhs, const constIterator& rhs)
        {
            return lhs.mIndex == rhs.mIndex;
        }

        friend bool operator!=(const constIterator& lhs, const constIterator& rhs)
        {
            return lhs.mIndex != rhs.mIndex;
        }

        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        const T* mBuffer;
    };

    class reverseIterator
    {
    public:
        reverseIterator()
            : mIndex(0)
            , mBuffer(nullptr)
        {
        }

        explicit reverseIterator(Buffer* buffer)
            : mIndex(buffer->mSize - 1)
            , mBuffer(buffer->mBuffer)
        {
        }

        reverseIterator(Buffer* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer->mBuffer)
        {
        }

        reverseIterator& operator++()
        {
            mIndex--;
            return *this;
        }

        T& operator*() const { return mBuffer[mIndex]; }
        T* operator->() const { return &(mBuffer[mIndex]); }

        friend bool operator==(const reverseIterator& lhs, const reverseIterator& rhs)
        {
            return lhs.mIndex == rhs.mIndex;
        }

        friend bool operator!=(const reverseIterator& lhs, const reverseIterator& rhs)
        {
            return lhs.mIndex != rhs.mIndex;
        }

        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        T* mBuffer;

        friend class reverseConstIterator;
    };

    class reverseConstIterator
    {
    public:
        reverseConstIterator()
            : mIndex(0)
            , mBuffer(nullptr)
        {
        }

        explicit reverseConstIterator(const Buffer* buffer)
            : mIndex(buffer->mSize - 1)
            , mBuffer(buffer->mBuffer)
        {
        }

        reverseConstIterator(const Buffer* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer->mBuffer)
        {
        }

        explicit reverseConstIterator(reverseIterator it)
            : mIndex(it.mIndex)
            , mBuffer(it.mBuffer)
        {
        }

        reverseConstIterator& operator++()
        {
            mIndex--;
            return *this;
        }

        const T& operator*() const { return mBuffer[mIndex]; }
        const T* operator->() const { return &(mBuffer[mIndex]); }

        friend bool operator==(const reverseConstIterator& lhs, const reverseConstIterator& rhs)
        {
            return lhs.mIndex == rhs.mIndex;
        }

        friend bool operator!=(const reverseConstIterator& lhs, const reverseConstIterator& rhs)
        {
            return lhs.mIndex != rhs.mIndex;
        }

        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        const T* mBuffer;
    };

public:
    iterator begin() { return iterator(this); }
    constIterator begin() const { return constIterator(this); }
    iterator end() { return iterator(this, mSize); }
    constIterator end() const { return constIterator(this, mSize); }

    iterator toIterator(s32 idx)
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mSize, idx);
            return end();
        }

        return iterator(this, idx);
    }

    constIterator toIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mSize, idx);
            return end();
        }

        return constIterator(this, idx);
    }

    constIterator constBegin() const { return constIterator(this); }
    constIterator constEnd() const { return constIterator(this, mSize); }

    constIterator toConstIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mSize, idx);
            return constEnd();
        }

        return constIterator(this, idx);
    }

    reverseIterator reverseBegin() { return reverseIterator(this); }
    reverseConstIterator reverseBegin() const { return reverseConstIterator(this); }
    reverseIterator reverseEnd() { return reverseIterator(this, -1); }
    reverseConstIterator reverseEnd() const { return reverseConstIterator(this, -1); }

    reverseIterator toReverseIterator(s32 idx)
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mSize, idx);
            return reverseEnd();
        }

        return reverseIterator(this, idx);
    }

    reverseConstIterator toReverseIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mSize, idx);
            return reverseEnd();
        }

        return reverseConstIterator(this, idx);
    }

    reverseConstIterator reverseConstBegin() const { return reverseConstIterator(this); }
    reverseConstIterator reverseConstEnd() const { return reverseConstIterator(this, -1); }

    reverseConstIterator toReverseConstIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(mSize))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", mSize, idx);
            return reverseConstEnd();
        }

        return reverseConstIterator(this, idx);
    }

protected:
    static s32 compareT(const T* a, const T* b)
    {
        if (*a < *b)
        {
            return -1;
        }

        if (*a > *b)
        {
            return 1;
        }

        return 0;
    }

    static s32 defaultBinarySearchCompare(const T& a, const T& b)
    {
        if (a < b)
        {
            return -1;
        }

        if (a > b)
        {
            return 1;
        }

        return 0;
    }

protected:
    s32 mSize;
    T* mBuffer;
};

} // namespace sead
