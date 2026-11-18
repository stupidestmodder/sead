#pragma once

#include <basis/seadTypes.h>

#if defined(SEAD_USE_GL)
#include <gfx/gl/seadGL.h>
#elif defined(SEAD_USE_WGPU)
#include <gfx/wgpu/seadWGPU.h>
#endif

namespace sead {

class Heap;
class ReadStream;

#if defined(SEAD_USE_GL)
class TextureGL;
#elif defined(SEAD_USE_WGPU)
class TextureWGPU;
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
#elif defined(SEAD_USE_WGPU)
    static bool createTextureImage2DWGPU(TextureWGPU* dst, u32 level, WGPUTextureFormat format, u32 width, u32 height, WGPUTextureUsage usage, u32 samples = 1);
#endif
};

} // namespace sead
