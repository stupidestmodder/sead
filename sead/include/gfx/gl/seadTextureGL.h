#pragma once

#include <gfx/gl/seadGL.h>
#include <gfx/seadTexture.h>

namespace sead {

class TextureGL : public Texture
{
    SEAD_RTTI_OVERRIDE(TextureGL, Texture);

public:
    TextureGL()
        : Texture()
        , mID(GL_NONE)
        , mTarget(GL_TEXTURE_2D)
        , mLevel(1)
        , mInternalFormat(GL_RGBA)
        , mWidth(0)
        , mHeight(0)
        , mBorderWidth(0)
        , mPixelFormat(GL_RGBA)
        , mDataType(GL_UNSIGNED_BYTE)
    {
    }

    ~TextureGL() override
    {
        if (mID)
        {
            glDeleteTextures(1, &mID);
            mID = GL_NONE;
        }
    }

    u32 getWidth() const override { return mWidth; }
    u32 getHeight() const override { return mHeight; }

    GLuint getID() const { return mID; }
    GLenum getTarget() const { return mTarget; }
    GLint getMipMapLevel() const { return mLevel; }
    GLint getInternalFormat() const { return mInternalFormat; }
    GLint getBorderWidth() const { return mBorderWidth; }
    GLint getPixelFormat() const { return mPixelFormat; }
    GLenum getDataType() const { return mDataType; }

    void setID(GLuint id) { mID = id; }
    void setTarget(GLenum target) { mTarget = target; }
    void setMipMapLevel(GLint level) { mLevel = level; }
    void setInternalFormat(GLint fmt) { mInternalFormat = fmt; }
    void setWidth(GLsizei width) { mWidth = width; }
    void setHeight(GLsizei height) { mHeight = height; }
    void setBorderWidth(GLint width) { mBorderWidth = width; }
    void setPixelFormat(GLint fmt) { mPixelFormat = fmt; }
    void setDataType(GLenum type) { mDataType = type; }

protected:
    GLuint mID;
    GLenum mTarget;
    GLint mLevel;
    GLint mInternalFormat;
    GLsizei mWidth;
    GLsizei mHeight;
    GLint mBorderWidth;
    GLint mPixelFormat;
    GLenum mDataType;
};

} // namespace sead
