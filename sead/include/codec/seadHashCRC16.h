#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace sead {

//? CRC-16/ARC
class HashCRC16
{
public:
    struct Context
    {
        Context()
            : hash(cContext)
        {
        }

        u16 hash;
    };

    static u16 calcHash(const void* dataptr, u32 datasize);
    static u16 calcHashWithContext(Context* ctx, const void* dataptr, u32 datasize);
    static u16 calcStringHash(const char* str);
    static u16 calcStringHashWithContext(Context* ctx, const char* str);

    static u16 calcStringHash(const SafeString& str)
    {
        return calcStringHash(str.cstr());
    }

    static u16 calcStringHashWithContext(Context* ctx, const SafeString& str)
    {
        return calcStringHashWithContext(ctx, str.cstr());
    }

    static void initialize();

private:
    static const u32 cTableSize = 256;
    static const u16 cTableMask = 0xA001;
    static const u16 cContext = 0x0;

    static u16 sTable[256];
    static bool sInitialized;
};

} // namespace sead
