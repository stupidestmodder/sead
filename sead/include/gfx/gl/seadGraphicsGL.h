#pragma once

#include <gfx/seadGraphics.h>

namespace sead {

class GraphicsGL : public Graphics
{
public:
    struct CreateArg
    {
        void* hglrc;
        void* hdc;
    };

    static GraphicsGL* instance() { return static_cast<GraphicsGL*>(sInstance); }

public:
    explicit GraphicsGL(const CreateArg& arg);

    void* getHGLRC() { return mHGLRC; }
    void* getHDC() { return mHDC; }

protected:
    void initializeDrawLockContext(Heap* heap) override;
    void initializeImpl(Heap* heap) override;
    void setViewportImpl(f32 x, f32 y, f32 w, f32 h) override;
    void setScissorImpl(f32 x, f32 y, f32 w, f32 h) override;
    void setDepthTestEnableImpl(bool enable) override;
    void setDepthWriteEnableImpl(bool enable) override;
    void setDepthFuncImpl(DepthFunc func) override;
    bool setVBlankWaitIntervalImpl(u32 interval) override;
    void setCullingModeImpl(CullingMode mode) override;
    void setBlendEnableImpl(bool enable) override;
    void setBlendEnableMRTImpl(u32 target, bool enable) override;
    void setBlendFactorImpl(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA) override;
    void setBlendFactorMRTImpl(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA) override;
    void setBlendEquationImpl(BlendEquation equationRgb, BlendEquation equationA) override;
    void setBlendEquationMRTImpl(u32 target, BlendEquation equationRgb, BlendEquation equationA) override;
    void setBlendConstantColorImpl(const Color4f& color) override;
    void waitForVBlankImpl() override;
    void setColorMaskImpl(bool r, bool g, bool b, bool a) override;
    void setColorMaskMRTImpl(u32 target, bool r, bool g, bool b, bool a) override;
    void setAlphaTestEnableImpl(bool enable) override;
    void setAlphaTestFuncImpl(AlphaFunc func, f32 ref) override;
    void setStencilTestEnableImpl(bool enable) override;
    void setStencilTestFuncImpl(StencilFunc func, s32 ref, u32 mask) override;
    void setStencilTestOpImpl(StencilOp fail, StencilOp zfail, StencilOp zpass) override;
    void setPolygonModeImpl(PolygonMode front, PolygonMode back) override;
    void setPolygonOffsetEnableImpl(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable) override;

protected:
    void* mHGLRC; //* GL Context Handle
    void* mHDC;   //* Device Context Handle
    u32 mVBlankWaitInterval;
};

} // namespace sead
