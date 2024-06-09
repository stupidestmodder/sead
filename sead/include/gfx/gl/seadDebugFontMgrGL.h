#pragma once

#include <gfx/seadFontBase.h>
#include <heap/seadDisposer.h>

namespace sead {

// TODO
class DebugFontMgrGL : public FontBase
{
    SEAD_SINGLETON_DISPOSER(DebugFontMgrGL);

public:
    DebugFontMgrGL();
    ~DebugFontMgrGL() override;

    void initialize();

    f32 getHeight() const override;
    f32 getWidth() const override;
    f32 getCharWidth(char16 letter) const override;
    Encoding getEncoding() const override;
    u32 getMaxDrawNum() const override;
    void begin(DrawContext* drawContext) const override;
    void end(DrawContext* drawContext) const override;
    void print(DrawContext* drawContext, const Projection& projection, const Camera& camera, const Matrix34f& mtx,
                       const Color4f& color, const void* text, s32 length) const override;

protected:
};

} // namespace sead
