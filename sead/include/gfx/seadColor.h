#pragma once

#include <basis/seadTypes.h>

#ifdef SEAD_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4201)
#endif // SEAD_COMPILER_MSVC

namespace sead {

// TODO
struct Color4f
{
public:
    Color4f()
        : r(cElementMin)
        , g(cElementMin)
        , b(cElementMin)
        , a(cElementMax)
    {
    }

    Color4f(f32 _r, f32 _g, f32 _b, f32 _a)
        : r(_r)
        , g(_g)
        , b(_b)
        , a(_a)
    {
    }

    static const f32 cElementMax;
    static const f32 cElementMin;

    static const Color4f cBlack;
    static const Color4f cGray;
    static const Color4f cWhite;
    static const Color4f cRed;
    static const Color4f cGreen;
    static const Color4f cBlue;
    static const Color4f cYellow;
    static const Color4f cMagenta;
    static const Color4f cCyan;

    union
    {
        struct
        {
            f32 r;
            f32 g;
            f32 b;
            f32 a;
        };

        struct
        {
            f32 c[4];
        };
    };
};

} // namespace sead

#ifdef SEAD_COMPILER_MSVC
#pragma warning(pop)
#endif // SEAD_COMPILER_MSVC
