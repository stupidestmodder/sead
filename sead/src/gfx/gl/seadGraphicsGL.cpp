#include <gfx/gl/seadGraphicsGL.h>

#include <basis/seadRawPrint.h>
#include <gfx/gl/seadGL.h>
#include <gfx/seadDrawLockContext.h>
#include <gfx/seadGraphicsContext.h>
#include <thread/seadThread.h>

#ifdef SEAD_DEBUG
static void __stdcall GLDebugCallback(GLenum source, GLenum type, u32 id, GLenum severity, GLsizei, const char* message, const void*)
{
    if (id == 131169 || id == 131185 || id == 131204 || id == 131218)
        return;

    sead::Thread* thread = sead::ThreadMgr::instance() ? sead::ThreadMgr::instance()->getCurrentThread() : nullptr;
    const char* threadName = thread ? thread->getName().cstr() : "null";

    const char* sourceStr = nullptr;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
    }

    const char* typeStr = nullptr;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
    }

    const char* severityStr = nullptr;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         severityStr = "High"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severityStr = "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          severityStr = "Low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "Notification"; break;
    }

    SEAD_PRINT("GL Debug Callback:\n");
    SEAD_PRINT("  thread:   %s\n", threadName);
    SEAD_PRINT("  source:   %s\n", sourceStr);
    SEAD_PRINT("  type:     %s\n", typeStr);
    SEAD_PRINT("  id:       %u\n", id);
    SEAD_PRINT("  severity: %s\n", severityStr);
    SEAD_PRINT("  message:  %s\n", message);

    //sead::system::DebugBreak();
}
#endif // SEAD_DEBUG

namespace sead {

GraphicsGL::GraphicsGL(const CreateArg& arg)
    : Graphics()
    , mHGLRC(arg.hglrc)
    , mHDC(arg.hdc)
    , mVBlankWaitInterval(0)
{
}

void GraphicsGL::initializeDrawLockContext(Heap* heap)
{
    mDrawLockContext->initialize(heap);
}

void GraphicsGL::initializeImpl(Heap*)
{
    SEAD_PRINT("OpenGL Initialized: %s\n", glGetString(GL_VERSION));

#ifdef SEAD_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(&GLDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif // SEAD_DEBUG

    glEnable(GL_SCISSOR_TEST);
    //glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);

    GraphicsContext context;
    context.apply(nullptr);
}

void GraphicsGL::setViewportImpl(f32 x, f32 y, f32 w, f32 h)
{
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(w), static_cast<GLsizei>(h));
}

void GraphicsGL::setScissorImpl(f32 x, f32 y, f32 w, f32 h)
{
    glScissor(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(w), static_cast<GLsizei>(h));
}

void GraphicsGL::setDepthTestEnableImpl(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void GraphicsGL::setDepthWriteEnableImpl(bool enable)
{
    if (enable)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);
}

void GraphicsGL::setDepthFuncImpl(DepthFunc func)
{
    glDepthFunc(static_cast<GLenum>(func));
}

bool GraphicsGL::setVBlankWaitIntervalImpl(u32 interval)
{
    mVBlankWaitInterval = interval;

    if (wglSwapIntervalEXT)
    {
        wglSwapIntervalEXT(interval);
        return true;
    }

    return false;
}

void GraphicsGL::setCullingModeImpl(CullingMode mode)
{
    switch (mode)
    {
        case Graphics::CullingMode::eFront:
        case Graphics::CullingMode::eBack:
            glEnable(GL_CULL_FACE);
            glCullFace(static_cast<GLenum>(mode));
            break;

        case CullingMode::eAll:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT_AND_BACK);
            break;

        case CullingMode::eNone:
            glDisable(GL_CULL_FACE);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined CullingMode(%d).", static_cast<s32>(mode));
            break;
    }
}

