#pragma once

#include <basis/seadAssert.h>

namespace sead {

template <typename T, s32 N>
class SafeArray;

template <typename T>
class SafeArray<T, 0>
{
};

template <typename T, s32 N>
class SafeArray
{
public:
    T& operator[](s32 idx)
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0, %d) : %d", N, idx);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

    const T& operator[](s32 idx) const
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0, %d) : %d", N, idx);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

    T& operator()(s32 idx)
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0, %d) : %d", N, idx);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

    const T& operator()(s32 idx) const
    {
        if (static_cast<u32>(idx) >= static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0, %d) : %d", N, idx);
            return mBuffer[0];
        }

        return mBuffer[idx];
    }

    T& front() { return operator()(0); }
    const T& front() const { return operator()(0); }

    T& back() { return operator()(N - 1); }
    const T& back() const { return operator()(N - 1); }

    s32 size() const { return N; }
    s32 getSize() const { return N; }
    u32 getByteSize() const { return N * sizeof(T); }

    T* getBufferPtr() { return mBuffer; }
    const T* getBufferPtr() const { return mBuffer; }

    void fill(const T& value)
    {
        for (s32 i = 0; i < N; i++)
        {
            mBuffer[i] = value;
        }
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

        iterator(T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        iterator& operator++()
        {
            mIndex++;
            return *this;
        }

        iterator& operator--()
        {
            mIndex--;
            return *this;
        }

        T& operator*() const { return mBuffer[mIndex]; }
        T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

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

        constIterator(const T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        constIterator(iterator it)
            : mIndex(it.mIndex)
            , mBuffer(it.mBuffer)
        {
        }

        constIterator& operator++()
        {
            mIndex++;
            return *this;
        }

        constIterator& operator--()
        {
            mIndex--;
            return *this;
        }

        const T& operator*() const { return mBuffer[mIndex]; }
        const T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const constIterator& lhs, const constIterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const constIterator& lhs, const constIterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

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

        reverseIterator(T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        reverseIterator& operator++()
        {
            mIndex--;
            return *this;
        }

        reverseIterator& operator--()
        {
            mIndex++;
            return *this;
        }

        T& operator*() const { return mBuffer[mIndex]; }
        T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

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

        reverseConstIterator(const T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        reverseConstIterator(reverseIterator it)
            : mIndex(it.mIndex)
            , mBuffer(it.mBuffer)
        {
        }

        reverseConstIterator& operator++()
        {
            mIndex--;
            return *this;
        }

        reverseConstIterator& operator--()
        {
            mIndex++;
            return *this;
        }

        const T& operator*() const { return mBuffer[mIndex]; }
        const T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        const T* mBuffer;
    };

public:
    iterator begin() { return iterator(mBuffer, 0); }
    constIterator begin() const { return constIterator(mBuffer, 0); }
    iterator end() { return iterator(mBuffer, N); }
    constIterator end() const { return constIterator(mBuffer, N); }

    iterator toIterator(s32 idx)
    {
        if (static_cast<u32>(idx) > static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", N, idx);
            return end();
        }

        return iterator(mBuffer, idx);
    }

    constIterator toIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", N, idx);
            return end();
        }

        return constIterator(mBuffer, idx);
    }

    constIterator constBegin() const { return constIterator(mBuffer, 0); }
    constIterator constEnd() const { return constIterator(mBuffer, N); }

    constIterator toConstIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", N, idx);
            return constEnd();
        }

        return constIterator(mBuffer, idx);
    }

    reverseIterator reverseBegin() { return reverseIterator(mBuffer, N - 1); }
    reverseConstIterator reverseBegin() const { return reverseConstIterator(mBuffer, N - 1); }
    reverseIterator reverseEnd() { return reverseIterator(mBuffer, -1); }
    reverseConstIterator reverseEnd() const { return reverseConstIterator(mBuffer, -1); }

    reverseIterator toReverseIterator(s32 idx)
    {
        if (static_cast<u32>(idx) > static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", N, idx);
            return reverseEnd();
        }

        return reverseIterator(mBuffer, idx);
    }

    reverseConstIterator toReverseIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", N, idx);
            return reverseEnd();
        }

        return reverseConstIterator(mBuffer, idx);
    }

    reverseConstIterator reverseConstBegin() const { return reverseConstIterator(mBuffer, N - 1); }
    reverseConstIterator reverseConstEnd() const { return reverseConstIterator(mBuffer, -1); }

    reverseConstIterator toReverseConstIterator(s32 idx) const
    {
        if (static_cast<u32>(idx) > static_cast<u32>(N))
        {
            SEAD_ASSERT_MSG(false, "range over [0,%d] : %d", N, idx);
            return reverseConstEnd();
        }

        return reverseConstIterator(mBuffer, idx);
    }

