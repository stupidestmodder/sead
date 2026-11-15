#include <gfx/wgpu/seadPrimitiveDrawMgrWGPU.h>

#include <gfx/seadPrimitiveDrawUtil.h>
#include <gfx/wgpu/seadTextureWGPU.h>
#include <gfx/seadDrawContext.h>
#include <gfx/wgpu/seadGraphicsWGPU.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(PrimitiveDrawMgrWGPU);

PrimitiveDrawMgrWGPU::PrimitiveDrawMgrWGPU()
    : PrimitiveDrawMgrBase()
    , mVS(nullptr)
    , mFS(nullptr)
    , mSampler(nullptr)
    , mUniforms(nullptr)
    , mDrawCalls(0)
    , mPipelineLayout(nullptr)
    , mBindGroup(nullptr)
    , mPrevView(nullptr)
    , mGroupLayout(nullptr)
    , mQuadVtx(nullptr)
    , mQuadIdx(nullptr)
{
}

PrimitiveDrawMgrWGPU::~PrimitiveDrawMgrWGPU()
{
    // TODO
}

void PrimitiveDrawMgrWGPU::prepareFromBinaryImpl(Heap*, const void*, u32)
{
    const sead::SafeString vsShader =
        "struct VSInput\n"
        "{\n"
        "    @location(0) aVertex: vec3<f32>,\n"
        "    @location(1) aTexCoord0: vec2<f32>,\n"
        "    @location(2) aColorRate: vec4<f32>\n"
        "};\n"
        "struct VSOutput\n"
        "{\n"
        "    @builtin(position) position: vec4<f32>,\n"
        "    @location(0) vColor: vec4<f32>,\n"
        "    @location(1) vTexCoord: vec2<f32>\n"
        "};\n"
        "struct Uniforms {\n"
        "    uWVP: mat4x4<f32>,\n"
        "    uUser: mat4x4<f32>,\n"
        "    uColor0: vec4<f32>,\n"
        "    uColor1: vec4<f32>,\n"
        "    uUVSrc: vec2<f32>,\n"
        "    uUVSize: vec2<f32>\n"
        "};\n"
        "@group(0) @binding(0)\n"
        "var<uniform> uniforms: Uniforms;\n"
        "@vertex\n"
        "fn main(\n"
        "    input: VSInput\n"
        ") -> VSOutput\n"
        "{\n"
        "    var output: VSOutput;\n"
        "    output.position = vec4<f32>(input.aVertex, 1.0) * uniforms.uUser * uniforms.uWVP;\n"
        "    output.vColor = uniforms.uColor0 * (1.0 - input.aColorRate.r) + uniforms.uColor1 * input.aColorRate.r;\n"
        "    output.vTexCoord = input.aTexCoord0 * uniforms.uUVSize + uniforms.uUVSrc;\n"
        "    return output;\n"
        "}\n";

    mVS = GraphicsWGPU::instance()->createShaderModule(vsShader);
    SEAD_ASSERT(mVS != nullptr);

    const sead::SafeString fsShader =
        "struct FSInput\n"
        "{\n"
        "    @location(0) vColor: vec4<f32>,\n"
        "    @location(1) vTexCoord: vec2<f32>\n"
        "};\n"
        "struct Uniforms {\n"
        "    uWVP: mat4x4<f32>,\n"
        "    uUser: mat4x4<f32>,\n"
        "    uColor0: vec4<f32>,\n"
        "    uColor1: vec4<f32>,\n"
        "    uUVSrc: vec2<f32>,\n"
        "    uUVSize: vec2<f32>,\n"
        "    uRate: f32\n"
        "};\n"
        "@group(0) @binding(1)\n"
        "var uTexture0: texture_2d<f32>;\n"
        "@group(0) @binding(2)\n"
        "var uSampler0: sampler;\n"
        "@group(0) @binding(0)\n"
        "var<uniform> uniforms: Uniforms;\n"
        "@fragment\n"
        "fn main(\n"
        "    input: FSInput\n"
        ") -> @location(0) vec4<f32>\n"
        "{\n"
        "    let color0 = textureSample(\n"
        "        uTexture0,\n"
        "        uSampler0,\n"
        "        input.vTexCoord\n"
        "    );\n"
        "    return input.vColor * mix(\n"
        "        vec4<f32>(1.0),\n"
        "        color0,\n"
        "        uniforms.uRate\n"
        "    );\n"
        "}\n";

    mFS = GraphicsWGPU::instance()->createShaderModule(fsShader);
    SEAD_ASSERT(mFS != nullptr);

    WGPUSamplerDescriptor samplerDesc = {};
    samplerDesc.addressModeU = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeV = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeW = WGPUAddressMode_ClampToEdge;
    samplerDesc.magFilter = WGPUFilterMode_Nearest;
    samplerDesc.minFilter = WGPUFilterMode_Nearest;
    samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Nearest;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 0.0f;
    samplerDesc.maxAnisotropy = 1;

    mSampler = wgpuDeviceCreateSampler(GraphicsWGPU::instance()->getDevice(), &samplerDesc);

    WGPUBufferDescriptor bufferDesc = {};
    bufferDesc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    bufferDesc.size = sizeof(Uniforms) * cFrameDrawLimit;

    mUniforms = wgpuDeviceCreateBuffer(GraphicsWGPU::instance()->getDevice(), &bufferDesc);

    WGPUBindGroupLayoutEntry layoutEntries[3] = {};
    // uniforms
    layoutEntries[0].binding = 0;
    layoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    layoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
    layoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);
    layoutEntries[0].buffer.hasDynamicOffset = true;
    // uTexture0
    layoutEntries[1].binding = 1;
    layoutEntries[1].visibility = WGPUShaderStage_Fragment;
    layoutEntries[1].texture.sampleType = WGPUTextureSampleType_Float;
    layoutEntries[1].texture.viewDimension = WGPUTextureViewDimension_2D;
    // uSampler0
    layoutEntries[2].binding = 2;
    layoutEntries[2].visibility = WGPUShaderStage_Fragment;
    layoutEntries[2].sampler.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutDescriptor layoutDesc = {
        .entryCount = 3,
        .entries = layoutEntries
    };
    mGroupLayout = wgpuDeviceCreateBindGroupLayout(GraphicsWGPU::instance()->getDevice(), &layoutDesc);

    WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = &mGroupLayout
    };
    mPipelineLayout = wgpuDeviceCreatePipelineLayout(GraphicsWGPU::instance()->getDevice(), &pipelineLayoutDesc);

    mAttributes[0] = {
        .format = WGPUVertexFormat_Float32x3,
        .offset = 0,
        .shaderLocation = 0
    };
    mAttributes[1] = {
        .format = WGPUVertexFormat_Float32x2,
        .offset = sizeof(Vector3f),
        .shaderLocation = 1
    };
    mAttributes[2] = {
        .format = WGPUVertexFormat_Float32x4,
        .offset = sizeof(Vector3f) + sizeof(Vector2f),
        .shaderLocation = 2
    };

    mVertexLayout = {
        .stepMode = WGPUVertexStepMode_Vertex,
        .arrayStride = sizeof(Vector3f) + sizeof(Vector2f) + sizeof(Color4f),
        .attributeCount = 3,
        .attributes = mAttributes
    };

    PrimitiveDrawUtil::Vertex vtx[4];
    u16 idx[3 * 2];
    PrimitiveDrawUtil::setQuadVertex(vtx, idx);
    createBuffer_(&mQuadVtx, &mQuadIdx, vtx, sizeof(vtx), idx, sizeof(idx));
}

