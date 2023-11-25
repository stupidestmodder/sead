#pragma once

#include <basis/seadTypes.h>

namespace sead {

// TODO
template <typename T>
class AtomicBase32
{
public:
    AtomicBase32();
    AtomicBase32(T value);

    T getValue() const;
    void setValue(T value);
    void setValueNonAtomic(T value);

    T add(s32 value);
    T increment() { return add(1); }
    T decrement() { return add(-1); }
    T orMask(T value);
    T andMask(T value);
    T xorMask(T value);

    T swap(T value);
    bool compareAndSwap(T compareValue, T newValue);
    bool compareAndSwapAndGetOriginal(T compareValue, T newValue, T* originalValue);

    bool isBitOn(u32 bit) const;
    bool setBitOn(u32 bit);
    bool setBitOff(u32 bit);

protected:
    volatile T mValue;
};

// TODO
template <typename T>
class AtomicBase64
{
public:
    AtomicBase64();
    AtomicBase64(T value);

    T getValue() const;
    void setValue(T value);
    void setValueNonAtomic(T value);

    T add(s64 value);
    T increment();
    T decrement();
    T orMask(T value);
    T andMask(T value);
    T xorMask(T value);

    T swap(T value);
    bool compareAndSwap(T compareValue, T newValue);
    bool compareAndSwapAndGetOriginal(T compareValue, T newValue, T* originalValue);

    bool isBitOn(u32 bit) const;
    bool setBitOn(u32 bit);
    bool setBitOff(u32 bit);

protected:
    volatile T mValue;
};

using AtomicU32 = AtomicBase32<u32>;
using AtomicU64 = AtomicBase64<u64>;

// TODO
template <typename T>
class AtomicPtr
{
public:
    AtomicPtr();
    AtomicPtr(T value);

    T getValue() const;
    void setValue(T value);
    void setValueNonAtomic(T value);

    T swap(T value);
    bool compareAndSwap(T compareValue, T newValue);
    bool compareAndSwapAndGetOriginal(T compareValue, T newValue, T* originalValue);

protected:
    volatile T mValue;
};

} // namespace sead