private:
    T mBuffer[N];
};

template <typename T, s32 N>
class UnsafeArray
{
public:
    T& operator[](s32 idx) { return mBuffer[idx]; }
    const T& operator[](s32 idx) const { return mBuffer[idx]; }

    T& operator()(s32 idx) { return mBuffer[idx]; }
    const T& operator()(s32 idx) const { return mBuffer[idx]; }

    T& front() { return mBuffer[0]; }
    const T& front() const { return mBuffer[0]; }

    T& back() { return mBuffer[N - 1]; }
    const T& back() const { return mBuffer[N - 1]; }

    s32 size() const { return N; }
    s32 getSize() const { return N; }
    u32 getByteSize() const { return N * sizeof(T); }

    T* getBufferPtr() { return mBuffer; }
    const T* getBufferPtr() const { return mBuffer; }

    void fill(const T& value)
    {
        for (s32 i = 0; i < N; i++)
        {
            mBuffer[i] = value;
        }
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

        iterator(T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        iterator& operator++()
        {
            mIndex++;
            return *this;
        }

        iterator& operator--()
        {
            mIndex--;
            return *this;
        }

        T& operator*() const { return mBuffer[mIndex]; }
        T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

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

        constIterator(const T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        constIterator(iterator it)
            : mIndex(it.mIndex)
            , mBuffer(it.mBuffer)
        {
        }

        constIterator& operator++()
        {
            mIndex++;
            return *this;
        }

        constIterator& operator--()
        {
            mIndex--;
            return *this;
        }

        const T& operator*() const { return mBuffer[mIndex]; }
        const T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const constIterator& lhs, const constIterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const constIterator& lhs, const constIterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

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

        reverseIterator(T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        reverseIterator& operator++()
        {
            mIndex--;
            return *this;
        }

        reverseIterator& operator--()
        {
            mIndex++;
            return *this;
        }

        T& operator*() const { return mBuffer[mIndex]; }
        T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const reverseIterator& lhs, const reverseIterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

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

        reverseConstIterator(const T* buffer, s32 index)
            : mIndex(index)
            , mBuffer(buffer)
        {
        }

        reverseConstIterator(reverseIterator it)
            : mIndex(it.mIndex)
            , mBuffer(it.mBuffer)
        {
        }

        reverseConstIterator& operator++()
        {
            mIndex--;
            return *this;
        }

        reverseConstIterator& operator--()
        {
            mIndex++;
            return *this;
        }

        const T& operator*() const { return mBuffer[mIndex]; }
        const T* operator->() const { return &mBuffer[mIndex]; }

        friend bool operator==(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mIndex == rhs.mIndex && lhs.mBuffer == rhs.mBuffer; }
        friend bool operator!=(const reverseConstIterator& lhs, const reverseConstIterator& rhs) { return lhs.mIndex != rhs.mIndex || lhs.mBuffer != rhs.mBuffer; }

        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        const T* mBuffer;
    };

public:
    iterator begin() { return iterator(mBuffer, 0); }
    constIterator begin() const { return constIterator(mBuffer, 0); }
    iterator end() { return iterator(mBuffer, N); }
    constIterator end() const { return constIterator(mBuffer, N); }
    iterator toIterator(s32 idx) { return iterator(mBuffer, idx); }
    constIterator toIterator(s32 idx) const{ return constIterator(mBuffer, idx); }

    constIterator constBegin() const { return constIterator(mBuffer, 0); }
    constIterator constEnd() const { return constIterator(mBuffer, N); }
    constIterator toConstIterator(s32 idx) const { return constIterator(mBuffer, idx); }

    reverseIterator reverseBegin() { return reverseIterator(mBuffer, N - 1); }
    reverseConstIterator reverseBegin() const { return reverseConstIterator(mBuffer, N - 1); }
    reverseIterator reverseEnd() { return reverseIterator(mBuffer, -1); }
    reverseConstIterator reverseEnd() const { return reverseConstIterator(mBuffer, -1); }
    reverseIterator toReverseIterator(s32 idx) { return reverseIterator(mBuffer, idx); }
    reverseConstIterator toReverseIterator(s32 idx) const { return reverseConstIterator(mBuffer, idx); }

    reverseConstIterator reverseConstBegin() const { return reverseConstIterator(mBuffer, N - 1); }
    reverseConstIterator reverseConstEnd() const { return reverseConstIterator(mBuffer, -1); }
    reverseConstIterator toReverseConstIterator(s32 idx) const{ return reverseConstIterator(mBuffer, idx); }

private:
    T mBuffer[N];
};

} // namespace sead