void PrimitiveDrawMgrWGPU::prepareImpl(Heap* heap, const SafeString&)
{
    prepareFromBinaryImpl(heap, nullptr, 0);
}

void PrimitiveDrawMgrWGPU::beginImpl(DrawContext* drawContext, const Matrix34f& viewMatrix, const Matrix44f& projectionMatrix)
{
    Matrix44f wvp;
    wvp.setMul(projectionMatrix, viewMatrix);

    mUniformData.wvp = wvp;
    mUniformData.user = Matrix44f::ident;

    drawContext->setShader(mVS, mFS);
}

void PrimitiveDrawMgrWGPU::endImpl(DrawContext* drawContext)
{
    drawContext->setVertexBuffer(nullptr, nullptr, nullptr, 0, nullptr);
    drawContext->setIndexBuffer(nullptr, WGPUIndexFormat_Undefined, 0, 0);
}

void PrimitiveDrawMgrWGPU::drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR)
{
}

void PrimitiveDrawMgrWGPU::drawQuadImpl(
    DrawContext* drawContext, const Matrix34f& modelMtx, const Texture& texture,
    const Color4f& colorL, const Color4f& colorR,
    const Vector2f& uvSrc, const Vector2f& uvSize)
{
    mUniformData.color0 = colorL;
    mUniformData.color1 = colorR;
    mUniformData.user = Matrix44f(modelMtx);
    mUniformData.rate = 1.0f;
    mUniformData.uvSrc = uvSrc;
    mUniformData.uvSize = uvSize;

    wgpuQueueWriteBuffer(GraphicsWGPU::instance()->getQueue(), mUniforms, mDrawCalls * sizeof(Uniforms), &mUniformData, sizeof(Uniforms));

    updateBindings_(&texture);

    u32 bindOffs[1] = { static_cast<u32>(mDrawCalls * sizeof(Uniforms)) };
    u32* bindOffsAll[] = { bindOffs };

    u32 bindOffsNum = 1;
    drawContext->setBindGroup(&mBindGroup, 1, bindOffsAll, &bindOffsNum, mPipelineLayout);

    u64 bufferOfs = 0;
    u64 bufferSize = sizeof(PrimitiveDrawUtil::Vertex) * 4;
    drawContext->setVertexBuffer(&mQuadVtx, &bufferOfs, &bufferSize, 1, &mVertexLayout);

    drawContext->setIndexBuffer(mQuadIdx, WGPUIndexFormat_Uint16, 0, sizeof(u16) * 3 * 2);

    drawContext->drawIndexed(WGPUPrimitiveTopology_TriangleList, 0, 6);

    mDrawCalls++;
    if (mDrawCalls >= cFrameDrawLimit)
    {
        // ring buffer
        mDrawCalls = 0;
    }
}

