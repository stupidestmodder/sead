#pragma once

#include <gfx/wgpu/seadWGPU.h>
#include <gfx/seadPrimitiveDrawMgrBase.h>
#include <heap/seadDisposer.h>

namespace sead {

// TODO
class PrimitiveDrawMgrWGPU : public PrimitiveDrawMgrBase
{
    SEAD_SINGLETON_DISPOSER(PrimitiveDrawMgrWGPU);

public:
    PrimitiveDrawMgrWGPU();
    ~PrimitiveDrawMgrWGPU() override;

    void prepareFromBinaryImpl(Heap* heap, const void* binData, u32 binSize) override;
    void prepareImpl(Heap* heap, const SafeString& path) override;
    void beginImpl(DrawContext* drawContext, const Matrix34f& viewMatrix, const Matrix44f& projectionMatrix) override;
    void endImpl(DrawContext* drawContext) override;
    void drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR) override;
    void drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Texture& texture,
                    const Color4f& colorL, const Color4f& colorR,
                    const Vector2f& uvSrc, const Vector2f& uvSize) override;
    void drawBoxImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR) override;
    // drawCubeImpl()
    // drawWireCubeImpl()
    void drawLineImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& c0, const Color4f& c1) override;
    void drawSphere4x8Impl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& north, const Color4f& south) override;
    void drawSphere8x16Impl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& north, const Color4f& south) override;
};

} // namespace sead
