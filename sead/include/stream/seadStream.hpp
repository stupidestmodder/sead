#pragma once

#include <stream/seadStream.h>

namespace sead {

// TODO: There are much more specializations...

template <>
inline void Serialization::write<u8>(WriteStream& stream, const u8& value)
{
    stream.writeU8(value);
}

template <>
inline void Serialization::write<u16>(WriteStream& stream, const u16& value)
{
    stream.writeU16(value);
}

template <>
inline void Serialization::write<u32>(WriteStream& stream, const u32& value)
{
    stream.writeU32(value);
}

template <>
inline void Serialization::write<u64>(WriteStream& stream, const u64& value)
{
    stream.writeU64(value);
}

template <>
inline void Serialization::write<s8>(WriteStream& stream, const s8& value)
{
    stream.writeS8(value);
}

template <>
inline void Serialization::write<s16>(WriteStream& stream, const s16& value)
{
    stream.writeS16(value);
}

template <>
inline void Serialization::write<s32>(WriteStream& stream, const s32& value)
{
    stream.writeS32(value);
}

template <>
inline void Serialization::write<s64>(WriteStream& stream, const s64& value)
{
    stream.writeS64(value);
}

} // namespace sead
