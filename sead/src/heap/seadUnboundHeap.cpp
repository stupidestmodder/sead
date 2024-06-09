#include <heap/seadUnboundHeap.h>

#include <heap/seadHeapMgr.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadScopedLock.h>

namespace sead {

UnboundHeap* UnboundHeap::create(const SafeString& name, bool enableLock)
{
    void* start = ::malloc(sizeof(UnboundHeap));
    SEAD_ASSERT_MSG(start, "UnboundHeap malloc failed");

    return new(start) UnboundHeap(name, enableLock);
}

UnboundHeap::UnboundHeap(const SafeString& name, bool enableLock)
    : Heap(name, nullptr, this, sizeof(UnboundHeap), HeapDirection::eForward, enableLock)
    , mMemBlockList()
    , mAllocedSize(0)
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    mMemBlockList.initOffset(offsetof(MemBlock, mListNode));
}

UnboundHeap::~UnboundHeap()
{
}

void UnboundHeap::destroy()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();

    if (HeapMgr::instance())
        HeapMgr::instance()->callDestroyCallback_(this);
#endif // SEAD_DEBUG

    freeAll();

    HeapMgr::removeFromFindContainHeapCache_(this);

    this->~UnboundHeap();

    ::free(this);
}

size_t UnboundHeap::adjust()
{
    return getSize();
}

void* UnboundHeap::tryAlloc(size_t size, s32 alignment)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    HeapMgr* heapMgr = HeapMgr::instance();

#ifdef SEAD_DEBUG
    HeapMgr::AllocCallbackArg allocCallbackArg;

    HeapMgr::IAllocCallback* allocCallback = nullptr;
    if (heapMgr)
    {
        allocCallback = heapMgr->getAllocCallback();
        if (allocCallback)
        {
            allocCallbackArg.request_size = size;
            allocCallbackArg.request_alignment = alignment;
            allocCallbackArg.heap = this;
        }
    }
#endif // SEAD_DEBUG

    HeapMgr::AllocFailedCallbackArg allocFailedCallbackArg;

    HeapMgr::IAllocFailedCallback* allocFailedCallback = nullptr;
    if (heapMgr)
    {
        allocFailedCallback = heapMgr->getAllocFailedCallback();
        if (allocFailedCallback)
        {
            allocFailedCallbackArg.request_size = size;
            allocFailedCallbackArg.request_alignment = alignment;
            allocFailedCallbackArg.heap = this;
        }
    }

    if (size < cMinAllocSize)
        size = cMinAllocSize;

    if (alignment < 0)
        alignment = -alignment;

    if (!Mathi::isPow2(alignment))
    {
        if (allocFailedCallback)
        {
            allocFailedCallbackArg.alloc_size = size;
            allocFailedCallbackArg.alloc_alignment = alignment;

            allocFailedCallback->invoke(&allocFailedCallbackArg);
        }

        SEAD_ASSERT_MSG(false, "bad alignment %d", alignment);

        return nullptr;
    }

    size = MathSizeT::roundUpPow2(size, cMinAlignment);

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

#ifdef SEAD_DEBUG
    if (allocCallback)
    {
        allocCallbackArg.alloc_size = size;
        allocCallbackArg.alloc_alignment = alignment;
    }
#endif // SEAD_DEBUG

    if (allocFailedCallback)
    {
        allocFailedCallbackArg.alloc_size = size;
        allocFailedCallbackArg.alloc_alignment = alignment;
    }

    MemBlock* memBlock = static_cast<MemBlock*>(::malloc(alignment + sizeof(MemBlock) + size));
    if (!memBlock)
    {
        if (allocFailedCallback)
            allocFailedCallback->invoke(&allocFailedCallbackArg);

        return nullptr;
    }

    memBlock = new(memBlock) MemBlock();

    memBlock->setHeapCheckTag(mHeapCheckTag);
    memBlock->setSize(size);

    mMemBlockList.pushBack(memBlock);

    void* ret = PtrUtil::roundUpPow2(PtrUtil::addOffset(memBlock, sizeof(MemBlock)), alignment);
    memBlock->setOffset(PtrUtil::diff(ret, PtrUtil::addOffset(memBlock, sizeof(MemBlock))));

    mAllocedSize += memBlock->getSizeWithManage();
    //SEAD_PRINT("Alloced %d (0x%X)\n", memBlock->getSizeWithManage(), memBlock->getSizeWithManage());

#ifdef SEAD_DEBUG
    if (isEnableDebugFillAlloc_())
    {
        if (heapMgr)
            memBlock->fill(heapMgr->getDebugFillAlloc());
        else
            memBlock->fill(HeapMgr::cDefaultDebugFillAlloc);
    }

    if (allocCallback)
    {
        allocCallbackArg.ptr = ret;

        allocCallback->invoke(&allocCallbackArg);
    }
