#include <heap/seadFrameHeap.h>

#include <basis/seadWarning.h>
#include <heap/seadHeapMgr.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadScopedLock.h>

namespace sead {

FrameHeap* FrameHeap::tryCreate(size_t size_, const SafeString& name, Heap* parent, HeapDirection direction, bool enableLock)
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

    if (size < sizeof(FrameHeap))
    {
        SEAD_ASSERT_MSG(size_ == 0, "size must be able to include manage area: size=%zu", size);
        return nullptr;
    }

    void* heapStart;

    {
#ifdef SEAD_DEBUG
        ScopedDebugFillSystemDisabler disabler(parent);
#endif // SEAD_DEBUG
        heapStart = parent->tryAlloc(size, static_cast<s32>(direction) * cMinAlignment);
    }

    if (!heapStart)
        return nullptr;

    if (parent->getDirection() == HeapDirection::eReverse)
        direction = static_cast<HeapDirection>(-static_cast<s32>(direction));

    FrameHeap* result;
    if (direction == HeapDirection::eForward)
    {
        result = new(heapStart) FrameHeap(name, parent, heapStart, size, direction, enableLock);
    }
    else
    {
        void* ptr = PtrUtil::addOffset(heapStart, size - sizeof(FrameHeap));
        result = new(ptr) FrameHeap(name, parent, heapStart, size, direction, enableLock);
    }

    result->initialize_();

    parent->pushBackChild_(result);

#ifdef SEAD_DEBUG
    {
        HeapMgr* mgr = HeapMgr::instance();
        if (mgr)
            mgr->callCreateCallback_(result);
    }
#endif // SEAD_DEBUG

    return result;
}

size_t FrameHeap::getManagementAreaSize(s32 alignment)
{
    return alignment + sizeof(FrameHeap);
}

FrameHeap::FrameHeap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock)
    : Heap(name, parent, start, size, direction, enableLock)
    , mState()
{
}

FrameHeap::~FrameHeap()
{
    destruct_();
}

void FrameHeap::destroy()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();

    HeapMgr* mgr = HeapMgr::instance();
    if (mgr)
        mgr->callDestroyCallback_(this);
#endif // SEAD_DEBUG

    Heap* parent = mParent;
    void* start = mStart;

#ifdef SEAD_DEBUG
    size_t size = mSize;

    void* headPtr = mState.mHeadPtr;
    void* tailPtr = mState.mTailPtr;

    bool isEnableFill = isEnableDebugFillHeapDestroy_();
#endif // SEAD_DEBUG

    this->~FrameHeap();

#ifdef SEAD_DEBUG
    if (isEnableFill)
    {
        u8 fillValue = HeapMgr::cDefaultDebugFillHeapDestroy;
        if (HeapMgr::instance())
            fillValue = HeapMgr::instance()->getDebugFillHeapDestroy();

        MemUtil::fill(start, fillValue, PtrUtil::diff(headPtr, start));

        size_t tailAreaSize = size - PtrUtil::diff(tailPtr, start);
        if (tailAreaSize != 0)
            MemUtil::fill(tailPtr, fillValue, tailAreaSize);
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

size_t FrameHeap::adjust()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    if (!mParent)
        return mSize;

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());
    ScopedLock<Heap> parentLock(mParent);

    size_t ret;
    if (mDirection == HeapDirection::eForward)
    {
        if (getAreaEnd_() == mState.mTailPtr)
            ret = adjustBack_();
        else
            ret = mSize;
    }
    else
    {
        if (getAreaStart_() == mState.mHeadPtr)
            ret = adjustFront_();
        else
            ret = mSize;
    }

    return ret;
}

