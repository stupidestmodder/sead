#pragma once

#include <heap/seadHeap.h>
#include <heap/seadMemBlock.h>

namespace sead {

// TODO
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

    static size_t getManagementAreaSize(s32);

protected:
    ExpHeap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock);
    ~ExpHeap() override;

public:
    void destroy() override;
    size_t adjust() override;
    void* tryAlloc(size_t size, s32 alignment = alignof(void*)) override;
    void free(void* ptr) override;
    void* resizeFront(void* ptr, size_t newSize) override;
    void* resizeBack(void* ptr, size_t newSize) override;
    void* tryRealloc(void* ptr, size_t newSize, s32 alignment) override;
    void freeAll() override;
    const void* getStartAddress() const override;
    const void* getEndAddress() const override;
    size_t getSize() const override;
    size_t getFreeSize() const override;
    size_t getMaxAllocatableSize(s32 alignment = alignof(void*)) const override;
    bool isInclude(const void* ptr) const override;
    bool isEmpty() const override;
    bool isFreeable() const override;
    bool isResizable() const override;
    bool isAdjustable() const override;

    virtual void setAllocMode(AllocMode);
    virtual AllocMode getAllocMode() const;

    void dump() const override;
    void dumpFreeList() const;
    void dumpUseList() const;
    void checkFreeList() const;
    bool tryCheckFreeList() const;
    void checkUseList() const;
    bool tryCheckUseList() const;
    size_t getFreeListSize() const;
    size_t getUseListSize() const;
    size_t getAllocatedSize(void*);

    void dumpYAML(WriteStream& stream, s32 indent) const override;

    constIterator constBeginFreeList() const;
    constIterator constEndFreeList() const;
    constIterator constBeginUseList() const;
    constIterator constEndUseList() const;

protected:
    static void doCreate(ExpHeap*, Heap*);
    static void createMaxSizeFreeMemBlock_(ExpHeap*);

    MemBlock* findFreeMemBlockFromHead_(size_t, FindMode) const;
    MemBlock* findFreeMemBlockFromHead_(size_t, s32, FindMode) const;
    MemBlock* findFreeMemBlockFromTail_(size_t, FindMode) const;
    MemBlock* findFreeMemBlockFromTail_(size_t, s32, FindMode) const;
    MemBlock* findLastMemBlockIfFree_();
    MemBlock* findFirstMemBlockIfFree_();

    void pushToUseList_(MemBlock*);
    void pushToFreeList_(MemBlock*);

    static s32 compareMemBlockAddr_(const MemBlock*, const MemBlock*);

    MemBlock* allocFromHead_(size_t);
    MemBlock* allocFromHead_(size_t, s32);
    MemBlock* allocFromTail_(size_t);
    MemBlock* allocFromTail_(size_t, s32);

    size_t adjustBack_();
    size_t adjustFront_();

    void* realloc_(void*, u8*, size_t, size_t, s32);

protected:
    AllocMode mAllocMode;
    MemBlockList mFreeList;
    MemBlockList mUseList;
};

} // namespace sead
