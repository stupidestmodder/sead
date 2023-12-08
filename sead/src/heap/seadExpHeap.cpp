#include <heap/seadExpHeap.h>

#include <heap/seadHeapMgr.h>
#include <prim/seadScopedLock.h>

namespace sead {

ExpHeap* ExpHeap::create(size_t size, const SafeString& name, Heap* parent, HeapDirection direction, bool enableLock)
{
    ExpHeap* heap = ExpHeap::tryCreate(size, name, parent, direction, enableLock);
    if (!heap)
    {
        if (!parent)
            parent = HeapMgr::instance()->getCurrentHeap();

        if (!parent)
        {
            SEAD_ASSERT_MSG(false, "heap create failed. [%s] size: %zu, parent: --(0)", name.cstr(), size);
        }
        else
        {
            SEAD_ASSERT_MSG(false, "heap create failed. [%s] size: %zu, parent: %s(0x%p), parent allocatable size: %zu",
                            name.cstr(), size, parent->getName().cstr(), parent, parent->getMaxAllocatableSize());
        }
    }

    return heap;
}

ExpHeap::ExpHeap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock)
    : Heap(name, parent, start, size, direction, enableLock)
    , mAllocMode(AllocMode::eFirstFit)
    , mFreeList()
    , mUseList()
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    mFreeList.initOffset(offsetof(MemBlock, mListNode));
    mUseList.initOffset(offsetof(MemBlock, mListNode));
}

ExpHeap::~ExpHeap()
{
    destruct_();
}

} // namespace sead
