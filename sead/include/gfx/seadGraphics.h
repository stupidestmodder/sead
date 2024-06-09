#pragma once

#include <gfx/seadColor.h>
#include <heap/seadDisposer.h>
#include <thread/seadCriticalSection.h>

#if SEAD_GFX_BACKEND == GL_GFX_GL
#include <gfx/gl/seadGL.h>
#endif

namespace sead {

class DrawLockContext;

class Graphics : public IDisposer
{
public:
    enum class DevicePosture
    {
        eSame = 0,
        eRotateRight,
        eRotateLeft,
        eRotateHalfAround,
        eFlipX,
        eFlipY,
        eFlipXY = 3,
        eInvalid
    };

    enum class DepthFunc : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        eLessEqual = GL_LEQUAL,
        eLess = GL_LESS,
        eGreaterEqual = GL_GEQUAL,
        eGreater = GL_GREATER,
        eAlways = GL_ALWAYS,
        eNever = GL_NEVER,
        eEqual = GL_EQUAL,
        eNotEqual = GL_NOTEQUAL,
#endif
        eInvalid = 0xFFFFFFFF
    };

    enum class CullingMode : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        eFront = GL_FRONT,
        eBack = GL_BACK,
        eNone = 0xFFFFFFF0,
        eAll = 0xFFFFFFF1,
#endif
        eInvalid = 0xFFFFFFFF
    };

    enum class BlendFactor : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        eZero = GL_ZERO,
        eOne = GL_ONE,
        eSrcColor = GL_SRC_COLOR,
        eInvSrcColor = GL_ONE_MINUS_SRC_COLOR,
        eSrcAlpha = GL_SRC_ALPHA,
        eInvSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        eDstColor = GL_DST_COLOR,
        eInvDstColor = GL_ONE_MINUS_DST_COLOR,
        eDstAlpha = GL_DST_ALPHA,
        eInvDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        eConstantColor = GL_CONSTANT_COLOR,
        eInvConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        eConstantAlpha = GL_CONSTANT_ALPHA,
        eInvConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
        eSrcAlphaSaturate = GL_SRC_ALPHA_SATURATE,
#endif
        eInvalid = 0xFFFFFFFF
    };

    enum class BlendEquation : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        eAdd = GL_FUNC_ADD,
        eSub = GL_FUNC_SUBTRACT,
        eReverseSub = GL_FUNC_REVERSE_SUBTRACT,
        eMin = GL_MIN,
        eMax = GL_MAX,
#endif
        eInvalid = 0xFFFFFFFF
    };

    enum class AlphaFunc : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        eLessEqual = GL_LEQUAL,
        eLess = GL_LESS,
        eGreaterEqual = GL_GEQUAL,
        eGreater = GL_GREATER,
        eAlways = GL_ALWAYS,
        eNever = GL_NEVER,
        eEqual = GL_EQUAL,
        eNotEqual = GL_NOTEQUAL,
#endif
        eInvalid = 0xFFFFFFFF
    };

    enum class StencilFunc : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        eLessEqual = GL_LEQUAL,
        eLess = GL_LESS,
        eGreaterEqual = GL_GEQUAL,
        eGreater = GL_GREATER,
        eAlways = GL_ALWAYS,
        eNever = GL_NEVER,
        eEqual = GL_EQUAL,
        eNotEqual = GL_NOTEQUAL,
#endif
        eInvalid = 0xFFFFFFFF
    };

    enum class StencilOp : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        eKeep = GL_KEEP,
        eZero = GL_ZERO,
        eReplace = GL_REPLACE,
        eIncrement = GL_INCR,
        eDecrement = GL_DECR,
        eInvert = GL_INVERT,
        eIncrementWrap = GL_INCR_WRAP,
        eDecrementWrap = GL_DECR_WRAP,
#endif
        eInvalid = 0xFFFFFFFF
    };

    enum class PolygonMode : u32
    {
#if SEAD_GFX_BACKEND == GL_GFX_GL
        ePoint = GL_POINT,
        eLine = GL_LINE,
        eFill = GL_FILL,
#endif
        eInvalid = 0xFFFFFFFF
    };

    using LockFunc = void (*)(bool isLock);

public:
    static Graphics* instance() { return sInstance; }
    static void setInstance(Graphics* impl) { sInstance = impl; }

