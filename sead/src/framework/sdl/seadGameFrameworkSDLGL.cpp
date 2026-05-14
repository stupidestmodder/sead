#include <framework/sdl/seadGameFrameworkSDLGL.h>

#include <framework/seadProcessMeter.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>
#include <framework/seadTaskMgr.h>
#include <gfx/gl/seadGL.h>
#include <gfx/gl/seadGraphicsGL.h>
#include <gfx/gl/seadTextureFrameBufferGL.h>
#include <gfx/gl/seadTextureGL.h>
#include <gfx/seadTextureUtil.h>

namespace sead {

GameFrameworkSDLGL::GameFrameworkSDLGL(const CreateArg& arg)
    : GameFrameworkBaseSDL(arg)
    , mGLContext(nullptr)
    , mDisplayBufferGL(nullptr)
    , mColorTexture(nullptr)
    , mDepthTexture(nullptr)
    , mCopyReservedFrameBuffer(nullptr)
    , mGPUQueries()
{
    mProcDrawCallback = nullptr;
}

GameFrameworkSDLGL::~GameFrameworkSDLGL()
{
}

void GameFrameworkSDLGL::initializeGraphicsSystem(Heap* heap, const Vector2f& virtualFbSize)
{
    sead::CurrentHeapSetter chs(mSDLHeap);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    #if defined(SEAD_TARGET_DEBUG)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    #endif

    SDL_GLContext glContext = SDL_GL_CreateContext(mWindow);
    //assert

    SDL_GL_MakeCurrent(mWindow, glContext);
    //asser

    s32 ret = gladLoaderLoadGL();
    SEAD_ASSERT(ret != 0);

    SDL_GL_SetSwapInterval(0); 

    SDL_GL_MakeCurrent(nullptr, nullptr);

    GraphicsGL::CreateArg arg;
    arg.hdc = mWindow;
    arg.hglrc = glContext;

    Graphics::setInstance(new(heap) GraphicsGL(arg));
    Graphics::instance()->initialize(heap);

    // mHDC = hdc;
    // mHGLRC = hglrc;

    createFrameBuffer_(heap, virtualFbSize);

    Graphics::instance()->lockDrawContext();
    {
        glGenQueries(2, mGPUQueries);
    }
    Graphics::instance()->unlockDrawContext();
}

void GameFrameworkSDLGL::procFrame_()
{
    sead::CurrentHeapSetter chs(mSDLHeap);
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

           // glQueryCounter(mGPUQueries[0], GL_TIMESTAMP);
            {
                procDraw_();
            }
           // glQueryCounter(mGPUQueries[1], GL_TIMESTAMP);

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
            mDisplayState = DisplayState::eShow;
        }

        SDL_GL_SwapWindow(mWindow);
    }
    Graphics::instance()->unlockDrawContext();
}

void GameFrameworkSDLGL::procDraw_()
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

void GameFrameworkSDLGL::clearFrameBuffers_(s32)
{
    if (mDefaultFrameBuffer)
    {
        Viewport vp(*mDefaultFrameBuffer);
        vp.applyScissor(nullptr, *mDefaultFrameBuffer);

        mDefaultFrameBuffer->clear(nullptr, FrameBuffer::ClearFlag::eAll, mArg.clear_color, 1.0f, 0);
    }
}

void GameFrameworkSDLGL::present_()
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

void GameFrameworkSDLGL::createFrameBuffer_(Heap* heap, const Vector2f& virtualFbSize)
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

void GameFrameworkSDLGL::resize_(f32 width, f32 height)
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
