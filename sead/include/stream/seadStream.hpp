#pragma once

#include <stream/seadStream.h>

namespace sead {

// TODO: There are much more specializations...

template <>
void Serialization::write<u32>(WriteStream& stream, const u32& value)
{
    stream.writeU32(value);
}

} // namespace sead
