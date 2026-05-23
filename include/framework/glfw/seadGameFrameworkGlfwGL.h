#pragma once

#include <framework/glfw/seadGameFrameworkBaseGlfw.h>
#include <gfx/gl/seadGL.h>
#include <math/seadVector.h>

namespace sead {

class DisplayBufferGL;
class TextureFrameBufferGL;
class TextureGL;

class GameFrameworkGlfwGL : public GameFrameworkBaseGlfw
{
    SEAD_RTTI_OVERRIDE(GameFrameworkGlfwGL, GameFrameworkBaseGlfw);

public:
    explicit GameFrameworkGlfwGL(const CreateArg& arg);
    ~GameFrameworkGlfwGL() override;

    virtual void initializeGraphicsSystem(Heap* heap, const Vector2f& virtualFbSize);

    // HDC getHDC() const
    // {
    //     return mHDC;
    // }

    // HGLRC getHGLRC() const
    // {
    //     return mHGLRC;
    // }

protected:
    void createWindow_() override;
    void procFrame_() override;
    void procDraw_() override;

    virtual void clearFrameBuffers_(s32 screenType);
    virtual void present_();

    void createFrameBuffer_(Heap* heap, const Vector2f& virtualFbSize);

    void resize_(f32 width, f32 height) override;

protected:
    // HGLRC mHGLRC;
    // HDC mHDC;
    Heap* mGLHeap;
    DisplayBufferGL* mDisplayBufferGL;
    TextureGL* mColorTexture;
    TextureGL* mDepthTexture;
    TextureFrameBufferGL* mCopyReservedFrameBuffer;
    GLuint mGPUQueries[2];
};

} // namespace sead