void* FrameHeap::tryAlloc(size_t size, s32 alignment)
{
    // TODO: Update this func

#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    HeapMgr* mgr = HeapMgr::instance();

    SEAD_ASSERT_MSG(alignment != 0, "alignment must not be zero");

#ifdef SEAD_DEBUG
    HeapMgr::AllocCallbackArg allocCallbackArg;

    HeapMgr::IAllocCallback* allocCallback = nullptr;
    if (mgr)
        allocCallback = mgr->getAllocCallback();

    if (allocCallback)
    {
        allocCallbackArg.heap = this;
        allocCallbackArg.request_size = size;
        allocCallbackArg.request_alignment = alignment;
    }
#endif // SEAD_DEBUG

    HeapMgr::AllocFailedCallbackArg allocFailedCallbackArg;

    HeapMgr::IAllocFailedCallback* allocFailedCallback = nullptr;
    if (mgr)
        allocFailedCallback = mgr->getAllocFailedCallback();

    if (allocFailedCallback)
    {
        allocFailedCallbackArg.heap = this;
        allocFailedCallbackArg.request_size = size;
        allocFailedCallbackArg.request_alignment = alignment;
    }

    if (size < cMinAllocSize)
        size = cMinAllocSize;

    size = MathSizeT::roundUpPow2(size, cMinAlignment);

    if (!Mathi::isPow2(Mathi::abs(alignment)))
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

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    alignment *= static_cast<s32>(mDirection);

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

    void* alloced;
    if (alignment > -1)
    {
        void* alignedAddr = PtrUtil::roundUpPow2(mState.mHeadPtr, alignment);
        void* nextAddr = PtrUtil::addOffset(alignedAddr, size);
        if (alignedAddr > nextAddr)
        {
            if (allocFailedCallback)
                allocFailedCallback->invoke(&allocFailedCallbackArg);

            SEAD_ASSERT_MSG(false, "size overflow");
            return nullptr;
        }

        if (nextAddr > mState.mTailPtr)
        {
            if (allocFailedCallback)
                allocFailedCallback->invoke(&allocFailedCallbackArg);

            return nullptr;
        }

        mState.mHeadPtr = nextAddr;

        alloced = alignedAddr;
    }
    else
    {
        void* addr = PtrUtil::addOffset(mState.mTailPtr, -static_cast<intptr_t>(size));
        if (addr > mState.mTailPtr)
        {
            if (allocFailedCallback)
                allocFailedCallback->invoke(&allocFailedCallbackArg);

            SEAD_ASSERT_MSG(false, "size overflow");
            return nullptr;
        }

        void* alignedAddr = PtrUtil::roundDownPow2(addr, -alignment);
        if (alignedAddr < mState.mHeadPtr)
        {
            if (allocFailedCallback)
                allocFailedCallback->invoke(&allocFailedCallbackArg);

            return nullptr;
        }

        mState.mTailPtr = alignedAddr;

        alloced = alignedAddr;
    }

#ifdef SEAD_DEBUG
    if (isEnableDebugFillAlloc_())
    {
        if (mgr)
            MemUtil::fill(alloced, mgr->getDebugFillAlloc(), size);
        else
            MemUtil::fill(alloced, HeapMgr::cDefaultDebugFillAlloc, size);
    }

    if (allocCallback)
    {
        allocCallbackArg.ptr = alloced;

        allocCallback->invoke(&allocCallbackArg);
    }
#endif // SEAD_DEBUG

    return alloced;
}

void FrameHeap::free(void* ptr)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();

    if (!ptr)
        return;

    HeapMgr* mgr = HeapMgr::instance();
    if (mgr)
    {
        HeapMgr::FreeCallbackArg freeCallbackArg;
        freeCallbackArg.heap = this;
        freeCallbackArg.ptr = ptr;

        mgr->callFreeCallback_(freeCallbackArg);
    }

    if (!mFlag.isOnBit(Flag::eDisposing) && isEnableWarning())
        SEAD_WARNING("Cannot free from FrameHeap [%s] 0x%p\n", getName().cstr(), ptr);
#endif // SEAD_DEBUG
}

void* FrameHeap::resizeFront(void* ptr, size_t)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();

    if (isEnableWarning())
        SEAD_WARNING("Cannot resizeFront from FrameHeap [%s] 0x%p\n", getName().cstr(), ptr);
#endif // SEAD_DEBUG

    return nullptr;
}

void* FrameHeap::resizeBack(void* ptr, size_t)
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();

    if (isEnableWarning())
        SEAD_WARNING("Cannot resizeBack from FrameHeap [%s] 0x%p\n", getName().cstr(), ptr);
#endif // SEAD_DEBUG

    return nullptr;
}

void FrameHeap::freeAll()
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    dispose_(nullptr, nullptr);

#ifdef SEAD_DEBUG
    if (isEnableDebugFillFree_())
    {
        HeapMgr* mgr = HeapMgr::instance();
        if (mgr)
            MemUtil::fill(getAreaStart_(), mgr->getDebugFillFree(), getAreaSize_());
        else
            MemUtil::fill(getAreaStart_(), HeapMgr::cDefaultDebugFillFree, getAreaSize_());
    }
