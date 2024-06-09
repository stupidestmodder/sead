#include <gfx/gl/seadTextureFrameBufferGL.h>

#include <gfx/gl/seadGraphicsGL.h>
#include <gfx/gl/seadTextureGL.h>
#include <gfx/seadGraphicsContext.h>
#include <gfx/seadTextureUtil.h>
#include <prim/seadSafeString.h>

namespace sead {

TextureFrameBufferGL::~TextureFrameBufferGL()
{
    if (mFBO != GL_NONE)
        glDeleteFramebuffers(1, &mFBO);
}

void TextureFrameBufferGL::copyToDisplayBuffer(DrawContext* drawContext, const DisplayBuffer* displayBuffer) const
{
    SEAD_UNUSED(drawContext);

    const DisplayBufferGL* displayBufferGL = DynamicCast<const DisplayBufferGL>(displayBuffer);
    SEAD_ASSERT(displayBufferGL);

    f32 w = displayBufferGL->getWidth();
    f32 h = displayBufferGL->getHeight();
    SEAD_ASSERT(w != 0 && h != 0);

    Graphics::instance()->setViewportRealPosition(0, 0, w, h);
    Graphics::instance()->setScissorRealPosition(0, 0, w, h);

    //! TODO: Copy framebuffer without using glBlit
    glBlitNamedFramebuffer(mFBO, displayBufferGL->getHandle(),
                           0, 0, static_cast<GLint>(w), static_cast<GLint>(h),
                           0, 0, static_cast<GLint>(w), static_cast<GLint>(h),
                           GL_COLOR_BUFFER_BIT, GL_LINEAR);

    displayBufferGL->copyToDisplay();
}

void TextureFrameBufferGL::clear(DrawContext* drawContext, u32 clrFlag, const Color4f& color, f32 depth, u32 stencil) const
{
    SEAD_UNUSED(drawContext);

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    bindImpl_(nullptr);

    GLbitfield mask = 0;

    if (clrFlag & ClearFlag::eColor)
        mask |= GL_COLOR_BUFFER_BIT;

    if (clrFlag & ClearFlag::eDepth)
    {
        mask |= GL_DEPTH_BUFFER_BIT;
        glDepthMask(GL_TRUE);
    }

    if (clrFlag & ClearFlag::eStencil)
        mask |= GL_STENCIL_BUFFER_BIT;

    glClearColor(color.r, color.g, color.b, color.a);

    glClearDepth(depth);
    glClearStencil(stencil);
    glClear(mask);

    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
}

void TextureFrameBufferGL::setup(TextureGL* colorTexture, TextureGL* depthTexture)
{
    if (mFBO != GL_NONE)
    {
        SEAD_ASSERT_MSG(false, "setup twice.");
        return;
    }

    SEAD_ASSERT(colorTexture || depthTexture);

    GraphicsGL::instance()->lockDrawContext();
    {
        GLint prevFBO;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

        glGenFramebuffers(1, &mFBO);
        SEAD_ASSERT(mFBO != GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        if (colorTexture)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->getID(), 0);

        if (depthTexture)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture->getID(), 0);

        SEAD_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "INVALID FRAMEBUFFER\n");

        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }
    GraphicsGL::instance()->unlockDrawContext();

    mColorTexture = colorTexture;
    mDepthTexture = depthTexture;
}

void TextureFrameBufferGL::bindImpl_(DrawContext* drawContext) const
{
    SEAD_UNUSED(drawContext);

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

DisplayBufferGL::~DisplayBufferGL()
{
    if (mSampler != GL_NONE)
    {
        glDeleteSamplers(1, &mSampler);
        mSampler = GL_NONE;
    }

    if (mProgram != GL_NONE)
    {
        glDeleteProgram(mProgram);
        mProgram = GL_NONE;
    }

    if (mFragShader != GL_NONE)
    {
        glDeleteShader(mFragShader);
        mFragShader = GL_NONE;
    }

    if (mVertShader != GL_NONE)
    {
        glDeleteShader(mVertShader);
        mVertShader = GL_NONE;
    }

    if (mVAO != GL_NONE)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = GL_NONE;
    }

    if (mFBO != GL_NONE)
    {
        glDeleteFramebuffers(1, &mFBO);
        mFBO = GL_NONE;
    }

    if (mTextureGL)
    {
        delete mTextureGL;
        mTextureGL = nullptr;
    }
}

