#pragma once

#include <heap/seadHeap.h>
#include <heap/seadMemBlock.h>

namespace sead {

class ExpHeap : public Heap
{
    SEAD_RTTI_OVERRIDE(ExpHeap, Heap);

public:
    enum class AllocMode
    {
        eFirstFit = 0,
        eBestFit
    };

    enum class FindMode
    {
        eFirstFit = 0,
        eBestFit,
        eMaxSize
    };

private:
    using constIterator = MemBlockList::constIterator;

public:
    static ExpHeap* create(size_t size, const SafeString& name, Heap* parent, HeapDirection direction = HeapDirection::eForward, bool enableLock = false);

    static ExpHeap* create(void* start, size_t size, const SafeString& name, bool enableLock = false)
    {
        ExpHeap* heap = ExpHeap::tryCreate(start, size, name, enableLock);
        SEAD_ASSERT_MSG(heap, "heap create failed. [%s] start: 0x%p, size: %zu", name.cstr(), start, size);
        return heap;
    }

    static ExpHeap* tryCreate(size_t size, const SafeString& name, Heap* parent, HeapDirection direction = HeapDirection::eForward, bool enableLock = false);
    static ExpHeap* tryCreate(void* start, size_t size, const SafeString& name, bool enableLock = false);

    static size_t getManagementAreaSize(s32 alignment = cMinAlignment);

protected:
    ExpHeap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock);
    ~ExpHeap() override;

public:
    void destroy() override;
    size_t adjust() override;
    void* tryAlloc(size_t size, s32 alignment = cMinAlignment) override;
    void free(void* ptr) override;
    void* resizeFront(void* ptr, size_t newSize) override;
    void* resizeBack(void* ptr, size_t newSize) override;
    void* tryRealloc(void* ptr, size_t newSize, s32 alignment = 0) override;
    void freeAll() override;
    const void* getStartAddress() const override;
    const void* getEndAddress() const override;
    size_t getSize() const override;
    size_t getFreeSize() const override;
    size_t getMaxAllocatableSize(s32 alignment = cMinAlignment) const override;
    bool isInclude(const void* ptr) const override;
    bool isEmpty() const override;
    bool isFreeable() const override;
    bool isResizable() const override;
    bool isAdjustable() const override;

    virtual void setAllocMode(AllocMode mode)
    {
        mAllocMode = mode;
    }

    virtual AllocMode getAllocMode() const
    {
        return mAllocMode;
    }

    void dump() const override;
    void dumpFreeList() const;
    void dumpUseList() const;

    // TODO
    void checkFreeList() const;

    bool tryCheckFreeList() const;

    // TODO
    void checkUseList() const;

    bool tryCheckUseList() const;

    size_t getFreeListSize() const { return mFreeList.size(); }
    size_t getUseListSize() const { return mUseList.size(); }
    size_t getAllocatedSize(void* ptr);

    void dumpYAML(WriteStream& stream, s32 indent) const override;

    constIterator constBeginFreeList() const { return mFreeList.constBegin(); }
    constIterator constEndFreeList() const { return mFreeList.constEnd(); }
    constIterator constBeginUseList() const { return mUseList.constBegin(); }
    constIterator constEndUseList() const { return mUseList.constEnd(); }

protected:
    static void doCreate(ExpHeap* heap, Heap* parent);
    static void createMaxSizeFreeMemBlock_(ExpHeap* heap);

    MemBlock* findFreeMemBlockFromHead_(size_t size, FindMode mode) const;
    MemBlock* findFreeMemBlockFromHead_(size_t size, s32 alignment, FindMode mode) const;
    MemBlock* findFreeMemBlockFromTail_(size_t size, FindMode mode) const;
    MemBlock* findFreeMemBlockFromTail_(size_t size, s32 alignment, FindMode mode) const;
    MemBlock* findLastMemBlockIfFree_();
    MemBlock* findFirstMemBlockIfFree_();

    void pushToUseList_(MemBlock* memBlock);
    void pushToFreeList_(MemBlock* memBlock);

#ifdef SEAD_DEBUG
    void fillMemBlockDebugFillFree_(void* addr);
#endif // SEAD_DEBUG

    static s32 compareMemBlockAddr_(const MemBlock* a, const MemBlock* b);

    MemBlock* allocFromHead_(size_t size);
    MemBlock* allocFromHead_(size_t size, s32 alignment);
    MemBlock* allocFromTail_(size_t size);
    MemBlock* allocFromTail_(size_t size, s32 alignment);

    size_t adjustBack_();
    size_t adjustFront_();

    void* realloc_(void* ptr, u8* oldMem, size_t copySize, size_t newSize, s32 alignment);

    friend class PrintFormatter;

protected:
    AllocMode mAllocMode;
    MemBlockList mFreeList;
    MemBlockList mUseList;
};

} // namespace sead
