#include <codec/seadHashCRC16.h>

namespace sead {

u16 HashCRC16::sTable[cTableSize];
bool HashCRC16::sInitialized = false;

void HashCRC16::initialize()
{
    u16 r = 0;
    for (u32 i = 0; i < cTableSize; i++)
    {
        r = i;
        for (u32 j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ cTableMask;
            }
            else
            {
                r >>= 1;
            }
        }
        sTable[i] = r;
    }

    sInitialized = true;
}

u16 HashCRC16::calcHash(const void* dataptr, u32 datasize)
{
    Context ctx;
    return calcHashWithContext(&ctx, dataptr, datasize);
}

u16 HashCRC16::calcHashWithContext(Context* ctx, const void* dataptr, u32 datasize)
{
    if (!sInitialized)
    {
        initialize();
    }

    u16 r = ctx->hash;
    const u8* data = static_cast<const u8*>(dataptr);

    for (u32 i = 0; i < datasize; i++)
    {
        u16 t = sTable[(r ^ *data++) & 0xFF];
        r = (r >> 8) ^ t;
    }

    ctx->hash = r;
    return r;
}

u16 HashCRC16::calcStringHash(const char* str)
{
    Context ctx;
    return calcStringHashWithContext(&ctx, str);
}

u16 HashCRC16::calcStringHashWithContext(Context* ctx, const char* str)
{
    if (!sInitialized)
    {
        initialize();
    }

    u16 r = ctx->hash;
    const u8* data = reinterpret_cast<const u8*>(str);

    while (*data)
    {
        u16 t = sTable[(r ^ *data++) & 0xFF];
        r = (r >> 8) ^ t;
    }

    ctx->hash = r;
    return r;
}

} // namespace sead
