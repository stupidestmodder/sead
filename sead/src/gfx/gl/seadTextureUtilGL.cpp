#include <gfx/seadTextureUtil.h>

#include <basis/seadAssert.h>
#include <gfx/gl/seadGL.h>
#include <gfx/gl/seadGraphicsGL.h>
#include <gfx/gl/seadTextureGL.h>
#include <math/seadMathCalcCommon.h>
#include <stream/seadStream.h>

namespace sead {

bool TextureUtil::createTextureImage2DGL(
    TextureGL* dst, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
    GLint border, GLint format, GLenum type, const GLvoid* pixels
)
{
    SEAD_ASSERT(dst);

    GLuint tex = GL_NONE;

    GraphicsGL::instance()->lockDrawContext();
    {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    GraphicsGL::instance()->unlockDrawContext();

    if (tex == GL_NONE)
        return false;

    dst->setID(tex);
    dst->setTarget(GL_TEXTURE_2D);
    dst->setMipMapLevel(level);
    dst->setInternalFormat(internalFormat);
    dst->setWidth(width);
    dst->setHeight(height);
    dst->setBorderWidth(border);
    dst->setPixelFormat(format);
    dst->setDataType(type);

    return true;
}

bool TextureUtil::createTextureImage2DGLWithoutLock(
    TextureGL* dst, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
    GLint border, GLint format, GLenum type, const GLvoid* pixels
)
{
    SEAD_ASSERT(dst);

    GLuint tex = GL_NONE;

    {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if (tex == GL_NONE)
    {
        SEAD_ASSERT_MSG(false, "* tex !\n");
        return false;
    }

    dst->setID(tex);
    dst->setTarget(GL_TEXTURE_2D);
    dst->setMipMapLevel(level);
    dst->setInternalFormat(internalFormat);
    dst->setWidth(width);
    dst->setHeight(height);
    dst->setBorderWidth(border);
    dst->setPixelFormat(format);
    dst->setDataType(type);

    return true;
}

bool TextureUtil::createTextureFromBitmapGL(TextureGL* dst, ReadStream* src, Heap* heap)
{
    SEAD_ASSERT(dst);
    SEAD_ASSERT(src);

    src->setMode(Stream::Modes::eBinary);

    BITMAPFILEHEADER fileHeader;
    src->readMemBlock(&fileHeader, sizeof(BITMAPFILEHEADER));

    if (fileHeader.bfType != 0x4D42)
    {
        SEAD_ASSERT_MSG(false, "not bitmap file.");
        return false;
    }

    BITMAPINFOHEADER infoHeader;
    src->readMemBlock(&infoHeader, sizeof(BITMAPINFOHEADER));

    if (infoHeader.biSize != sizeof(BITMAPINFOHEADER))
    {
        SEAD_ASSERT_MSG(false, "file must be INFO type bitmap.");
        return false;
    }

    if (infoHeader.biWidth <= 0)
    {
        SEAD_ASSERT_MSG(false, "invalid format. biWidth must be greater than zero.");
        return false;
    }

    if (infoHeader.biHeight == 0)
    {
        SEAD_ASSERT_MSG(false, "invalid format. biHeight must not be zero.");
        return false;
    }

    if (infoHeader.biBitCount != 24)
    {
        SEAD_ASSERT_MSG(false, "unsupported format. biBitCount must be 24.");
        return false;
    }

    if (infoHeader.biCompression != 0)
    {
        SEAD_ASSERT_MSG(false, "unsupported format. biCompression must be zero.");
        return false;
    }

    if (infoHeader.biClrUsed != 0)
    {
        SEAD_ASSERT_MSG(false, "unsupported format. biClrUsed must be zero.");
        return false;
    }

    if (fileHeader.bfOffBits >= 54)
    {
        src->rewind();
        src->skip(fileHeader.bfOffBits);
    }
    else
    {
        fileHeader.bfOffBits = 0;
    }

    u32 width = Mathi::roundUpPow2(infoHeader.biWidth * 3, 4);
    u32 height = Mathi::abs(infoHeader.biHeight);

    u32 dataSize = width * height;
    u8* data = new(heap, -32) u8[dataSize];

    if (infoHeader.biHeight > 0)
    {
        src->readMemBlock(data, dataSize);
    }
    else
    {
        s32 i = height - 1;
        if (i >= 0)
        {
            u8* ptr = &data[i * width];
            do
            {
                src->readMemBlock(ptr, width);
                ptr -= width;
                i--;
            } while (i >= 0);
        }
    }

    createTextureImage2DGL(dst, 0, GL_RGB, infoHeader.biWidth, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    delete[] data;

    return true;
}

} // namespace sead