void GraphicsGL::setBlendEnableImpl(bool enable)
{
    if (enable)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void GraphicsGL::setBlendEnableMRTImpl(u32 target, bool enable)
{
    if (enable)
        glEnablei(GL_BLEND, target);
    else
        glDisablei(GL_BLEND, target);
}

void GraphicsGL::setBlendFactorImpl(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
{
    glBlendFuncSeparate(static_cast<GLenum>(srcFactorRgb), static_cast<GLenum>(dstFactorRgb),
                        static_cast<GLenum>(srcFactorA), static_cast<GLenum>(dstFactorA));
}

void GraphicsGL::setBlendFactorMRTImpl(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
{
    glBlendFuncSeparatei(target,
                         static_cast<GLenum>(srcFactorRgb), static_cast<GLenum>(dstFactorRgb),
                         static_cast<GLenum>(srcFactorA), static_cast<GLenum>(dstFactorA));
}

void GraphicsGL::setBlendEquationImpl(BlendEquation equationRgb, BlendEquation equationA)
{
    glBlendEquationSeparate(static_cast<GLenum>(equationRgb), static_cast<GLenum>(equationA));
}

void GraphicsGL::setBlendEquationMRTImpl(u32 target, BlendEquation equationRgb, BlendEquation equationA)
{
    glBlendEquationSeparatei(target, static_cast<GLenum>(equationRgb), static_cast<GLenum>(equationA));
}

void GraphicsGL::setBlendConstantColorImpl(const Color4f& color)
{
    glBlendColor(color.r, color.g, color.b, color.a);
}

void GraphicsGL::waitForVBlankImpl()
{
}

void GraphicsGL::setColorMaskImpl(bool r, bool g, bool b, bool a)
{
    glColorMask(r, g, b, a);
}

void GraphicsGL::setColorMaskMRTImpl(u32 target, bool r, bool g, bool b, bool a)
{
    glColorMaski(target, r, g, b, a);
}

void GraphicsGL::setAlphaTestEnableImpl(bool)
{
    SEAD_ASSERT_MSG(false, "deprecated");

    //if (enable)
    //    glEnable(GL_ALPHA_TEST);
    //else
    //    glDisable(GL_ALPHA_TEST);
}

void GraphicsGL::setAlphaTestFuncImpl(AlphaFunc, f32)
{
    SEAD_ASSERT_MSG(false, "deprecated");

    //glAlphaFunc(static_cast<GLenum>(func), ref);
}

void GraphicsGL::setStencilTestEnableImpl(bool enable)
{
    if (enable)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
}

void GraphicsGL::setStencilTestFuncImpl(StencilFunc func, s32 ref, u32 mask)
{
    glStencilFunc(static_cast<GLenum>(func), ref, mask);
}

void GraphicsGL::setStencilTestOpImpl(StencilOp fail, StencilOp zfail, StencilOp zpass)
{
    glStencilOp(static_cast<GLenum>(fail), static_cast<GLenum>(zfail), static_cast<GLenum>(zpass));
}

void GraphicsGL::setPolygonModeImpl(PolygonMode front, PolygonMode back)
{
    SEAD_ASSERT(front == back);
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(front));

    //glPolygonMode(GL_FRONT, static_cast<GLenum>(front));
    //glPolygonMode(GL_BACK, static_cast<GLenum>(back));
}

void GraphicsGL::setPolygonOffsetEnableImpl(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable)
{
    if (fillFrontEnable)
        glEnable(GL_POLYGON_OFFSET_FILL);
    else
        glDisable(GL_POLYGON_OFFSET_FILL);

    SEAD_UNUSED(fillBackEnable);

    if (pointLineEnable)
    {
        glEnable(GL_POLYGON_OFFSET_POINT);
        glEnable(GL_POLYGON_OFFSET_LINE);
    }
    else
    {
        glDisable(GL_POLYGON_OFFSET_POINT);
        glDisable(GL_POLYGON_OFFSET_LINE);
    }
}

} // namespace sead
