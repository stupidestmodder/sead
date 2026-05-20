#pragma once

#include <basis/seadTypes.h>

//? I'm not about to implement atomics in every platform soo std::atomic it is
#include <atomic>

namespace sead {

template <typename T>
class AtomicBase32
{
public:
    AtomicBase32()
        : mValue(0)
    {
    }

    explicit AtomicBase32(T value)
        : mValue(value)
    {
    }

    T getValue() const
    {
        return mValue.load();
    }

    void setValue(T value)
    {
        mValue.store(value);
    }

    void setValueNonAtomic(T value)
    {
        volatile T* ptr = reinterpret_cast<volatile T*>(&mValue);
        *ptr = value;
    }

    T add(s32 value)
    {
        return mValue.fetch_add(value);
    }

    T increment()
    {
        return add(1);
    }

    T decrement()
    {
        return add(-1);
    }

    T orMask(T value)
    {
        return mValue.fetch_or(value);
    }

    T andMask(T value)
    {
        return mValue.fetch_and(value);
    }

    T xorMask(T value)
    {
        return mValue.fetch_xor(value);
    }

    T swap(T value)
    {
        return mValue.exchange(value);
    }

    bool compareAndSwap(T compareValue, T newValue)
    {
        return mValue.compare_exchange_strong(compareValue, newValue);
    }

    bool compareAndSwapAndGetOriginal(T compareValue, T newValue, T* originalValue)
    {
        *originalValue = mValue.load(std::memory_order_relaxed);
        return mValue.compare_exchange_strong(compareValue, newValue);
    }

    bool isBitOn(u32 bit) const
    {
        return (getValue() & (static_cast<T>(1) << bit)) != 0;
    }

    bool setBitOn(u32 bit)
    {
        const T old = mValue.fetch_or(static_cast<T>(1) << bit);

        return (old & (static_cast<T>(1) << bit)) == 0;
    }

    bool setBitOff(u32 bit)
    {
        const T old = mValue.fetch_and(~(static_cast<T>(1) << bit));

        return (old & (static_cast<T>(1) << bit)) != 0;
    }

protected:
    //volatile T mValue;
    std::atomic<T> mValue;

    static_assert(sizeof(mValue) == sizeof(T), "std::atomic<T> and T must have the same size");
    static_assert(alignof(decltype(mValue)) == alignof(volatile T), "std::atomic<T> and T must have the same alignment");
    static_assert(std::atomic<T>::is_always_lock_free, "std::atomic<T>::is_always_lock_free must be true");
};

template <typename T>
class AtomicBase64
{
public:
    AtomicBase64()
        : mValue(0)
    {
    }

    explicit AtomicBase64(T value)
        : mValue(value)
    {
    }

    T getValue() const
    {
        return mValue.load();
    }

    void setValue(T value)
    {
        mValue.store(value);
    }

    void setValueNonAtomic(T value)
    {
        volatile T* ptr = reinterpret_cast<volatile T*>(&mValue);
        *ptr = value;
    }

    T add(s64 value)
    {
        return mValue.fetch_add(value);
    }

    T increment()
    {
        return add(1);
    }

    T decrement()
    {
        return add(-1);
    }

    T orMask(T value)
    {
        return mValue.fetch_or(value);
    }

    T andMask(T value)
    {
        return mValue.fetch_and(value);
    }

    T xorMask(T value)
    {
        return mValue.fetch_xor(value);
    }

    T swap(T value)
    {
        return mValue.exchange(value);
    }

    bool compareAndSwap(T compareValue, T newValue)
    {
        return mValue.compare_exchange_strong(compareValue, newValue);
    }

    bool compareAndSwapAndGetOriginal(T compareValue, T newValue, T* originalValue)
    {
        *originalValue = mValue.load(std::memory_order_relaxed);
        return mValue.compare_exchange_strong(compareValue, newValue);
    }

    bool isBitOn(u32 bit) const
    {
        return (getValue() & (static_cast<T>(1) << bit)) != 0;
    }

    bool setBitOn(u32 bit)
    {
        const T old = mValue.fetch_or(static_cast<T>(1) << bit);

        return (old & (static_cast<T>(1) << bit)) == 0;
    }

    bool setBitOff(u32 bit)
    {
        const T old = mValue.fetch_and(~(static_cast<T>(1) << bit));

        return (old & (static_cast<T>(1) << bit)) != 0;
    }

protected:
    //volatile T mValue;
    std::atomic<T> mValue;

    static_assert(sizeof(mValue) == sizeof(T), "std::atomic<T> and T must have the same size");
    static_assert(alignof(decltype(mValue)) == alignof(volatile T), "std::atomic<T> and T must have the same alignment");
    static_assert(std::atomic<T>::is_always_lock_free, "std::atomic<T>::is_always_lock_free must be true");
};

using AtomicU32 = AtomicBase32<u32>;
using AtomicU64 = AtomicBase64<u64>;

template <typename T>
class AtomicPtr
{
public:
    AtomicPtr()
        : mValue(nullptr)
    {
    }

    explicit AtomicPtr(T value)
        : mValue(value)
    {
    }

    T getValue() const
    {
        return mValue.load();
    }

    void setValue(T value)
    {
        mValue.store(value);
    }

    void setValueNonAtomic(T value)
    {
        volatile T* ptr = reinterpret_cast<volatile T*>(&mValue);
        *ptr = value;
    }

    T swap(T value)
    {
        return mValue.exchange(value);
    }

    bool compareAndSwap(T compareValue, T newValue)
    {
        return mValue.compare_exchange_strong(compareValue, newValue);
    }

    bool compareAndSwapAndGetOriginal(T compareValue, T newValue, T* originalValue)
    {
        *originalValue = mValue.load(std::memory_order_relaxed);
        return mValue.compare_exchange_strong(compareValue, newValue);
    }

protected:
    //volatile T mValue;
    std::atomic<T> mValue;

    static_assert(sizeof(mValue) == sizeof(T), "std::atomic<T> and T must have the same size");
    static_assert(alignof(decltype(mValue)) == alignof(volatile T), "std::atomic<T> and T must have the same alignment");
    static_assert(std::atomic<T>::is_always_lock_free, "std::atomic<T>::is_always_lock_free must be true");
};

} // namespace sead
