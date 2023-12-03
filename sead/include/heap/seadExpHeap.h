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

public:
    static ExpHeap* create(size_t size, const SafeString& name, Heap* parent, HeapDirection direction, bool enableLock);
    static ExpHeap* create(void* start, size_t size, const SafeString& name, bool enableLock);
    static ExpHeap* tryCreate(size_t size, const SafeString& name, Heap* parent, HeapDirection direction, bool enableLock);
    static ExpHeap* tryCreate(void* start, size_t size, const SafeString& name, bool enableLock);

    static size_t getManagementAreaSize(s32);

protected:
    AllocMode mAllocMode;
    MemBlockList mFreeList;
    MemBlockList mUseList;
};

} // namespace sead
