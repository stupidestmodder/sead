#include <gfx/seadPrimitiveRenderer.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(PrimitiveRenderer);

PrimitiveRenderer::PrimitiveRenderer()
    : mDrawer(nullptr)
    , mViewMtx(Matrix34f::ident)
    , mProjMtx(Matrix44f::ident)
    , mModelMtx(Matrix34f::ident)
{
}

PrimitiveRenderer::~PrimitiveRenderer()
{
}

void PrimitiveRenderer::prepare(Heap* heap, const SafeString& shaderPath)
{
    PrimitiveDrawer::prepareMgr(heap, shaderPath);
}

void PrimitiveRenderer::begin()
{
    mDrawer.begin();
}

void PrimitiveRenderer::end()
{
    mDrawer.end();
}

void PrimitiveRenderer::drawQuad(const Color4f& colorL, const Color4f& colorR)
{
    mDrawer.drawQuad(colorL, colorR);
}

} // namespace sead
