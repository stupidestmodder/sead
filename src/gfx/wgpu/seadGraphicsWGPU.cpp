#include <gfx/wgpu/seadGraphicsWGPU.h>

#include <basis/seadRawPrint.h>
#include <gfx/wgpu/seadPipelineHashWGPU.h>
#include <heap/seadHeapMgr.h>

#include <GLFW/glfw3.h>
#if defined(SEAD_PLATFORM_LINUX)
    #define GLFW_EXPOSE_NATIVE_X11
    #define GLFW_EXPOSE_NATIVE_WAYLAND
#elif defined(SEAD_PLATFORM_MACOSX)
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif // TODO: windows?
#include <GLFW/glfw3native.h>

// TODO: I'm skipping all the emscripten stuff here for now. Technically, "WGPU" is not even the right target because "WGPU" is for native webgpu, while "WebGPU" is everything including browsers
//* Come back to it later.

namespace sead {

GraphicsWGPU::GraphicsWGPU(const CreateArg& arg)
    : Graphics()
    , mWindow(arg.window)
    , mInstance(nullptr)
    , mSurface(nullptr)
    , mAdapter(nullptr)
    , mDevice(nullptr)
    , mQueue(nullptr)
    , mSurfaceConfig({})
    , mMailboxSupported(false)
    , mDefaultCommandBuffer(nullptr)
    , mPipelineCacheSize(arg.pipeline_cache_size)
{ }

GraphicsWGPU::~GraphicsWGPU()
{
    for (auto pipeline : mPipelineCache.list)
    {
        wgpuRenderPipelineRelease(pipeline.pipeline);
    }

    if (mSurface != nullptr)
    {
        wgpuSurfaceUnconfigure(mSurface);
        wgpuSurfaceRelease(mSurface);
        mSurface = nullptr;
    }

    if (mQueue != nullptr)
    {
        wgpuQueueRelease(mQueue);
        mQueue = nullptr;
    }

    if (mDevice != nullptr)
    {
        wgpuDeviceRelease(mDevice);
        mDevice = nullptr;
    }

    if (mAdapter != nullptr)
    {
        wgpuAdapterRelease(mAdapter);
        mAdapter = nullptr;
    }

    if (mInstance != nullptr)
    {
        wgpuInstanceRelease(mInstance);
        mInstance = nullptr;
    }
}

void GraphicsWGPU::initializeImpl(Heap* heap)
{
    CurrentHeapSetter chs(heap);

    mPipelineCache.list.allocBuffer(mPipelineCacheSize, heap);
    mPipelineCache.map.allocBuffer(mPipelineCacheSize, heap);

    //* WebGPU depth range is [0, 1] while sead uses the OpenGL convention ([-1, 1]) so remap Z in the device
    sDefaultDeviceZScale = 0.5f;
    sDefaultDeviceZOffset = 0.5f;

    WGPUInstanceDescriptor instanceDescriptor = {};

    WGPUInstanceExtras instanceExtras = {};
    instanceExtras.chain.sType = static_cast<WGPUSType>(WGPUSType_InstanceExtras);
#if defined(SEAD_PLATFORM_LINUX)
    instanceExtras.backends = WGPUInstanceBackend_Vulkan;
#elif defined(SEAD_PLATFORM_MACOSX)
    instanceExtras.backends = WGPUInstanceBackend_Metal;
#elif defined(SEAD_PLATFORM_WINDOWS)
    instanceExtras.backends = WGPUInstanceBackend_Vulkan;
#endif

    instanceDescriptor.nextInChain = &instanceExtras.chain;

    mInstance = wgpuCreateInstance(&instanceDescriptor);
    SEAD_ASSERT(mInstance);

    WGPUSurfaceDescriptor surfaceDescriptor = {};

#if defined(SEAD_PLATFORM_LINUX)
    GLFWwindow* window = static_cast<GLFWwindow*>(mWindow);
    
    WGPUSurfaceSourceXlibWindow xlibSource = {};
    WGPUSurfaceSourceWaylandSurface waylandSource = {};

    switch (glfwGetPlatform())
    {
    case GLFW_PLATFORM_X11:
    {
        Display* display = glfwGetX11Display();
        Window x11Window = glfwGetX11Window(window);

        SEAD_ASSERT(display && x11Window);

        xlibSource.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
        xlibSource.display = display;
        xlibSource.window = x11Window;

        surfaceDescriptor.nextInChain = &xlibSource.chain;
        break;
    }

    case GLFW_PLATFORM_WAYLAND:
    {
        wl_display* display = glfwGetWaylandDisplay();
        wl_surface* wlSurface = glfwGetWaylandWindow(window);

        SEAD_ASSERT(display && wlSurface);

        waylandSource.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
        waylandSource.display = display;
        waylandSource.surface = wlSurface;

        surfaceDescriptor.nextInChain = &waylandSource.chain;
        break;
    }

    default:
        SEAD_ASSERT_MSG(false, "unknown windowing system.");
    }

#endif // TODO: support mac and windows

    mSurface = wgpuInstanceCreateSurface(mInstance, &surfaceDescriptor);
    SEAD_ASSERT(mSurface != nullptr);

    WGPURequestAdapterOptions adapterOptions = {};
    adapterOptions.powerPreference = WGPUPowerPreference_HighPerformance;
    adapterOptions.compatibleSurface = mSurface;
#if defined(SEAD_PLATFORM_LINUX)
    adapterOptions.backendType = WGPUBackendType_Vulkan;
#elif defined(SEAD_PLATFORM_MACOSX)
    adapterOptions.backendType = WGPUBackendType_Metal;
#elif defined(SEAD_PLATFORM_WINDOWS)
    adapterOptions.backendType = WGPUBackendType_Vulkan;
#endif

    WGPUAdapter adapter = nullptr;
    WGPURequestAdapterCallbackInfo adapterCallback = {};
    adapterCallback.mode = WGPUCallbackMode_AllowProcessEvents;
    adapterCallback.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata, void*) -> void
    {
        if (status == WGPURequestAdapterStatus_Success)
        {
            SEAD_ASSERT(adapter != nullptr);
            *static_cast<WGPUAdapter*>(userdata) = adapter;
        }
    };
    adapterCallback.userdata1 = &adapter;
    WGPUFuture adapterFuture = wgpuInstanceRequestAdapter(mInstance, &adapterOptions, adapterCallback);

