#pragma once

#include <gfx/gl/seadGL.h>
#include <gfx/seadFrameBuffer.h>
#include <heap/seadDisposer.h>

namespace sead {

class TextureGL;

class TextureFrameBufferGL : public FrameBuffer, public IDisposer
{
    SEAD_RTTI_OVERRIDE(TextureFrameBufferGL, FrameBuffer);

public:
    TextureFrameBufferGL()
        : FrameBuffer()
        , IDisposer()
        , mFBO(GL_NONE)
        , mColorTexture(nullptr)
        , mDepthTexture(nullptr)
    {
    }

    TextureFrameBufferGL(const Vector2f& virtualSize, const BoundBox2f& physicalArea)
        : FrameBuffer(virtualSize, physicalArea)
        , IDisposer()
        , mFBO(GL_NONE)
        , mColorTexture(nullptr)
        , mDepthTexture(nullptr)
    {
    }

    TextureFrameBufferGL(const Vector2f& virtualSize, f32 physicalX, f32 physicalY, f32 physicalW, f32 physicalH)
        : FrameBuffer(virtualSize, physicalX, physicalY, physicalW, physicalH)
        , IDisposer()
        , mFBO(GL_NONE)
        , mColorTexture(nullptr)
        , mDepthTexture(nullptr)
    {
    }

    ~TextureFrameBufferGL() override;

    void copyToDisplayBuffer(DrawContext* drawContext, const DisplayBuffer* displayBuffer) const override;
    void clear(DrawContext* drawContext, u32 clrFlag, const Color4f& color, f32 depth, u32 stencil) const override;

    void setup(TextureGL* colorTexture, TextureGL* depthTexture = nullptr);

    GLuint getFBO() const
    {
        return mFBO;
    }

protected:
    void bindImpl_(DrawContext* drawContext) const override;

protected:
    GLuint mFBO;
    TextureGL* mColorTexture;
    TextureGL* mDepthTexture;
};

class DisplayBufferGL : public DisplayBuffer
{
    SEAD_RTTI_OVERRIDE(DisplayBufferGL, DisplayBuffer);

public:
    DisplayBufferGL()
        : DisplayBuffer()
        , mTextureGL(nullptr)
        , mFBO(GL_NONE)
        , mVAO(GL_NONE)
        , mVertShader(GL_NONE)
        , mFragShader(GL_NONE)
        , mProgram(GL_NONE)
        , mSampler(GL_NONE)
        , mGammaUniformLoc(-1)
        , mGamma(0.0f)
    {
    }

    ~DisplayBufferGL() override;

    void copyToDisplay() const;

    const TextureGL* getTexture() const
    {
        return mTextureGL;
    }

    GLuint getHandle() const
    {
        return mFBO;
    }

    void setGamma(f32 gamma)
    {
        mGamma = gamma;
    }

    void reconfigure(f32 width, f32 height);

protected:
    void initializeImpl_(Heap* heap) override;

protected:
    TextureGL* mTextureGL;
    GLuint mFBO;
    GLuint mVAO;
    GLuint mVertShader;
    GLuint mFragShader;
    GLuint mProgram;
    GLuint mSampler;
    GLint mGammaUniformLoc;
    f32 mGamma;
};

} // namespace sead
