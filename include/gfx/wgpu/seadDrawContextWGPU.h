#pragma once

#include <container/seadObjList.h>
#include <container/seadTreeMap.h>
#include <gfx/seadColor.h>
#include <gfx/wgpu/seadGraphicsWGPU.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class Framework;

class DrawContext
{
    SEAD_RTTI_BASE(DrawContext);

public:
    DrawContext();
    virtual ~DrawContext() = default;

    void begin();
    void submit();

    void beginPass(const WGPURenderPassColorAttachment* colorAttachments, const WGPUTexture* colorTextures, u32 colorAttachmentNum,
                   const WGPURenderPassDepthStencilAttachment* depthStencilAttachment = nullptr, const WGPUTexture depthStencilTexture = nullptr);
    void endPass();

    void setClear(u32 clrFlag, const Color4f* color, f32 depth, u32 stencil);
    void setViewport(f32 x, f32 y, f32 width, f32 height);
    void setScissor(f32 x, f32 y, f32 width, f32 height);
    void setGraphicsContext(
        Graphics::CullingMode cullMode,
        Graphics::PolygonMode polygonMode,
        bool depthTestEnable,
        bool depthWriteEnable,
        Graphics::DepthFunc depthFunc,
        bool stencilTestEnable,
        Graphics::StencilFunc stencilFunc,
        u32 stencilTestMask,
        u32 stencilTestReference,
        Graphics::StencilOp stencilFailOp,
        Graphics::StencilOp stencilDepthFailOp,
        Graphics::StencilOp stencilPassOp,

        //* MRT unsupported here
        bool blendEnable,
        Graphics::BlendFactor blendFactorSrcRGB,
        Graphics::BlendFactor blendFactorDstRGB,
        Graphics::BlendEquation blendEquationRGB,
        Graphics::BlendFactor blendFactorSrcA,
        Graphics::BlendFactor blendFactorDstA,
        Graphics::BlendEquation blendEquationA,
        bool colorWriteMaskR, bool colorWriteMaskG, bool colorWriteMaskB, bool colorWriteMaskA,
        const Color4f& blendConstant
    );
    void setShader(WGPUShaderModule vertexShader, WGPUShaderModule fragmentShader);
    void setBindGroup(const WGPUBindGroup* groups, u32 groupNum, WGPUPipelineLayout layout);

    void drawInstanced(WGPUPrimitiveTopology mode, u32 first, u32 count, u32 instances);
    void draw(WGPUPrimitiveTopology mode, u32 first, u32 count) { drawInstanced(mode, first, count, 1); }

    WGPUCommandEncoder getEncoder() const { return mEncoder; }
    WGPURenderPassEncoder getRenderPass();

    static void initializeCache(Heap* heap, s32 entries);

protected:
    void commitRenderPass_();
    void resetRenderPassState_();
    void resetPipelineState_();
    void resetBindingState_();
    WGPURenderPipeline commitRenderPipeline_();

protected:
    static class PipelineCache
    {
    public:
        void initialize(Heap* heap, s32 entries);

        WGPURenderPipeline getOrCreate(WGPUDevice device, const WGPURenderPipelineDescriptor& desc);

    protected:
        struct Entry
        {
            WGPURenderPipeline pipeline;
            u64 key;
        };

        ObjList<Entry> mList;
        TreeMap<u64, Entry*> mMap;
    } sPipelineCache;

protected:
    WGPUCommandEncoder mEncoder;
    WGPURenderPassEncoder mRenderPassEncoder;
    WGPUDevice mDevice;
    WGPUQueue mQueue;

    struct RenderPassState
    {
        WGPURenderPassColorAttachment colorAttachments[8];
        WGPURenderPassDepthStencilAttachment depthStencilAttachment;
        WGPUTextureFormat colorFormats[8];
        WGPUTextureFormat depthStencilFormat;
        u32 sampleCount;
        u32 colorAttachmentCount;
        bool hasDepthStencil;
        struct
        {
            f32 x, y, width, height;
        } viewport, scissor;
        u32 stencilReference;
        WGPUColor blendConstant;
    } mRenderPassState;

    struct PipelineState
    {
        WGPUShaderModule vs;
        WGPUShaderModule fs;

        u32 vertexAttributeCount;
        WGPUVertexAttribute vertexAttributes[8];
        u64 vertexArrayStride;
        WGPUVertexStepMode vertexStepMode;

        WGPUPrimitiveTopology topology;

        WGPUFrontFace frontFace;
        Graphics::CullingMode cullMode;
        Graphics::PolygonMode polygonMode; // front and back must be the same, API restriction

        bool depthTestEnable;
        bool depthWriteEnable;
        Graphics::DepthFunc depthFunc;

        bool stencilTestEnable;
        Graphics::StencilFunc stencilFunc;
        u32 stencilTestMask;
        Graphics::StencilOp stencilFailOp;
        Graphics::StencilOp stencilDepthFailOp;
        Graphics::StencilOp stencilPassOp;

        bool blendEnable[8];
        WGPUBlendComponent blendColor[8];
        WGPUBlendComponent blendAlpha[8];
        WGPUColorWriteMask colorWriteMask[8];
    } mPipelineState;
    struct BindingState
    {
        WGPUBindGroup bindGroups[4];
        WGPUPipelineLayout bindLayout;

        WGPUBuffer vertexBuffers[8];
        u64 vertexBufferOffsets[8];
        u64 vertexBufferSizes[8];

        WGPUBuffer indexBuffer;
        WGPUIndexFormat indexFormat;
        u64 indexBufferOffset;
        u64 indexBufferSize;
    } mBindingState;
};

} // namespace sead
