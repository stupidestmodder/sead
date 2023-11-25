#include <heap/seadHeapMgr.h>

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

// TODO
HeapMgr::HeapMgr()
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

// TODO
void HeapMgr::destroy()
{
}

// TODO
/*
Heap* HeapMgr::findContainHeap(const void* ptr) const
{
    ScopedLock<CriticalSection> lock(&sHeapTreeLockCS);

    for (RootHeaps::iterator it = sRootHeaps.begin(); it != sRootHeaps.end(); ++it)
    {
        Heap* heap = it->findContainHeap_(ptr);
        if (heap != nullptr)
        {
            return heap;
        }
    }

    for (IndependentHeaps::iterator it = sIndependentHeaps.begin(); it != sIndependentHeaps.end(); ++it)
    {
        Heap* heap = it->findContainHeap_(ptr);
        if (heap != nullptr)
        {
            return heap;
        }
    }

    return nullptr;
}
*/

Heap* HeapMgr::getCurrentHeap() const
{
    const ThreadMgr* threadMgr = ThreadMgr::instance();
    SEAD_ASSERT(threadMgr);

    return threadMgr->getCurrentThread()->getCurrentHeap();
}
/*
Heap* HeapMgr::setCurrentHeap_(Heap* heap)
{
    const ThreadMgr* threadMgr = ThreadMgr::instance();
    SEAD_ASSERT(threadMgr);

    Thread* currentThread = threadMgr->getCurrentThread();
    Heap* prevHeap = currentThread->getCurrentHeap();
    currentThread->setCurrentHeap_(heap);

    return prevHeap;
}
*/
} // namespace sead
