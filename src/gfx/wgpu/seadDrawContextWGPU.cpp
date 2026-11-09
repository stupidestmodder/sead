#include "gfx/wgpu/seadDrawContextWGPU.h"
#include "gfx/wgpu/seadPipelineHashWGPU.h"
#include "webgpu/webgpu.h"
#include <prim/seadMemUtil.h>
#include <gfx/seadFrameBuffer.h>
#include <gfx/seadDrawContext.h>
#include <gfx/wgpu/seadWGPU.h>

namespace sead {

DrawContext::PipelineCache DrawContext::sPipelineCache;

DrawContext::DrawContext()
    : mEncoder(nullptr)
    , mRenderPassEncoder(nullptr)
    , mDevice(GraphicsWGPU::instance()->getDevice())
    , mQueue(GraphicsWGPU::instance()->getQueue())
{
    resetRenderPassState_();
    resetPipelineState_();
    resetBindingState_();
}

void DrawContext::begin()
{
    mEncoder = wgpuDeviceCreateCommandEncoder(mDevice, nullptr);
}

void DrawContext::submit()
{
    SEAD_ASSERT(mEncoder != nullptr);

    // force commit pending operations
    if (mRenderPassEncoder == nullptr && (mRenderPassState.colorAttachmentCount > 0 || mRenderPassState.hasDepthStencil))
        getRenderPass();
    endPass();

    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(mEncoder, nullptr);
    wgpuCommandEncoderRelease(mEncoder);
    mEncoder = nullptr;
    wgpuQueueSubmit(mQueue, 1, &commandBuffer);
    wgpuCommandBufferRelease(commandBuffer);
}

void DrawContext::beginPass(const WGPURenderPassColorAttachment* colorAttachments, const WGPUTexture* colorTextures, u32 colorAttachmentNum, const WGPURenderPassDepthStencilAttachment* depthStencilAttachment, const WGPUTexture depthStencilTexture)
{
    endPass();

    mRenderPassState.colorAttachmentCount = colorAttachmentNum;

    for (u32 i = 0; i < colorAttachmentNum; i++)
    {
        mRenderPassState.colorAttachments[i] = colorAttachments[i];
        mRenderPassState.colorFormats[i] = wgpuTextureGetFormat(colorTextures[i]);
        mRenderPassState.colorAttachments[i].depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    }

    if (colorAttachmentNum > 0)
    {
        mRenderPassState.sampleCount = wgpuTextureGetSampleCount(colorTextures[0]);

        for (u32 i = 1; i < colorAttachmentNum; i++)
        {
            SEAD_ASSERT_MSG(wgpuTextureGetSampleCount(colorTextures[i]) == mRenderPassState.sampleCount, "Color attachments must have the same sample count.");
        }
    }

    if ((mRenderPassState.hasDepthStencil = depthStencilAttachment != nullptr))
    {
        SEAD_ASSERT(depthStencilTexture != nullptr);

        mRenderPassState.depthStencilAttachment = *depthStencilAttachment;
        mRenderPassState.depthStencilFormat = wgpuTextureGetFormat(depthStencilTexture);

        if (colorAttachmentNum > 0)
        {
            SEAD_ASSERT_MSG(wgpuTextureGetSampleCount(depthStencilTexture) == mRenderPassState.sampleCount, "Depth attachment sample count must match the color attachments.");
        }
        else
        {
            mRenderPassState.sampleCount = wgpuTextureGetSampleCount(depthStencilTexture);
        }
    }
}

void DrawContext::endPass()
{
    if (mRenderPassEncoder == nullptr)
        return;

    wgpuRenderPassEncoderEnd(mRenderPassEncoder);
    wgpuRenderPassEncoderRelease(mRenderPassEncoder);
    mRenderPassEncoder = nullptr;
    //resetRenderPassState_(); //? allow state leakage like OpenGL
}

WGPURenderPassEncoder DrawContext::getRenderPass()
{
    commitRenderPass_();
    return mRenderPassEncoder;
}

void DrawContext::initializeCache(Heap* heap, s32 entries)
{
    sPipelineCache.initialize(heap, entries);
}

void DrawContext::setClear(u32 clrFlag, const Color4f* color, f32 depth, u32 stencil)
{
    if (clrFlag & FrameBuffer::eColor)
    {
        SEAD_ASSERT(mRenderPassState.colorAttachmentCount > 0);
        SEAD_ASSERT(color != nullptr);
        for (u32 i = 0; i < mRenderPassState.colorAttachmentCount; i++)
        {
            mRenderPassState.colorAttachments[i].loadOp = WGPULoadOp_Clear;
            mRenderPassState.colorAttachments[i].clearValue = { color->r, color->g, color->b, color->a };
        }
    }

    if (mRenderPassState.hasDepthStencil)
    {
        if (clrFlag & FrameBuffer::eDepth)
        {
            mRenderPassState.depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
            mRenderPassState.depthStencilAttachment.depthClearValue = depth;
        }
        if (clrFlag & FrameBuffer::eStencil)
        {
            mRenderPassState.depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
            mRenderPassState.depthStencilAttachment.stencilClearValue = stencil;
        }
    }

    if (mRenderPassEncoder != nullptr)
    {
        endPass();
    }

    commitRenderPass_();
}

void DrawContext::setGraphicsContext(
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
    bool blendEnable,
    Graphics::BlendFactor blendFactorSrcRGB,
    Graphics::BlendFactor blendFactorDstRGB,
    Graphics::BlendEquation blendEquationRGB,
    Graphics::BlendFactor blendFactorSrcA,
    Graphics::BlendFactor blendFactorDstA,
    Graphics::BlendEquation blendEquationA,
    bool colorWriteMaskR, bool colorWriteMaskG, bool colorWriteMaskB, bool colorWriteMaskA,
    const Color4f& blendConstant
)
{
    mPipelineState.cullMode = cullMode;
    mPipelineState.polygonMode = polygonMode;
    mPipelineState.depthTestEnable = depthTestEnable;
    mPipelineState.depthWriteEnable = depthWriteEnable;
    mPipelineState.depthFunc = depthFunc;
    mPipelineState.stencilTestEnable = stencilTestEnable;
    mPipelineState.stencilFunc = stencilFunc;
    mPipelineState.stencilTestMask = stencilTestMask;
    mRenderPassState.stencilReference = stencilTestReference;
    mRenderPassState.blendConstant = { blendConstant.r, blendConstant.g, blendConstant.b, blendConstant.a };
    mPipelineState.stencilFailOp = stencilFailOp;
    mPipelineState.stencilDepthFailOp = stencilDepthFailOp;
    mPipelineState.stencilPassOp = stencilPassOp;
    mPipelineState.blendEnable[0] = blendEnable;
    mPipelineState.blendColor[0].srcFactor = static_cast<WGPUBlendFactor>(blendFactorSrcRGB);
    mPipelineState.blendColor[0].dstFactor = static_cast<WGPUBlendFactor>(blendFactorDstRGB);
    mPipelineState.blendColor[0].operation = static_cast<WGPUBlendOperation>(blendEquationRGB);
    mPipelineState.blendAlpha[0].srcFactor = static_cast<WGPUBlendFactor>(blendFactorSrcA);
    mPipelineState.blendAlpha[0].dstFactor = static_cast<WGPUBlendFactor>(blendFactorDstA);
    mPipelineState.blendAlpha[0].operation = static_cast<WGPUBlendOperation>(blendEquationA);
    mPipelineState.colorWriteMask[0] = (colorWriteMaskR ? WGPUColorWriteMask_Red : 0) |
                                       (colorWriteMaskG ? WGPUColorWriteMask_Green : 0) |
                                       (colorWriteMaskB ? WGPUColorWriteMask_Blue : 0) |
                                       (colorWriteMaskA ? WGPUColorWriteMask_Alpha : 0);

    if (mRenderPassEncoder != nullptr)
    {
        wgpuRenderPassEncoderSetStencilReference(mRenderPassEncoder, mRenderPassState.stencilReference);
        wgpuRenderPassEncoderSetBlendConstant(mRenderPassEncoder, &mRenderPassState.blendConstant);
    }
}

void DrawContext::setViewport(f32 x, f32 y, f32 width, f32 height)
{
    mRenderPassState.viewport = { x, y, width, height };

    if (mRenderPassEncoder != nullptr)
    {
        wgpuRenderPassEncoderSetViewport(mRenderPassEncoder, mRenderPassState.viewport.x, mRenderPassState.viewport.y, mRenderPassState.viewport.width, mRenderPassState.viewport.height, 0.0f, 1.0f);
    }
}

void DrawContext::setScissor(f32 x, f32 y, f32 width, f32 height)
{
    mRenderPassState.scissor = { x, y, width, height };

    if (mRenderPassEncoder != nullptr)
    {
        wgpuRenderPassEncoderSetScissorRect(mRenderPassEncoder, mRenderPassState.scissor.x, mRenderPassState.scissor.y, mRenderPassState.scissor.width, mRenderPassState.scissor.height);
    }
}

void DrawContext::setShader(WGPUShaderModule vertexShader, WGPUShaderModule fragmentShader)
{
    mPipelineState.vs = vertexShader;
    mPipelineState.fs = fragmentShader;
}

void DrawContext::setBindGroup(const WGPUBindGroup* groups, u32 groupNum, WGPUPipelineLayout layout)
{
    SEAD_ASSERT(groupNum <= 4); // API restriction

    for (u32 i = 0; i < 4; i++)
    {
        if (i < groupNum)
            mBindingState.bindGroups[i] = groups[i];
        else
            mBindingState.bindGroups[i] = nullptr;
    }

    mBindingState.bindLayout = layout;
}

void DrawContext::commitRenderPass_()
{
    if (mRenderPassEncoder != nullptr)
        return;

    WGPURenderPassDescriptor desc = {};
    desc.colorAttachments = mRenderPassState.colorAttachments;
    desc.colorAttachmentCount = mRenderPassState.colorAttachmentCount;

    if (mRenderPassState.hasDepthStencil)
    {
        desc.depthStencilAttachment = &mRenderPassState.depthStencilAttachment;
    }
    mRenderPassEncoder = wgpuCommandEncoderBeginRenderPass(mEncoder, &desc);

    // apply deferred state we accumulated
    wgpuRenderPassEncoderSetScissorRect(mRenderPassEncoder, mRenderPassState.scissor.x, mRenderPassState.scissor.y, mRenderPassState.scissor.width, mRenderPassState.scissor.height);
    wgpuRenderPassEncoderSetViewport(mRenderPassEncoder, mRenderPassState.viewport.x, mRenderPassState.viewport.y, mRenderPassState.viewport.width, mRenderPassState.viewport.height, 0.0f, 1.0f);
    wgpuRenderPassEncoderSetStencilReference(mRenderPassEncoder, mRenderPassState.stencilReference);
    wgpuRenderPassEncoderSetBlendConstant(mRenderPassEncoder, &mRenderPassState.blendConstant);

    // consume the clear trigger
    for (u32 i = 0; i < mRenderPassState.colorAttachmentCount; i++)
    {
        mRenderPassState.colorAttachments[i].loadOp = WGPULoadOp_Load;
    }
    if (mRenderPassState.hasDepthStencil)
    {
        mRenderPassState.depthStencilAttachment.depthLoadOp = WGPULoadOp_Load;
        mRenderPassState.depthStencilAttachment.stencilLoadOp = WGPULoadOp_Load;
    }
}

void DrawContext::resetRenderPassState_()
{
    sead::MemUtil::fillZero(&mRenderPassState, sizeof(mRenderPassState));

    const Vector2f& surfaceSize = GraphicsWGPU::instance()->getSurfaceSize();

    mRenderPassState.viewport = mRenderPassState.scissor = {
        .x = 0, .y = 0,
        .width = surfaceSize.x, .height = surfaceSize.y
    };
}

void DrawContext::resetPipelineState_()
{
    sead::MemUtil::fillZero(&mPipelineState, sizeof(mPipelineState));
    mPipelineState.frontFace = WGPUFrontFace_CCW; //* GraphicsContext has no way to modify this, and the OpenGL default is CCW
    mPipelineState.vertexStepMode = WGPUVertexStepMode_Vertex; //* Same as above
}

void DrawContext::resetBindingState_()
{
    sead::MemUtil::fillZero(&mBindingState, sizeof(mBindingState));
}

WGPURenderPipeline DrawContext::commitRenderPipeline_()
{
    static const sead::SafeString cShaderEntry = "main";
    static const size_t cShaderEntryLength = cShaderEntry.calcLength();

    WGPURenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.layout = mBindingState.bindLayout;
    pipelineDesc.vertex = {
        .module = mPipelineState.vs,
        .entryPoint = { .data = cShaderEntry.cstr(), .length = cShaderEntryLength },
        
        .constantCount = 0,
        .constants = nullptr,

        // TODO
        .bufferCount = 0,
        .buffers = nullptr,
    };

    //* MRT is unsupported here
    WGPUBlendState blend = {
        .color = {
            .operation = mPipelineState.blendColor[0].operation,
            .srcFactor = mPipelineState.blendColor[0].srcFactor,
            .dstFactor = mPipelineState.blendColor[0].dstFactor
        },
        .alpha = {
            .operation = mPipelineState.blendAlpha[0].operation,
            .srcFactor = mPipelineState.blendAlpha[0].srcFactor,
            .dstFactor = mPipelineState.blendAlpha[0].dstFactor
        }
    };

    WGPUColorTargetState targets[8] = {};
    for (u32 i = 0; i < mRenderPassState.colorAttachmentCount; i++)
    {
        WGPUColorTargetState& target = targets[i];
        target.format = mRenderPassState.colorFormats[i];
        target.blend = mPipelineState.blendEnable[0] ? &blend : nullptr; //* MRT is unsupported here
        target.writeMask = mPipelineState.colorWriteMask[0]; //* MRT is unsupported here
    }

    WGPUFragmentState fragment = {
        .module = mPipelineState.fs,
        .entryPoint = { .data = cShaderEntry.cstr(), .length = cShaderEntryLength },

        .constantCount = 0,
        .constants = nullptr,

        .targetCount = mRenderPassState.colorAttachmentCount,
        .targets = targets
    };
    pipelineDesc.fragment = &fragment;

    pipelineDesc.primitive = {
        .topology = mPipelineState.topology,
        .stripIndexFormat = (mPipelineState.topology == WGPUPrimitiveTopology_LineStrip || mPipelineState.topology == WGPUPrimitiveTopology_TriangleStrip) ? WGPUIndexFormat_Uint16 : WGPUIndexFormat_Undefined , // TODO: dehardcode
        .frontFace = mPipelineState.frontFace,
        .cullMode = static_cast<WGPUCullMode>(mPipelineState.cullMode),
        .unclippedDepth = false
    };

    WGPUDepthStencilState depthStencil = {};
    pipelineDesc.depthStencil = nullptr;
    if (mRenderPassState.hasDepthStencil)
    {
        depthStencil.format = mRenderPassState.depthStencilFormat;
        depthStencil.depthWriteEnabled = static_cast<WGPUOptionalBool>(mPipelineState.depthWriteEnable);
        depthStencil.depthCompare = mPipelineState.depthTestEnable ? static_cast<WGPUCompareFunction>(mPipelineState.depthFunc) : WGPUCompareFunction_Always;

        WGPUStencilFaceState stencilFace = {
            .compare = mPipelineState.stencilTestEnable ? static_cast<WGPUCompareFunction>(mPipelineState.stencilFunc) : WGPUCompareFunction_Always,
            .failOp = static_cast<WGPUStencilOperation>(mPipelineState.stencilFailOp),
            .depthFailOp = static_cast<WGPUStencilOperation>(mPipelineState.stencilDepthFailOp),
            .passOp = static_cast<WGPUStencilOperation>(mPipelineState.stencilPassOp)
        };
        depthStencil.stencilFront = stencilFace;
        depthStencil.stencilBack = stencilFace;
        depthStencil.stencilReadMask = mPipelineState.stencilTestMask;
        depthStencil.stencilWriteMask = mPipelineState.stencilTestEnable ? mPipelineState.stencilTestMask : 0;
        
        depthStencil.depthBias = 0;
        depthStencil.depthBiasSlopeScale = 0.0f;
        depthStencil.depthBiasClamp = 0.0f;

        pipelineDesc.depthStencil = &depthStencil;
    }

    pipelineDesc.multisample = {
        .count = mRenderPassState.sampleCount,
        .mask = 0xFFFFFFFF,
        .alphaToCoverageEnabled = false
    };

    return sPipelineCache.getOrCreate(mDevice, pipelineDesc);
}

void DrawContext::drawInstanced(WGPUPrimitiveTopology mode, u32 first, u32 count, u32 instances)
{
    commitRenderPass_();

    mPipelineState.topology = mode;
    WGPURenderPipeline pipeline = commitRenderPipeline_();
    wgpuRenderPassEncoderSetPipeline(mRenderPassEncoder, pipeline);
    for (u32 i = 0; i < 4; i++)
    {
        if (mBindingState.bindGroups[i])
            wgpuRenderPassEncoderSetBindGroup(mRenderPassEncoder, i, mBindingState.bindGroups[i], 0, nullptr);
    }
    wgpuRenderPassEncoderDraw(mRenderPassEncoder, count, instances, first, 0);
}

void DrawContext::PipelineCache::initialize(Heap* heap, s32 entries)
{
    mList.allocBuffer(entries, heap);
    mMap.allocBuffer(entries, heap);
}

WGPURenderPipeline DrawContext::PipelineCache::getOrCreate(WGPUDevice device, const WGPURenderPipelineDescriptor& desc)
{
    // TODO: mutex here?

    u64 key;
    bool hashed = PipelineHashWGPU::calcHash(&key, desc);
    SEAD_ASSERT(hashed);

    if (Entry** entry = mMap.find(key))
    {
        mList.moveBefore(mList.front(), *entry); // promote to newest used
        return (*entry)->pipeline;
    }

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &desc);
    SEAD_ASSERT(pipeline != nullptr);

    if (mList.isFull()) // kick last entry
    {
        Entry* entry = mList.back();
        wgpuRenderPipelineRelease(entry->pipeline);
        mMap.erase(entry->key);
        mList.erase(entry);
    }

    mList.birthFront();
    *mList.back() = { pipeline, key };
    mMap.insert(key, mList.back());

    return pipeline;
}

} // namespace sead