    while (adapter == nullptr)
    {
        wgpuInstanceProcessEvents(mInstance);
    }
    mAdapter = adapter;
    SEAD_ASSERT(mAdapter != nullptr);

    WGPUDeviceDescriptor deviceDescriptor = {};
    deviceDescriptor.label = WGPUStringView { "GPU", 3 };
    deviceDescriptor.requiredFeatureCount = 0;
    deviceDescriptor.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
    deviceDescriptor.deviceLostCallbackInfo.callback = [](WGPUDevice const*, WGPUDeviceLostReason reason, WGPUStringView message, void*, void*) -> void
    {
        SEAD_PRINT("WebGPU device lost %#x: %.*s\n", reason, static_cast<s32>(message.length), message.data != nullptr ? message.data : "");
    };
    deviceDescriptor.uncapturedErrorCallbackInfo.callback = [](WGPUDevice const*, WGPUErrorType type, WGPUStringView message, void*, void*) -> void
    {
        SEAD_PRINT("WebGPU error %#x: %.*s\n", type, static_cast<s32>(message.length), message.data != nullptr ? message.data : "");
    };

    WGPUDevice device = nullptr;
    WGPURequestDeviceCallbackInfo deviceCallback = {};
    deviceCallback.mode = WGPUCallbackMode_AllowProcessEvents;
    deviceCallback.callback = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata, void*) -> void
    {
        if (status == WGPURequestDeviceStatus_Success)
        {
            SEAD_ASSERT(device != nullptr);
            *static_cast<WGPUDevice*>(userdata) = device;
        }
    };
    deviceCallback.userdata1 = &device;
    WGPUFuture deviceFuture = wgpuAdapterRequestDevice(mAdapter, &deviceDescriptor, deviceCallback);
    while (device == nullptr)
    {
        wgpuInstanceProcessEvents(mInstance);
    }
    mDevice = device;
    SEAD_ASSERT(mDevice != nullptr);

    mQueue = wgpuDeviceGetQueue(mDevice);

    WGPUSurfaceCapabilities surfaceCapabilities = {};
    WGPUStatus s = wgpuSurfaceGetCapabilities(mSurface, mAdapter, &surfaceCapabilities);
    SEAD_ASSERT(s == WGPUStatus_Success && surfaceCapabilities.formatCount > 0); // TODO: do a pass over everything to make sure stuff gets freed in error cases

    mSurfaceConfig.device = sead::GraphicsWGPU::instance()->getDevice();
    mSurfaceConfig.format = surfaceCapabilities.formats[0];
    mSurfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
    mSurfaceConfig.presentMode = WGPUPresentMode_Fifo;
    mSurfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;

    for (size_t i = 0; i < surfaceCapabilities.presentModeCount; i++)
    {
        if (surfaceCapabilities.presentModes[i] == WGPUPresentMode_Mailbox)
        {
            mMailboxSupported = true;
            break;
        }
    }

    wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);
}

