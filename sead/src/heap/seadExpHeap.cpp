#include <heap/seadExpHeap.h>

#include <heap/seadHeapMgr.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadFormatPrint.h>
#include <prim/seadScopedLock.h>
#include <stream/seadStream.h>
#include <thread/seadThreadUtil.h>

#include <climits>

namespace sead {

const size_t cMinAlignment = alignof(void*);
const size_t cDefaultAlignment = cMinAlignment;

ExpHeap* ExpHeap::create(size_t size, const SafeString& name, Heap* parent, HeapDirection direction, bool enableLock)
{
    ExpHeap* heap = ExpHeap::tryCreate(size, name, parent, direction, enableLock);

#ifdef SEAD_DEBUG
    if (!heap)
    {
        if (!parent)
            parent = HeapMgr::instance()->getCurrentHeap();

        if (!parent)
        {
            SEAD_ASSERT_MSG(false, "heap create failed. [%s] size: %zu, parent: --(0x0)", name.cstr(), size);
        }
        else
        {
            SEAD_ASSERT_MSG(false, "heap create failed. [%s] size: %zu, parent: %s(0x%p), parent allocatable size: %zu",
                            name.cstr(), size, parent->getName().cstr(), parent, parent->getMaxAllocatableSize());
        }
    }
#endif // SEAD_DEBUG

    return heap;
}

ExpHeap* ExpHeap::tryCreate(size_t size_, const SafeString& name, Heap* parent, HeapDirection direction, bool enableLock)
{
    if (!parent)
    {
        parent = HeapMgr::instance()->getCurrentHeap();
        if (!parent)
        {
            SEAD_ASSERT_MSG(false, "current heap is null");
            return nullptr;
        }
    }

    size_t size;
    if (size_ == 0)
    {
        size = parent->getMaxAllocatableSize();
        SEAD_ASSERT_MSG(MathSizeT::isMultiplePow2(size, cMinAlignment),
                        "[bug] getMaxAllocatableSize must return multiple of cMinAlignment");
        size = MathSizeT::roundDownPow2(size, cMinAlignment);
    }
    else
    {
        size = MathSizeT::roundUpPow2(size_, cMinAlignment);
    }

    if (size < sizeof(ExpHeap) + sizeof(MemBlock) + 0x1)
    {
        SEAD_ASSERT_MSG(size_ == 0, "size must be able to include manage area: size=%zu", size);
        return nullptr;
    }

    void* start;

    {
#ifdef SEAD_DEBUG
        ScopedDebugFillSystemDisabler disabler(parent);
#endif // SEAD_DEBUG
        start = parent->tryAlloc(size, static_cast<s32>(direction) * cMinAlignment);
    }

    if (!start)
        return nullptr;

    if (parent->getDirection() == HeapDirection::eReverse)
        direction = static_cast<HeapDirection>(-static_cast<s32>(direction));

    ExpHeap* heap;
    if (direction == HeapDirection::eForward)
    {
        heap = new(start) ExpHeap(name, parent, start, size, direction, enableLock);
    }
    else
    {
        void* ptr = PtrUtil::addOffset(start, size - sizeof(ExpHeap));
        heap = new(ptr) ExpHeap(name, parent, start, size, direction, enableLock);
    }

    ExpHeap::doCreate(heap, parent);

    return heap;
}

ExpHeap* ExpHeap::tryCreate(void* start, size_t size, const SafeString& name, bool enableLock)
{
    size = MathSizeT::roundDownPow2(size, cMinAlignment);
    if (size < sizeof(ExpHeap) + sizeof(MemBlock) + 0x1)
    {
        SEAD_ASSERT_MSG(false, "size must be able to include manage area: size=%zu", size);
        return nullptr;
    }

    ExpHeap* heap = new(start) ExpHeap(name, nullptr, start, size, HeapDirection::eForward, enableLock);
    ExpHeap::doCreate(heap, nullptr);

    return heap;
}

size_t ExpHeap::getManagementAreaSize(s32 alignment)
{
    return alignment + sizeof(ExpHeap) + sizeof(MemBlock);
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

    //mFreeSize = getFreeSize();
}

ExpHeap::~ExpHeap()
{
    destruct_();
}

void ExpHeap::destroy()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();

    if (HeapMgr::instance())
        HeapMgr::instance()->callDestroyCallback_(this);
#endif // SEAD_DEBUG

    Heap* parent = mParent;
    void* start = mStart;

#ifdef SEAD_DEBUG
    size_t size = mSize;

    MemBlock* block = findFreeMemBlockFromHead_(0, FindMode::eMaxSize);

    size_t blockSize = 0;
    if (block)
        blockSize = block->getSizeWithManage();

    bool isEnableDebugFill = isEnableDebugFillHeapDestroy_();
#endif // SEAD_DEBUG

