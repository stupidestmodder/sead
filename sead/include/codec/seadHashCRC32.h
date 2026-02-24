#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace sead {

class HashCRC32
{
public:
    struct Context
    {
        Context()
            : hash(cContext)
        {
        }

        u32 hash;
    };

    static u32 calcHash(const void* dataptr, u32 datasize);
    static u32 calcHashWithContext(Context* ctx, const void* dataptr, u32 datasize);
    static u32 calcStringHash(const char* str);
    static u32 calcStringHashWithContext(Context* ctx, const char* str);

    static u32 calcStringHash(const SafeString& str)
    {
        return calcStringHash(str.cstr());
    }

    static u32 calcStringHashWithContext(Context* ctx, const SafeString& str)
    {
        return calcStringHashWithContext(ctx, str.cstr());
    }

    static void initialize();

private:
    static const u32 cTableSize = 256;
    static const u32 cTableMask = 0xEDB88320;
    static const u32 cContext = 0xFFFFFFFF;

    static u32 sTable[cTableSize];
    static bool sInitialized;
};

} // namespace sead
