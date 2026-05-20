#pragma once

#include <framework/sdl/seadGameFrameworkBaseSDL.h>
#include <gfx/gl/seadGL.h>
#include <math/seadVector.h>

namespace sead {

class DisplayBufferGL;
class TextureFrameBufferGL;
class TextureGL;

class GameFrameworkSDLGL : public GameFrameworkBaseSDL
{
    SEAD_RTTI_OVERRIDE(GameFrameworkSDLGL, GameFrameworkBaseSDL);

public:
    explicit GameFrameworkSDLGL(const CreateArg& arg);
    ~GameFrameworkSDLGL() override;

    virtual void initializeGraphicsSystem(Heap* heap, const Vector2f& virtualFbSize);

    SDL_Window* getWindow() const
    {
        return mWindow;
    }

    SDL_GLContext getGLContext() const
    {
        return mGLContext;
    }

protected:
    void procFrame_() override;
    void procDraw_() override;

    virtual void clearFrameBuffers_(s32 screenType);
    virtual void present_();

    void createFrameBuffer_(Heap* heap, const Vector2f& virtualFbSize);

    void resize_(f32 width, f32 height) override;

protected:
    SDL_GLContext mGLContext;
    DisplayBufferGL* mDisplayBufferGL;
    TextureGL* mColorTexture;
    TextureGL* mDepthTexture;
    TextureFrameBufferGL* mCopyReservedFrameBuffer;
    GLuint mGPUQueries[2];
};

} // namespace sead
