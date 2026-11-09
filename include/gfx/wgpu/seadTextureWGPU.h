#pragma once

#include <gfx/wgpu/seadWGPU.h>
#include <gfx/seadTexture.h>

namespace sead {

class TextureWGPU : public Texture
{
    SEAD_RTTI_OVERRIDE(TextureWGPU, Texture);

public:
    TextureWGPU()
        : Texture()
        , mID(nullptr)
        , mView(nullptr)
        , mWidth(0)
        , mHeight(0)
        , mLevel(0)
        , mFormat(WGPUTextureFormat_Undefined)
        , mUsage(WGPUTextureUsage_None)
        , mSamples(0)
    {
    }

    ~TextureWGPU() override
    {
        if (mID != nullptr)
        {
            wgpuTextureRelease(mID);
            mID = nullptr;
        }
        if (mView != nullptr)
        {
            wgpuTextureViewRelease(mView);
            mView = nullptr;
        }
    }

    u32 getWidth() const override { return mWidth; }
    u32 getHeight() const override { return mHeight; }
    const WGPUTexture& getID() const { return mID; }
    const WGPUTextureView& getView() const { return mView; }
    u32 getMipMapLevel() const { return mLevel; }
    const WGPUTextureFormat& getFormat() const  { return mFormat; }
    const WGPUTextureUsage& getUsage() const { return mUsage; }
    u32 getSamples() const { return mSamples; }

    void setWidth(u32 width) { mWidth = width; }
    void setHeight(u32 height) { mHeight = height; }
    void setID(WGPUTexture id) { mID = id; }
    void setView(WGPUTextureView view) { mView = view; }
    void setMipMapLevel(u32 level) { mLevel = level; }
    void setFormat(WGPUTextureFormat format) { mFormat = format; }
    void setUsage(WGPUTextureUsage usage) { mUsage = usage; }
    void setSamples(u32 samples) { mSamples = samples; }

protected:
    WGPUTexture mID;
    WGPUTextureView mView;
    u32 mWidth;
    u32 mHeight;
    u32 mLevel;
    WGPUTextureFormat mFormat;
    WGPUTextureUsage mUsage;
    u32 mSamples;
};

} // namespace sead