#endif // SEAD_DEBUG

    return ret;
}

void UnboundHeap::free(void* ptr)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!ptr)
        return;

    HeapMgr* heapMgr = HeapMgr::instance();
#ifdef SEAD_DEBUG
    if (heapMgr)
    {
        HeapMgr::FreeCallbackArg arg;
        arg.heap = this;
        arg.ptr = ptr;

        heapMgr->callFreeCallback_(arg);
    }
#endif // SEAD_DEBUG

    MemBlock* memBlock = MemBlock::FindManageArea(ptr);
    if (!memBlock)
    {
        SEAD_ASSERT_MSG(false, "Invalid pointer: 0x%p", ptr);
        return;
    }

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    if (memBlock->mHeapCheckTag != mHeapCheckTag)
    {
        SEAD_ASSERT_MSG(false, "Name:(%s) Illegal free : 0x%p MemBlock:%u UnboundHeap:%u HeapMgr:%u",
                        getName().cstr(), ptr, memBlock->mHeapCheckTag, mHeapCheckTag, heapMgr->peekHeapCheckTag());
        return;
    }

    mMemBlockList.erase(memBlock);

    mAllocedSize -= memBlock->getSizeWithManage();
    //SEAD_PRINT("Freed %d (0x%X)\n", memBlock->getSizeWithManage(), memBlock->getSizeWithManage());

#ifdef SEAD_DEBUG
    if (isEnableDebugFillFree_())
    {
        heapMgr = HeapMgr::instance();
        if (heapMgr)
            memBlock->fill(heapMgr->getDebugFillFree());
        else
            memBlock->fill(HeapMgr::cDefaultDebugFillFree);
    }
#endif // SEAD_DEBUG

    ::free(memBlock);
}

void* UnboundHeap::resizeFront(void* ptr, size_t newSize)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    MemBlock* memBlock = MemBlock::FindManageArea(ptr);
    if (!memBlock)
    {
        SEAD_ASSERT_MSG(false, "Invalid pointer: 0x%p", ptr);
        return nullptr;
    }

    mMemBlockList.erase(memBlock);

    void* ret = ::realloc(memBlock, newSize);
    if (!ret)
    {
        mMemBlockList.pushBack(memBlock);
        return nullptr;
    }

    MemBlock* newMemBlock = static_cast<MemBlock*>(ret);

    mMemBlockList.pushBack(newMemBlock);

    return ret;
}

void* UnboundHeap::resizeBack(void* ptr, size_t newSize)
{
    return nullptr;
}

void UnboundHeap::freeAll()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    dispose_(nullptr, nullptr);

    for (auto it = mMemBlockList.robustBegin(); it != mMemBlockList.robustEnd(); ++it)
    {
        MemBlock* memBlock = &(*it);

        mMemBlockList.erase(memBlock);

        mAllocedSize -= memBlock->getSizeWithManage();

        ::free(memBlock);
    }

    SEAD_ASSERT(mAllocedSize == 0);
}

const void* UnboundHeap::getStartAddress() const
{
    return this;
}

const void* UnboundHeap::getEndAddress() const
{
    return PtrUtil::addOffset(this, getSize());
}

size_t UnboundHeap::getSize() const
{
    return mAllocedSize + sizeof(UnboundHeap);
}

size_t UnboundHeap::getFreeSize() const
{
    return 1024 * 1024 * 1024; // 1 GB
}

size_t UnboundHeap::getMaxAllocatableSize(s32 alignment) const
{
    return 1024 * 1024 * 1024; // 1 GB
}

bool UnboundHeap::isInclude(const void* ptr) const
{
/*
    for (const MemBlock& memBlock : mMemBlockList)
    {
        if (memBlock.isInclude(ptr))
            return true;
    }

    return false;
*/

    for (Heap& heap : mChildren)
    {
        if (heap.isInclude(ptr))
            return true;
    }

    MemBlock* memBlock = MemBlock::FindManageArea(const_cast<void*>(ptr));
    if (!memBlock)
        return false;

    if (memBlock->mHeapCheckTag != mHeapCheckTag)
        return false;

    return true;
}

bool UnboundHeap::isEmpty() const
{
    return mMemBlockList.size() == 0;
}

bool UnboundHeap::isFreeable() const
{
    return true;
}

bool UnboundHeap::isResizable() const
{
    return true;
}

bool UnboundHeap::isAdjustable() const
{
    return false;
}

void UnboundHeap::dump() const
{
    // TODO
}

void UnboundHeap::dumpYAML(WriteStream& stream, s32 indent) const
{
    // TODO
}

} // namespace sead
