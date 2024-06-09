#pragma once

#include <math/seadMatrix.h>

namespace sead {

class Camera;
class Color4f;
class DrawContext;
class Projection;

class FontBase
{
public:
    enum class Encoding
    {
        eSJIS = 0,
        eUTF8,
        eUTF16
    };

public:
    FontBase()
    {
    }

    virtual ~FontBase()
    {
    }

    virtual f32 getHeight() const = 0;
    virtual f32 getWidth() const = 0;
    virtual f32 getCharWidth(char16 letter) const = 0;
    virtual Encoding getEncoding() const = 0;
    virtual u32 getMaxDrawNum() const = 0;
    virtual void begin(DrawContext* drawContext) const = 0;
    virtual void end(DrawContext* drawContext) const = 0;
    virtual void print(DrawContext* drawContext, const Projection& projection, const Camera& camera, const Matrix34f& mtx,
                       const Color4f& color, const void* text, s32 length) const = 0;
};

} // namespace sead