public:
    Graphics();
    ~Graphics() override;

    void initialize(Heap* heap);

    void lockDrawContext();
    void unlockDrawContext();

    void setViewportRealPosition(f32 x, f32 y, f32 w, f32 h)
    {
        setViewportImpl(x, y, w, h);
    }

    void setScissorRealPosition(f32 x, f32 y, f32 w, f32 h)
    {
        setScissorImpl(x, y, w, h);
    }

    void setDepthEnable(bool testEnable, bool writeEnable)
    {
        setDepthTestEnable(testEnable);
        setDepthWriteEnable(writeEnable);
    }

    void setDepthTestEnable(bool enable)
    {
        setDepthTestEnableImpl(enable);
    }

    void setDepthWriteEnable(bool enable)
    {
        setDepthWriteEnableImpl(enable);
    }

    void setDepthFunc(DepthFunc func)
    {
        setDepthFuncImpl(func);
    }

    bool setVBlankWaitInterval(u32 interval)
    {
        return setVBlankWaitIntervalImpl(interval);
    }

    void setCullingMode(CullingMode mode)
    {
        setCullingModeImpl(mode);
    }

    void setBlendEnable(bool enable)
    {
        setBlendEnableImpl(enable);
    }

    void setBlendEnableMRT(u32 target, bool enable)
    {
        setBlendEnableMRTImpl(target, enable);
    }

    void setBlendFactor(BlendFactor srcFactor, BlendFactor dstFactor)
    {
        setBlendFactorImpl(srcFactor, dstFactor, srcFactor, dstFactor);
    }

    void setBlendFactorSeparate(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
    {
        setBlendFactorImpl(srcFactorRgb, dstFactorRgb, srcFactorA, dstFactorA);
    }

    void setBlendFactorMRT(u32 target, BlendFactor srcFactor, BlendFactor dstFactor)
    {
        setBlendFactorMRTImpl(target, srcFactor, dstFactor, srcFactor, dstFactor);
    }

    void setBlendFactorMRTSeparate(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
    {
        setBlendFactorMRTImpl(target, srcFactorRgb, dstFactorRgb, srcFactorA, dstFactorA);
    }

    void setBlendEquation(BlendEquation equation)
    {
        setBlendEquationImpl(equation, equation);
    }

    void setBlendEquationSeparate(BlendEquation equationRgb, BlendEquation equationA)
    {
        setBlendEquationImpl(equationRgb, equationA);
    }

    void setBlendEquationMRT(u32 target, BlendEquation equation)
    {
        setBlendEquationMRTImpl(target, equation, equation);
    }

    void setBlendEquationMRTSeparate(u32 target, BlendEquation equationRgb, BlendEquation equationA)
    {
        setBlendEquationMRTImpl(target, equationRgb, equationA);
    }

    void setBlendConstantColor(const Color4f& color)
    {
        setBlendConstantColorImpl(color);
    }

    void waitForVBlank()
    {
        waitForVBlankImpl();
    }

    void setColorMask(bool r, bool g, bool b, bool a)
    {
        setColorMaskImpl(r, g, b, a);
    }

    void setColorMaskMRT(u32 target, bool r, bool g, bool b, bool a)
    {
        setColorMaskMRTImpl(target, r, g, b, a);
    }

    void setAlphaTestEnable(bool enable)
    {
        setAlphaTestEnableImpl(enable);
    }

    void setAlphaTestFunc(AlphaFunc func, f32 ref)
    {
        setAlphaTestFuncImpl(func, ref);
    }

    void setStencilTestEnable(bool enable)
    {
        setStencilTestEnableImpl(enable);
    }

    void setStencilTestFunc(StencilFunc func, s32 ref, u32 mask)
    {
        setStencilTestFuncImpl(func, ref, mask);
    }

    void setStencilTestOp(StencilOp fail, StencilOp zfail, StencilOp zpass)
    {
        setStencilTestOpImpl(fail, zfail, zpass);
    }

    void setPolygonMode(PolygonMode front, PolygonMode back)
    {
        setPolygonModeImpl(front, back);
    }

    void setPolygonOffsetEnable(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable)
    {
        setPolygonOffsetEnableImpl(fillFrontEnable, fillBackEnable, pointLineEnable);
    }

    static DevicePosture getDefaultDevicePosture() { return sDefaultDevicePosture; }
    static f32 getDefaultDeviceZScale() { return sDefaultDeviceZScale; }
    static f32 getDefaultDeviceZOffset() { return sDefaultDeviceZOffset; }

protected:
    virtual void initializeDrawLockContext(Heap* heap);
    virtual void initializeImpl(Heap* heap) = 0;
    virtual void setViewportImpl(f32 x, f32 y, f32 w, f32 h) = 0;
    virtual void setScissorImpl(f32 x, f32 y, f32 w, f32 h) = 0;
    virtual void setDepthTestEnableImpl(bool enable) = 0;
    virtual void setDepthWriteEnableImpl(bool enable) = 0;
    virtual void setDepthFuncImpl(DepthFunc func) = 0;
    virtual bool setVBlankWaitIntervalImpl(u32 interval) = 0;
    virtual void setCullingModeImpl(CullingMode mode) = 0;
    virtual void setBlendEnableImpl(bool enable) = 0;
    virtual void setBlendEnableMRTImpl(u32 target, bool enable) = 0;
    virtual void setBlendFactorImpl(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA) = 0;
    virtual void setBlendFactorMRTImpl(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA) = 0;
    virtual void setBlendEquationImpl(BlendEquation equationRgb, BlendEquation equationA) = 0;
    virtual void setBlendEquationMRTImpl(u32 target, BlendEquation equationRgb, BlendEquation equationA) = 0;
    virtual void setBlendConstantColorImpl(const Color4f& color) = 0;
    virtual void waitForVBlankImpl() = 0;
    virtual void setColorMaskImpl(bool r, bool g, bool b, bool a) = 0;
    virtual void setColorMaskMRTImpl(u32 target, bool r, bool g, bool b, bool a) = 0;
    virtual void setAlphaTestEnableImpl(bool enable) = 0;
    virtual void setAlphaTestFuncImpl(AlphaFunc func, f32 ref) = 0;
    virtual void setStencilTestEnableImpl(bool enable) = 0;
    virtual void setStencilTestFuncImpl(StencilFunc func, s32 ref, u32 mask) = 0;
    virtual void setStencilTestOpImpl(StencilOp fail, StencilOp zfail, StencilOp zpass) = 0;
    virtual void setPolygonModeImpl(PolygonMode front, PolygonMode back) = 0;
    virtual void setPolygonOffsetEnableImpl(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable) = 0;

protected:
    static Graphics* sInstance;

    static DevicePosture sDefaultDevicePosture;
    static f32 sDefaultDeviceZScale;
    static f32 sDefaultDeviceZOffset;

protected:
    LockFunc mContextLockFunc;
    DrawLockContext* mDrawLockContext;
};

} // namespace sead
