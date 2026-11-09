#pragma once

#include <gfx/wgpu/seadGraphicsWGPU.h>
#include <gfx/seadFrameBuffer.h>
#include <heap/seadDisposer.h>

namespace sead {

class TextureWGPU;

class TextureFrameBufferWGPU : public FrameBuffer, public IDisposer
{
    SEAD_RTTI_OVERRIDE(TextureFrameBufferWGPU, FrameBuffer);

public:
    TextureFrameBufferWGPU()
        : FrameBuffer()
        , IDisposer()
        , mColorTexture(nullptr)
        , mDepthTexture(nullptr)
    {
    }

    TextureFrameBufferWGPU(const Vector2f& virtualSize, const BoundBox2f& physicalArea)
        : FrameBuffer(virtualSize, physicalArea)
        , IDisposer()
        , mColorTexture(nullptr)
        , mDepthTexture(nullptr)
    {
    }

    TextureFrameBufferWGPU(const Vector2f& virtualSize, f32 physicalX, f32 physicalY, f32 physicalW, f32 physicalH)
        : FrameBuffer(virtualSize, physicalX, physicalY, physicalW, physicalH)
        , IDisposer()
        , mColorTexture(nullptr)
        , mDepthTexture(nullptr)
    {
    }

    ~TextureFrameBufferWGPU() override;

    void copyToDisplayBuffer(DrawContext* drawContext, const DisplayBuffer* displayBuffer) const override;
    void clear(DrawContext* drawContext, u32 clrFlag, const Color4f& color, f32 depth, u32 stencil) const override;

    void setup(TextureWGPU* colorTexture, TextureWGPU* depthTexture = nullptr);

protected:
    void bindImpl_(DrawContext* drawContext) const override;

protected:
    TextureWGPU* mColorTexture;
    TextureWGPU* mDepthTexture;
};

class DisplayBufferWGPU : public DisplayBuffer
{
    SEAD_RTTI_OVERRIDE(DisplayBufferWGPU, DisplayBuffer);

public:
    DisplayBufferWGPU();
    ~DisplayBufferWGPU() override;

    void copyToDisplay(sead::DrawContext* drawContext) const;
    void reconfigure(f32 width, f32 height);

    TextureWGPU* getTexture() const { return mTextureWGPU; }

    void setGamma(f32 gamma) { mGamma = gamma; }

protected:
    void initializeImpl_(Heap* heap) override;

protected:
    TextureWGPU* mTextureWGPU;
    WGPUShaderModule mVertShader;
    WGPUShaderModule mFragShader;
    WGPUBuffer mGammaBuffer;
    WGPUBindGroup mBindGroup;
    WGPUPipelineLayout mPipelineLayout;
    WGPUSampler mSampler;
    f32 mGamma;
};

} // namespace sead
