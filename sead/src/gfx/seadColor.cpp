#include <gfx/seadColor.h>

namespace sead {

const f32 Color4f::cElementMax = 1.0f;
const f32 Color4f::cElementMin = 0.0f;

const Color4f Color4f::cBlack(0.0f, 0.0f, 0.0f, 1.0f);
const Color4f Color4f::cGray(0.5f, 0.5f, 0.5f, 1.0f);
const Color4f Color4f::cWhite(1.0f, 1.0f, 1.0f, 1.0f);
const Color4f Color4f::cRed(1.0f, 0.0f, 0.0f, 1.0f);
const Color4f Color4f::cGreen(0.0f, 1.0f, 0.0f, 1.0f);
const Color4f Color4f::cBlue(0.0f, 0.0f, 1.0f, 1.0f);
const Color4f Color4f::cYellow(1.0f, 1.0f, 0.0f, 1.0f);
const Color4f Color4f::cMagenta(1.0f, 0.0f, 1.0f, 1.0f);
const Color4f Color4f::cCyan(0.0f, 1.0f, 1.0f, 1.0f);

Color4f& Color4f::operator*=(const Color4f& rhs)
{
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    a *= rhs.a;
    return *this;
}

Color4f operator*(const Color4f& lhs, const Color4f& rhs)
{
    return Color4f(lhs) *= rhs;
}

} // namespace sead
