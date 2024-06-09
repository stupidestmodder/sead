#include <gfx/seadGraphicsContext.h>

namespace sead {

GraphicsContext::GraphicsContext()
    : mDepthTestEnable(true)
    , mDepthWriteEnable(true)
    , mDepthFunc(Graphics::DepthFunc::eLessEqual)

    , mCullingMode(Graphics::CullingMode::eBack)

    , mBlendEnable(true)
    , mBlendFactorSrcRGB(Graphics::BlendFactor::eSrcAlpha)
    , mBlendFactorSrcA(Graphics::BlendFactor::eSrcAlpha)
    , mBlendFactorDstRGB(Graphics::BlendFactor::eInvSrcAlpha)
    , mBlendFactorDstA(Graphics::BlendFactor::eInvSrcAlpha)
    , mBlendEquationRGB(Graphics::BlendEquation::eAdd)
    , mBlendEquationA(Graphics::BlendEquation::eAdd)
    , mBlendConstantColor(Color4f::cWhite)

    , mAlphaTestEnable(false)
    , mAlphaTestFunc(Graphics::AlphaFunc::eGreater)
    , mAlphaTestRef(0.0f)

    , mColorMaskR(true)
    , mColorMaskG(true)
    , mColorMaskB(true)
    , mColorMaskA(true)

    , mStencilTestEnable(false)
    , mStencilTestFunc(Graphics::StencilFunc::eNever)
    , mStencilTestRef(0)
    , mStencilTestMask(0xFFFFFFFF)
    , mStencilOpFail(Graphics::StencilOp::eKeep)
    , mStencilOpZFail(Graphics::StencilOp::eKeep)
    , mStencilOpZPass(Graphics::StencilOp::eKeep)

    , mPolygonModeFront(Graphics::PolygonMode::eFill)
    , mPolygonModeBack(Graphics::PolygonMode::eFill)
    , mPolygonOffsetFrontEnable(false)
    , mPolygonOffsetBackEnable(false)
    , mPolygonOffsetPointLineEnable(false)
{
}

void GraphicsContext::apply(DrawContext* drawContext) const
{
#if SEAD_GFX_BACKEND == GL_GFX_GL
    SEAD_UNUSED(drawContext);

    Graphics* gfx = Graphics::instance();

    // Depth
    gfx->setDepthEnable(mDepthTestEnable, mDepthWriteEnable);
    gfx->setDepthFunc(mDepthFunc);

    // Culling
    gfx->setCullingMode(mCullingMode);

    // Blend
    gfx->setBlendEnable(mBlendEnable);
    gfx->setBlendFactorSeparate(mBlendFactorSrcRGB, mBlendFactorDstRGB, mBlendFactorSrcA, mBlendFactorDstA);
    gfx->setBlendEquationSeparate(mBlendEquationRGB, mBlendEquationA);
    gfx->setBlendConstantColor(mBlendConstantColor);

    //// Alpha
    //gfx->setAlphaTestEnable(mAlphaTestEnable);
    //gfx->setAlphaTestFunc(mAlphaTestFunc, mAlphaTestRef);

    // ColorMask
    gfx->setColorMask(mColorMaskR, mColorMaskG, mColorMaskB, mColorMaskA);

    // Stencil
    gfx->setStencilTestEnable(mStencilTestEnable);
    gfx->setStencilTestFunc(mStencilTestFunc, mStencilTestRef, mStencilTestMask);
    gfx->setStencilTestOp(mStencilOpFail, mStencilOpZFail, mStencilOpZPass);

    // PolygonMode
    gfx->setPolygonMode(mPolygonModeFront, mPolygonModeBack);

    // PolygonOffset
    gfx->setPolygonOffsetEnable(mPolygonOffsetFrontEnable, mPolygonOffsetBackEnable, mPolygonOffsetPointLineEnable);
#else
#error "Unsupported GFX backend"
#endif
}

} // namespace sead
