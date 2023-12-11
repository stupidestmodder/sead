#include <heap/seadHeapMgr.h>

#include <heap/seadExpHeap.h>
#include <heap/seadHeap.h>
#include <prim/seadScopedLock.h>
#include <thread/seadThread.h>

namespace sead {

HeapMgr* HeapMgr::sInstancePtr = nullptr;
HeapMgr HeapMgr::sInstance;

Arena* HeapMgr::sArena = nullptr;
Arena HeapMgr::sDefaultArena;

AtomicU32 HeapMgr::sHeapCheckTag;
CriticalSection HeapMgr::sHeapTreeLockCS;

HeapMgr::RootHeaps HeapMgr::sRootHeaps;
HeapMgr::IndependentHeaps HeapMgr::sIndependentHeaps;

HeapMgr::HeapMgr()
#ifdef SEAD_DEBUG
    : mDebugFillHeapCreate(cDefaultDebugFillHeapCreate)
    , mDebugFillAlloc(cDefaultDebugFillAlloc)
    , mDebugFillFree(cDefaultDebugFillFree)
    , mDebugFillHeapDestroy(cDefaultDebugFillHeapDestroy)
    , mIsEnableDebugFillHeapCreate(false)
    , mIsEnableDebugFillAlloc(true)
    , mIsEnableDebugFillFree(true)
    , mIsEnableDebugFillHeapDestroy(true)
    , mAllocCallback(nullptr)
    , mAllocFailedCallback(nullptr)
    , mFreeCallback(nullptr)
    , mCreateCallback(nullptr)
    , mDestroyCallback(nullptr)
#else
    : mAllocFailedCallback(nullptr)
#endif // SEAD_DEBUG
{
}

HeapMgr::~HeapMgr()
{
}

void HeapMgr::initialize(size_t size)
{
    ScopedLock<CriticalSection> lock(getHeapTreeLockCS_());

    sArena = &sDefaultArena;
    sArena->initialize(size);

    HeapMgr::initializeImpl_();
}

void HeapMgr::initialize(Arena* arena)
{
    sArena = arena;
    HeapMgr::initializeImpl_();
}

void HeapMgr::destroy()
{
    ScopedLock<CriticalSection> lock(getHeapTreeLockCS_());

#ifdef SEAD_DEBUG
    sInstance.mAllocCallback = nullptr;
    sInstance.mAllocFailedCallback = nullptr;
    sInstance.mFreeCallback = nullptr;
    sInstance.mCreateCallback = nullptr;
    sInstance.mDestroyCallback = nullptr;
#else
    sInstance.mAllocFailedCallback = nullptr;
#endif // SEAD_DEBUG

    while (sIndependentHeaps.size() != 0)
    {
        Heap* heap = sIndependentHeaps.back();
        heap->destroy();
        sIndependentHeaps.popBack();
    }

    while (sRootHeaps.size() != 0)
    {
        Heap* heap = sRootHeaps.back();
        heap->destroy();
        sRootHeaps.popBack();
    }

    sInstancePtr = nullptr;

    sArena->destroy();
    sArena = nullptr;
}

Heap* HeapMgr::findContainHeap(const void* memBlock) const
{
    FindContainHeapCache* heapCache = nullptr;

    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (threadMgr)
    {
        Thread* currentThread = threadMgr->getCurrentThread();
        if (currentThread)
            heapCache = currentThread->getFindContainHeapCache();
    }

    if (heapCache)
    {
        FindContainHeapCacheAccessor heapAccessor(&heapCache->heap);

        Heap* heap = heapAccessor.getHeap();
        if (heap && heap->hasNoChild_() && heap->isInclude(memBlock))
        {
#ifdef SEAD_DEBUG
            heapCache->nolockhit++;
#endif // SEAD_DEBUG
            return heap;
        }
    }

    ScopedLock<CriticalSection> lock(&sHeapTreeLockCS);

    if (heapCache)
    {
#ifdef SEAD_DEBUG
        heapCache->call++;
#endif // SEAD_DEBUG

        Heap* heap = heapCache->getHeap();
        if (heap)
        {
            Heap* containHeap = heap->findContainHeap_(memBlock);
            if (containHeap)
            {
                if (containHeap == heap)
                {
#ifdef SEAD_DEBUG
                    heapCache->hit++;
#endif // SEAD_DEBUG
                }
                else
                {
#ifdef SEAD_DEBUG
                    heapCache->miss++;
#endif // SEAD_DEBUG
                    heapCache->setHeap(containHeap);
                }

                return containHeap;
            }
        }
    }

    for (Heap& heap : sRootHeaps)
    {
        Heap* containHeap = heap.findContainHeap_(memBlock);
        if (containHeap)
        {
            if (heapCache)
            {
#ifdef SEAD_DEBUG
                heapCache->miss++;
#endif // SEAD_DEBUG
                heapCache->setHeap(containHeap);
            }

            return containHeap;
        }
    }

    for (Heap& heap : sIndependentHeaps)
    {
        Heap* containHeap = heap.findContainHeap_(memBlock);
        if (containHeap)
        {
            if (heapCache)
            {
#ifdef SEAD_DEBUG
                heapCache->miss++;
#endif // SEAD_DEBUG
                heapCache->setHeap(containHeap);
            }

            return containHeap;
        }
    }

#ifdef SEAD_DEBUG
    if (heapCache)
        heapCache->notfound++;
#endif // SEAD_DEBUG

    return nullptr;
}

Heap* HeapMgr::findHeapByName(const SafeString& name, s32 index) const
{
    ScopedLock<CriticalSection> lock(&sHeapTreeLockCS);

    for (Heap& heap : sRootHeaps)
    {
        Heap* foundHeap = HeapMgr::findHeapByName_(&heap, name, &index);
        if (foundHeap)
            return foundHeap;
    }

    for (Heap& heap : sIndependentHeaps)
    {
        Heap* foundHeap = HeapMgr::findHeapByName_(&heap, name, &index);
        if (foundHeap)
            return foundHeap;
    }

    return nullptr;
}

Heap* HeapMgr::getCurrentHeap() const
{
    const ThreadMgr* threadMgr = ThreadMgr::instance();
    SEAD_ASSERT(threadMgr);

    Thread* currentThread = threadMgr->getCurrentThread();
    return currentThread->getCurrentHeap();
}

void HeapMgr::removeRootHeap(Heap* heap)
{
    s32 idx = sRootHeaps.indexOf(heap);
    if (idx == -1)
        return;

    sRootHeaps.erase(idx);
}

bool HeapMgr::isContainedInAnyHeap(const void* addr)
{
    // TODO: Sead does not lock sHeapTreeLockCS here, should we ?

    for (Heap& heap : sRootHeaps)
    {
        if (heap.isInclude(addr))
            return true;
    }

    for (Heap& heap : sIndependentHeaps)
    {
        if (heap.isInclude(addr))
            return true;
    }

    return false;
}

void HeapMgr::dumpTreeYAML(WriteStream& stream)
{
    ScopedLock<CriticalSection> lock(&sHeapTreeLockCS);

    for (Heap& heap : sRootHeaps)
    {
        heap.dumpTreeYAML(stream, 0);
    }

    for (Heap& heap : sIndependentHeaps)
    {
        heap.dumpTreeYAML(stream, 0);
    }
}

#ifdef SEAD_DEBUG
HeapMgr::IAllocCallback* HeapMgr::setAllocCallback(IAllocCallback* callback)
{
    IAllocCallback* prev = mAllocCallback;
    mAllocCallback = callback;
    return prev;
}
#endif // SEAD_DEBUG

HeapMgr::IAllocFailedCallback* HeapMgr::setAllocFailedCallback(IAllocFailedCallback* callback)
{
    IAllocFailedCallback* prev = mAllocFailedCallback;
    mAllocFailedCallback = callback;
    return prev;
}

#ifdef SEAD_DEBUG
HeapMgr::IFreeCallback* HeapMgr::setFreeCallback(IFreeCallback* callback)
{
    IFreeCallback* prev = mFreeCallback;
    mFreeCallback = callback;
    return prev;
}

HeapMgr::ICreateCallback* HeapMgr::setCreateCallback(ICreateCallback* callback)
{
    ICreateCallback* prev = mCreateCallback;
    mCreateCallback = callback;
    return prev;
}

HeapMgr::IDestroyCallback* HeapMgr::setDestroyCallback(IDestroyCallback* callback)
{
    IDestroyCallback* prev = mDestroyCallback;
    mDestroyCallback = callback;
    return prev;
}
#endif // SEAD_DEBUG

void HeapMgr::removeFromFindContainHeapCache_(Heap* heap)
{
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* mainThread = threadMgr->getMainThread();
    if (mainThread)
    {
        while (true)
        {
            FindContainHeapCache* heapCache = mainThread->getFindContainHeapCache();
            bool success = heapCache->tryRemoveHeap(heap);
            if (success)
                break;

#ifdef SEAD_DEBUG
            heapCache->sleep++;
#endif // SEAD_DEBUG

            Thread::sleep(TickSpan::makeFromMicroSeconds(10));
        }
    }

    while (true)
    {
        bool operationFailed = false;

        {
            ScopedLock<CriticalSection> lock(threadMgr->getIterateLockCS());

            for (ThreadList::constIterator it = threadMgr->constBegin(); it != threadMgr->constEnd(); ++it)
            {
                FindContainHeapCache* heapCache = (*it)->getFindContainHeapCache();

                bool success = heapCache->tryRemoveHeap(heap);
                if (!success)
                {
#ifdef SEAD_DEBUG
                    heapCache->sleep++;
#endif // SEAD_DEBUG

                    operationFailed = true;
                    break;
                }
            }

            if (!operationFailed)
                return;
        }

        Thread::sleep(TickSpan::makeFromMicroSeconds(10));
    }
}

#ifdef SEAD_DEBUG
void HeapMgr::dumpFindContainHeapCacheStatistics()
{
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    SEAD_PRINT("FindContainHeapCache Statistics: \n");

    Thread* mainThread = threadMgr->getMainThread();
    if (mainThread)
    {
        SEAD_PRINT("  [%16s] ", mainThread->getName().cstr());

        FindContainHeapCache* cache = mainThread->getFindContainHeapCache();
        cache->dumpStatistics();
    }

    ScopedLock<CriticalSection> lock(threadMgr->getIterateLockCS());

    for (auto it = threadMgr->constBegin(); it != threadMgr->constEnd(); ++it)
    {
        SEAD_PRINT("  [%16s] ", (*it)->getName().cstr());

        FindContainHeapCache* cache = (*it)->getFindContainHeapCache();
        cache->dumpStatistics();
    }
}

void HeapMgr::clearFindContainHeapCacheStatistics()
{
    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (!threadMgr)
        return;

    Thread* mainThread = threadMgr->getMainThread();
    if (mainThread)
    {
        FindContainHeapCache* cache = mainThread->getFindContainHeapCache();
        cache->clearStatistics();
    }

    ScopedLock<CriticalSection> lock(threadMgr->getIterateLockCS());

    for (auto it = threadMgr->constBegin(); it != threadMgr->constEnd(); ++it)
    {
        FindContainHeapCache* cache = (*it)->getFindContainHeapCache();
        cache->clearStatistics();
    }
}
#endif // SEAD_DEBUG

Heap* HeapMgr::setCurrentHeap_(Heap* heap)
{
    ThreadMgr* threadMgr = ThreadMgr::instance();
    SEAD_ASSERT(threadMgr);

    Thread* currentThread = threadMgr->getCurrentThread();
    Heap* prevHeap = currentThread->getCurrentHeap();

    currentThread->setCurrentHeap_(heap);

    return prevHeap;
}

void HeapMgr::createRootHeap_()
{
    SEAD_ASSERT(sRootHeaps.size() == 0);

    Heap* heap = ExpHeap::create(sArena->getStartAddr(), sArena->getSize(), "RootHeap", false);
    SEAD_ASSERT(heap);

    sRootHeaps.pushBack(heap);
}

void HeapMgr::initializeImpl_()
{
#ifdef SEAD_DEBUG
    sInstance.mAllocCallback = nullptr;
    sInstance.mAllocFailedCallback = nullptr;
    sInstance.mFreeCallback = nullptr;
    sInstance.mCreateCallback = nullptr;
    sInstance.mDestroyCallback = nullptr;
#else
    sInstance.mAllocFailedCallback = nullptr;
#endif // SEAD_DEBUG

    HeapMgr::createRootHeap_();

    sInstancePtr = &sInstance;
}

Heap* HeapMgr::findHeapByName_(Heap* heap, const SafeString& name, s32* index)
{
    if (heap->getName() == name)
    {
        if (*index == 0)
            return heap;

        *index -= 1;
    }

    for (Heap& child : heap->mChildren)
    {
        Heap* foundHeap = HeapMgr::findHeapByName_(&child, name, index);
        if (foundHeap)
            return foundHeap;
    }

    return nullptr;
}

} // namespace sead