    this->~ExpHeap();

#ifdef SEAD_DEBUG
    if (isEnableDebugFill)
    {
        u8 debugFill = HeapMgr::cDefaultDebugFillHeapDestroy;
        if (HeapMgr::instance())
            debugFill = HeapMgr::instance()->getDebugFillHeapDestroy();

        if (!block)
        {
            MemUtil::fill(start, debugFill, size);
        }
        else
        {
            MemUtil::fill(start, debugFill, PtrUtil::diff(block, start));

            void* addr = PtrUtil::addOffset(block, blockSize);
            size_t remainSize = size - PtrUtil::diff(addr, start);
            if (remainSize > 0)
                MemUtil::fill(addr, debugFill, remainSize);
        }
    }
#endif // SEAD_DEBUG

    if (parent && parent->isFreeable())
    {
#ifdef SEAD_DEBUG
        ScopedDebugFillSystemDisabler disabler(parent);
#endif // SEAD_DEBUG
        parent->free(start);
    }
}

size_t ExpHeap::adjust()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!mParent)
        return mSize;

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    size_t newSize = 0;

    {
        ScopedLock<Heap> parentLock(mParent);

        if (mDirection == HeapDirection::eForward)
            newSize = adjustBack_();
        else
            newSize = adjustFront_();
    }

    //mFreeSize = getFreeSize();

    return newSize;
}

void* ExpHeap::tryAlloc(size_t size, s32 alignment)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    HeapMgr* heapMgr = HeapMgr::instance();

    size_t allocSize = size < cMinAlignment ? cMinAlignment : size;
    s32 allocAlignment = alignment;

    if (allocSize >= mSize)
    {
        if (heapMgr)
        {
            HeapMgr::IAllocFailedCallback* callback = heapMgr->getAllocFailedCallback();
            if (callback)
            {
                HeapMgr::AllocFailedCallbackArg arg;
                arg.heap = this;
                arg.request_size = size;
                arg.request_alignment = alignment;
                arg.alloc_size = allocSize;
                arg.alloc_alignment = allocAlignment;

                callback->invoke(&arg);
            } 
        }

        return nullptr;
    }

    if (!Mathi::isPow2(Mathi::abs(alignment)))
    {
        if (heapMgr)
        {
            HeapMgr::IAllocFailedCallback* callback = heapMgr->getAllocFailedCallback();
            if (callback)
            {
                HeapMgr::AllocFailedCallbackArg arg;
                arg.heap = this;
                arg.request_size = size;
                arg.request_alignment = alignment;
                arg.alloc_size = allocSize;
                arg.alloc_alignment = allocAlignment;

                callback->invoke(&arg);
            } 
        }

        SEAD_ASSERT_MSG(false, "bad alignment %d", allocAlignment);

        return nullptr;
    }

    allocSize = MathSizeT::roundUpPow2(allocSize, cMinAlignment);

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    allocAlignment *= static_cast<s32>(mDirection);

    MemBlock* block = nullptr;
    if (allocAlignment < 0)
    {
        allocAlignment = -allocAlignment;
        if (allocAlignment <= cMinAlignment)
            block = allocFromTail_(allocSize);
        else
            block = allocFromTail_(allocSize, allocAlignment);
    }
    else
    {
        if (allocAlignment <= cMinAlignment)
            block = allocFromHead_(allocSize);
        else
            block = allocFromHead_(allocSize, allocAlignment);
    }

    void* ret = nullptr;
    if (block)
    {
        block->setHeapCheckTag(mHeapCheckTag);
        ret = block->memory();

#ifdef SEAD_DEBUG
        if (isEnableDebugFillAlloc_())
        {
            if (heapMgr)
                block->fill(heapMgr->getDebugFillAlloc());
            else
                block->fill(HeapMgr::cDefaultDebugFillAlloc);
        }

        if (heapMgr)
        {
            HeapMgr::IAllocCallback* callback = heapMgr->getAllocCallback();
            if (callback)
            {
                HeapMgr::AllocCallbackArg arg;
                arg.heap = this;
                arg.ptr = ret;
                arg.request_size = size;
                arg.request_alignment = alignment;
                arg.alloc_size = allocSize;
                arg.alloc_alignment = allocAlignment;

                callback->invoke(&arg);
            }
        }
#endif // SEAD_DEBUG
    }
    else
    {
        if (heapMgr)
        {
            HeapMgr::IAllocFailedCallback* callback = heapMgr->getAllocFailedCallback();
            if (callback)
            {
                HeapMgr::AllocFailedCallbackArg arg;
                arg.heap = this;
                arg.request_size = size;
                arg.request_alignment = alignment;
                arg.alloc_size = allocSize;
                arg.alloc_alignment = allocAlignment;

                callback->invoke(&arg);
            } 
        }
    }

    //mFreeSize = getFreeSize();

    return ret;
}

void ExpHeap::free(void* ptr)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!ptr)
        return;

#ifdef SEAD_DEBUG
    HeapMgr* heapMgr = HeapMgr::instance();
    if (heapMgr)
    {
        HeapMgr::FreeCallbackArg arg;
        arg.heap = this;
        arg.ptr = ptr;

        heapMgr->callFreeCallback_(arg);
    }
