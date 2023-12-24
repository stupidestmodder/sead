#include <basis/seadNew.h>

#include <basis/seadAssert.h>
#include <basis/seadWarning.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>

#include <cstdlib>

namespace sead { namespace system {

void* AllocFromSDK(size_t size)
{
    SEAD_WARNING("alloced[%zu] before sead system initialize", size);

#ifdef SEAD_PLATFORM_WINDOWS
    return std::malloc(size);
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void FreeFromSDK(void* ptr)
{
    SEAD_WARNING("free[0x%p] before sead system initialize", ptr);

#ifdef SEAD_PLATFORM_WINDOWS
    std::free(ptr);
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void* NewImpl(Heap* heap, size_t size, s32 alignment, bool useAssert)
{
    if (!HeapMgr::isInitialized())
        return AllocFromSDK(size);

    if (!heap)
    {
        heap = HeapMgr::instance()->getCurrentHeap();
        if (!heap)
        {
            SEAD_ASSERT_MSG(false, "Current heap is null. Cannot alloc.");
            return nullptr;
        }
    }

    void* ptr = heap->tryAlloc(size, alignment);
    if (!ptr && useAssert)
    {
        SEAD_ASSERT_MSG(false, "alloc failed. size: %zu, allocatable size: %zu, alignment: %d, heap: %s",
                        size, heap->getMaxAllocatableSize(alignment), alignment, heap->getName().cstr());
        return nullptr;
    }

    return ptr;
}

void DeleteImpl(void* p)
{
    if (!HeapMgr::isInitialized())
    {
        FreeFromSDK(p);
        return;
    }

    if (!p)
        return;

    Heap* heap = HeapMgr::instance()->findContainHeap(p);
    if (heap)
        heap->free(p);
    else
        SEAD_ASSERT_MSG(false, "delete bad pointer [0x%p]", p);
}

} // namespace system

#ifdef SEAD_DEBUG
void AllocFailAssert(Heap* heap, size_t size, s32 alignment)
{
    if (!heap)
    {
        heap = HeapMgr::instance()->getCurrentHeap();
        SEAD_ASSERT_MSG(heap, "Current heap is null. Cannot alloc.");
    }

    SEAD_ASSERT_MSG(false, "alloc failed. size: %zu, allocatable size: %zu, alignment: %d, heap: %s",
                    size, heap->getMaxAllocatableSize(alignment), alignment, heap->getName().cstr());
}
#endif // SEAD_DEBUG

} // namespace sead

// operator new(size_t)

void* operator new(size_t size)
{
    return sead::system::NewImpl(nullptr, size, sead::cDefaultAlignment, true);
}

void* operator new[](size_t size)
{
    return sead::system::NewImpl(nullptr, size, sead::cDefaultAlignment, true);
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(nullptr, size, sead::cDefaultAlignment, false);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(nullptr, size, sead::cDefaultAlignment, false);
}

// operator new(size_t, Heap*)

void* operator new(size_t size, sead::Heap* heap)
{
    return sead::system::NewImpl(heap, size, sead::cDefaultAlignment, true);
}

void* operator new[](size_t size, sead::Heap* heap)
{
    return sead::system::NewImpl(heap, size, sead::cDefaultAlignment, true);
}

void* operator new(size_t size, sead::Heap* heap, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(heap, size, sead::cDefaultAlignment, false);
}

void* operator new[](size_t size, sead::Heap* heap, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(heap, size, sead::cDefaultAlignment, false);
}

// operator new(size_t, s32)

void* operator new(size_t size, s32 alignment)
{
    return sead::system::NewImpl(nullptr, size, alignment, true);
}

void* operator new[](size_t size, s32 alignment)
{
    return sead::system::NewImpl(nullptr, size, alignment, true);
}

void* operator new(size_t size, s32 alignment, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(nullptr, size, alignment, false);
}

void* operator new[](size_t size, s32 alignment, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(nullptr, size, alignment, false);
}

// operator new(size_t, sead::Heap*, s32)

void* operator new(size_t size, sead::Heap* heap, s32 alignment)
{
    return sead::system::NewImpl(heap, size, alignment, true);
}

void* operator new[](size_t size, sead::Heap* heap, s32 alignment)
{
    return sead::system::NewImpl(heap, size, alignment, true);
}

void* operator new(size_t size, sead::Heap* heap, s32 alignment, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(heap, size, alignment, false);
}

void* operator new[](size_t size, sead::Heap* heap, s32 alignment, const std::nothrow_t&) noexcept
{
    return sead::system::NewImpl(heap, size, alignment, false);
}

// operator delete(void*)

void operator delete(void* ptr) noexcept
{
    sead::system::DeleteImpl(ptr);
}

void operator delete[](void* ptr) noexcept
{
    sead::system::DeleteImpl(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    sead::system::DeleteImpl(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    sead::system::DeleteImpl(ptr);
}
