#include <gfx/seadTextureUtil.h>
#include <gfx/wgpu/seadGraphicsWGPU.h>
#include <gfx/wgpu/seadTextureWGPU.h>

namespace sead {

bool TextureUtil::createTextureImage2DWGPU(TextureWGPU* dst, u32 level, WGPUTextureFormat format, u32 width, u32 height, WGPUTextureUsage usage, u32 samples)
{
    if (dst->getID() != nullptr)
    {
        wgpuTextureRelease(dst->getID());
        dst->setID(nullptr);
    }
    if (dst->getView() != nullptr)
    {
        wgpuTextureViewRelease(dst->getView());
        dst->setView(nullptr);
    }

    WGPUTextureDescriptor desc = {};
    desc.dimension = WGPUTextureDimension_2D;
    desc.size = {
        width, height, 1
    };
    desc.format = format;
    desc.mipLevelCount = level;
    desc.sampleCount = samples;
    desc.usage = usage;

    WGPUTexture texture = wgpuDeviceCreateTexture(sead::GraphicsWGPU::instance()->getDevice(), &desc);
    if (texture == nullptr)
        return false;

    WGPUTextureViewDescriptor viewDesc = {};
    viewDesc.format = format;
    viewDesc.dimension = WGPUTextureViewDimension_2D;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = level;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = 1;
    viewDesc.aspect = WGPUTextureAspect_All;

    WGPUTextureView view = wgpuTextureCreateView(texture, &viewDesc);
    if (view == nullptr)
    {
        wgpuTextureRelease(texture);
        return false;
    }

    dst->setID(texture);
    dst->setView(view);
    dst->setWidth(width);
    dst->setHeight(height);
    dst->setMipMapLevel(level);
    dst->setFormat(format);
    dst->setUsage(usage);
    dst->setSamples(samples);

    return true;
}

} // namespace sead