#endif // SEAD_DEBUG

    if (!isInclude(ptr))
    {
        SEAD_ASSERT_MSG(false, "free error. This heap(%s) is not including 0x%p.", getName().cstr(), ptr);
        return;
    }

    if (mFlag.isOnBit(Flag::eDisposing))
        return;

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    MemBlock* block = MemBlock::FindManageArea(ptr);
    if (!block)
    {
        dumpUseList();
        dumpFreeList();
        SEAD_ASSERT_MSG(false, "free failed: 0x%p", ptr);
        return;
    }

#ifdef SEAD_DEBUG
    if (block->memory() != ptr)
    {
        SEAD_PRINT("===================================\n");
        SEAD_PRINT("Name:(%s) Invalid pointer: 0x%p\n", getName().cstr(), ptr);
        SEAD_PRINT("[ block (0x%p) ]\n", block);
        MemUtil::dumpMemoryBinary(block, 16 * 0, 16 * 2, true);
        SEAD_PRINT("\n");
        SEAD_PRINT("[ ptr (0x%p) ]\n", ptr);
        MemUtil::dumpMemoryBinary(ptr, 16 * 2, 16 * 2, true);
        SEAD_PRINT("===================================\n");

        SEAD_ASSERT_MSG(false, "Name:(%s) Invalid pointer: 0x%p\n", getName().cstr(), ptr);
    }
#endif // SEAD_DEBUG

    if (block->mHeapCheckTag != mHeapCheckTag)
    {
        SEAD_ASSERT_MSG(false, "Name:(%s) Illegal free : 0x%p MemBlock:%u ExpHeap:%u HeapMgr:%u",
                        getName().cstr(), ptr, block->mHeapCheckTag, mHeapCheckTag, heapMgr->peekHeapCheckTag());
        return;
    }

#ifdef SEAD_DEBUG
    if (isEnableDebugFillFree_())
    {
        heapMgr = HeapMgr::instance();
        if (heapMgr)
            block->fill(heapMgr->getDebugFillFree());
        else
            block->fill(HeapMgr::cDefaultDebugFillFree);
    }
#endif // SEAD_DEBUG

    mUseList.erase(block);

    block->setSize(block->getSize() + block->getOffset());
    block->setOffset(0);

    pushToFreeList_(block);

    //mFreeSize = getFreeSize();
}

void* ExpHeap::resizeFront(void* ptr, size_t newSize)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!isInclude(ptr))
    {
        SEAD_ASSERT_MSG(false, "resize error. This heap(%s) is not including 0x%p.", getName().cstr(), ptr);
        return nullptr;
    }

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    MemBlock* block = MemBlock::FindManageArea(ptr);

    newSize = MathSizeT::roundUpPow2(newSize, cMinAlignment);
    if (newSize > block->getSize())
    {
        SEAD_ASSERT_MSG(false, "newSize[%zu] > block->getSize()[%zu]", newSize, block->getSize());
        return nullptr;
    }

    if (newSize == block->getSize())
        return block->memory();

    size_t remainSize = block->getSizeWithManage() - newSize - sizeof(MemBlock);
    if (remainSize < sizeof(MemBlock))
    {
        size_t offset = block->getOffset() + block->getSize() - newSize;
        SEAD_ASSERT_MSG(offset <= UINT16_MAX, "Offset is too large.");
        block->setOffset(static_cast<u16>(offset));
        return block->memory();
    }
    else
    {
        MemBlock* newBlock = new(PtrUtil::addOffset(block, remainSize)) MemBlock();
        newBlock->setHeapCheckTag(mHeapCheckTag);
        newBlock->setSize(newSize);
        pushToUseList_(newBlock);

        mUseList.erase(block);
        block->setSize(remainSize - sizeof(MemBlock));
        block->setOffset(0);
        pushToFreeList_(block);

        //mFreeSize = getFreeSize();

        return newBlock->memory();
    }
}

void* ExpHeap::resizeBack(void* ptr, size_t newSize)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!isInclude(ptr))
    {
        SEAD_ASSERT_MSG(false, "resize error. This heap(%s) is not including 0x%p.", getName().cstr(), ptr);
        return nullptr;
    }

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    MemBlock* block = MemBlock::FindManageArea(ptr);

    newSize = MathSizeT::roundUpPow2(newSize, cMinAlignment);
    if (newSize > block->getSize())
    {
        SEAD_ASSERT_MSG(false, "newSize[%zu] > block->getSize()[%zu]", newSize, block->getSize());
        return nullptr;
    }

    if (newSize == block->getSize())
        return block->memory();

    size_t remainSize = block->getSize() - newSize;
    if (remainSize < sizeof(MemBlock))
    {
        return block->memory();
    }
    else
    {
        block->setSize(newSize);

        MemBlock* newBlock = new(PtrUtil::addOffset(block->memory(), newSize)) MemBlock();
        newBlock->setSize(remainSize - sizeof(MemBlock));
        pushToFreeList_(newBlock);

        //mFreeSize = getFreeSize();

        return block->memory();
    }
}

