#pragma once

#include <gfx/seadColor.h>
#include <math/seadBoundBox.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>

#include <cstdarg>

namespace sead {

class Camera;
class DrawContext;
class FontBase;
class Projection;
class Viewport;

// TODO
class TextWriter
{
public:
    enum class LineFeedType
    {
        eNone = 0,
        eSymbol,
        eWidthLimit
    };

    static FontBase* sDefaultFont;

public:
    explicit TextWriter(DrawContext*);
    TextWriter(DrawContext*, const Viewport*) {}
    virtual ~TextWriter();

    static FontBase* getDefaultFont();
    static void setDefaultFont(FontBase*);

    static void setupGraphics(DrawContext* drawContext);

    f32 getLineHeight() const
    {
        // TODO
        return 1.0f;
    }

    void setProjectionAndCamera(const Projection*, const Camera*);
    void getCursorFromTopLeft(Vector2f*) const;
    void setCursorFromTopLeft(const Vector2f&);
    void setCursorXFromTopLeft(f32);
    void setCursorYFromTopLeft(f32);
    void setScale(const Vector2f& scale) { mScale = scale; }
    void setScaleFromFontSize(const Vector2f&);
    void setScaleFromFontHeight(f32);
    void setColor(const Color4f& color) { mColor = color; }
    void setLineSpaceFromLineHeight(f32);
    void setFormatBuffer(char16*, s32);

    void beginDraw();
    void endDraw();

    void printf(const char16*, ...);
    void printf(const char*, ...);
    void printfWithCalcRect(BoundBox2f*, const char16*, ...);
    void printfWithCalcRect(BoundBox2f*, const char*, ...);
    void calcFormatStringRect(BoundBox2f*, const char16*, ...);
    void calcFormatStringRect(BoundBox2f*, const char*, ...);

protected:
    void printImpl_(const char16*, s32, bool, BoundBox2f*, const Projection*, const Camera*);
    void printImpl_(const char16*, s32, bool, BoundBox2f*);
    void printImpl_(const char*, s32, bool, BoundBox2f*);
    void vprintfImpl_(const char16*, std::va_list, bool, BoundBox2f*);
    void vprintfImpl_(const char*, std::va_list, bool, BoundBox2f*);

protected:
    const Viewport* mViewport;
    const Projection* mProjection;
    const Camera* mCamera;
    const FontBase* mFont;
    Vector2f mCursorPos;
    Vector2f mScale;
    Color4f mColor;
    f32 mCharSpace;
    f32 mLineSpace;
    const Matrix34f* mModelMatrix;
    f32 mWidthLimit;
    char16* mFormatBuffer;
    s32 mFormatBufferLength;
    bool mIsNeedBeginEnd;
};

} // namespace sead
