#include <heap/seadHeap.h>

#include <heap/seadHeapMgr.h>
#include <prim/seadScopedLock.h>
#include <thread/seadThread.h>

namespace sead {

Heap::Heap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock)
    : IDisposer(parent, HeapNullOption::eFindContainHeap)
    , INamable(name)
    , mStart(start)
    , mSize(size)
    , mParent(parent)
    , mChildren()
    , mListNode()
    , mDisposerList()
    , mDirection(direction)
    , mCS(parent)
#ifdef SEAD_DEBUG
    , mFlag(1 << Flag::eEnableWarning | 1 << Flag::cEnableDebugFillSystem | 1 << Flag::cEnableDebugFillUser)
#else
    , mFlag(1 << Flag::eEnableWarning)
#endif // SEAD_DEBUG
    , mHeapCheckTag(static_cast<u16>(HeapMgr::getHeapCheckTag()))
#ifdef SEAD_DEBUG
    , mAccessThread(nullptr)
#endif // SEAD_DEBUG
{
    mFlag.changeBit(Flag::eEnableLock, enableLock);

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    mChildren.initOffset(offsetof(Heap, mListNode));
    mDisposerList.initOffset(IDisposer::getListNodeOffset());
}

Heap::~Heap()
{
}

Heap* Heap::findContainHeap_(const void* ptr)
{
    if (!isInclude(ptr))
        return nullptr;

    for (Heap& heap : mChildren)
    {
        if (heap.isInclude(ptr))
            return heap.findContainHeap_(ptr);
    }

    return this;
}

void Heap::destruct_()
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    dispose_(nullptr, nullptr);

    HeapMgr::removeFromFindContainHeapCache_(this);

    if (mParent)
        mParent->eraseChild_(this);
}

void Heap::dispose_(const void* begin, const void* end)
{
    mFlag.setBit(Flag::eDisposing);

    for (auto it = mDisposerList.begin(); it != mDisposerList.end(); )
    {
        if (!it->mDisposerHeap || (begin || end) && !PtrUtil::isInclude(&*it, begin, end))
        {
            ++it;
            continue;
        }

        it->~IDisposer();

        it = mDisposerList.begin();
    }

    mFlag.resetBit(Flag::eDisposing);
}

void Heap::appendDisposer_(IDisposer* disposer)
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());
    mDisposerList.pushBack(disposer);
}

void Heap::removeDisposer_(IDisposer* disposer)
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());
    mDisposerList.erase(disposer);
}

void Heap::pushBackChild_(Heap* child)
{
    ScopedLock<CriticalSection> treeLock(HeapMgr::getHeapTreeLockCS_());
    ConditionalScopedLock<CriticalSection> heapLock(&mCS, isEnableLock());

    mChildren.pushBack(child);
}

void Heap::eraseChild_(Heap* child)
{
    ScopedLock<CriticalSection> treeLock(HeapMgr::getHeapTreeLockCS_());
    ConditionalScopedLock<CriticalSection> heapLock(&mCS, isEnableLock());

    mChildren.erase(child);
}

void Heap::checkAccessThread_() const
{
#ifdef SEAD_DEBUG
    if (!mAccessThread)
        return;

    if (isEnableLock())
        return;

    Thread* currentThread = ThreadMgr::instance()->getCurrentThread();
    if (mAccessThread != currentThread)
    {
        SEAD_ASSERT_MSG(false, "Current thread is %s(0x%p). This heap can access from %s(0x%p) only.",
                        currentThread->getName().cstr(), currentThread, mAccessThread->getName().cstr(), mAccessThread);
    }
#endif // SEAD_DEBUG
}

} // namespace sead
