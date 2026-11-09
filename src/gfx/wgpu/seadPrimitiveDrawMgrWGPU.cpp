#include <gfx/wgpu/seadPrimitiveDrawMgrWGPU.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(PrimitiveDrawMgrWGPU);

PrimitiveDrawMgrWGPU::PrimitiveDrawMgrWGPU()
    : PrimitiveDrawMgrBase()
{
    
}

PrimitiveDrawMgrWGPU::~PrimitiveDrawMgrWGPU()
{
}

void PrimitiveDrawMgrWGPU::prepareFromBinaryImpl(Heap* heap, const void*, u32)
{
}

void PrimitiveDrawMgrWGPU::prepareImpl(Heap* heap, const SafeString&)
{
    prepareFromBinaryImpl(heap, nullptr, 0);
}

void PrimitiveDrawMgrWGPU::beginImpl(DrawContext* drawContext, const Matrix34f& viewMatrix, const Matrix44f& projectionMatrix)
{
}

void PrimitiveDrawMgrWGPU::endImpl(DrawContext* drawContext)
{
}

void PrimitiveDrawMgrWGPU::drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR)
{
}

void PrimitiveDrawMgrWGPU::drawQuadImpl(
    DrawContext* drawContext, const Matrix34f& modelMtx, const Texture& texture,
    const Color4f& colorL, const Color4f& colorR,
    const Vector2f& uvSrc, const Vector2f& uvSize)
{
}

void PrimitiveDrawMgrWGPU::drawBoxImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR)
{
}

void PrimitiveDrawMgrWGPU::drawLineImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& c0, const Color4f& c1)
{
}

void PrimitiveDrawMgrWGPU::drawSphere4x8Impl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& north, const Color4f& south)
{
}

void PrimitiveDrawMgrWGPU::drawSphere8x16Impl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& north, const Color4f& south)
{
}

} // namespace sead
