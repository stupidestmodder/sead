#pragma once

#include <framework/win/seadGameFrameworkBaseWin.h>
#include <gfx/gl/seadGL.h>
#include <math/seadVector.h>

namespace sead {

class DisplayBufferGL;
class TextureFrameBufferGL;
class TextureGL;

class GameFrameworkWinGL : public GameFrameworkBaseWin
{
    SEAD_RTTI_OVERRIDE(GameFrameworkWinGL, GameFrameworkBaseWin);

public:
    explicit GameFrameworkWinGL(const CreateArg& arg);
    ~GameFrameworkWinGL() override;

    virtual void initializeGraphicsSystem(Heap* heap, const Vector2f& virtualFbSize);

    HDC getHDC() const
    {
        return mHDC;
    }

    HGLRC getHGLRC() const
    {
        return mHGLRC;
    }

protected:
    void procFrame_() override;
    void procDraw_() override;

    virtual void clearFrameBuffers_(s32 screenType);
    virtual void present_();

    void createFrameBuffer_(Heap* heap, const Vector2f& virtualFbSize);

    void resize_(f32 width, f32 height) override;

protected:
    HGLRC mHGLRC;
    HDC mHDC;
    DisplayBufferGL* mDisplayBufferGL;
    TextureGL* mColorTexture;
    TextureGL* mDepthTexture;
    TextureFrameBufferGL* mCopyReservedFrameBuffer;
    GLuint mGPUQueries[2];
};

} // namespace sead
