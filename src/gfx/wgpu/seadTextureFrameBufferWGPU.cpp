#include <gfx/wgpu/seadTextureFrameBufferWGPU.h>

#include <gfx/seadGraphicsContext.h>
#include <gfx/seadViewport.h>
#include <gfx/wgpu/seadTextureWGPU.h>
#include <gfx/seadDrawContext.h>
#include <gfx/seadTextureUtil.h>

namespace sead {

TextureFrameBufferWGPU::~TextureFrameBufferWGPU()
{
}

void TextureFrameBufferWGPU::setup(TextureWGPU* colorTexture, TextureWGPU* depthTexture)
{
    if (mColorTexture || mDepthTexture)
    {
        SEAD_ASSERT_MSG(false, "setup twice.");
        return;
    }

    SEAD_ASSERT(colorTexture || depthTexture);

    mColorTexture = colorTexture;
    mDepthTexture = depthTexture;
}

void TextureFrameBufferWGPU::copyToDisplayBuffer(DrawContext* drawContext, const DisplayBuffer* displayBuffer) const
{
    const DisplayBufferWGPU* displayBufferWGPU = DynamicCast<const DisplayBufferWGPU>(displayBuffer);
    SEAD_ASSERT(displayBufferWGPU);

    drawContext->endPass();

    WGPUTexelCopyTextureInfo source = {};
    source.texture = mColorTexture->getID();
    source.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = displayBufferWGPU->getTexture()->getID();
    destination.aspect = WGPUTextureAspect_All;

    WGPUExtent3D copySize = {
        .width = static_cast<u32>(displayBufferWGPU->getWidth()),
        .height = static_cast<u32>(displayBufferWGPU->getHeight()),
        .depthOrArrayLayers = 1
    };

    wgpuCommandEncoderCopyTextureToTexture(drawContext->getEncoder(), &source, &destination, &copySize);

    displayBufferWGPU->copyToDisplay(drawContext);
}

void TextureFrameBufferWGPU::bindImpl_(DrawContext* drawContext) const
{
    WGPURenderPassColorAttachment color = {};
    if (mColorTexture != nullptr)
    {
        color.view = mColorTexture->getView();
        color.storeOp = WGPUStoreOp_Store;
        color.loadOp = WGPULoadOp_Load;
    }

    WGPURenderPassDepthStencilAttachment depth = {};
    if (mDepthTexture != nullptr)
    {
        depth.view = mDepthTexture->getView();
        depth.depthStoreOp = WGPUStoreOp_Store;
        depth.stencilStoreOp = WGPUStoreOp_Store;
        depth.stencilLoadOp = WGPULoadOp_Load;
        depth.depthLoadOp = WGPULoadOp_Load;
    }

    drawContext->beginPass(
        mColorTexture != nullptr ? &color : nullptr,
        mColorTexture != nullptr ? &mColorTexture->getID() : nullptr,
        mColorTexture != nullptr ? 1 : 0,
        mDepthTexture != nullptr ? &depth : nullptr,
        mDepthTexture != nullptr ? mDepthTexture->getID() : nullptr
    );
}

void TextureFrameBufferWGPU::clear(DrawContext* drawContext, u32 clrFlag, const Color4f& color, f32 depth, u32 stencil) const
{
    drawContext->setClear(clrFlag, &color, depth, stencil);
}

DisplayBufferWGPU::DisplayBufferWGPU()
    : DisplayBuffer()
    , mTextureWGPU(nullptr)
    , mFragShader(nullptr)
    , mVertShader(nullptr)
    , mGammaBuffer(nullptr)
    , mBindGroup(nullptr)
    , mPipelineLayout(nullptr)
    , mSampler(nullptr)
    , mGamma(1.0f)
{
}

DisplayBufferWGPU::~DisplayBufferWGPU()
{
    // TODO
}

void DisplayBufferWGPU::reconfigure(f32 width, f32 height)
{
    mWidth = width;
    mHeight = height;

    TextureUtil::createTextureImage2DWGPU(mTextureWGPU, mTextureWGPU->getMipMapLevel(), mTextureWGPU->getFormat(), width, height, mTextureWGPU->getUsage(), mTextureWGPU->getSamples());
}

void DisplayBufferWGPU::initializeImpl_(Heap* heap)
{
    mTextureWGPU = new(heap) TextureWGPU();
    s32 width = static_cast<s32>(mWidth);
    s32 height = static_cast<s32>(mHeight);

    bool b = TextureUtil::createTextureImage2DWGPU(mTextureWGPU, 1, WGPUTextureFormat_RGBA8Unorm, width, height, WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst);
    SEAD_ASSERT(b);

    sead::SafeString vertShaderCode =
        "struct VSOutput\n"
        "{\n"
        "    @builtin(position) position: vec4<f32>,\n"
        "    @location(0) vFragCoord: vec2<f32>,\n"
        "};\n"
        "\n"
        "@vertex\n"
        "fn main(\n"
        "    @builtin(vertex_index) vertexID: u32\n"
        ") -> VSOutput\n"
        "{\n"
        "    var output: VSOutput;\n"
        "    let uv = vec2<f32>(\n"
        "        f32((vertexID << 1u) & 2u),\n"
        "        f32(vertexID & 2u)\n"
        "    );\n"
        "    output.vFragCoord = vec2<f32>(uv.x, 1.0 - uv.y); // flip to match coordinate systems\n"
        "    output.position = vec4<f32>(\n"
        "        uv * 2.0 - vec2<f32>(1.0),\n"
        "        0.0,\n"
        "        1.0\n"
        "    );\n"
        "    return output;\n"
        "}\n";

    mVertShader = GraphicsWGPU::instance()->createShaderModule(vertShaderCode);

    const char* fragShaderCode =
        "struct FSInput\n"
        "{\n"
        "    @location(0) vFragCoord: vec2<f32>,\n"
        "};\n"
        "\n"
        "@group(0) @binding(0)\n"
        "var uTexture: texture_2d<f32>;\n"
        "@group(0) @binding(1)\n"
        "var uSampler: sampler;\n"
        "@group(0) @binding(2)\n"
        "var<uniform> uGamma: f32;\n"
        "\n"
        "@fragment\n"
        "fn main(\n"
        "    input: FSInput\n"
        ") -> @location(0) vec4<f32>\n"
        "{\n"
        "    let color = textureSample(\n"
        "        uTexture,\n"
        "        uSampler,\n"
        "        input.vFragCoord\n"
        "    );\n"
        "    let gamma = uGamma;\n"
        "    return pow(\n"
        "        color,\n"
        "        vec4<f32>(gamma, gamma, gamma, 1.0)\n"
        "    );\n"
        "}\n";

    mFragShader = GraphicsWGPU::instance()->createShaderModule(fragShaderCode);

    WGPUSamplerDescriptor samplerDesc = {};
    samplerDesc.addressModeU = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeV = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeW = WGPUAddressMode_ClampToEdge;
    samplerDesc.magFilter = WGPUFilterMode_Linear;
    samplerDesc.minFilter = WGPUFilterMode_Linear;
    samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Nearest;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 0.0f;
    samplerDesc.maxAnisotropy = 1;

    mSampler = wgpuDeviceCreateSampler(sead::GraphicsWGPU::instance()->getDevice(), &samplerDesc);

    WGPUBufferDescriptor gammaDesc = {};
    gammaDesc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    gammaDesc.size = sizeof(f32);

    mGammaBuffer = wgpuDeviceCreateBuffer(GraphicsWGPU::instance()->getDevice(), &gammaDesc);

    WGPUBindGroupLayoutEntry layoutEntries[3] = {};
    // uTexture
    layoutEntries[0].binding = 0;
    layoutEntries[0].visibility = WGPUShaderStage_Fragment;
    layoutEntries[0].texture.sampleType = WGPUTextureSampleType_Float;
    layoutEntries[0].texture.viewDimension = WGPUTextureViewDimension_2D;
    // uSampler
    layoutEntries[1].binding = 1;
    layoutEntries[1].visibility = WGPUShaderStage_Fragment;
    layoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;
    // uGamma
    layoutEntries[2].binding = 2;
    layoutEntries[2].visibility = WGPUShaderStage_Fragment;
    layoutEntries[2].buffer.type = WGPUBufferBindingType_Uniform;
    layoutEntries[2].buffer.minBindingSize = sizeof(f32);

    WGPUBindGroupLayoutDescriptor layoutDesc = {
        .entryCount = 3,
        .entries = layoutEntries
    };
    WGPUBindGroupLayout layout = wgpuDeviceCreateBindGroupLayout(GraphicsWGPU::instance()->getDevice(), &layoutDesc);

    WGPUBindGroupEntry entries[3] = {};
    // uTexture
    entries[0].binding = 0;
    entries[0].textureView = mTextureWGPU->getView();
    // uSampler
    entries[1].binding = 1;
    entries[1].sampler = mSampler;
    // uGamma
    entries[2].binding = 2;
    entries[2].buffer = mGammaBuffer;
    entries[2].offset = 0;
    entries[2].size = sizeof(f32);

    WGPUBindGroupDescriptor bindDesc = {
        .layout = layout,
        .entryCount = 3,
        .entries = entries
    };
    mBindGroup = wgpuDeviceCreateBindGroup(GraphicsWGPU::instance()->getDevice(), &bindDesc);

    WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = &layout
    };
    mPipelineLayout = wgpuDeviceCreatePipelineLayout(GraphicsWGPU::instance()->getDevice(), &pipelineLayoutDesc);
}