void PrimitiveDrawMgrWGPU::drawBoxImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR)
{
}

void PrimitiveDrawMgrWGPU::drawLineImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& c0, const Color4f& c1)
{
}

void PrimitiveDrawMgrWGPU::drawSphere4x8Impl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& north, const Color4f& south)
{
}

void PrimitiveDrawMgrWGPU::drawSphere8x16Impl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& north, const Color4f& south)
{
}

void PrimitiveDrawMgrWGPU::updateBindings_(const Texture* texture)
{
    const TextureWGPU* textureWGPU = DynamicCast<const TextureWGPU>(texture);
    if (!textureWGPU)
    {
        SEAD_ASSERT_MSG(false, "texture must be TextureWGPU.");
        return;
    }

    if (mPrevView != textureWGPU->getView())
    {
        if (mPrevView)
            wgpuTextureViewRelease(mPrevView);

        wgpuTextureViewAddRef(textureWGPU->getView());
        mPrevView = textureWGPU->getView();

        if (mBindGroup)
            wgpuBindGroupRelease(mBindGroup);
        if (mPipelineLayout)
            wgpuPipelineLayoutRelease(mPipelineLayout);
    
        WGPUBindGroupEntry entries[3] = {};
        // uniforms
        entries[0].binding = 0;
        entries[0].buffer = mUniforms;
        entries[0].offset = 0;
        entries[0].size = sizeof(Uniforms);
        // uTexture0
        entries[1].binding = 1;
        entries[1].textureView = textureWGPU->getView();
        // uSampler0
        entries[2].binding = 2;
        entries[2].sampler = mSampler;

        WGPUBindGroupDescriptor bindDesc = {
            .layout = mGroupLayout,
            .entryCount = 3,
            .entries = entries
        };
        mBindGroup = wgpuDeviceCreateBindGroup(GraphicsWGPU::instance()->getDevice(), &bindDesc);
    
        WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {
            .bindGroupLayoutCount = 1,
            .bindGroupLayouts = &mGroupLayout
        };
        mPipelineLayout = wgpuDeviceCreatePipelineLayout(GraphicsWGPU::instance()->getDevice(), &pipelineLayoutDesc);
    }
}

void PrimitiveDrawMgrWGPU::createBuffer_(WGPUBuffer* vertex, WGPUBuffer* index, PrimitiveDrawUtil::Vertex* vertices, size_t verticesSize, u16* indices, size_t indicesSize)
{
    WGPUBufferDescriptor vbDesc = {
        .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
        .size = verticesSize
    };

    WGPUBufferDescriptor ibDesc = {
        .usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
        .size = indicesSize
    };

    *vertex = wgpuDeviceCreateBuffer(GraphicsWGPU::instance()->getDevice(), &vbDesc);
    wgpuQueueWriteBuffer(GraphicsWGPU::instance()->getQueue(), *vertex, 0, vertices, verticesSize);

    *index = wgpuDeviceCreateBuffer(GraphicsWGPU::instance()->getDevice(), &ibDesc);
    wgpuQueueWriteBuffer(GraphicsWGPU::instance()->getQueue(), *index, 0, indices, indicesSize);
}

} // namespace sead