void* ExpHeap::tryRealloc(void* ptr, size_t newSize, s32 alignment)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!ptr)
        return tryAlloc(newSize, alignment);

    if (newSize == 0)
    {
        free(ptr);
        return nullptr;
    }

    if (!isInclude(ptr))
    {
        SEAD_ASSERT_MSG(false, "realloc error. This heap(%s) is not including 0x%p.", getName().cstr(), ptr);
        return nullptr;
    }

    if (alignment < 0)
    {
        SEAD_ASSERT_MSG(false, "realloc alignment under 0");
        return nullptr;
    }

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    MemBlock* block = MemBlock::FindManageArea(ptr);

    newSize = MathSizeT::roundUpPow2(newSize, cMinAlignment);
    if (newSize > block->getSize())
    {
        if (alignment == 0)
            alignment = cMinAlignment;

        return realloc_(ptr, block->memory(), block->getSize(), newSize, alignment);
    }

    if (newSize == block->getSize())
    {
        if (alignment != 0 && !PtrUtil::isAlignedPow2(block->memory(), alignment))
                return realloc_(ptr, block->memory(), block->getSize(), newSize, alignment);

        return block->memory();
    }

    if (alignment != 0 && !PtrUtil::isAlignedPow2(block->memory(), alignment))
        return realloc_(ptr, block->memory(), newSize, newSize, alignment);

    size_t remainSize = block->getSize() - newSize;
    if (remainSize < sizeof(MemBlock))
    {
        return block->memory();
    }
    else
    {
        block->setSize(newSize);

        MemBlock* newBlock = new(PtrUtil::addOffset(block->memory(), newSize)) MemBlock();
        newBlock->setSize(remainSize - sizeof(MemBlock));
        pushToFreeList_(newBlock);

        //mFreeSize = getFreeSize();

        return block->memory();
    }
}

void ExpHeap::freeAll()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    dispose_(nullptr, nullptr);

    mUseList.clear();
    mFreeList.clear();

    ExpHeap::createMaxSizeFreeMemBlock_(this);
}

const void* ExpHeap::getStartAddress() const
{
    return mStart;
}

const void* ExpHeap::getEndAddress() const
{
    return PtrUtil::addOffset(mStart, mSize);
}

size_t ExpHeap::getSize() const
{
    return mSize;
}

size_t ExpHeap::getFreeSize() const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    size_t freeSize = 0;
    for (MemBlock& block : mFreeList)
    {
        freeSize += block.getSize();
    }

    return freeSize;
}

size_t ExpHeap::getMaxAllocatableSize(s32 alignment) const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!Mathi::isPow2(Mathi::abs(alignment)))
    {
        SEAD_ASSERT_MSG(false, "bad alignment %d", alignment);
        return 0;
    }

    const size_t cPtrSize = sizeof(void*);

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    MemBlock* block = nullptr;
    size_t diff = 0;

    alignment *= static_cast<s32>(mDirection);
    if (alignment < 0)
    {
        alignment = -alignment;
        if (alignment <= cMinAlignment)
        {
            block = findFreeMemBlockFromTail_(cPtrSize, FindMode::eMaxSize);
        }
        else
        {
            block = findFreeMemBlockFromTail_(cPtrSize, alignment, FindMode::eMaxSize);
            if (block)
            {
                void* memory = PtrUtil::addOffset(block->memory(), block->getSize() - cPtrSize);
                diff = PtrUtil::diff(memory, PtrUtil::roundDownPow2(memory, alignment));
            }
        }
    }
    else
    {
        if (alignment <= cMinAlignment)
        {
            block = findFreeMemBlockFromHead_(cPtrSize, FindMode::eMaxSize);
        }
        else
        {
            block = findFreeMemBlockFromHead_(cPtrSize, alignment, FindMode::eMaxSize);
            if (block)
            {
                void* memory = block->memory();
                diff = PtrUtil::diff(PtrUtil::roundUpPow2(memory, alignment), memory);
            }
        }
    }

    if (!block)
        return 0;

    return block->getSize() - diff;
}

bool ExpHeap::isInclude(const void* ptr) const
{
    void* start = nullptr;
    void* end = nullptr;

    if (mDirection == HeapDirection::eForward)
    {
        start = PtrUtil::addOffset(mStart, sizeof(ExpHeap));
        end = PtrUtil::addOffset(mStart, mSize);
    }
    else
    {
        start = mStart;
        end = PtrUtil::addOffset(mStart, mSize - sizeof(ExpHeap));
    }

    return start <= ptr && ptr < end;
}

bool ExpHeap::isEmpty() const
{
    return mUseList.size() == 0;
}

bool ExpHeap::isFreeable() const
{
    return true;
}

bool ExpHeap::isResizable() const
{
    return true;
}

bool ExpHeap::isAdjustable() const
{
    return true;
}