void DisplayBufferGL::initializeImpl_(Heap* heap)
{
    {
        mTextureGL = new(heap) TextureGL();

        s32 width = static_cast<s32>(mWidth);
        s32 height = static_cast<s32>(mHeight);

        bool b = TextureUtil::createTextureImage2DGLWithoutLock(mTextureGL, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        SEAD_ASSERT(b);
    }

    {
        GLint prevFBO;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

        glGenFramebuffers(1, &mFBO);
        SEAD_ASSERT(mFBO != GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureGL->getID(), 0);
        SEAD_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "INVALID FRAMEBUFFER\n");

        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }

    {
        glGenVertexArrays(1, &mVAO);
        SEAD_ASSERT(mVAO != GL_NONE);
    }

    {
        mVertShader = glCreateShader(GL_VERTEX_SHADER);
        SEAD_ASSERT(mVertShader != GL_NONE);

        const char* vertShaderCode = 
            "#version 460 core\n "
            "out vec2 vFragCoord; "
            "void main(void) "
            "{ "
            "    vFragCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2); "
            "    gl_Position = vec4(vFragCoord * 2.0f + -1.0f, 0.0f, 1.0f); "
            "} ";

        s32 vertShaderCodeSize = SafeString(vertShaderCode).calcLength() + 1;

        glShaderSource(mVertShader, 1, &vertShaderCode, &vertShaderCodeSize);
        glCompileShader(mVertShader);

#ifdef SEAD_DEBUG
        GLint success = 0;
        glGetShaderiv(mVertShader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint logSize = 0;
            glGetShaderiv(mVertShader, GL_INFO_LOG_LENGTH, &logSize);

            char* log = new(heap) char[logSize];
            glGetShaderInfoLog(mVertShader, logSize, &logSize, log);

            SEAD_ASSERT_MSG(false, "Vert Shader Error: %s", log);

            delete[] log;
        }
#endif // SEAD_DEBUG
    }

    {
        mFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        SEAD_ASSERT(mFragShader != GL_NONE);

        const char* fragShaderCode =
            "#version 460 core\n "
            "out vec4 oFragColor; "
            "uniform sampler2D uTexture; "
            "uniform float uGamma; "
            "in vec2 vFragCoord; "
            "void main(void) "
            "{ "
            "    vec4 color = texture2D(uTexture, vFragCoord); "
            "    oFragColor = pow(color, vec4(uGamma, uGamma, uGamma, 1.0f)); "
            "} ";

        s32 fragShaderCodeSize = SafeString(fragShaderCode).calcLength() + 1;

        glShaderSource(mFragShader, 1, &fragShaderCode, &fragShaderCodeSize);
        glCompileShader(mFragShader);

#ifdef SEAD_DEBUG
        GLint success = 0;
        glGetShaderiv(mFragShader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint logSize = 0;
            glGetShaderiv(mFragShader, GL_INFO_LOG_LENGTH, &logSize);

            char* log = new(heap) char[logSize];
            glGetShaderInfoLog(mFragShader, logSize, &logSize, log);

            SEAD_ASSERT_MSG(false, "Frag Shader Error: %s", log);

            delete[] log;
        }
#endif // SEAD_DEBUG
    }

    {
        mProgram = glCreateProgram();
        SEAD_ASSERT(mProgram != GL_NONE);

        glAttachShader(mProgram, mVertShader);
        glAttachShader(mProgram, mFragShader);
        glLinkProgram(mProgram);

#ifdef SEAD_DEBUG
        GLint success = 0;
        glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint logSize = 0;
            glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logSize);

            char* log = new(heap) char[logSize];
            glGetProgramInfoLog(mProgram, logSize, &logSize, log);

            SEAD_ASSERT_MSG(false, "Program Error: %s", log);

            delete[] log;
        }
#endif // SEAD_DEBUG
    }

    {
        glGenSamplers(1, &mSampler);
        SEAD_ASSERT(mSampler != GL_NONE);

        glSamplerParameteri(mSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(mSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    {
        mGammaUniformLoc = glGetUniformLocation(mProgram, "uGamma");
        SEAD_ASSERT(mGammaUniformLoc != -1);

        mGamma = 1.0f;
    }
}

void DisplayBufferGL::copyToDisplay() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    s32 w = static_cast<s32>(mWidth);
    s32 h = static_cast<s32>(mHeight);
    SEAD_ASSERT(w != 0 && h != 0);

    GraphicsContext context;
    context.setDepthEnable(false, false);
    context.setBlendEnable(false);
    context.setColorMask(true, true, true, false);
    context.apply(nullptr);

    glViewport(0, 0, w, h);
    glScissor(0, 0, w, h);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(mProgram);
    glUniform1f(mGammaUniformLoc, 1.0f / (mGamma > Mathf::epsilon() ? mGamma : Mathf::epsilon()));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureGL->getID());
    glBindSampler(0, mSampler);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void DisplayBufferGL::reconfigure(f32 width, f32 height)
{
    mWidth = width;
    mHeight = height;

    GLuint handle = mTextureGL->getID();

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(mTextureGL->getTarget(), 0, mTextureGL->getInternalFormat(), width, height, 0, mTextureGL->getPixelFormat(), mTextureGL->getDataType(), nullptr);
}

} // namespace sead
