#include <framework/glfw/seadGameFrameworkGlfwGL.h>

#include <framework/seadProcessMeter.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <gfx/gl/seadGL.h>
#include <gfx/gl/seadGraphicsGL.h>
#include <gfx/gl/seadTextureFrameBufferGL.h>
#include <gfx/gl/seadTextureGL.h>
#include <gfx/seadTextureUtil.h>

namespace sead {

GameFrameworkGlfwGL::GameFrameworkGlfwGL(const CreateArg& arg)
    : GameFrameworkBaseGlfw(arg)
    // , mHGLRC(nullptr)
    // , mHDC(nullptr)
    , mDisplayBufferGL(nullptr)
    , mColorTexture(nullptr)
    , mDepthTexture(nullptr)
    , mCopyReservedFrameBuffer(nullptr)
    , mGPUQueries()
{
    mProcDrawCallback = nullptr;
}

GameFrameworkGlfwGL::~GameFrameworkGlfwGL()
{
}

void GameFrameworkGlfwGL::initializeGraphicsSystem(Heap* heap, const Vector2f& virtualFbSize)
{
    glfwMakeContextCurrent(mWindow);
    {
        s32 result = gladLoaderLoadGL();
        SEAD_ASSERT(result != 0);
    }
    glfwMakeContextCurrent(nullptr);

    GraphicsGL::CreateArg arg;
    arg.window = mWindow;

    Graphics::setInstance(new(heap) GraphicsGL(arg));
    Graphics::instance()->initialize(heap);

    createFrameBuffer_(heap, virtualFbSize);

    Graphics::instance()->lockDrawContext();
    {
        glGenQueries(2, mGPUQueries);
    }
    Graphics::instance()->unlockDrawContext();
}

void GameFrameworkGlfwGL::procFrame_()
{
    Graphics::instance()->lockDrawContext();
    {
        ProcessMeter* proc = ProcessMeter::instance();
        if (proc)
            proc->measureBeginFrame();

        //* Frame process
        {
            TickTime frameBegin;

            GLint64 frameBeginTimeGL;
            glGetInteger64v(GL_TIMESTAMP, &frameBeginTimeGL);

            glQueryCounter(mGPUQueries[0], GL_TIMESTAMP);
            {
                procDraw_();
            }
            glQueryCounter(mGPUQueries[1], GL_TIMESTAMP);

            procCalc_();
            procReset_();

            present_();

            //* Update GPUMeter
            {
                GLuint64 start;
                GLuint64 end;
                glGetQueryObjectui64v(mGPUQueries[0], GL_QUERY_RESULT, &start);
                glGetQueryObjectui64v(mGPUQueries[1], GL_QUERY_RESULT, &end);

                TickTime gpuTimeStart = frameBegin + TickSpan::makeFromNanoSeconds(start - frameBeginTimeGL);
                mGPUMeter.measureBegin(gpuTimeStart);

                TickTime gpuTimeEnd = gpuTimeStart + TickSpan::makeFromNanoSeconds(end - start);
                mGPUMeter.measureEnd(gpuTimeEnd);
            }
        }

        if (proc)
            proc->measureEndFrame();

        if (getDisplayState() == DisplayState::eReady)
        {
            glfwShowWindow(mWindow);

            mDisplayState = DisplayState::eShow;
        }

        glfwSwapBuffers(mWindow);
    }
    Graphics::instance()->unlockDrawContext();
}

void GameFrameworkGlfwGL::procDraw_()
{
    if (mProcDrawCallback)
        mProcDrawCallback(true);

    mDrawMeter.measureBegin();
    {
        if (mDefaultFrameBuffer)
        {
            mDefaultFrameBuffer->bind(nullptr);
            clearFrameBuffers_(3);
        }

        DynamicCast<SingleScreenMethodTreeMgr>(getMethodTreeMgr())->draw();
    }
    mDrawMeter.measureEnd();

    if (mProcDrawCallback)
        mProcDrawCallback(false);
}

void GameFrameworkGlfwGL::clearFrameBuffers_(s32)
{
    if (mDefaultFrameBuffer)
    {
        Viewport vp(*mDefaultFrameBuffer);
        vp.applyScissor(nullptr, *mDefaultFrameBuffer);

        mDefaultFrameBuffer->clear(nullptr, FrameBuffer::ClearFlag::eAll, mArg.clear_color, 1.0f, 0);
    }
}

void GameFrameworkGlfwGL::present_()
{
    if (mCopyReservedFrameBuffer)
    {
        mCopyReservedFrameBuffer->copyToDisplayBuffer(nullptr, mDisplayBufferGL);
        mCopyReservedFrameBuffer = nullptr;
    }
    else if (mDefaultFrameBuffer)
    {
        mDefaultFrameBuffer->copyToDisplayBuffer(nullptr, mDisplayBufferGL);
    }
}

void GameFrameworkGlfwGL::createFrameBuffer_(Heap* heap, const Vector2f& virtualFbSize)
{
    Graphics::instance()->lockDrawContext();
    {
        s32 width = mArg.width;
        s32 height = mArg.height;

        if (mArg.create_default_framebuffer)
        {
            TextureFrameBufferGL* fb = new(heap) TextureFrameBufferGL(virtualFbSize, 0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height));
            SEAD_ASSERT(fb);

            mColorTexture = new(heap) TextureGL();
            mDepthTexture = new(heap) TextureGL();

            TextureUtil::createTextureImage2DGL(mColorTexture, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            TextureUtil::createTextureImage2DGL(mDepthTexture, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

            fb->setup(mColorTexture, mDepthTexture);

            mDefaultFrameBuffer = fb;
            mDefaultFrameBuffer->bind(nullptr);
        }

        mDefaultLogicalFrameBuffer.setVirtualSize(virtualFbSize);
        mDefaultLogicalFrameBuffer.setPhysicalArea(0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height));

        mDisplayBufferGL = new(heap) DisplayBufferGL();
        mDisplayBufferGL->initialize(static_cast<f32>(width), static_cast<f32>(height), heap);

        if (!setWaitVBlankInterval(mArg.wait_vblank))
            mArg.wait_vblank = 0;
    }
    Graphics::instance()->unlockDrawContext();
}

void GameFrameworkGlfwGL::resize_(f32 width, f32 height)
{
    Graphics::instance()->lockDrawContext();
    {
        {
            TextureGL* tex = mColorTexture;

            GLuint handle = tex->getID();

            glBindTexture(GL_TEXTURE_2D, handle);
            glTexImage2D(tex->getTarget(), 0, tex->getInternalFormat(), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, tex->getPixelFormat(), tex->getDataType(), nullptr);
        }

        {
            TextureGL* tex = mDepthTexture;

            GLuint handle = tex->getID();

            glBindTexture(GL_TEXTURE_2D, handle);
            glTexImage2D(tex->getTarget(), 0, tex->getInternalFormat(), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, tex->getPixelFormat(), tex->getDataType(), nullptr);
        }

        mDisplayBufferGL->reconfigure(width, height);
    }
    Graphics::instance()->unlockDrawContext();
}

} // namespace sead
