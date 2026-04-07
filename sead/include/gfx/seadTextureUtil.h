#pragma once

#if defined(SEAD_USE_GL)
#include <gfx/gl/seadGL.h>
#endif

namespace sead {

class Heap;
class ReadStream;

#if defined(SEAD_USE_GL)
class TextureGL;
#endif

class TextureUtil
{
public:
#if defined(SEAD_USE_GL)
    static bool createTextureImage2DGL(TextureGL* dst, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                                       GLint border, GLint format, GLenum type, const GLvoid* pixels);

    static bool createTextureImage2DGLWithoutLock(TextureGL* dst, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                                       GLint border, GLint format, GLenum type, const GLvoid* pixels);

    static bool createTextureFromBitmapGL(TextureGL* dst, ReadStream* src, Heap* heap);
#endif
};

} // namespace sead
