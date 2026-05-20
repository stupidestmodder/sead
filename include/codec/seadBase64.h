#pragma once

#include <basis/seadTypes.h>

namespace sead {

class Base64
{
public:
    //? Note: This function probably does not exists, but it's here for convenience.
    //? Also does not include null terminator, add 1 to the result if you want to include it.
    static size_t calcEncodeSize(size_t srcSize)
    {
        return (srcSize / 3 + (srcSize % 3 != 0)) * 4;
    }

    static void encode(char* dst, const void* src, size_t srcSize, bool urlSafe);
    static void encodeWithNullTerminate(char* dst, const void* src, size_t srcSize, bool urlSafe)
    {
        dst[calcEncodeSize(srcSize)] = '\0';
        encode(dst, src, srcSize, urlSafe);
    }

    static bool decode(void* dst, size_t dstSize, const char* src, size_t srcSize, size_t* decodedSize);
};

} // namespace sead
