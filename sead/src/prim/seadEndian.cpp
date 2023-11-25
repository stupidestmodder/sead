#include <prim/seadEndian.h>

static u8 Swap8(u8 x)
{
    return x;
}

static u8 Null8(u8 x)
{
    return x;
}

static u16 Swap16(u16 x)
{
    return (x << 8 | x >> 8) & 0xFFFF;
}

static u16 Null16(u16 x)
{
    return x;
}

static u32 Swap32(u32 x)
{
    return x << 24 |
          (x & 0xFF00) << 8 |
           x >> 24 |
           x >> 8 & 0xFF00;
}

static u32 Null32(u32 x)
{
    return x;
}

static u64 Swap64(u64 x)
{
    return x << 56 |
          (x & 0xFF00) << 40 |
          (x & 0xFF0000) << 24 |
          (x & 0xFF000000) << 8 |
           x >> 56 |
           x >> 40 & 0xFF00 |
           x >> 24 & 0xFF0000 |
           x >> 8 & 0xFF000000;
}

static u64 Null64(u64 x)
{
    return x;
}

namespace sead {

const Endian::Types Endian::cHostEndian = Endian::markToEndian(0xFEFF);

const Endian::ConvFuncTable Endian::cConvFuncTable = {
    { &Null8,  &Swap8 },
    { &Null16, &Swap16 },
    { &Null32, &Swap32 },
    { &Null64, &Swap64 }
};

} // namespace sead