void GraphicsWGPU::resizeSurface(f32 x, f32 y)
{
    mSurfaceConfig.width = x;
    mSurfaceConfig.height = y;
    wgpuSurfaceConfigure(mSurface, &mSurfaceConfig);
}

WGPUShaderModule GraphicsWGPU::createShaderModule(const sead::SafeString& source) const
{
    // TODO: shader cache?

    WGPUShaderSourceWGSL sourceWGSL = {};
    sourceWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
    sourceWGSL.code = {
        .data = source.cstr(),
        .length = static_cast<size_t>(source.calcLength())
    };

    WGPUShaderModuleDescriptor desc = {};
    desc.nextInChain = &sourceWGSL.chain;
    return wgpuDeviceCreateShaderModule(mDevice, &desc);
}

bool GraphicsWGPU::setVBlankWaitIntervalImpl(u32 interval)
{
    //? why does GraphicsGL store the interval but never read from it

    SEAD_ASSERT(interval == 0 || interval == 1);

    mSurfaceConfig.presentMode = interval > 0
        ? WGPUPresentMode_Fifo
        : (mMailboxSupported ? WGPUPresentMode_Mailbox : WGPUPresentMode_Fifo);
    wgpuSurfaceConfigure(mSurface, &mSurfaceConfig);
    return true;
}

WGPURenderPipeline GraphicsWGPU::createRenderPipeline(WGPUDevice device, const WGPURenderPipelineDescriptor& desc)
{
    u64 key;
    bool hashed = PipelineHashWGPU::calcHash(&key, desc);
    SEAD_ASSERT(hashed);

    Graphics::instance()->lockDrawContext();

    ObjList<PipelineCache::Entry>& list = mPipelineCache.list;
    TreeMap<u64, PipelineCache::Entry*>& map = mPipelineCache.map;

    if (PipelineCache::Entry** entry = map.find(key))
    {
        list.moveBefore(list.front(), *entry); // promote to newest used
        Graphics::instance()->unlockDrawContext();
        return (*entry)->pipeline;
    }

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &desc);
    SEAD_ASSERT(pipeline != nullptr);

    // TODO: register an extra ref to the internal objects to prevent pointer reuse?

    if (list.isFull()) // kick last entry
    {
        PipelineCache::Entry* entry = list.back();
        wgpuRenderPipelineRelease(entry->pipeline);
        map.erase(entry->key);
        list.erase(entry);
    }

    PipelineCache::Entry* entry = list.birthFront();
    *entry = { pipeline, key };
    map.insert(key, entry);

    Graphics::instance()->unlockDrawContext();

    return pipeline;
}

void GraphicsWGPU::setViewportImpl(f32 x, f32 y, f32 w, f32 h)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setScissorImpl(f32 x, f32 y, f32 w, f32 h)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setDepthTestEnableImpl(bool enable)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setDepthWriteEnableImpl(bool enable)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setDepthFuncImpl(DepthFunc func)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setCullingModeImpl(CullingMode mode)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setBlendEnableImpl(bool enable)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setBlendEnableMRTImpl(u32 target, bool enable)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setBlendFactorImpl(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setBlendFactorMRTImpl(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setBlendEquationImpl(BlendEquation equationRgb, BlendEquation equationA)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setBlendEquationMRTImpl(u32 target, BlendEquation equationRgb, BlendEquation equationA)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setBlendConstantColorImpl(const Color4f& color)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::waitForVBlankImpl()
{
    //???
}

void GraphicsWGPU::setColorMaskImpl(bool r, bool g, bool b, bool a)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setColorMaskMRTImpl(u32 target, bool r, bool g, bool b, bool a)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setAlphaTestEnableImpl(bool enable)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setAlphaTestFuncImpl(AlphaFunc func, f32 ref)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setStencilTestEnableImpl(bool enable)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setStencilTestFuncImpl(StencilFunc func, s32 ref, u32 mask)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setStencilTestOpImpl(StencilOp fail, StencilOp zfail, StencilOp zpass)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setPolygonModeImpl(PolygonMode front, PolygonMode back)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

void GraphicsWGPU::setPolygonOffsetEnableImpl(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable)
{
    SEAD_ASSERT_MSG(false, "must use DrawContext");
}

} // namespace sead
