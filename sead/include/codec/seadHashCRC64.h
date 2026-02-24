#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace sead {

//? CRC-64/XZ
class HashCRC64
{
public:
    struct Context
    {
        Context()
            : hash(cContext)
        {
        }

        u64 hash;
    };

    static u64 calcHash(const void* dataptr, u32 datasize);
    static u64 calcHashWithContext(Context* ctx, const void* dataptr, u32 datasize);
    static u64 calcStringHash(const char* str);
    static u64 calcStringHashWithContext(Context* ctx, const char* str);

    static u64 calcStringHash(const SafeString& str)
    {
        return calcStringHash(str.cstr());
    }

    static u64 calcStringHashWithContext(Context* ctx, const SafeString& str)
    {
        return calcStringHashWithContext(ctx, str.cstr());
    }

    static void initialize();

private:
    static const u32 cTableSize = 256;
    static const u64 cTableMask = 0xC96C5795D7870F42;
    static const u64 cContext = 0xFFFFFFFFFFFFFFFF;

    static u64 sTable[256];
    static bool sInitialized;
};

} // namespace sead