#endif // SEAD_DEBUG

    mState.mHeadPtr = getAreaStart_();
    mState.mTailPtr = getAreaEnd_();
}

const void* FrameHeap::getStartAddress() const
{
    return mStart;
}

const void* FrameHeap::getEndAddress() const
{
    return PtrUtil::addOffset(mStart, mSize);
}

size_t FrameHeap::getSize() const
{
    return mSize;
}

size_t FrameHeap::getFreeSize() const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    SEAD_ASSERT(mState.mTailPtr >= mState.mHeadPtr);

    return PtrUtil::diff(mState.mTailPtr, mState.mHeadPtr);
}

size_t FrameHeap::getMaxAllocatableSize(s32 alignment) const
{
#ifdef SEAD_DEBUG
    if (mAccessThread)
        checkAccessThread_();
#endif // SEAD_DEBUG

    s32 absAlignment = Mathi::abs(alignment);
    if (!Mathi::isPow2(absAlignment))
    {
        SEAD_ASSERT_MSG(false, "bad alignment %d", alignment);
        return 0;
    }

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    void* alignedAddr = PtrUtil::roundUpPow2(mState.mHeadPtr, absAlignment);

    if (alignedAddr > mState.mTailPtr)
        return 0;

    return PtrUtil::diff(mState.mTailPtr, alignedAddr);
}

bool FrameHeap::isInclude(const void* ptr) const
{
    return getAreaStart_() <= ptr && ptr < getAreaEnd_();
}

bool FrameHeap::isEmpty() const
{
    return mState.mHeadPtr == getAreaStart_() && mState.mTailPtr == getAreaEnd_();
}

bool FrameHeap::isFreeable() const
{
    return false;
}

bool FrameHeap::isResizable() const
{
    return false;
}

bool FrameHeap::isAdjustable() const
{
    return true;
}

void FrameHeap::dump() const
{
    // TODO
}

void FrameHeap::dumpYAML(WriteStream& stream, s32 indent) const
{
    // TODO
}

void FrameHeap::initialize_()
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    mState.mHeadPtr = getAreaStart_();
    mState.mTailPtr = getAreaEnd_();

#ifdef SEAD_DEBUG
    HeapMgr* mgr = HeapMgr::instance();
    if (mgr && mgr->isEnableDebugFillHeapCreate())
        MemUtil::fill(getAreaStart_(), mgr->getDebugFillHeapCreate(), getAreaSize_());
#endif // SEAD_DEBUG
}

void* FrameHeap::getAreaStart_() const
{
    if (mDirection == HeapDirection::eForward)
        return PtrUtil::addOffset(mStart, sizeof(FrameHeap));
    else
        return mStart;
}

void* FrameHeap::getAreaEnd_() const
{
    if (mDirection == HeapDirection::eForward)
        return PtrUtil::addOffset(mStart, mSize);
    else
        return PtrUtil::addOffset(mStart, mSize - sizeof(FrameHeap));
}

size_t FrameHeap::adjustBack_()
{
    SEAD_ASSERT_MSG(mDirection == HeapDirection::eForward, "Reverse Heap. Cannot adjustBack_. [%s]", getName().cstr());

    size_t newSize = PtrUtil::diff(mState.mHeadPtr, getStartAddress());

    void* resizedMemory = mParent->resizeBack(mStart, newSize);
    if (!resizedMemory)
    {
        SEAD_ASSERT_MSG(false, "Resize failed.(Parent heap is not support resize.)");
        return mSize;
    }

    mState.mTailPtr = mState.mHeadPtr;
    mSize = newSize;

    return newSize;
}

size_t FrameHeap::adjustFront_()
{
    SEAD_ASSERT_MSG(mDirection == HeapDirection::eReverse, "Forward Heap. Cannot adjustFront_. [%s]", getName().cstr());

    size_t newSize = PtrUtil::diff(getEndAddress(), mState.mTailPtr);

    void* resizedMemory = mParent->resizeFront(mStart, newSize);
    if (!resizedMemory)
    {
        SEAD_ASSERT_MSG(false, "Resize failed.(Parent heap is not support resize.)");
        return mSize;
    }

    mState.mHeadPtr = mState.mTailPtr;
    mStart = mState.mHeadPtr;
    mSize = newSize;

    SEAD_ASSERT(PtrUtil::addOffset(mStart, mSize - sizeof(FrameHeap)) == this);

    return newSize;
}

} // namespace sead
