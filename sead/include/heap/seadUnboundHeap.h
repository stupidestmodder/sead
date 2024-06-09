#pragma once

#include <heap/seadHeap.h>
#include <heap/seadMemBlock.h>

namespace sead {

// TODO
class UnboundHeap : public Heap
{
    SEAD_RTTI_OVERRIDE(UnboundHeap, Heap);

public:
    static UnboundHeap* create(const SafeString& name, bool enableLock = false);

    //static UnboundHeap* tryCreate(const SafeString& name, bool enableLock = false);

protected:
    UnboundHeap(const SafeString& name, bool enableLock);
    ~UnboundHeap() override;

public:
    void destroy() override;
    size_t adjust() override;
    void* tryAlloc(size_t size, s32 alignment = cMinAlignment) override;
    void free(void* ptr) override;
    void* resizeFront(void* ptr, size_t newSize) override;
    void* resizeBack(void* ptr, size_t newSize) override;
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
    void dump() const override;
    void dumpYAML(WriteStream& stream, s32 indent) const override;

    u32 getAllocedSize() const
    {
        return mAllocedSize;
    }

    s32 getMemBlockListSize() const
    {
        return mMemBlockList.size();
    }

protected:
    MemBlockList mMemBlockList;
    u32 mAllocedSize;
};

} // namespace sead