void ExpHeap::dump() const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

#ifdef SEAD_DEBUG
    {
        BufferingPrintFormatter formatter;

        formatter << "%@", *this;
        formatter.flush();
    }
#endif // SEAD_DEBUG

    dumpUseList();
    dumpFreeList();
}

void ExpHeap::dumpFreeList() const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    SEAD_PRINT("--------- dumpFreeList %s ---------\n", getName().cstr());

    if constexpr (sizeof(void*) == 4)
        SEAD_PRINT("[ %-10s ] %-10s | size       | offset   | %-10s | %-10s\n", "HeaderAddr", "memory()", "prev", "next");
    else if constexpr (sizeof(void*) == 8)
        SEAD_PRINT("[ %-18s ] %-18s | size       | offset   | %-18s | %-18s\n", "HeaderAddr", "memory()", "prev", "next");

    for (MemBlock& block : mFreeList)
    {
        SEAD_PRINT("[ 0x%p ] 0x%p | %10zu | %8d | 0x%p | 0x%p\n",
                   &block, block.memory(), block.getSize(), block.getOffset(), mFreeList.prev(&block), mFreeList.next(&block));
    }

    SEAD_PRINT("--------- dumpFreeList done ---------\n");
}

void ExpHeap::dumpUseList() const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    SEAD_PRINT("--------- dumpUseList %s ---------\n", getName().cstr());

    if constexpr (sizeof(void*) == 4)
        SEAD_PRINT("[ %-10s ] %-10s | size       | offset   | %-10s | %-10s\n", "HeaderAddr", "memory()", "prev", "next");
    else if constexpr (sizeof(void*) == 8)
        SEAD_PRINT("[ %-18s ] %-18s | size       | offset   | %-18s | %-18s\n", "HeaderAddr", "memory()", "prev", "next");

    for (MemBlock& block : mUseList)
    {
        SEAD_PRINT("[ 0x%p ] 0x%p | 0x%zX | %8d | 0x%p | 0x%p\n",
                   &block, block.memory(), block.getSize(), block.getOffset(), mUseList.prev(&block), mUseList.next(&block));
    }

    SEAD_PRINT("--------- dumpUseList done ---------\n");
}

bool ExpHeap::tryCheckFreeList() const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    for (MemBlock& block : mFreeList)
    {
        if (block.getOffset() != 0)
            return false;

        if (block.getSize() == 0)
            return false;

        if (reinterpret_cast<uintptr_t>(block.memory()) % cMinAlignment != 0)
            return false;
    }

    return mFreeList.checkLinks();
}

bool ExpHeap::tryCheckUseList() const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    for (MemBlock& block : mUseList)
    {
        if (!block.isValid(mStart, mSize))
        {
            SEAD_PRINT("Heap %s (0x%p-0x%p) : MemoryBlock corruption is detected!\n",
                       getName().cstr(), mStart, reinterpret_cast<intptr_t>(mStart) + mSize);
            block.dump();
            return false;
        }
    }

    return mUseList.checkLinks();
}

size_t ExpHeap::getAllocatedSize(void* ptr)
{
    if (!isInclude(ptr))
        return 0;

    MemBlock* block = MemBlock::FindManageArea(ptr);
    return block->getSize();
}

void ExpHeap::dumpYAML(WriteStream& stream, s32 indent) const
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    Heap::dumpYAML(stream, indent);

    FixedSafeString<128> buf("");
    buf.append(' ', indent);
    buf.appendWithFormat("  heap_type: ExpHeap\n");
    stream.writeDecorationText(buf);

    buf.clear();
    buf.append(' ', indent);

    const char* allocMode;

    if (getAllocMode() == AllocMode::eFirstFit)
        allocMode = "First Fit";
    else
        allocMode = "Best Fit";

    buf.appendWithFormat("  alloc_mode: %s\n", allocMode);
    stream.writeDecorationText(buf);

    buf.clear();
    buf.append(' ', indent);
    buf.appendWithFormat("  use_list_size: %d\n", mUseList.size());
    stream.writeDecorationText(buf);

    buf.clear();
    buf.append(' ', indent);
    buf.appendWithFormat("  free_list_size: %d\n", mFreeList.size());
    stream.writeDecorationText(buf);
}

void ExpHeap::doCreate(ExpHeap* heap, Heap* parent)
{
    SEAD_ASSERT_MSG(heap, "heap is null");

    ExpHeap::createMaxSizeFreeMemBlock_(heap);

    if (parent)
        parent->pushBackChild_(heap);

#ifdef SEAD_DEBUG
    HeapMgr* heapMgr = HeapMgr::instance();
    if (heapMgr)
        heapMgr->callCreateCallback_(heap);
#endif // SEAD_DEBUG
}

