#include <codec/seadBase64.h>

#include <basis/seadAssert.h>

static u32 decodeChar_(u32 c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9')
    {
        return c - '0' + 52;
    }
    if (c == '+' || c == '-')
    {
        return 62;
    }
    if (c == '/' || c == '_')
    {
        return 63;
    }

    return 64; //? Invalid character
}

namespace sead {

void Base64::encode(char* dst, const void* src, size_t srcSize, bool urlSafe)
{
    SEAD_ASSERT_MSG(dst, "dst must not be null.");

    // size_t loopcount;
    
    const char* base64;
    if (urlSafe)
    {
        base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    }
    else
    {
        base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    }

    const u8* srcU8 = static_cast<const u8*>(src);

    for (size_t i = 0; i < (srcSize / 3) * 3; i += 3)
    {
        dst[0] = base64[srcU8[0] >> 2];
        dst[1] = base64[((srcU8[0] & 0x3) << 4) | ((srcU8[1] & 0xF0) >> 4)];
        dst[2] = base64[((srcU8[1] & 0xF) << 2) | ((srcU8[2] & 0xC0) >> 6)];
        dst[3] = base64[srcU8[2] & 0x3F];
        dst += 4;
        srcU8 += 3;
    }

    if (srcSize % 3 == 1)
    {
        dst[0] = base64[srcU8[0] >> 2];
        dst[1] = base64[(srcU8[0] & 0x3) << 4];
        dst[2] = '=';
        dst[3] = '=';
    }
    else if (srcSize % 3 == 2)
    {
        dst[0] = base64[srcU8[0] >> 2];
        dst[1] = base64[((srcU8[0] & 0x3) << 4) | ((srcU8[1] & 0xF0) >> 4)];
        dst[2] = base64[(srcU8[1] & 0xF) << 2];
        dst[3] = '=';
    }
}

bool Base64::decode(void* dst, size_t dstSize, const char* src, size_t srcSize, size_t* decodedSize)
{
    SEAD_ASSERT_MSG(dst, "dst must not be null.");

    size_t size = 0;
    u8* dstU8 = static_cast<u8*>(dst);
    for (size_t i = 0; i < srcSize; i += 4)
    {
        if (src[0] < ' ')
        {
            if (srcSize <= i + 1)
            {
                break;
            }

            src++;
            i++;

            if (src[0] < ' ')
            {
                src++;
                i++;

                if (srcSize <= i)
                {
                    break;
                }
            }
        }

        if (srcSize < i + 4)
        {
            return false;
        }

        u32 a = decodeChar_(*(src++)) & 0x3F;
        u32 b = decodeChar_(*(src++)) & 0x3F;
        u32 c = decodeChar_(*(src++)) & 0x7F;
        u32 d = decodeChar_(*(src++)) & 0x7F;

        if (size >= dstSize)
        {
            return false;
        }

        dstU8[size++] = (a << 2) | (b >> 4);
        if (c < 64)
        {
            if (size >= dstSize)
            {
                return false;
            }

            dstU8[size++] = (b << 4) | (c >> 2);
            if (d < 64)
            {
                if (size >= dstSize)
                {
                    return false;
                }

                dstU8[size++] = (c << 6) | d;
            }
        }
    }

    if (decodedSize)
    {
        *decodedSize = size;
    }

    return true;
}

} // namespace sead
