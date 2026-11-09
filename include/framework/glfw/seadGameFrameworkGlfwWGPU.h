#pragma once

#include <framework/glfw/seadGameFrameworkBaseGlfw.h>
#include <gfx/wgpu/seadTextureFrameBufferWGPU.h>
#include <gfx/wgpu/seadTextureWGPU.h>
#include <gfx/wgpu/seadWGPU.h>

namespace sead {

class GameFrameworkGlfwWGPU : public GameFrameworkBaseGlfw
{
    SEAD_RTTI_OVERRIDE(GameFrameworkGlfwWGPU, GameFrameworkBaseGlfw)

public:
    struct CreateArg
    {
        s32 width = 1280;
        s32 height = 720;
        u32 wait_vblank = 1;
        f32 fps = 60.0f;
        SafeString window_name = "GameFrameworkGlfwWGPU";
        // DWORD window_ex_style = 0;
        Color4f clear_color = Color4f::cGray;
        bool create_default_framebuffer = true;
        u32 pipeline_cache_size = 2048;
    };

public:
    explicit GameFrameworkGlfwWGPU(const CreateArg& arg);
    ~GameFrameworkGlfwWGPU() override;

    virtual void initializeGraphicsSystem(Heap* heap, const Vector2f& virtualFbSize);

protected:
    void procFrame_() override;
    void procDraw_() override;
    void resize_(f32 width, f32 height) override;

    virtual void present_(DrawContext* drawContext);

    void createFrameBuffer_(Heap* heap, const Vector2f& virtualFbSize);

protected:
    TextureWGPU* mColorTexture;
    TextureWGPU* mDepthTexture;
    DisplayBufferWGPU* mDisplayBufferWGPU;
    Vector2f mSize;
    u32 mPipelineCacheSize;
};

} // namespace sead