void ExpHeap::createMaxSizeFreeMemBlock_(ExpHeap* heap)
{
    ConditionalScopedLock<CriticalSection> lock(&heap->mCS, heap->isEnableLock());

    MemBlock* block = nullptr;
    if (heap->mDirection == HeapDirection::eForward)
        block = new(PtrUtil::addOffset(heap, sizeof(ExpHeap))) MemBlock();
    else
        block = new(heap->mStart) MemBlock();

    block->setSize(heap->mSize - sizeof(ExpHeap) - sizeof(MemBlock));
    heap->pushToFreeList_(block);

#ifdef SEAD_DEBUG
    HeapMgr* heapMgr = HeapMgr::instance();
    if (heapMgr && heapMgr->isEnableDebugFillHeapCreate())
        block->fill(heapMgr->getDebugFillHeapCreate());
#endif // SEAD_DEBUG
}

MemBlock* ExpHeap::findFreeMemBlockFromHead_(size_t size, FindMode mode) const
{
    MemBlock* ret = nullptr;

    for (MemBlock& block : mFreeList)
    {
        size_t blockSize = block.getSize();
        if (blockSize >= size)
        {
            if (mode == FindMode::eFirstFit)
                return &block;

            if (!ret
                || mode == FindMode::eBestFit && blockSize < ret->getSize()
                || mode == FindMode::eMaxSize && blockSize > ret->getSize())
            {
                ret = &block;
            }
        }
    }

    return ret;
}

MemBlock* ExpHeap::findFreeMemBlockFromHead_(size_t size, s32 alignment, FindMode mode) const
{
    MemBlock* ret = nullptr;

    for (MemBlock& block : mFreeList)
    {
        size_t blockSize = block.getSize();
        if (blockSize >= size)
        {
            void* memory = block.memory();
            if (blockSize >= size + PtrUtil::diff(PtrUtil::roundUpPow2(memory, alignment), memory))
            {
                if (mode == FindMode::eFirstFit)
                    return &block;

                if (!ret
                    || mode == FindMode::eBestFit && blockSize < ret->getSize()
                    || mode == FindMode::eMaxSize && blockSize > ret->getSize())
                {
                    ret = &block;
                }
            }
        }
    }

    return ret;
}

MemBlock* ExpHeap::findFreeMemBlockFromTail_(size_t size, FindMode mode) const
{
    MemBlock* ret = nullptr;

    for (MemBlock* block = mFreeList.back(); block != nullptr; block = mFreeList.prev(block))
    {
        size_t blockSize = block->getSize();
        if (blockSize >= size)
        {
            if (mode == FindMode::eFirstFit)
                return block;

            if (!ret
                || mode == FindMode::eBestFit && blockSize < ret->getSize()
                || mode == FindMode::eMaxSize && blockSize > ret->getSize())
            {
                ret = block;
            }
        }
    }

    return ret;
}

MemBlock* ExpHeap::findFreeMemBlockFromTail_(size_t size, s32 alignment, FindMode mode) const
{
    MemBlock* ret = nullptr;

    for (MemBlock* block = mFreeList.back(); block != nullptr; block = mFreeList.prev(block))
    {
        size_t blockSize = block->getSize();
        if (blockSize >= size)
        {
            void* memory = PtrUtil::addOffset(block->memory(), block->getSize() - size);
            if (blockSize >= size + PtrUtil::diff(memory, PtrUtil::roundDownPow2(memory, alignment)))
            {
                if (mode == FindMode::eFirstFit)
                    return block;

                if (!ret
                    || mode == FindMode::eBestFit && blockSize < ret->getSize()
                    || mode == FindMode::eMaxSize && blockSize > ret->getSize())
                {
                    ret = block;
                }
            }
        }
    }

    return ret;
}

MemBlock* ExpHeap::findLastMemBlockIfFree_()
{
    mUseList.sort(&ExpHeap::compareMemBlockAddr_);

    MemBlock* backFreeBlock = mFreeList.back();
    MemBlock* backUseBlock = mUseList.back();

    if (backFreeBlock <= backUseBlock)
        return nullptr;

    return backFreeBlock;
}

MemBlock* ExpHeap::findFirstMemBlockIfFree_()
{
    mUseList.sort(&ExpHeap::compareMemBlockAddr_);

    MemBlock* frontFreeBlock = mFreeList.front();
    MemBlock* frontUseBlock = mUseList.front();

    if (frontUseBlock && frontUseBlock <= frontFreeBlock)
        return nullptr;

    return frontFreeBlock;
}

void ExpHeap::pushToUseList_(MemBlock* memBlock)
{
    if (mDirection == HeapDirection::eForward)
        mUseList.pushBack(memBlock);
    else
        mUseList.pushFront(memBlock);
}

