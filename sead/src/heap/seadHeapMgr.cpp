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

void HeapMgr::initialize(Arena* arena)
{
    sArena = arena;
    initializeImpl_();
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
    FindContainHeapCache* findContainHeapCache = nullptr;

    ThreadMgr* threadMgr = ThreadMgr::instance();
    if (threadMgr)
    {
        Thread* currentThread = threadMgr->getCurrentThread();
        if (currentThread)
            findContainHeapCache = currentThread->getFindContainHeapCache();
    }

    if (findContainHeapCache)
    {
        FindContainHeapCacheAccessor heapAccessor(&findContainHeapCache->heap);

        Heap* heap = heapAccessor.getHeap();
        if (heap && heap->hasNoChild_() && heap->isInclude(memBlock))
        {
#ifdef SEAD_DEBUG
            findContainHeapCache->nolockhit++;
#endif // SEAD_DEBUG
            return heap;
        }
    }

    ScopedLock<CriticalSection> lock(&sHeapTreeLockCS);

    if (findContainHeapCache)
    {
#ifdef SEAD_DEBUG
        findContainHeapCache->call++;
#endif // SEAD_DEBUG

        Heap* heap = findContainHeapCache->getHeap();
        if (heap)
        {
            Heap* containHeap = heap->findContainHeap_(memBlock);
            if (containHeap)
            {
                if (containHeap == heap)
                {
#ifdef SEAD_DEBUG
                    findContainHeapCache->hit++;
#endif // SEAD_DEBUG
                }
                else
                {
#ifdef SEAD_DEBUG
                    findContainHeapCache->miss++;
#endif // SEAD_DEBUG
                    findContainHeapCache->setHeap(containHeap);
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
            if (findContainHeapCache)
            {
#ifdef SEAD_DEBUG
                findContainHeapCache->miss++;
#endif // SEAD_DEBUG
                findContainHeapCache->setHeap(containHeap);
            }

            return containHeap;
        }
    }

    for (Heap& heap : sIndependentHeaps)
    {
        Heap* containHeap = heap.findContainHeap_(memBlock);
        if (containHeap)
        {
            if (findContainHeapCache)
            {
#ifdef SEAD_DEBUG
                findContainHeapCache->miss++;
#endif // SEAD_DEBUG
                findContainHeapCache->setHeap(containHeap);
            }

            return containHeap;
        }
    }

#ifdef SEAD_DEBUG
    if (findContainHeapCache)
        findContainHeapCache->notfound++;
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

// TODO: Refactor
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
            FindContainHeapCache* findContainHeapCache = mainThread->getFindContainHeapCache();
            bool success = findContainHeapCache->tryRemoveHeap(heap);
            if (success)
                break;

#ifdef SEAD_DEBUG
            findContainHeapCache->sleep++;
#endif // SEAD_DEBUG

            Thread::sleep(TickSpan::makeFromMicroSeconds(10));
        }
    }

    bool failed;

    do
    {
        failed = false;

        {
            ScopedLock<CriticalSection> lock(threadMgr->getIterateLockCS());

            for (ThreadList::constIterator it = threadMgr->constBegin(); it != threadMgr->constEnd(); ++it)
            {
                FindContainHeapCache* findContainHeapCache = (*it)->getFindContainHeapCache();
                if (!findContainHeapCache->tryRemoveHeap(heap))
                {
#ifdef SEAD_DEBUG
                    findContainHeapCache->sleep++;
#endif // SEAD_DEBUG
                    failed = true;
                    break;
                }
            }

            if (!failed)
                return;
        }

        Thread::sleep(TickSpan::makeFromMicroSeconds(10));
    } while (true);
}

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
