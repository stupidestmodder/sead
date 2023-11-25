#pragma once

#include <basis/seadTypes.h>

#include <new>

namespace sead {

class Heap;

void AllocFailAssert(Heap* heap, size_t size, s32 alignment);

#ifndef SEAD_DEBUG
inline void AllocFailAssert(Heap*, size_t, s32)
{
}
#endif // SEAD_DEBUG

} // namespace sead

//? So MSVC won't complain

#ifdef SEAD_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 28251)
#endif

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, const std::nothrow_t&) noexcept;
void* operator new[](size_t size, const std::nothrow_t&) noexcept;

#ifdef SEAD_COMPILER_MSVC
#pragma warning(pop)
#endif

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

void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr, const std::nothrow_t&) noexcept;
