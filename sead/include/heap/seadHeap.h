#pragma once

#include <container/seadOffsetList.h>
#include <heap/seadDisposer.h>
#include <prim/seadBitFlag.h>
#include <prim/seadNamable.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <thread/seadCriticalSection.h>

namespace sead {

class Thread;
class WriteStream;

class Heap : public IDisposer, public INamable
{
    SEAD_RTTI_BASE(Heap);

#ifdef SEAD_DEBUG
protected:
    class ScopedDebugFillSystemDisabler
    {
    public:
        ScopedDebugFillSystemDisabler(Heap* heap)
            : mHeap(heap)
        {
            SEAD_ASSERT(mHeap);

            if (mHeap->isEnableLock())
                mHeap->mCS.lock();

            SEAD_ASSERT(Heap::isEnableDebugFillSystem_(mHeap));

            Heap::setEnableDebugFillSystem_(mHeap, false);
        }

        ~ScopedDebugFillSystemDisabler()
        {
            Heap::setEnableDebugFillSystem_(mHeap, true);

            if (mHeap->isEnableLock())
                mHeap->mCS.unlock();
        }

    private:
        Heap* mHeap;
    };
#endif // SEAD_DEBUG

public:
    enum HeapDirection
    {
        eForward =  1,
        eReverse = -1
    };

    // TODO: SEAD_ENUM(Flag)
    enum Flag
    {
        eEnableLock = 0,
        eDisposing,
        eEnableWarning,
#ifdef SEAD_DEBUG
        cEnableDebugFillSystem,
        cEnableDebugFillUser
#endif // SEAD_DEBUG
    };

private:
    using HeapList = OffsetList<Heap>;
    using DisposerList = OffsetList<IDisposer>;

protected:
    Heap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock);
    ~Heap() override;

public:
    virtual void destroy() = 0;
    virtual size_t adjust() = 0;

    void* alloc(size_t size, s32 alignment = alignof(void*))
    {
        void* ptr = tryAlloc(size, alignment);
        SEAD_ASSERT_MSG(ptr, "alloc failed. size: %zu, allocatable size: %zu, alignment: %d, heap: %s",
                        size, getMaxAllocatableSize(alignment), alignment, getName().cstr());
        return ptr;
    }

    virtual void* tryAlloc(size_t size, s32 alignment = alignof(void*)) = 0;
    virtual void free(void* ptr) = 0;
    virtual void* resizeFront(void* ptr, size_t newSize) = 0;
    virtual void* resizeBack(void* ptr, size_t newSize) = 0;

    void* realloc(void* ptr, size_t newSize, s32 alignment)
    {
        void* ret = tryRealloc(ptr, newSize, alignment);
        SEAD_ASSERT_MSG(newSize == 0 || ret, "realloc failed. size: %zu, allocatable size: %zu, alignment: %d, heap: %s",
                        newSize, getMaxAllocatableSize(alignment), alignment, getName().cstr());
        return ret;
    }

    virtual void* tryRealloc(void* ptr, size_t newSize, s32 alignment)
    {
        SEAD_UNUSED(ptr);
        SEAD_UNUSED(newSize);
        SEAD_UNUSED(alignment);

        SEAD_ASSERT_MSG(false, "tryRealloc is not implement.");
        return nullptr;
    }

    virtual void freeAll() = 0;
    virtual const void* getStartAddress() const = 0;
    virtual const void* getEndAddress() const = 0;
    virtual size_t getSize() const = 0;
    virtual size_t getFreeSize() const = 0;
    virtual size_t getMaxAllocatableSize(s32 alignment = alignof(void*)) const = 0;
    virtual bool isInclude(const void* ptr) const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool isFreeable() const = 0;
    virtual bool isResizable() const = 0;
    virtual bool isAdjustable() const = 0;

    virtual void dump() const { }

    Heap* getParent() const { return mParent; }
    HeapDirection getDirection() const { return mDirection; }

    void setEnableLock(bool enable) { mFlag.changeBit(Flag::eEnableLock, enable); }
    bool isEnableLock() const { return mFlag.isOnBit(Flag::eEnableLock); }

    bool lock()
    {
        if (!isEnableLock())
            return false;

        mCS.lock();
        return true;
    }

    bool unlock()
    {
        if (!isEnableLock())
            return false;

        mCS.unlock();
        return true;
    }

    void setEnableWarning(bool enable) { mFlag.changeBit(Flag::eEnableWarning, enable); }
    bool isEnableWarning() const { return mFlag.isOnBit(Flag::eEnableWarning); }

    void setEnableDebugFill(bool enable);
    bool isEnableDebugFill();

    void unsafeSetDisposingFlag(bool);

    void dumpTreeYAML(WriteStream& stream, s32 indent) const;
    virtual void dumpYAML(WriteStream& stream, s32 indent) const;

    HeapList::constIterator childBegin() const { return mChildren.constBegin(); }
    HeapList::constIterator childEnd() const { return mChildren.constEnd(); }
    DisposerList::constIterator disposerBegin() const { return mDisposerList.constBegin(); }
    DisposerList::constIterator disposerEnd() const { return mDisposerList.constEnd(); }

    virtual void pushBackChild_(Heap* child);

protected:
    Heap* findContainHeap_(const void* ptr);
    bool hasNoChild_() const { return mChildren.size() == 0; }
    void destruct_();
    void dispose_(const void* begin, const void* end);
    void appendDisposer_(IDisposer* disposer);
    void removeDisposer_(IDisposer* disposer);
    void eraseChild_(Heap* child);
    void checkAccessThread_() const;

#ifdef SEAD_DEBUG
    bool isEnableDebugFillAlloc_() const;
    bool isEnableDebugFillFree_() const;
    bool isEnableDebugFillHeapDestroy_() const;

    static void setEnableDebugFillSystem_(Heap* heap, bool enable)
    {
        heap->mFlag.changeBit(Flag::cEnableDebugFillSystem, enable);
    }

    static bool isEnableDebugFillSystem_(Heap* heap)
    {
        return heap->mFlag.isOnBit(Flag::cEnableDebugFillSystem);
    }
#endif // SEAD_DEBUG

    friend class IDisposer;
    friend class HeapMgr;

protected:
    void* mStart;
    size_t mSize;
    Heap* mParent;
    HeapList mChildren;
    ListNode mListNode;
    DisposerList mDisposerList;
    HeapDirection mDirection;
    CriticalSection mCS;
    BitFlag16 mFlag;
    u16 mHeapCheckTag;
#ifdef SEAD_DEBUG
    Thread* mAccessThread;
#endif // SEAD_DEBUG
};

} // namespace sead
