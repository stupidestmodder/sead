#pragma once

// TODO: More stuff to add.

namespace sead {

template <bool B, typename T = void>
struct EnableIfC
{
};

template <typename T>
struct EnableIfC<true, T>
{
    using Type = T;
};

} // namespace sead
