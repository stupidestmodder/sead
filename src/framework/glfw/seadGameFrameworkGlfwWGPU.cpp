#include <framework/glfw/seadGameFrameworkGlfwWGPU.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <gfx/seadTextureUtil.h>
#include <gfx/seadDrawContext.h>
#include <gfx/wgpu/seadTextureFrameBufferWGPU.h>
#include <gfx/wgpu/seadTextureWGPU.h>
#include <framework/seadProcessMeter.h>
#include <gfx/wgpu/seadGraphicsWGPU.h>

namespace sead {

GameFrameworkGlfwWGPU::GameFrameworkGlfwWGPU(const CreateArg& arg)
    : GameFrameworkBaseGlfw({
        .width = arg.width,
        .height = arg.height,
        .wait_vblank = arg.wait_vblank,
        .fps = arg.fps,
        .window_name = arg.window_name,
        .clear_color = arg.clear_color,
        .create_default_framebuffer = arg.create_default_framebuffer
    })
    , mColorTexture(nullptr)
    , mDepthTexture(nullptr)
    , mDisplayBufferWGPU(nullptr)
{
    mPipelineCacheSize = arg.pipeline_cache_size;
    mProcDrawCallback = nullptr;
}

GameFrameworkGlfwWGPU::~GameFrameworkGlfwWGPU()
{
    // TODO: shutdown?
}

void GameFrameworkGlfwWGPU::initializeGraphicsSystem(Heap* heap, const Vector2f& virtualFbSize)
{
    createWindow_();

    GraphicsWGPU::CreateArg arg = { .window = mWindow };
    Graphics::setInstance(new(heap) GraphicsWGPU(arg));
    Graphics::instance()->initialize(heap);

    DrawContext::initializeCache(heap, mPipelineCacheSize);

    resize_(virtualFbSize.x, virtualFbSize.y);
    createFrameBuffer_(heap, virtualFbSize);
}

void GameFrameworkGlfwWGPU::procFrame_()
{
    ProcessMeter* proc = ProcessMeter::instance();
    if (proc)
        proc->measureBeginFrame();

    //* Frame process
    {
        TickTime frameBegin;

        procDraw_();
        procCalc_();
        procReset_();

        //* Update GPUMeter
        {
            // TODO
        }
    }

    if (proc)
        proc->measureEndFrame();

    if (getDisplayState() == DisplayState::eReady)
    {
        glfwShowWindow(mWindow);

        mDisplayState = DisplayState::eShow;
    }
}

void GameFrameworkGlfwWGPU::procDraw_()
{
    DrawContext drawContext;
    drawContext.begin();

    if (mProcDrawCallback)
        mProcDrawCallback(true);

    mDrawMeter.measureBegin();
    {
        if (mDefaultFrameBuffer)
        {
            mDefaultFrameBuffer->bind(&drawContext);
            Viewport vp(*mDefaultFrameBuffer);
            vp.applyScissor(&drawContext, *mDefaultFrameBuffer);
            mDefaultFrameBuffer->clear(&drawContext, FrameBuffer::ClearFlag::eAll, mArg.clear_color, 1.0f, 0);
        }
        
        DynamicCast<SingleScreenMethodTreeMgr>(getMethodTreeMgr())->draw();
    }
    mDrawMeter.measureEnd();

    if (mProcDrawCallback)
        mProcDrawCallback(false);

    present_(&drawContext);
}

void GameFrameworkGlfwWGPU::present_(DrawContext* drawContext)
{
    if (mDefaultFrameBuffer)
    {
        mDefaultFrameBuffer->copyToDisplayBuffer(drawContext, mDisplayBufferWGPU);
    }

    drawContext->submit();

    wgpuSurfacePresent(GraphicsWGPU::instance()->getSurface());
    wgpuDevicePoll(GraphicsWGPU::instance()->getDevice(), false, nullptr);
}

void GameFrameworkGlfwWGPU::createFrameBuffer_(Heap* heap, const Vector2f& virtualFbSize)
{
    s32 width = mArg.width;
    s32 height = mArg.height;
    glfwGetFramebufferSize(mWindow, &width, &height);

    if (mArg.create_default_framebuffer)
    {
        TextureFrameBufferWGPU* fb = new(heap) TextureFrameBufferWGPU(virtualFbSize, 0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height));
        
        mColorTexture = new(heap) TextureWGPU();
        mDepthTexture = new(heap) TextureWGPU();

        TextureUtil::createTextureImage2DWGPU(mColorTexture, 1, WGPUTextureFormat_RGBA8Unorm, width, height, WGPUTextureUsage_CopySrc | WGPUTextureUsage_CopyDst | WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding);
        TextureUtil::createTextureImage2DWGPU(mDepthTexture, 1, WGPUTextureFormat_Depth24PlusStencil8, width, height, WGPUTextureUsage_CopySrc | WGPUTextureUsage_CopyDst | WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding);

        fb->setup(mColorTexture, mDepthTexture);

        mDefaultFrameBuffer = fb;
    }

    mDefaultLogicalFrameBuffer.setVirtualSize(virtualFbSize);
    mDefaultLogicalFrameBuffer.setPhysicalArea(0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height));

    mDisplayBufferWGPU = new(heap) DisplayBufferWGPU();
    mDisplayBufferWGPU->initialize(static_cast<f32>(width), static_cast<f32>(height), heap);

    if (!setWaitVBlankInterval(0))
        mArg.wait_vblank = 0;
}

void GameFrameworkGlfwWGPU::resize_(f32 width, f32 height)
{
    if (mSize.x == width && mSize.y == height)
        return;

    mSize = Vector2f(width, height);

    Graphics::instance()->lockDrawContext();
    {
        wgpuDevicePoll(GraphicsWGPU::instance()->getDevice(), true, nullptr);

        GraphicsWGPU::instance()->resizeSurface(width, height);

        if (mColorTexture != nullptr)
            TextureUtil::createTextureImage2DWGPU(mColorTexture, mColorTexture->getMipMapLevel(), mColorTexture->getFormat(), width, height, mColorTexture->getUsage(), mColorTexture->getSamples());
        if (mDepthTexture != nullptr)
            TextureUtil::createTextureImage2DWGPU(mDepthTexture, mDepthTexture->getMipMapLevel(), mDepthTexture->getFormat(), width, height, mDepthTexture->getUsage(), mColorTexture->getSamples());

        if (mDisplayBufferWGPU != nullptr)
            mDisplayBufferWGPU->reconfigure(width, height);
    }
    Graphics::instance()->unlockDrawContext();
}

} // namespace sead