void ExpHeap::pushToFreeList_(MemBlock* memBlock)
{
    bool blockInserted = false;
    MemBlock* prevBlock = nullptr;

    for (MemBlock& currentBlock : mFreeList)
    {
        if (memBlock < &currentBlock)
        {
            bool merged = false;
            MemBlock* mergedBlock = memBlock;

            if (prevBlock)
            {
                if (PtrUtil::addOffset(prevBlock->memory(), prevBlock->getSize()) == mergedBlock)
                {
                    prevBlock->setSize(prevBlock->getSize() + mergedBlock->getSizeWithManage());
#ifdef SEAD_DEBUG
                    fillMemBlockDebugFillFree_(mergedBlock);
#endif // SEAD_DEBUG
                    mergedBlock = prevBlock;
                    merged = true;
                }
            }

            if (PtrUtil::addOffset(mergedBlock->memory(), mergedBlock->getSize()) == &currentBlock)
            {
                if (!merged)
                    mFreeList.insertBefore(&currentBlock, mergedBlock);

                mergedBlock->setSize(mergedBlock->getSize() + currentBlock.getSizeWithManage());
                mFreeList.erase(&currentBlock);
#ifdef SEAD_DEBUG
                fillMemBlockDebugFillFree_(&currentBlock);
#endif // SEAD_DEBUG
                merged = true;
            }

            if (!merged)
                mFreeList.insertBefore(&currentBlock, mergedBlock);

            blockInserted = true;
            break;
        }

        prevBlock = &currentBlock;
    }

    if (!blockInserted)
    {
        if (mFreeList.isEmpty())
        {
            mFreeList.pushBack(memBlock);
        }
        else
        {
            if (PtrUtil::addOffset(prevBlock->memory(), prevBlock->getSize()) == memBlock)
            {
                prevBlock->setSize(prevBlock->getSize() + memBlock->getSizeWithManage());
#ifdef SEAD_DEBUG
                fillMemBlockDebugFillFree_(memBlock);
#endif // SEAD_DEBUG
            }
            else
            {
                mFreeList.pushBack(memBlock);
            }
        }
    }
}

s32 ExpHeap::compareMemBlockAddr_(const MemBlock* a, const MemBlock* b)
{
    return Mathi::sign(static_cast<s32>(PtrUtil::diff(a, b)));
}

MemBlock* ExpHeap::allocFromHead_(size_t size)
{
    MemBlock* memBlock = findFreeMemBlockFromHead_(size, static_cast<FindMode>(mAllocMode));
    if (!memBlock)
        return nullptr;

    SEAD_ASSERT(memBlock->getOffset() == 0);
    SEAD_ASSERT(reinterpret_cast<uintptr_t>(memBlock->memory()) % cDefaultAlignment == 0);

    void* memory = memBlock->memory();
    size_t remainSize = memBlock->getSize() - size;

    memBlock->setSize(size);

    mFreeList.erase(memBlock);
    pushToUseList_(memBlock);

    if (remainSize < sizeof(MemBlock) + 1)
    {
        if (remainSize != 0)
            memBlock->setSize(size + remainSize);
    }
    else
    {
        MemBlock* newBlock = new(PtrUtil::addOffset(memory, size)) MemBlock();
        newBlock->setSize(remainSize - sizeof(MemBlock));
        pushToFreeList_(newBlock);
    }

    return memBlock;
}

MemBlock* ExpHeap::allocFromHead_(size_t size, s32 alignment)
{
    MemBlock* memBlock = findFreeMemBlockFromHead_(size, alignment, static_cast<FindMode>(mAllocMode));
    if (!memBlock)
        return nullptr;

    void* memory = memBlock->memory();
    size_t offset = PtrUtil::diff(PtrUtil::roundUpPow2(memory, alignment), memory);
    size_t remainSize = memBlock->getSize() - (size + offset);

    if (offset <= UINT16_MAX)
    {
        memBlock->setOffset(static_cast<u16>(offset));
        memBlock->setSize(size);
        mFreeList.erase(memBlock);
    }
    else
    {
        memBlock->setOffset(0);
        memBlock->setSize(offset - sizeof(MemBlock));

        memBlock = new(PtrUtil::addOffset(memBlock, memBlock->getSizeWithManage())) MemBlock();
        memBlock->setOffset(0);
        memBlock->setSize(size);
    }

    pushToUseList_(memBlock);

    if (remainSize < sizeof(MemBlock) + 1)
    {
        if (remainSize != 0)
            memBlock->setSize(size + remainSize);
    }
    else
    {
        MemBlock* newBlock = new(PtrUtil::addOffset(memBlock->memory(), size)) MemBlock();
        newBlock->setSize(remainSize - sizeof(MemBlock));
        pushToFreeList_(newBlock);
    }

    return memBlock;
}

MemBlock* ExpHeap::allocFromTail_(size_t size)
{
    MemBlock* memBlock = findFreeMemBlockFromTail_(size, static_cast<FindMode>(mAllocMode));
    if (!memBlock)
        return nullptr;

    SEAD_ASSERT(memBlock->getOffset() == 0);
    SEAD_ASSERT(reinterpret_cast<uintptr_t>(memBlock->memory()) % cDefaultAlignment == 0);

    size_t remainSize = memBlock->getSize() - size;
    if (remainSize < sizeof(MemBlock) + 1)
    {
        mFreeList.erase(memBlock);
        pushToUseList_(memBlock);
    }
    else
    {
        remainSize -= sizeof(MemBlock);
        memBlock->setSize(remainSize);

        memBlock = new(PtrUtil::addOffset(memBlock->memory(), remainSize)) MemBlock();
        memBlock->setSize(size);
        pushToUseList_(memBlock);
    }

    return memBlock;
}

