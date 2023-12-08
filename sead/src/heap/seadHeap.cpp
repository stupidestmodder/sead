#include <heap/seadHeap.h>

#include <heap/seadHeapMgr.h>
#include <prim/seadFormatPrint.h>
#include <prim/seadScopedLock.h>
#include <stream/seadStream.h>
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
    , mFlag((1 << Flag::eEnableWarning) | (1 << Flag::eEnableDebugFillSystem) | (1 << Flag::eEnableDebugFillUser))
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

void Heap::dumpTreeYAML(WriteStream& stream, s32 indent) const
{
    dumpYAML(stream, indent);

    FixedSafeString<128> str("");
    str.append(' ', indent);
    str.appendWithFormat("  children:\n");
    stream.writeDecorationText(str);

    for (Heap& child : mChildren)
    {
        child.dumpTreeYAML(stream, indent + 4);
    }
}

void Heap::dumpYAML(WriteStream& stream, s32 indent) const
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    FixedSafeString<128> str("");
    str.append(' ', indent);
    str.appendWithFormat("- name: \"%s\"\n", getName().cstr());
    stream.writeDecorationText(str);

    str.clear();
    str.append(' ', indent);
    str.appendWithFormat("  start_address: 0x%p\n", getStartAddress());
    stream.writeDecorationText(str);

    str.clear();
    str.append(' ', indent);
    str.appendWithFormat("  end_address: 0x%p\n", getEndAddress());
    stream.writeDecorationText(str);

    str.clear();
    str.append(' ', indent);

    const char* parent;

    if (getParent())
        parent = getParent()->getName().cstr();
    else
        parent = "--";

    str.appendWithFormat("  parent: %s\n", parent);
    stream.writeDecorationText(str);

    str.clear();
    str.append(' ', indent);

    const char* direction;

    if (getDirection() == HeapDirection::eForward)
        direction = "Forward";
    else
        direction = "Reverse";

    str.appendWithFormat("  direction: %s\n", direction);
    stream.writeDecorationText(str);

    str.clear();
    str.append(' ', indent);
    str.appendWithFormat("  size: %zu\n", getSize());
    stream.writeDecorationText(str);

    str.clear();
    str.append(' ', indent);
    str.appendWithFormat("  free_size: %zu\n", getFreeSize());
    stream.writeDecorationText(str);

    str.clear();
    str.append(' ', indent);
    str.appendWithFormat("  max_allocatable_size: %zu\n", getMaxAllocatableSize());
    stream.writeDecorationText(str);
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
    else
        HeapMgr::removeRootHeap(this);
}

void Heap::dispose_(const void* begin, const void* end)
{
    mFlag.setBit(Flag::eDisposing);

    for (auto it = mDisposerList.begin(); it != mDisposerList.end(); )
    {
        if (!it->mDisposerHeap || (begin || end) && !PtrUtil::isInclude(&(*it), begin, end))
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
    if (currentThread != mAccessThread)
    {
        SEAD_ASSERT_MSG(false, "Current thread is %s(0x%p). This heap can access from %s(0x%p) only.",
                        currentThread->getName().cstr(), currentThread, mAccessThread->getName().cstr(), mAccessThread);
    }
#endif // SEAD_DEBUG
}

#ifdef SEAD_DEBUG
bool Heap::isEnableDebugFillAlloc_() const
{
    return HeapMgr::instance()->isEnableDebugFillAlloc() && mFlag.isOnAll((1 << Flag::eEnableDebugFillSystem) | (1 << Flag::eEnableDebugFillUser));
}

bool Heap::isEnableDebugFillFree_() const
{
    return HeapMgr::instance()->isEnableDebugFillFree() && mFlag.isOnAll((1 << Flag::eEnableDebugFillSystem) | (1 << Flag::eEnableDebugFillUser));
}

bool Heap::isEnableDebugFillHeapDestroy_() const
{
    return HeapMgr::instance()->isEnableDebugFillHeapDestroy() && mFlag.isOn(1 << Flag::eEnableDebugFillUser);
}
#endif // SEAD_DEBUG

template <>
void PrintFormatter::out<Heap>(const Heap& obj, const char*, PrintOutput* output)
{
    ConditionalScopedLock<CriticalSection> lock(&obj.mCS, obj.isEnableLock());

    FixedSafeString<128> str;

    PrintFormatter::out(SafeString("\n"), nullptr, output);
    PrintFormatter::out(SafeString("==================================================\n"), nullptr, output);

    str.format("              Name: %s\n", obj.getName().cstr());
    PrintFormatter::out(SafeString(str.cstr()), nullptr, output);

    str.format("             Range: [0x%p - 0x%p)\n", obj.getStartAddress(), obj.getEndAddress());
    PrintFormatter::out(SafeString(str.cstr()), nullptr, output);

    const char* parent;

    if (obj.getParent())
        parent = obj.getParent()->getName().cstr();
    else
        parent = "--";

    str.format("            Parent: %s (0x%p)\n", parent, obj.getParent());
    PrintFormatter::out(SafeString(str.cstr()), nullptr, output);

    const char* direction;

    if (obj.getDirection() == Heap::HeapDirection::eForward)
        direction = "Forward";
    else
        direction = "Reverse";

    str.format("         Direction: %s\n", direction);
    PrintFormatter::out(SafeString(str.cstr()), nullptr, output);

    str.format("              Size: %zu\n", obj.getSize());
    PrintFormatter::out(SafeString(str.cstr()), nullptr, output);

    str.format("          FreeSize: %zu\n", obj.getFreeSize());
    PrintFormatter::out(SafeString(str.cstr()), nullptr, output);

    str.format("MaxAllocatableSize: %zu\n", obj.getMaxAllocatableSize());
    PrintFormatter::out(SafeString(str.cstr()), nullptr, output);

    PrintFormatter::out(SafeString("--------------------------------------------------\n"), nullptr, output);
}

} // namespace sead
