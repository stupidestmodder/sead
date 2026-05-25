#pragma once

#include <basis/seadTypes.h>

#include <new>

namespace sead {

const size_t cPtrSize = sizeof(void*);
const s32 cDefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

class Heap;

void AllocFailAssert(Heap* heap, size_t size, s32 alignment);

#if !defined(SEAD_TARGET_DEBUG)
inline void AllocFailAssert(Heap* heap, size_t size, s32 alignment)
{
    SEAD_UNUSED(heap);
    SEAD_UNUSED(size);
    SEAD_UNUSED(alignment);
}
#endif // SEAD_TARGET_DEBUG

} // namespace sead

//* So MSVC won't complain
#if defined(SEAD_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 28251)
#endif // SEAD_COMPILER_MSVC

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, const std::nothrow_t&) noexcept;
void* operator new[](size_t size, const std::nothrow_t&) noexcept;

#if defined(SEAD_COMPILER_MSVC)
#pragma warning(pop)
#endif // SEAD_COMPILER_MSVC

void* operator new(size_t size, std::align_val_t alignment);
void* operator new[](size_t size, std::align_val_t alignment);
void* operator new(size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;
void* operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;

void* operator new(size_t size, sead::Heap* heap);
void* operator new[](size_t size, sead::Heap* heap);
void* operator new(size_t size, sead::Heap* heap, const std::nothrow_t&) noexcept;
void* operator new[](size_t size, sead::Heap* heap, const std::nothrow_t&) noexcept;

void* operator new(size_t size, s32 alignment);
void* operator new[](size_t size, s32 alignment);
void* operator new(size_t size, s32 alignment, const std::nothrow_t&) noexcept;
void* operator new[](size_t size, s32 alignment, const std::nothrow_t&) noexcept;

void* operator new(size_t size, sead::Heap* heap, s32 alignment);
void* operator new[](size_t size, sead::Heap* heap, s32 alignment);
void* operator new(size_t size, sead::Heap* heap, s32 alignment, const std::nothrow_t&) noexcept;
void* operator new[](size_t size, sead::Heap* heap, s32 alignment, const std::nothrow_t&) noexcept;

void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete(void* ptr, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr, const std::nothrow_t&) noexcept;

void operator delete(void* ptr, size_t size) noexcept;
void operator delete[](void* ptr, size_t size) noexcept;
void operator delete(void* ptr, size_t size, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr, size_t size, const std::nothrow_t&) noexcept;

void operator delete(void* ptr, std::align_val_t alignment) noexcept;
void operator delete[](void* ptr, std::align_val_t alignment) noexcept;
void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept;

void operator delete(void* ptr, size_t size, std::align_val_t alignment) noexcept;
void operator delete[](void* ptr, size_t size, std::align_val_t alignment) noexcept;
void operator delete(void* ptr, size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr, size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;