void DisplayBufferWGPU::copyToDisplay(sead::DrawContext* drawContext) const
{
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(GraphicsWGPU::instance()->getSurface(), &surfaceTexture);

    // TODO: idk about this
    if ((surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal && surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
        || !surfaceTexture.texture
    )
    {
        if (surfaceTexture.texture != nullptr)
            wgpuTextureRelease(surfaceTexture.texture);
        SEAD_ASSERT(false);
    }

    WGPUTextureView surfaceView = wgpuTextureCreateView(surfaceTexture.texture, nullptr);
    SEAD_ASSERT(surfaceView != nullptr);

    WGPURenderPassColorAttachment color = {};
    color.view = surfaceView;
    color.storeOp = WGPUStoreOp_Store;
    color.loadOp = WGPULoadOp_Clear;
    drawContext->beginPass(&color, &surfaceTexture.texture, 1);

    GraphicsContext context;
    context.setDepthEnable(false, false);
    context.setBlendEnable(false);
    context.setColorMask(true, true, true, false);
    context.apply(drawContext);

    LogicalFrameBuffer fb(Vector2f(mWidth, mHeight), BoundBox2f(0, 0, mWidth, mHeight));
    Viewport vp(0, 0, mWidth, mHeight);
    vp.apply(drawContext, fb);

    // TODO: reset buffers to 0?

    drawContext->setShader(mVertShader, mFragShader);
    wgpuQueueWriteBuffer(GraphicsWGPU::instance()->getQueue(), mGammaBuffer, 0, &mGamma, sizeof(mGamma));
    drawContext->setBindGroup(&mBindGroup, 1, mPipelineLayout);

    drawContext->draw(WGPUPrimitiveTopology_TriangleList, 0, 3);
}

} // namespace sead
