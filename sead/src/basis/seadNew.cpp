#include <basis/seadNew.h>

#include <basis/seadAssert.h>
#include <basis/seadRawPrint.h>
#include <basis/seadWarning.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>

#include <cstdlib>

namespace sead { namespace system {

void* AllocFromSDK(size_t size, s32 alignment)
{
    SEAD_WARNING("alloced[" SEAD_FMT_SIZE_T "] before sead system initialize", size);

#if defined(SEAD_PLATFORM_WINDOWS)
    return ::_aligned_malloc(size, alignment);
#else
    #error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void FreeFromSDK(void* ptr)
{
    SEAD_WARNING("free[" SEAD_FMT_UINTPTR "] before sead system initialize", ptr);

#if defined(SEAD_PLATFORM_WINDOWS)
    ::_aligned_free(ptr);
#else
    #error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void* NewImpl(Heap* heap, size_t size, s32 alignment, bool assertOnFailure)
{
    if (!HeapMgr::isInitialized())
    {
        return AllocFromSDK(size, alignment);
    }

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
    if (ptr)
    {
        return ptr;
    }

    if (assertOnFailure)
    {
        SEAD_ASSERT_MSG(
            false, "alloc failed. size: " SEAD_FMT_SIZE_T ", allocatable size: " SEAD_FMT_SIZE_T ", alignment: %d, heap: %s",
            size, heap->getMaxAllocatableSize(alignment), alignment, heap->getName().cstr()
        );
    }

    return nullptr;
}

void DeleteImpl(void* ptr)
{
    if (!HeapMgr::isInitialized())
    {
        FreeFromSDK(ptr);
        return;
    }

    if (!ptr)
    {
        return;
    }

    Heap* heap = HeapMgr::instance()->findContainHeap(ptr);
    if (heap)
    {
        heap->free(ptr);
    }
    else
    {
        SEAD_ASSERT_MSG(false, "delete bad pointer [" SEAD_FMT_UINTPTR "]", ptr);
    }
}

} // namespace system

#if defined(SEAD_TARGET_DEBUG)
void AllocFailAssert(Heap* heap, size_t size, s32 alignment)
{
    if (!heap)
    {
        heap = HeapMgr::instance()->getCurrentHeap();
        SEAD_ASSERT_MSG(heap, "Current heap is null. Cannot alloc.");
    }

    SEAD_ASSERT_MSG(
        false, "alloc failed. size: " SEAD_FMT_SIZE_T ", allocatable size: " SEAD_FMT_SIZE_T ", alignment: %d, heap: %s",
        size, heap->getMaxAllocatableSize(alignment), alignment, heap->getName().cstr()
    );
}
#endif // SEAD_TARGET_DEBUG

} // namespace sead

// operator new(size_t)

//* So MSVC won't complain

#if defined(SEAD_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 28251)
#endif // SEAD_COMPILER_MSVC

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

#if defined(SEAD_COMPILER_MSVC)
#pragma warning(pop)
#endif // SEAD_COMPILER_MSVC

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
