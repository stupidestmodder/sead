#pragma once

#include <container/seadBuffer.h>
#include <prim/seadSafeString.h>

#include <cstdarg>

// TODO
namespace sead { namespace StringUtil {

struct Char16Pair
{
    char16 before;
    char16 after;
};

enum class CardinalNumber
{
    eAuto = -1,
    eBase2 = 2,
    eBase8 = 8,
    eBase10 = 10,
    eBase16 = 16
};

bool tryParseU8(u8* dst, const SafeString& str, CardinalNumber cardinalNumber);
bool tryParseU16(u16* dst, const SafeString& str, CardinalNumber cardinalNumber);
bool tryParseU32(u32* dst, const SafeString& str, CardinalNumber cardinalNumber);
bool tryParseU64(u64* dst, const SafeString& str, CardinalNumber cardinalNumber);

bool tryParseS8(s8* dst, const SafeString& str, CardinalNumber cardinalNumber);
bool tryParseS16(s16* dst, const SafeString& str, CardinalNumber cardinalNumber);
bool tryParseS32(s32* dst, const SafeString& str, CardinalNumber cardinalNumber);
bool tryParseS64(s64* dst, const SafeString& str, CardinalNumber cardinalNumber);

bool tryParseF32(f32* dst, const SafeString& str);
bool tryParseF64(f64* dst, const SafeString& str);

u8 parseU8(const SafeString& str, CardinalNumber cardinalNumber);
u16 parseU16(const SafeString& str, CardinalNumber cardinalNumber);
u32 parseU32(const SafeString& str, CardinalNumber cardinalNumber);
u64 parseU64(const SafeString& str, CardinalNumber cardinalNumber);

s8 parseS8(const SafeString& str, CardinalNumber cardinalNumber);
s16 parseS16(const SafeString& str, CardinalNumber cardinalNumber);
s32 parseS32(const SafeString& str, CardinalNumber cardinalNumber);
s64 parseS64(const SafeString& str, CardinalNumber cardinalNumber);

f32 parseF32(const SafeString& str);
f64 parseF64(const SafeString& str);

s32 snprintf(char* dst, size_t dstSize, const char* format, ...);
s32 vsnprintf(char* dst, size_t dstSize, const char* format, std::va_list args);

s32 sw16printf(char16* dst, size_t dstLen, const char16* format, ...);
s32 vsw16printf(char16* dst, size_t dstLen, const char16* format, std::va_list args);

char16* wcs16cpy(char16* dst, size_t dstLen, const char16* src);

s32 convertUtf8ToSjis(char* dst, u32 dstLen, const char* src, s32 srcLen);
s32 convertUtf8ToUtf16(char16* dst, u32 dstLen, const char* src, s32 srcLen);

s32 convertUtf16ToSjis(char* dst, u32 dstLen, const char16* src, s32 srcLen);
s32 convertUtf16ToUtf8(char* dst, u32 dstLen, const char16* src, s32 srcLen);

s32 convertSjisToUtf8(char* dst, u32 dstLen, const char* src, s32 srcLen);
s32 convertSjisToUtf16(char16* dst, u32 dstLen, const char* src, s32 srcLen);

inline s32 convertUtf8ToSjis(BufferedSafeString* dst, const SafeString& src, s32 srcLen)
{
    return convertUtf8ToSjis(dst->getBuffer(), dst->getBufferSize(), src.cstr(), srcLen);
}

inline s32 convertUtf16ToUtf8(BufferedSafeString* dst, const WSafeString& src, s32 srcLen)
{
    return convertUtf16ToUtf8(dst->getBuffer(), dst->getBufferSize(), src.cstr(), srcLen);
}

s32 compareChar16Pair(const Char16Pair& value, const char16& target);

char16 replace(char16 c, const Buffer<const Char16Pair>& sortedTable);

char16 toUpperCapital(char16 c);
void toUpperCapitalFirstCharactor(WBufferedSafeString* str);

char16 toLowerCapital(char16 c);
void toLowerCapitalFirstCharactor(WBufferedSafeString* str);

} } // namespace sead::StringUtil
