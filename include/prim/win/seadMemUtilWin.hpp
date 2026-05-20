#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>
#include <prim/seadMemUtil.h>
#include <prim/seadPtrUtil.h>

#include <cstring>

namespace sead {

inline void* MemUtil::fill(void* dst, u8 val, size_t size)
{
    return std::memset(dst, val, size);
}

inline void* MemUtil::fillZero(void* dst, size_t size)
{
    return std::memset(dst, 0, size);
}

inline void* MemUtil::copyOverlap(void* dst, const void* src, size_t size)
{
    return std::memmove(dst, src, size);
}

inline void* MemUtil::copy(void* dst, const void* src, size_t size)
{
    SEAD_ASSERT_MSG(!PtrUtil::isInclude(src, dst, PtrUtil::addOffset(dst, size)) &&
                    !PtrUtil::isInclude(dst, src, PtrUtil::addOffset(src, size)),
                    "cross copy area");
    return std::memcpy(dst, src, size);
}

inline s32 MemUtil::compare(const void* addr1, const void* addr2, size_t size)
{
    return std::memcmp(addr1, addr2, size);
}

} // namespace sead
