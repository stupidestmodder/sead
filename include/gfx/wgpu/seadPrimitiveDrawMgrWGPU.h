#pragma once

#include <gfx/wgpu/seadWGPU.h>
#include <gfx/wgpu/seadTextureWGPU.h>
#include <gfx/seadPrimitiveDrawMgrBase.h>
#include <gfx/seadPrimitiveDrawUtil.h>
#include <heap/seadDisposer.h>

namespace sead {

class PrimitiveDrawMgrWGPU : public PrimitiveDrawMgrBase
{
    SEAD_SINGLETON_DISPOSER(PrimitiveDrawMgrWGPU);

protected:
    static constexpr u32 cFrameDrawLimit = 1024;

    struct alignas(256) Uniforms
    {
        Matrix44f wvp = Matrix44f::ident;
        Matrix44f user = Matrix44f::ident;
        Color4f color0 = Color4f::cBlack;
        Color4f color1 = Color4f::cBlack;
        Vector2f uvSrc = Vector2f::zero;
        Vector2f uvSize = Vector2f::zero;
        f32 rate = 0.0f;
    };

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

protected:
    void updateBindings_(const Texture* texture);
    void createBuffer_(WGPUBuffer* vertex, WGPUBuffer* index, const PrimitiveDrawUtil::Vertex* vertices, size_t verticesSize, const u16* indices, size_t indicesSize);
    void drawVertexBuffer_(DrawContext* drawContext, const Matrix34f& modelMtx,
                           WGPUBuffer vertex, u64 vertexSize,
                           const Color4f& color0, const Color4f& color1);
    void drawPolygon_(DrawContext* drawContext, const Matrix34f& modelMtx,
                      WGPUBuffer vertex, WGPUBuffer index,
                      u64 vertexSize, u64 indexSize, u32 indexCount,
                      const Color4f& color0, const Color4f& color1);
    void drawLines_(DrawContext* drawContext, const Matrix34f& modelMtx,
                    WGPUPrimitiveTopology topology,
                    WGPUBuffer vertex, WGPUBuffer index,
                    u64 vertexSize, u64 indexSize, u32 indexCount,
                    const Color4f& color0, const Color4f& color1);

protected:
    WGPUShaderModule mVS;
    WGPUShaderModule mFS;
    WGPUSampler mSampler;
    WGPUBuffer mUniforms;
    Uniforms mUniformData;
    u32 mDrawCalls;
    WGPUPipelineLayout mPipelineLayout;
    WGPUBindGroup mBindGroup;
    WGPUTextureView mPrevView;
    WGPUBindGroupLayout mGroupLayout;

    WGPUVertexAttribute mAttributes[3];
    WGPUVertexBufferLayout mVertexLayout;
    WGPUBuffer mQuadVtx;
    WGPUBuffer mQuadIdx;
    WGPUBuffer mBoxVtx;
    WGPUBuffer mBoxIdx;
    WGPUBuffer mLineVtx;
    WGPUBuffer mLineIdx;
    WGPUBuffer mSphereSVtx;
    WGPUBuffer mSphereSIdx;
    WGPUBuffer mSphereLVtx;
    WGPUBuffer mSphereLIdx;
    TextureWGPU mDefaultTexture;
};

} // namespace sead