MemBlock* ExpHeap::allocFromTail_(size_t size, s32 alignment)
{
    MemBlock* memBlock = findFreeMemBlockFromTail_(size, alignment, static_cast<FindMode>(mAllocMode));
    if (!memBlock)
        return nullptr;

    void* memory = PtrUtil::addOffset(memBlock->memory(), memBlock->getSize() - size);
    void* alignedAddr = PtrUtil::roundDownPow2(memory, alignment);

    size_t newBlockSize = size + PtrUtil::diff(memory, alignedAddr);

    size_t remainSize = memBlock->getSize() - newBlockSize;
    if (remainSize < sizeof(MemBlock) + 1)
    {
        size_t offset = memBlock->getSize() - newBlockSize;
        SEAD_ASSERT_MSG(offset <= UINT16_MAX, "Offset is too large.");
        memBlock->setOffset(static_cast<u16>(offset));
        memBlock->setSize(newBlockSize);

        mFreeList.erase(memBlock);
        pushToUseList_(memBlock);

        SEAD_ASSERT(memBlock->memory() == alignedAddr);

        return memBlock;
    }
    else
    {
        remainSize -= sizeof(MemBlock);
        memBlock->setSize(remainSize);

        MemBlock* newBlock = new(PtrUtil::addOffset(memBlock->memory(), remainSize)) MemBlock();
        newBlock->setSize(newBlockSize);

        SEAD_ASSERT(newBlock->memory() == alignedAddr);

        pushToUseList_(newBlock);

        return newBlock;
    }
}

size_t ExpHeap::adjustBack_()
{
    MemBlock* memBlock = findLastMemBlockIfFree_();
    if (!memBlock)
        return mSize;

    SEAD_ASSERT(memBlock >= mStart);

    size_t newSize = PtrUtil::diff(memBlock, mStart);

    mFreeList.erase(memBlock);

    void* addr = mParent->resizeBack(mStart, newSize);
    if (addr)
        mSize = newSize;
    else
        SEAD_ASSERT_MSG(false, "Resize failed.(Parent heap is not support resize.)");

    return mSize;
}

size_t ExpHeap::adjustFront_()
{
    MemBlock* memBlock = findFirstMemBlockIfFree_();
    if (!memBlock)
        return mSize;

    size_t newSize = mSize - memBlock->getSizeWithManage();

    mFreeList.erase(memBlock);

    void* addr = mParent->resizeFront(mStart, newSize);
    if (addr)
    {
        mSize = newSize;
        ThreadUtil::PlaceMemoryBarrier();
        mStart = addr;
    }
    else
    {
        SEAD_ASSERT_MSG(false, "Resize failed.(Parent heap is not support resize.)");
    }

    return mSize;
}

void* ExpHeap::realloc_(void* ptr, u8* oldMem, size_t copySize, size_t newSize, s32 alignment)
{
    void* ret = tryAlloc(newSize, alignment);
    if (!ret)
        return nullptr;

    MemUtil::copy(ret, oldMem, copySize);
    free(ptr);

    return ret;
}

#ifdef SEAD_DEBUG
void ExpHeap::fillMemBlockDebugFillFree_(void* addr)
{
    if (!isEnableDebugFillFree_())
        return;

    HeapMgr* heapMgr = HeapMgr::instance();
    if (heapMgr)
        MemUtil::fill(addr, heapMgr->getDebugFillFree(), sizeof(MemBlock));
    else
        MemUtil::fill(addr, HeapMgr::cDefaultDebugFillFree, sizeof(MemBlock));
}
#endif // SEAD_DEBUG

template <>
void PrintFormatter::out<ExpHeap>(const ExpHeap& obj, const char*, PrintOutput* output)
{
    ConditionalScopedLock<CriticalSection> lock(&obj.mCS, obj.isEnableLock());

    PrintFormatter::out<Heap>(obj, nullptr, output);

    FixedSafeString<128> buf;

    const char* allocMode;

    if (obj.getAllocMode() == ExpHeap::AllocMode::eFirstFit)
        allocMode = "First Fit";
    else
        allocMode = "Best Fit";

    buf.format("         AllocMode: %s\n", allocMode);
    PrintFormatter::out(SafeString(buf.cstr()), nullptr, output);

    buf.format("      UseList size: %d\n", obj.mUseList.size());
    PrintFormatter::out(SafeString(buf.cstr()), nullptr, output);

    buf.format("     FreeList size: %d\n", obj.mFreeList.size());
    PrintFormatter::out(SafeString(buf.cstr()), nullptr, output);

    PrintFormatter::out(SafeString("==================================================\n"), nullptr, output);
}

} // namespace sead
