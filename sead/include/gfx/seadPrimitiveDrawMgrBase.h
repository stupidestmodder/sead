#pragma once

#include <gfx/seadColor.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>

namespace sead {

class DrawContext;
class Texture;

class PrimitiveDrawMgrBase
{
public:
    virtual void prepareFromBinaryImpl(Heap* heap, const void* binData, u32 binSize) = 0;
    virtual void prepareImpl(Heap* heap, const SafeString& path) = 0;
    virtual void beginImpl(DrawContext* drawContext, const Matrix34f& viewMatrix, const Matrix44f& projectionMatrix) = 0;
    virtual void endImpl(DrawContext* drawContext) = 0;
    virtual void drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR) = 0;
    virtual void drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Texture& texture,
                              const Color4f& colorL, const Color4f& colorR,
                              const Vector2f& uvSrc, const Vector2f& uvSize) = 0;
    virtual void drawBoxImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR) = 0;
    virtual void drawLineImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& c0, const Color4f& c1) = 0;
    // TODO: Add draw methods
    virtual ~PrimitiveDrawMgrBase() { }
};

} // namespace sead
